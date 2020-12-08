#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <time.h>
#include <vlc/vlc.h>

#define MAXLINE 1024
#define BUFFSIZE 100

#define ERROR 0    	// Represents error
#define REQ 1		// Represents REQUEST for connection
#define ENDOFFILE 2	// Represents file is ended
#define ACK 3		// Represents acknowledgement of a particular sequence packet
#define DATA 4		// Represents DATA of the song requested

int PORT1, PORT2;

/*
*	Datagram : sends song packets along with seqno, name of the file, and timestamps for latency calculation
*/
struct Datagram
{
	int type;              
	int Seq_no;
	char filename[25];
	char buffer[MAXLINE];
	struct timeval tv;
};

/*
*	ListOfSongs : contains number of songs present and the name of the song files in 2d array of data
*/
struct ListOfSongs
{
	int size;
	char **data;
};

/*
* on : if 1 then song is playing and 0 if not playing. 
*/
int on = 0;

// Jitter maintains sum of difference in latencies
unsigned long jitter = 0;
// Addn maintains sum of all the latencies.
unsigned long addn = 0;

libvlc_instance_t *my_instance;
libvlc_media_t *my_media_file;
libvlc_media_player_t *my_player;

void *func(void)
{

	on = 1;
	// VLC engine initiation.
	my_instance = libvlc_new(0, NULL);

	if (my_instance == NULL)
	{
		printf("There was an error initializing VLC\n");
		exit(1);
	}

	my_media_file = libvlc_media_new_path(my_instance, "new.mp3");

	// Create player
	my_player = libvlc_media_player_new_from_media(my_media_file);

	// Start playing
	libvlc_media_player_play(my_player);

	pthread_exit(NULL);
}
/*
*	udp_receive_list : Executed when user enters "-list" and will accept ListOfSongs from server
*/
void udp_recieve_list(char *ip_addr)
{
	char buffer[BUFFSIZE];
	char *message = "Hello Server";
	int sockfd, n;
	struct sockaddr_in servaddr;

	// clear servaddr
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_addr.s_addr = inet_addr(ip_addr);
	servaddr.sin_port = htons(PORT2);
	servaddr.sin_family = AF_INET;

	// create datagram socket
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);

	// connect to server
	if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
	{
		printf("\n Error : Connect Failed \n");
		exit(0);
	}

	int size = 0;
	sendto(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)NULL, sizeof(servaddr));

	// waiting for response
	recvfrom(sockfd, &size, sizeof(size), 0, (struct sockaddr *)NULL, NULL);

	printf("================= LIST OF SONGS ==================\n");
	for (int j = 0; j < size; j++)
	{
		recvfrom(sockfd, buffer, BUFFSIZE, 0, (struct sockaddr *)NULL, NULL);
		printf("%0d %s\n",j+1,buffer);
	}
	printf("==================== ======= ======================\n");
		
	// close the descriptor
	close(sockfd);
}

void usage()
{
	printf("./client <ip-address> <port-no-1> <port-no-2>\n");
	printf("<port-no-1> - This port is for song transmission.\n");
	printf("<port-no-2> - This port is for transmitting list of songs available on the server side.\n");
	exit(8);
}

int main(int argc, char **argv)
{
	if(argc<4)
	{
		usage();
	}

	PORT1 = atoi(argv[2]);
	PORT2 = atoi(argv[3]);

	int sockfd;
	char buffer[MAXLINE];
	struct Datagram data;
	char command[100];
	struct sockaddr_in servaddr;
	unsigned long latency = 0;			// Maintains latency of the current packet
	addn = 0;						
	struct timeval tv;					// used to obtain system time 

	// Creating socket file descriptor
	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		perror("socket creation failed");
		exit(EXIT_FAILURE);
	}
	memset(&servaddr, 0, sizeof(servaddr));
	memset(&data, 0, sizeof(data));

	// Filling server information
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(PORT1);
	servaddr.sin_addr.s_addr = inet_addr(argv[1]);

	int n, len;
	// Thread for playing song in VLC without interfering with any other process
	pthread_t pthread;
	system("clear");
	/*
	* Infinitely takes input from user as commands and does appropriate task
	*/
	while (1)
	{
		udp_recieve_list(argv[1]);
		memset(&command, 0, sizeof(command));
		printf("Enter the command: \n");
		gets(command);

		char file_name[100] = {0};
		printf("Command is %s\n", command);
		// Returns list of songs
		if (!strcmp("-list", command))
		{
			udp_recieve_list(argv[1]);
			continue;
		}
		// Pauses the song
		else if (on && !strcmp("-pause", command))
		{
			libvlc_media_player_pause(my_player);
			continue;
		}
		// Resumes song from where it was paused
		else if (on && !strcmp("-resume", command))
		{
			libvlc_media_player_play(my_player);
			continue;
		}
		// Plays the song whose name is given
		else if (!strncmp("-play", command, 5))
		{
			int k = 0;
			for (int i = 6; i < strlen(command); i++)
				file_name[k++] = command[i];
		}
		// Shows different commands with their descriptions
		else if (!strcmp("-help", command))
		{
			printf("==================== COMMANDS ======================\n");
			printf("-play song.mp3/.wav/.mp4 : to play song with name song.  \n-pause : pause the song. \n-resume :resume the song. \n-list : get the list of all songs from server side\n-help :show the details of all commads \n");
			
			continue;
		}
		// Shows error if unknown command is entered by the user
		else
		{
			printf("Please use appropiate syntax!\n");
			sleep(1);
			continue;
		}

		if (on)
		{
			// stop playing
			libvlc_media_player_stop(my_player);

			// free the media_player
			libvlc_media_player_release(my_player);

			libvlc_release(my_instance);
		}

		memset(&data, 0, sizeof(data));

		data.type = REQ;
		data.Seq_no = -1;

		strcpy(data.filename, file_name);
		memset(&data.buffer, 0, sizeof(data.buffer));

		sendto(sockfd, &data, sizeof(data),
			   0, (const struct sockaddr *)&servaddr,
			   sizeof(servaddr));

		printf("Request sent.\n");

		int cnt = 0;
		int prv_seq = -1;

		FILE *fp;
		fp = fopen("new.mp3", "w");
		/*
		*	Infinitely receives song packets from server
		*/
		while (1)
		{

			n = recvfrom(sockfd, (struct Datagram *)&data, sizeof(data),
						 MSG_WAITALL, (struct sockaddr *)&servaddr,
						 &len);

			if (data.type == ERROR)
			{
				printf("Something went wrong, please try again!\n");
				break;
			}

			if (data.type == ENDOFFILE)
			{
				printf("Transmission Completed Successfully.\n");
				break;
			}

			if (prv_seq + 1 != data.Seq_no)
				continue;
			else
				prv_seq++;

			// Stores latency of the previous packet
			unsigned long prev_latency = latency;
			gettimeofday(&tv, NULL);
			// Substraction of current time in microsecond and sent packet time in microsecond gives latency
			latency = (1000000 * tv.tv_sec + tv.tv_usec) - (1000000 * data.tv.tv_sec + data.tv.tv_usec);
			// Maintains sum of the latencies
			addn += latency;
			// Adds difference of latencies to the jitter
			if (prev_latency != 0)
				jitter += abs(latency - prev_latency);
			printf("Latency: %lu us\n", latency);
			int siz = fwrite(data.buffer, sizeof(data.buffer), 1, fp);

			if (data.Seq_no == 300)
			{
				pthread_create(&pthread, NULL, func, NULL);
			}
			sendto(sockfd, &data, sizeof(data),
				   0, (const struct sockaddr *)&servaddr,
				   sizeof(servaddr));
			cnt++;
		}
		// Prints jitter, avg. latency and also percentage difference between jitter and avg. latency
		printf("Jitter: %0.3fus  Avg. latency: %0.3fus\n", (jitter) / (float)(data.Seq_no - 1), (addn)/(float)(data.Seq_no));
		if((jitter) / (float)(data.Seq_no - 1) > (addn)/(float)(data.Seq_no))
			printf("Jitter exceeds Avg. latency by %0.3f\%\n", 100*((((jitter) / (float)(data.Seq_no - 1))/((addn)/(float)(data.Seq_no)))-1));
		else 
			printf("Jitter falls behind Avg. latency by %0.3f\%\n", 100*((((addn)/(float)(data.Seq_no))/((jitter) / (float)(data.Seq_no - 1)))-1));
		jitter = 0;
		addn = 0;
		fclose(fp);
	}
	close(sockfd);
	return 0;
}
