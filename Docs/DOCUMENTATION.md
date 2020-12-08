# Common Notations
At server/client side it sends/receives structure which has following parameters.
```C
struct Datagram
{
	int type;
	int Seq_no;
	char filename[25];
	char buffer[MAXLINE];
	struct timeval tv;
};
```
Here, `type` is field to represents the packet type, there are 4 types which is represented as following.
```C
#define ERROR 0    	// Represents error
#define REQ 1		// Represents REQUEST for connection
#define ENDOFFILE 2	// Represents file is ended
#define ACK 3		// Represents acknowledgement of a particular sequence packet
#define DATA 4		// Represents DATA of the song requested
#define LIST 5		// Represents LIST OF THE SONGS is requested
```
The fields with name </br></br>
`seq_no` represents the sequence number of packet which is sent or received. </br></br>
`filename` field represents the name of file which client has requested or server is sending. </br></br>
`buffer` is responsible for containing tha actual data of requested file/song. </br></br>
`tv` is timestamp for data packet, which denotes the time when packet was sent.	</br></br>

## Server Side
At the server side first it creates the necessary bindings for socket,ip-address and ports etc. Here server uses "UDP protocol" as socket type, then it waits for any client for handshake request with type `REQ`, if connection is successful then it waits for client to send a file name to be transfered wuth packet type `DATA`. Now if file is not present at serverside then server sends packet with type `ERROR`. If file is present then it sends data of file in chunks of size `MAXLINE` with packet type `DATA` and respective `seq_no` starting from 0. After transferring each packet it waits for acknowledgement with packet type `ACK`. If ack is correct then it transfers next chunk else it retransmits the previous chunk which is called stop and wailt protocol. This all will happen at `port1`.
</br></br>
Another thread is created to transfer data for request of list of avalable songs' files at `port2`. Here server at port2 gives list of songs infinitely. Thread is created because list request should not interrupt the main song transmission process.  

## Client Side

Client side when executed gives list of songs initially that are available on the server side. This transmission of songs' list is done on a separate port so that it doesn't conflicts with the port that transmits the actual song. And there are some commands which user can write for different functionalities, and they are as follows:

```
==================== COMMANDS ======================
-play song.mp3/.wav/.mp4 : to play song with name song.  
-pause : pause the song. 
-resume :resume the song. 
-list : get the list of all songs from server side
-help :show the details of all commads
```

When user wants to play the song, a separate VLC thread is created which keeps playing the requested song. The reason for creating separate thread for song playing is that, if user wants to see the list of songs when some is song is already playing, then it must not interrupt the already playing song. Also, when user enters ```-list``` command, it listens on another port of the same ```IP-address``` and receives the list of songs that server is sending infinitely. Also, client computes jitter and average latency which is explained more [here](https://github.com/mrchocha/Audio-Streaming-in-C/blob/main/Docs/Measuring_Jitter_And_Average_Latency.md#jitter). And the computed jitter and average latency is shown after the whole song is buffered at the client. 
