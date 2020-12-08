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
Here, `type` is field to represents the packet type. there are 4 types which represents following.
```C
#define ERROR 0    	// Represents error
#define REQ 1		// Represents REQUEST for connection
#define ENDOFFILE 2	// Represents file is ended
#define ACK 3		// Represents acknowledgement of a particular sequence packet
#define DATA 4		// Represents DATA of the song requested
#define LIST 5		// Represents LIST OF THE SONGS is requested
```
The field with name `seq_no` represents the sequence number of packet which is sent or received. </br>
`filename` field represents the name of file which client has requested or server is sending. </br>
`buffer` is responsible for containing tha actual data of requested file/song. </br>
`timeval` is timestemp for data packet, which denotes the time when packet was sent.	
