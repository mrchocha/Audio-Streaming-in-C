# Audio-Streaming-in-C
Audio streaming is done using customized protocol to stream the audio from server to client. UDP protocol is used to transfer data and reliability is added by stop-and-wait protocol. Client has functionality to play, pause and resume the song.

## Table Of Contents
- [Introduction](#introduction)
- [Documentation](#documentation)
- [Installing Dependencies](#installing)
- [Compile and Running](#running)
- [Measuring Jitter and Average latency](#measuring-jitter-and-average-latency)
- [Authors](#authors)
- [Words from Authors](#words-from-authors)
- [References](#references)



# Installing
You need to install ` gcc ` in ubuntu or macOS to run the code. Another dependencies are ` libvlc ` and ` pthread ` librarys and `vlc media player`. Now to install it, run command
```
make install
```

# Compile and Running
To compile bith `client` and `server` run command 
```
make
```

To compile only `client` run command
```
make client
```

To compile only `server` run command
```
make server
```

Now to run `server` run command
```
./server <ip-address> <port1> <port2>
```

Now to run `client` run command
```
./client <ip-address> <port1> <port2>
```

i.e. `./server localhost 8080 8081`, `./client localhost 8080 8081` 
make sure that poert1 and port2 are same for both client and server.


# Authors
* Rahul Chocha [mrchocha](https://github.com/mrchocha/)
* Jeet Karia [JeetKaria06](https://github.com/JeetKaria06)

# Words from Authors
Special Thanks to Prof. Jitendra Bhtiya and T.A. [Parshwa Shah](https://github.com/parshwa1999) for your valuable guidence.

# References 
* [geeks for geeks](https://www.geeksforgeeks.org/udp-server-client-implementation-c/)
* [libvlc documentation](https://www.videolan.org/developers/vlc/doc/doxygen/html/group__libvlc.html)

