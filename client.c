//Author: Jose Enriquez
//Email: jose.enriquez@okstate.edu
//Date: 10/25/2022
//Description: TBD


#include "include.h"



int check(int exp, const char *message){
    if(exp == -1){
        perror(message);
        exit(1);
    }

    return exp;
}

int main(int argc, char const *argv[])
{
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        printf("socket creation failed...\n");
        exit(0);
    }
    struct sockaddr_in serverAddr = {0};

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serverAddr.sin_port = htons(SERVERPORT);

    //inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);

  

    check((connect(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr))),
        "Falied to connect to server");
    
    char buffer[BUFSIZE] = {0};
    int readInput = 0;

    //reading from server
    while(read(serverSocket, buffer, sizeof(buffer)) == 0){
            //read(serverSocket, buffer, sizeof(buffer));
    }
    
    printf("%s", buffer);

    while(1){

        //writing to server
        buffer[0] = 0;
        fgets(buffer, sizeof(buffer), stdin);
        buffer[strlen(buffer)-1] = '\0';
        write(serverSocket, buffer, sizeof(buffer));

        if(strcmp(buffer, "1") == 0){
            printf("Customer chose: 1. Looking at the jewelry menu\n");
        }

        if(strcmp(buffer, "2") == 0){
            printf("Customer chose: 2. Making specific jewelry inguiry\n");
        }

        if(strcmp(buffer, "3") == 0){
            printf("Customer chose: 3. Making purchase\n");
        }

        if(strcmp(buffer, "4") == 0){
            printf("4. Returning the purchase\n");
        }

        if(strcmp(buffer, "5") == 0){
            return 0;
        }

        fflush(stdout);
        buffer[0] = 0;
    } 
    
    
    //printf("%d", 0|POLL_IN);
    //strcpy(buffer, "cartier_catalog.txt");
    //printf("%s", buffer);
    //write(serverSocket, buffer, sizeof(buffer));
    // while(1){
    //     while(read(serverSocket, buffer, sizeof(buffer)) > 0){
    //         printf("%s\n", buffer);
    //         buffer[0] = 0;
    //     }
    // }
    return 0;
}
