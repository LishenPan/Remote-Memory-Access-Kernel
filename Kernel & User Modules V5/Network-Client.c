#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h> 
#include <stdlib.h>
#define DATA "TESTING PURPOSES"

int main(int argc, char *argv[])
{
	int socketfd;
	char buffer[1024];
	struct sockaddr_in client_address;
	struct hostent *host;

	int readvalue;
     
    //create a socket
    socketfd=socket(AF_INET, SOCK_STREAM, 0);

    if(socketfd<0)
	{
		perror("Error, no socket number assigned");
		exit(1);
	}

	client_address.sin_family = AF_INET;
	client_address.sin_port= htons(9011);
	client_address.sin_addr.s_addr = INADDR_ANY;

	/*if(inet_pton(AF_INET, argv[1], &client_address.sin_addr)<= 0)
    {
        printf("\n inet_pton error occured\n");
        return 1;
    } */

	//host = gethostbyname(argv[1]);

	if (host == 0)
	{
		perror("Failed to get host by name");
		close(socketfd);
		exit(1);
	}
    
 //   memcpy(&client_address.sin_addr, host->h_addr, host->h_length);

    

	if(connect(socketfd, (struct sockaddr *) &client_address, sizeof(client_address)) < 0)
	{
		perror("Connecting failed");
		close(socketfd);
		exit(1);
	}

	if(send(socketfd, DATA, sizeof(DATA), 0) < 0)
	{
		perror("Sending failed");
		close(socketfd);
		exit(1);
	}
	printf("Sent %s \n", DATA);

    close(socketfd);
	return 0;	

}
