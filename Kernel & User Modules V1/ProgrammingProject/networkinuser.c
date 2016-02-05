#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

int main(int argc, char *argv[])
{
	int socketfd, newsocketfd;
	int portnumber, clientlength;
	char buffer[256];
	struct sockaddr_in server_address, client_address;
	int readvalue;
     
    if(argc < 2)
    {
    	printf("Error no port provided for socket\n");
    }
	socketfd=socket(AF_INET, SOCK_STREAM, 0);

	if(socketfd<0)
	{
		printf("Error, no socket number assigned\n");
	}

	bzero((char *) &server_address, sizeof(server_address));

	portnumber = atoi(argv[1]);    //storing the port number, i need to convert character array to integer value

	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = INADDR_ANY;
	server_address.sin_port = htons(portnumber);

	if(bind(socketfd,(struct sockaddr *)&server_address, sizeof(server_address)) < 0)
	{
		printf("Error, binding of the socket failed!\n");
	}
	
	listen(socketfd,5);   //listening with the open socket file descriptor

	clientlength = sizeof(client_address);

	newsocketfd= accept(socketfd, (struct sockaddr *) &client_address, &clientlength);

	if(newsocketfd < 0)
	{
		printf("Error, new socket creation failed\n");
	}
    bzero(buffer,256);

    readvalue=read(newsocketfd,buffer,255);

    if(readvalue < 0)
    {
    	printf("Error when reading from the socket")
    }
    printf("Here is the message received: \n%s\n", buffer);

    readvalue = write(newsocketfd, "messege receive successful", 18);
    
    if(readvalue < 0)
    {
    	printf("Error writing to socket\n");
    }
    
	return 0;
}
