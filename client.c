//Author: Jose Enriquez
//Email: jose.enriquez@okstate.edu
//Date: 10/25/2022
//Description: CLient program to interact with the server

#include "include.h"

/// @brief function to aid in debugging bu printing error messages
/// @param exp numeric result of a function
/// @param message accompanying message to be printed if the numeric reuslt implies error.
/// @return exp numeric result of a function
int check(int exp, const char *message){
    if(exp == -1){
        perror(message);
        exit(1);
    }

    return exp;
}

/// @brief From research, Apparently the kernal will sometimes decide to not send all the data when reading and writing.
///         SO i wrote a function modeling the normal write/read functions that will make sure that full data gets sent,
///         This avoiding partial reads/writes. The loop will continuously send/recieve data until the number of bytes sent/received matches len
/// @param fd the socket to send data to
/// @param buf the buffer contaning the data to be sent
/// @param len the number of bytes that need to be sent
/// @return the number of bvytes sent
ssize_t fullwrite(int fd, const void *buf, size_t len){
	size_t total = 0;
	const char *p = buf;
	while(total < len){
		ssize_t r = write(fd, p + total, len -total);
		switch (r) {
		case -1: return -1;
		case 0: return total;
		default: total += r;
		}
	}
	return total;
}

/// @brief From research, Apparently the kernal will sometimes decide to not send all the data when reading and writing.
///         SO i wrote a function modeling the normal write/read functions that will make sure that full data gets sent,
///         This avoiding partial reads/writes. The loop will continuously send/receive data until the number of bytes sent/received matches len
/// @param fd the socket to read data from
/// @param buf the buffer to store the data
/// @param len the number of bytes that need to be read
/// @return the number of bytes read
ssize_t fullread(int fd, void *buf, size_t len){
	size_t total = 0;
	char *p = buf;
	while(total < len){
		ssize_t r = read(fd, p+total, len-total);
		switch(r){
			case -1: return -1;
			case 0: return total;
			default: total += r;
		}
	}

	return total;
}


int main(int argc, char const *argv[])
{
    //sets up connection to server
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        printf("socket creation failed...\n");
        exit(0);
    }
    struct sockaddr_in serverAddr = {0};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serverAddr.sin_port = htons(SERVERPORT);

    check((connect(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr))),
        "Falied to connect to server");

    char buffer[BUFSIZE] = {0};
    int readInput = 0;


    while(1){

        ///reading from server to determine what the client is being handled by
        fullread(serverSocket, buffer, sizeof(buffer));
        int setting = strtol(buffer, NULL, 10);
        buffer[0] = 0;

        if(setting == 0){
            //handled by shop assitant

            //reading from server
            fullread(serverSocket, buffer, sizeof(buffer));
            //display server message
            printf("%s", buffer);

            while(1){

                //writing to server, the clients' input
                buffer[0] = 0;
                fgets(buffer, sizeof(buffer), stdin);
                buffer[strlen(buffer)-1] = '\0';
                fullwrite(serverSocket, buffer, sizeof(buffer));

              if(strcmp(buffer, "1") == 0){
                    //client chose option 1: look at jewelry menu
                    buffer[0] = 0;

                    //loop to receive all the items from server
                    for(int i = 0; i < 47; i++){
                        fullread(serverSocket, buffer, sizeof(buffer));
                        printf("%s",buffer);
                        buffer[0] = 0;
                    }
                }

                if(strcmp(buffer, "2") == 0){
                    //client chose option 2: specific inquirey about item

                    //read from server
                    fullread(serverSocket, buffer, sizeof(buffer));
                    printf("%s", buffer);
                    buffer[0] = 0;

                    //write client input to server
                    fgets(buffer, sizeof(buffer), stdin);
                    buffer[strlen(buffer)-1] = '\0';
                    fullwrite(serverSocket, buffer, sizeof(buffer));
                    buffer[0] = 0;

                    //read information about item from server
                    fullread(serverSocket, buffer, sizeof(buffer));
                    printf("\n%s\n", buffer);
                    buffer[0] = 0;

                }

                if(strcmp(buffer, "3") == 0){
                    //client chose option 3: make purchase

                    //read message from server
                    fullread(serverSocket, buffer, sizeof(buffer));
                    printf("%s", buffer);
                    buffer[0] = 0;

                    //write purchase Quantity to server
                    fgets(buffer, sizeof(buffer), stdin);
                    int purchaseQnty = (int) strtol(buffer, NULL, 10);
                    fullwrite(serverSocket, buffer, sizeof(buffer));
                    buffer[0] = 0;

                    //read message from server
                    fullread(serverSocket, buffer, sizeof(buffer));
                    printf("%s", buffer);
                    buffer[0] = 0;

                    //loop to send refs to server

                    for(int i = 0; i < purchaseQnty;i++){
                        fgets(buffer, sizeof(buffer), stdin);
                        buffer[strlen(buffer) - 1] = '\0';
                        //removes the new line character

                        //printf("User entered: %s\n", buffer);
                        fullwrite(serverSocket, buffer, sizeof(buffer));
                        buffer[0] = 0;
                    }

                    printf("\n");

                    //Loop to read receipt from server
                    for(int i=0; i < (purchaseQnty) +1 ; i++){

                        fullread(serverSocket, buffer, sizeof(buffer));
                        printf("%s", buffer);
                        buffer[0] = 0;
                    }
                }

                if(strcmp(buffer, "4") == 0){
                    //client chose option 4: return purchase

                    //read message from server
                    fullread(serverSocket, buffer, sizeof(buffer));
                    printf("%s", buffer);
                    buffer[0] = 0;

                    //write return Quantity to server
                    fgets(buffer, sizeof(buffer), stdin);
                    int returnQnty = (int) strtol(buffer, NULL, 10);
                    fullwrite(serverSocket, buffer, sizeof(buffer));
                    buffer[0] = 0;

                    //read message from server
                    fullread(serverSocket, buffer, sizeof(buffer));
                    printf("%s", buffer);
                    buffer[0] = 0;

                    //loop to send refs to server

                    for(int i = 0;i < returnQnty;i++){
                        fgets(buffer, sizeof(buffer), stdin);
                        buffer[strlen(buffer) - 1] = '\0';
                        //removes the new line character

                        //printf("User entered: %s\n", buffer);
                        fullwrite(serverSocket, buffer, sizeof(buffer));
                        buffer[0] = 0;
                    }

                    //Loop to read return log from server
                    printf("\n");
                    for(int i = 0; i < returnQnty; i++){

                        fullread(serverSocket, buffer, sizeof(buffer));
                        printf("%s", buffer);
                        buffer[0] = 0;

                    }
                }

                if(strcmp(buffer, "5") == 0){
                    //client chose option 5: exit store

                    fullread(serverSocket, buffer, sizeof(buffer));
                    //display server message
                    printf("%s", buffer);
                    return 0;
                }

                fflush(stdout);
                buffer[0] = 0;
            }
        }

        if(setting == 1){
            //handled by sofa

            fullread(serverSocket, buffer, sizeof(buffer));
            printf("%s", buffer);
            fflush(stdout);
            buffer[0] = 0;

            //client chooses to either leave or wait and sends responce to server
            fgets(buffer, sizeof(buffer), stdin);
            buffer[strlen(buffer)-1] = '\0';
            fullwrite(serverSocket, buffer, sizeof(buffer));
            fflush(stdout);
            //buffer[0] = 0;

            if(strcmp(buffer, "0") == 0){
                //client wants to wait
            }
            if(strcmp(buffer, "1") == 0){
                //client wants to leave

                fullread(serverSocket, buffer, sizeof(buffer));
                printf("%s", buffer);
                fflush(stdout);
                return 0;
            }
        }
        if(setting == 2){
            //handled by waiting room
        }
        if(setting == 3){
            //no room in shop getting booted or you chose to leave

            fullread(serverSocket, buffer, sizeof(buffer));
            printf("%s", buffer);
            return 0;
        }
    }

    return 0;
}