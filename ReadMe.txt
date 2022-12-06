# FinalGroupProject

*With each new file, we have to add it to the compilation list down below.
*Make sure to make only the necessary functions global in incldue.h. 


To compile server.c: gcc -lpthread menuFunctions.c parseData.c queue.c server.c -o server.out
To run server:       ./server.out 2 2 1

To compile client.c: gcc -lpthread parseData.c client.c -o client.out
To run client: ./client.out

Try -pthread if -lpthread doesnt work

They have to be compiled and ran on separate terminals.