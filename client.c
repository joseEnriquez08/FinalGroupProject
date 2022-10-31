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

    check((connect(serverSocket, (struct sockaddr*)&serverAddr, (socklen_t*)sizeof(serverAddr) != 0)),
        "Falied to connect to server");
       

    char buffer[BUFSIZE];
    strcpy(buffer, "cartier_catalog.txt");
    write(serverSocket, buffer, sizeof(buffer));
    while(read(serverSocket, buffer, sizeof(buffer)) > 0){
        printf("%s\n", buffer);
        buffer[0] = 0;
    }
    return 0;
}
