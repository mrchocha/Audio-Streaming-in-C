# Audio-Streaming-in-C
Audio streaming is done using customized protocol to stream the audio from server to client. UDP protocol is used to transfer data and reliability is added by stop-and-wait protocol. Client has functionality to play, pause and resume the song.

## Table Of Contents
- [Documentation](#Documentation)
- [Installing Dependencies](#installing-dependencies)
- [Compile and Running](#compile-and-running)
- [Measuring Jitter and Average latency](#measuring-jitter-and-average-latency)
- [Authors](#authors)
- [Words from Authors](#words-from-authors)
- [References](#references)

## Documentation

Refer docs [here](https://github.com/mrchocha/Audio-Streaming-in-C/tree/main/Docs#common-notations).

## Installing Dependencies
You must have ` gcc ` on your ubuntu or macOS to run the code. Other dependencies are ` libvlc ` and ` pthread ` libraries and `vlc media player`. To install them, run 
```
make install
```

## Compile and Running
To compile both `client` and `server` run command 
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
./server <ip-address> <port-no-1> <port-no-2>
```

Now to run `client` run command
```
./client <ip-address> <port-no-1> <port-no-2>
```

> i.e. `./server 127.0.0.1 8080 8081`, `./client 127.0.0.1 8080 8081` 
> make sure that port1 and port2 are same for both client and server. </br>
> `<port-no-1>` This port is for song transmission. </br>
> `<port-no-2>` This port is for transmitting list of songs available on the server side.

## Measuring jitter and average latency

For detailed understanding of jitter and average latency calculation go [here](https://github.com/mrchocha/Audio-Streaming-in-C/blob/main/Docs/Measuring_Jitter_And_Average_Latency.md#jitter).

## Authors
* Rahul Chocha [mrchocha](https://github.com/mrchocha/)
* Jeet Karia [JeetKaria06](https://github.com/JeetKaria06)

## Words from Authors
Special Thanks to Prof. Jitendra Bhatia and Teaching Assitant [Parshwa Shah](https://github.com/parshwa1999) for their valuable guidence.

## References 
* [Geeks For Geeks](https://www.geeksforgeeks.org/udp-server-client-implementation-c/)
* [libvlc documentation](https://www.videolan.org/developers/vlc/doc/doxygen/html/group__libvlc.html)

