To Compile server.c file :- 
gcc server.c  get_AllJewelfunc.c get_Jewelfunc.c return_Jewelfunc.c purchase_Jewelfunc.c  queue.c readFile.c -lpthread

To execute server.c file :-
./a.out Na Ns Nmax (Nmax >= Ns)

To Compile client.c file :-
gcc client.c  -lpthread

To execute client.c file :-
./a.out

Assumptions:
1) The third command argument for server execution(./a.out Na Ns Nmax) should be greater than or equal to the second
   Nmax >= Ns
   
2) Assume a client gets assigned to the sofa or waiting room, they have to choose whether they are staying or leaving
   before any other client gets connected to the server.
 
3) Menu Options will not be displayed again but the client can choose any number of options.

4) For disconnecting enter option "5". Ctrl+C does not work.
 
4) Have all the files in the same folder.
