//Author: Jose Enriquez
//Email: jose.enriquez@okstate.edu
//Date: 10/25/2022
//Description: TBD

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


int main(int argc, char const *argv[])
{
    //sets up server
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
        while(read(serverSocket, buffer, sizeof(buffer)) == 0){
            
        }
        int setting = strtol(buffer, NULL, 10);
        buffer[0] = 0;
        
        if(setting == 0){
            //handled by shop assitant

            //reading from server
            while(read(serverSocket, buffer, sizeof(buffer)) == 0){
                
            }
            //display server message
            printf("%s", buffer);


            while(1){

                //writing to server, the clients' input
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
                    while(read(serverSocket, buffer, sizeof(buffer)) == 0){
                        
                    }
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

            //read from server
            while(read(serverSocket, buffer, sizeof(buffer)) == 0){
                //read(serverSocket, buffer, sizeof(buffer));
            }
            printf("%s", buffer);
            fflush(stdout);
            buffer[0] = 0;

            //client chooses to either leave or wait and sends responce to server
            fgets(buffer, sizeof(buffer), stdin);
            buffer[strlen(buffer)-1] = '\0';
            write(serverSocket, buffer, sizeof(buffer));
            fflush(stdout);
            //buffer[0] = 0;

            if(strcmp(buffer, "0") == 0){
                //client wants to wait
            }
            if(strcmp(buffer, "1") == 0){
                //client wants to leave
                while(read(serverSocket, buffer, sizeof(buffer)) == 0){
               
                }
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

            while(read(serverSocket, buffer, sizeof(buffer)) == 0){
                //read(serverSocket, buffer, sizeof(buffer));
            }
            printf("%s", buffer);
            return 0;
        }
    }
    
    return 0;
}
