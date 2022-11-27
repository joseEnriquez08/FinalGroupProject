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

        //writing to server, the clients' input
        buffer[0] = 0;
        fgets(buffer, sizeof(buffer), stdin);
        buffer[strlen(buffer)-1] = '\0';
        write(serverSocket, buffer, sizeof(buffer));

        if(strcmp(buffer, "1") == 0){
            buffer[0] = 0;

            //loop to receive data
            for(int i = 0; i < 46; i++){
                char other_buf[1000];
                while(read(serverSocket, other_buf, sizeof(other_buf)) == 0){
        
                }
                printf("%s",other_buf);
                buffer[0] = 0;
                //fflush(stdin);
            }
        }
        fflush(stdout);
        buffer[0] = 0;
    
        
    }
    return 0;
}