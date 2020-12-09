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

> ## This quantities tells us about the state of the network.
