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

# Server Side
At the server side first it creates the nesasory bindings for socket,ip-address and ports etc. here server uses "UDP protocol" as socket type. then it waits for any client for handshake request with type `REQ`. if connection is sucessfull then it waits for client to send a file name to be transfered wuth packet type `DATA`. now if file is not present at serverside then server sends packet with type `ERROR`. if filwe is present then it sends data of file in chunks of size `MAXLINE` with packet type `DATA` and respective `seq_no` starting from 0. After transfarinig each packet it waits for aacknowledgement with packet type `ACK`. if ack is correct then it transfers next chunck else it retransfers the previous chunk.
