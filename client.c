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

          char regmsg[150];


        	while(1){

            	//writing to server, the clients' input
            	buffer[0] = 0;
            	fgets(buffer, sizeof(buffer), stdin);
            	buffer[strlen(buffer)-1] = '\0';
            	write(serverSocket, buffer, sizeof(buffer));

            	if(strcmp(buffer, "1") == 0){
                	printf("Customer chose: 1. Looking at the jewelry menu\n");
                
                // Added by Venkata Ragavendra Vavilthota
                char other_buf[1000];
                for(int i = 0; i < 47; i++){
                
                while(read(serverSocket, other_buf, sizeof(other_buf)) == 0){
        
                }
                printf("%s",other_buf);

                }
                
                  char server_msg1[1000];
                  while(read(serverSocket, server_msg1, sizeof(server_msg1)) == 0){

                	}
                  printf("%s", server_msg1);
                
                // Added by Venkata Ragavendra Vavilthota
            	}

            	if(strcmp(buffer, "2") == 0){
                	printf("Customer chose: 2. Making specific jewelry inguiry\n");

                // Added by Venkata Ragavendra Vavilthota
                // Get the reference number from client
                  char buf_ref[10];
                  printf("Enter ref Item to get more details:\n");
                  scanf("%s", buf_ref);

                  // send the data to server
                  write(serverSocket, buf_ref, sizeof(buf_ref));
                  char client_rcvmsg[1000];
                	while(read(serverSocket, client_rcvmsg, sizeof(client_rcvmsg)) == 0){

                	}
                	//display reference data from server
                	printf("%s", client_rcvmsg);

                  char server_msg2[1000];
                  while(read(serverSocket, server_msg2, sizeof(server_msg2)) == 0){

                	}
                  printf("%s", server_msg2);
                

                	// Added by Venkata Ragavendra Vavilthota
            	}

            	if(strcmp(buffer, "3") == 0){
                	printf("Customer chose: 3. Making purchase\n");
                
                // Added by Venkata Ragavendra Vavilthota
                // Get the reference number to buy from client
                  char buf_buy[10];
                  printf("Enter ref Item to buy:\n");
                  scanf("%s", buf_buy);

                  // send the data to server
                  write(serverSocket, buf_buy, sizeof(buf_buy));
                
                  char client_buymsg[1000];
                	while(read(serverSocket, client_buymsg, sizeof(client_buymsg)) == 0){

                	}
                	//display reference data from server
                	printf("%s", client_buymsg);
                
                // Read question from server
                 char server_msg3[1000];
                  while(read(serverSocket, server_msg3, sizeof(server_msg3)) == 0){

                	}
                  printf("%s", server_msg3);
                // Added by Venkata Ragavendra Vavilthota
                
            	}

            	if(strcmp(buffer, "4") == 0){
                	printf("4. Returning the purchase\n");
                
                // Added by Venkata Ragavendra Vavilthota
                // Get the reference number to return from client
                  char buf_ret[10];
                  printf("Enter ref Item to return:\n");
                  scanf("%s", buf_ret);

                  // send the data to server
                  write(serverSocket, buf_ret, sizeof(buf_ret));
                
                  //display reference data from server
                  char client_retmsg[1000];
                	while(read(serverSocket, client_retmsg, sizeof(client_retmsg)) == 0){

                	}
                	
                	printf("%s", client_retmsg);
                
                 //Read the ques from sever
                 char server_msg4[1000];
                  while(read(serverSocket, server_msg4, sizeof(server_msg4)) == 0){

                	}
                  printf("%s", server_msg4);
                // Added by Venkata Ragavendra Vavilthota

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
            	fflush(stdin);
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
