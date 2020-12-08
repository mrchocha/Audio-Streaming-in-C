#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <pthread.h>
#include <time.h>

#include "listFiles.h"

#define MAXLINE 1024    
#define BUFFSIZE 100

#define ERROR 0    	// Represents error
#define REQ 1		// Represents REQUEST for connection
#define ENDOFFILE 2	// Represents file is ended
#define ACK 3		// Represents acknowledgement of a particular sequence packet
#define DATA 4		// Represents DATA of the song requested
#define LIST 5		// Represents LIST OF THE SONGS is requested

int PORT1, PORT2;

/*
 * error - wrapper for perror
 */
void error(char *msg)
{
	perror(msg);
	exit(1);
}
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
*	udp_send_list : sends number of songs and list of song file names using UDP protocol
*/
void *udp_send_list(char *ip_addr)
{
	char buffer[BUFFSIZE];
	char *message = "Hello Client";
	int listenfd, len;
	struct sockaddr_in servaddr, cliaddr;
	bzero(&servaddr, sizeof(servaddr));

	// Create a UDP Socket
	listenfd = socket(AF_INET, SOCK_DGRAM, 0);
	servaddr.sin_addr.s_addr = inet_addr(ip_addr);
	servaddr.sin_port = htons(PORT2);
	servaddr.sin_family = AF_INET;

	// bind server address to socket descriptor
	bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
	while (1)
	{
		//receive the datagram
		int size = 0;
		len = sizeof(cliaddr);
		int n = recvfrom(listenfd, buffer, sizeof(buffer),
						 0, (struct sockaddr *)&cliaddr, &len); //receive message from server

		char **arr = listFiles(&size);
		sendto(listenfd, &size, sizeof(size), 0,
			   (struct sockaddr *)&cliaddr, sizeof(cliaddr));

		for (int j = 0; j < size; j++)
		{
			sendto(listenfd, arr[j], BUFFSIZE, 0,
				   (struct sockaddr *)&cliaddr, sizeof(cliaddr));
		}
	}

	pthread_exit(NULL);
}

void usage()
{
	printf("./server <ip-address> <port-no-1> <port-no-2>\n");
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
	int sockfd;					   /* socket */
	int clientlen;				   /* byte size of client's address */
	struct sockaddr_in serveraddr; /* server's addr */
	struct sockaddr_in clientaddr; /* client addr */
	struct hostent *hostp;		   /* client host info */
	char *hostaddrp;			   /* dotted decimal host addr string */
	int optval;					   /* flag value for setsockopt */
	int n;						   /* message byte size */
	struct Datagram data;
	

	/* 
   	* socket: create the parent socket 
   	*/
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sockfd < 0)
		error("ERROR opening socket");

	optval = 1;
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR,
			   (const void *)&optval, sizeof(int));

	/*
   	* build the server's Internet address
   	*/
	bzero((char *)&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr(argv[1]);
	serveraddr.sin_port = htons(PORT1);

	/* 
   	* bind: associate the parent socket with a port 
   	*/
	if (bind(sockfd, (struct sockaddr *)&serveraddr,
			 sizeof(serveraddr)) < 0)
		error("ERROR on binding");

	/* 
	* List of songs thread: A thread is created which will infinitely send number of songs and list of file names on PORT2 
   	*/
   	pthread_t pthread;
	pthread_create(&pthread, NULL, udp_send_list, argv[1]);
	
	/*
	* 	Infinitely accepts name of the file to be played from client side and sending that file packets to client side 
	*/
	clientlen = sizeof(clientaddr);
	while (1)
	{

		memset(&data, 0, sizeof(data));
		n = recvfrom(sockfd, (struct Datagram *)&data, sizeof(data), 0,
					 (struct sockaddr *)&clientaddr, &clientlen);

		if (data.type != REQ)
			continue;

		if (n < 0)
			error("ERROR in recvfrom");

		hostp = gethostbyaddr((const char *)&clientaddr.sin_addr.s_addr,
							  sizeof(clientaddr.sin_addr.s_addr), AF_INET);
		if (hostp == NULL)
			error("ERROR on gethostbyaddr");

		hostaddrp = inet_ntoa(clientaddr.sin_addr);
		if (hostaddrp == NULL)
			error("ERROR on inet_ntoa\n");

		/* 
     	* Opens file name received and shows error if it doesn't exist 
     	*/
		FILE *fp, *fp2;
		printf("Client requested: %s\n", data.filename);
		fp = fopen(data.filename, "r");
		if (fp == NULL)
		{
			printf("File %s not found!\n", data.filename);

			data.Seq_no = -1;
			data.type = ERROR;
			gettimeofday(&(data.tv), NULL);
			sendto(sockfd, (struct Datagram *)&data, sizeof(data),
				   MSG_CONFIRM, (const struct sockaddr *)&clientaddr,
				   clientlen);
			continue;
		}
		int cnt = 0;

		while (1)
		{
			memset(&data, 0, sizeof(data));

			data.Seq_no = cnt;
			data.type = DATA;
			gettimeofday(&(data.tv), NULL);
			int siz = fread(&data.buffer, sizeof(data.buffer), 1, fp);

			if (siz <= 0)
			{
				data.type = ENDOFFILE;
				gettimeofday(&(data.tv), NULL);
				sendto(sockfd, &data, sizeof(data),
					   0, (const struct sockaddr *)&clientaddr,
					   sizeof(clientaddr));
				break;
			}
			gettimeofday(&(data.tv), NULL);
			sendto(sockfd, (struct Datagram *)&data, sizeof(data),
				   MSG_CONFIRM, (const struct sockaddr *)&clientaddr,
				   clientlen);

			/*
			* STOP AND WAIT : Infinitely keeps receiving until appropiate ACK is received
			*/
			while (1)
			{
				struct Datagram data2;
				n = recvfrom(sockfd, (struct Datagram *)&data2, sizeof(data2), 0,
							 (struct sockaddr *)&clientaddr, &clientlen);

				if (data2.Seq_no == cnt)
					break;

				gettimeofday(&(data.tv), NULL);
				sendto(sockfd, (struct Datagram *)&data, sizeof(data),
					   MSG_CONFIRM, (const struct sockaddr *)&clientaddr,
					   clientlen);

				free((struct Datagram *)&data2);
			}
			cnt++;
		}

		printf("Song Buffered to the Client side.\n", data.filename);
	}
}
