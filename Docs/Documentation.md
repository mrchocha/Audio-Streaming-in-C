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
#define REQ 1		// Represents REQUEST for connection/file
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
At the server side first it creates the necessary bindings for socket,ip-address and ports etc. Here server uses "UDP protocol" as socket type, then it waits for any client for  request with type `REQ` which has file name to be transfered. If file is not present at serverside then server sends packet with type `ERROR`. If file is present then it sends data of file in chunks of size `MAXLINE` with packet type `DATA` and respective `seq_no` starting from 0. After transferring each packet it waits for acknowledgement with packet type `ACK`. If ack is correct then it transfers next chunk else it retransmits the previous chunk which is called stop and wailt protocol. This all will happen at `port1`.
</br></br>
Another thread is created to transfer data for request of list of avalable songs' files at `port2`. Here server at port2 gives list of songs infinitely. Thread is created because list request should not interrupt the main song transmission process. The list of songs comes from `list.c` file which is responsible for fetchig all files in current directory with type '.mp3/.wav/.mp4' and store it in the linked list and then to 2d array which is returned to the server and then ultimatly sent to client.

## Client Side

Client side when executed, first binds with the provided ```IP-address``` and ```PORT-NO```. It then gets list of songs initially that are available on the server side from separate port on which server is infinitely sending data. This transmission of songs' list is done on a separate port so that it doesn't conflicts with the port that transmits the actual song. And there are some commands which user can write for different functionalities, and they are as follows:

```
==================== COMMANDS ======================
-play song.mp3/.wav/.mp4 : to play song with name song.  
-pause : pause the song. 
-resume :resume the song. 
-list : get the list of all songs from server side
-help :show the details of all commads
```

At any time of execution, if server sends data packet type as ```ERROR``` then client stops execution and again asks for song to play. When users executes ```-play``` command, it will be sent to server with data type as ```REQ```, and also a separate VLC thread is created which keeps playing the requested song. The reason for creating separate thread for song playing is that, if user wants to see the list of songs when some is song is already playing, then it must not interrupt the already playing song. Also, when user enters ```-list``` command, it listens on another port of the same ```IP-address``` and receives the list of songs that server is sending infinitely. Also, client computes jitter and average latency which is explained more [here](https://github.com/mrchocha/Audio-Streaming-in-C/blob/main/Docs/Measuring_Jitter_And_Average_Latency.md#jitter). And the computed jitter and average latency is shown after the whole song is buffered at the client. 

# Measuring jitter and Average Latency

# Jitter
Jitter is the variation in the latency of the received packets.

Steps
=====

1 - Caculating latency
----------------------

The data packet sent from the server side sends structure which consists of timeval field which contains the time (in microseconds) when the data was sent from the server.

When that data packet is received, a variable is maintained on the client side named ```latency``` which substacts cuurent time (in microseconds) with received data packet time and that gives the delay that packet took in reaching client from server, and that it latency.

Before changing the variable ```latency``` we are storing the value of it in variable named ```prev_latency```, so that it maintains latency of the previous received data packet and can be used in the calculation explained further.

2 - Taking difference between latencies
---------------------------------------

Now, we need to take difference of current data packet latency with previous data packet latency.

Consider a example, latencies of 5 packets are: 1, 2, 3, 4, 5 </br>
Differences will be: 2-1, 3-2, 4-3, 5-4 &#8594; 1, 1, 1, 1

Here, first packet is discarded because there is no packet latency before the first packet.

So, for ```n``` packets' latencies, we will get ```n-1``` difference terms.

3 - Summation
-------------

Now, we need to take summation of the difference terms for all the received data packets and divide it by ```#packets_received - 1``` (explained in the above step about subtracting 1). And the resulting term is jitter which is displayed in our case after the whole song is buffered by the client.

# Average Latency

Average latency is taking sum of all the latencies of all the received data packets and dividing it by number of received data packets.
<br>

> ## This quanties tells us about the state of the network.
