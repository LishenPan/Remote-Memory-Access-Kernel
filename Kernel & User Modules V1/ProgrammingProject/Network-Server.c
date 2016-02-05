#include <stdio.h>
#include <cstdio>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <cstdlib>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
	int socketfd, newsocketfd;
	int portnumber, clientlength;
	char buffer[1024];
	struct sockaddr_in server_address, client_address;
	int readvalue;
     
    //Creating a Socket

	socketfd=socket(AF_INET, SOCK_STREAM, 0);

	if(socketfd<0)
	{
		perror("Error, no socket number assigned");
		exit(1);
	}

	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = INADDR_ANY;
	server_address.sin_port = htons(9011);

    //binding

	if(bind(socketfd, (struct sockaddr *)&server_address, sizeof(server_address))<0)
	{
		perror("Error, binding of the socket failed!");
		close(newsocketfd);
		exit(1);
	}
	
	//listening 

	listen(socketfd,5);  

	//accepting
  
do{
	newsocketfd= accept(socketfd, (struct sockaddr *) 0, 0);
  	if(newsocketfd == -1)
	{
		perror("Error, accepting failed\n");
        close(newsocketfd);
		exit(1);
	}

	if(newsocketfd == 0 || newsocketfd > 0)
	{
        memset(buffer,0, sizeof(buffer));

        if((readvalue == recv(newsocketfd, buffer, sizeof(buffer), 0)) < 0)
        {
        	perror("Reading stream message error");
        	close(newsocketfd);
        }
        else if (readvalue = 0)
        {
            printf("Reading connection\n");  
        }
        else
        {
        	printf("MESSSAGE: %s \n", buffer);
        }
        close(newsocketfd);
	}
 }while(1);

return 0;
}
