#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/socket.h>
#include <linux/net.h>
#include <net/sock.h>
#include <linux/string.h>

//serverside
#define PORT 9339
#define SNDBUF 1024



static int the_socket(void)
{
	struct socket *socketnumber1;
	struct socket *socketnumber2;
	struct sockaddr_in *server_address;
	//struct sockaddr_in *client_addres;
	struct msghdr *sock_msg;
	struct iovec iov;
	int accepting1;
	int size;
	int readvalue;
	char *buffer;
	int a,b;
	//mm_segment_t oldms;

	printk(KERN_INFO "We are infact loading");

	a = sock_create_kern(PF_INET, SOCK_STREAM,IPPROTO_TCP, &socketnumber1);
	b = sock_create_lite(AF_INET, SOCK_STREAM, 0, &socketnumber2);
    server_address=(struct sockaddr_in*) kmalloc(sizeof(struct sockaddr_in),GFP_KERNEL);
   // client_address=(struct sockaddr_in*) kmalloc(sizeof(struct sockaddr_in),GFP_KERNEL);

    //iov = (struct iovec) kmalloc(sizeof(iov),GFP_KERNEL);
	sock_msg = (struct msghdr*) kmalloc(sizeof(struct msghdr),GFP_KERNEL);
	buffer=(char *)kmalloc((sizeof(char)*1024),GFP_KERNEL);
    
    printk(KERN_INFO "PHASE 1");
	if(a<0) 
	{
			printk(KERN_ERR "CREATE SOCKET ERROR");
			return -1;
	}

	memset(server_address,0, sizeof(struct sockaddr_in));
	//memset(newsocketfd,0, sizeof(struct socket));
	//memset(socketinfo,0, sizeof(struct socket));

    server_address->sin_family = AF_INET;
    server_address->sin_port = htons(PORT);
    server_address->sin_addr.s_addr =htonl(INADDR_ANY);
      
    //bind
    a = socketnumber1->ops->bind(socketnumber1,(struct sockaddr*)server_address,sizeof(struct sockaddr_in));
    
    //listen

    a = socketnumber1->ops->listen(socketnumber1,5);

    //accept

	iov.iov_base=buffer;
	size=SNDBUF;
	

	iov.iov_len=size;
	//sock_msg->msg_name= client_address;
	sock_msg->msg_namelen=sizeof(struct sockaddr_in);
	sock_msg->msg_iov=&iov;
	sock_msg->msg_iovlen=1;
	sock_msg->msg_control=NULL;
	sock_msg->msg_controllen=0;
	sock_msg->msg_flags=0;

 // 	size = SNDBUF;
	
	accepting1 = socketnumber1->ops->accept(socketnumber1,socketnumber2, 0);

	memset(&iov,0,sizeof(iov));
	memset(sock_msg,0,sizeof(struct msghdr));
	memset(buffer,0,SNDBUF);

  	if(accepting1 == -1)
	{
		printk(KERN_ERR "Error, accepting failed");
		 sock_release((struct socket *)socketnumber2);
		return -1;
	}
	if(accepting1 == 0 || accepting1 > 0)
	{
		//oldms = get_fs(); set_fs(KERNEL_DS);
		readvalue=sock_recvmsg(socketnumber2,sock_msg,size,0);
		//set_fs(oldms);
		if(readvalue < 0 )
        {
        	printk(KERN_ERR "Reading stream message error");
        	sock_release((struct socket *)socketnumber2);
        }
        else if (readvalue == 0)
        {
            printk(KERN_INFO"Reading connection\n");  
        }
        else
        {
        	printk(KERN_INFO "MESSSAGE: %s",buffer);
        }
        sock_release((struct socket *)socketnumber2);
	}
  

	 return 0;
}
static int module5_init(void)
{
	the_socket();
	return 0;	
}
static void module5_exit(void)
{
	printk(KERN_ALERT "The struggle is fucken real!");
	return;	
}
module_init(module5_init);
module_exit(module5_exit);
MODULE_AUTHOR("Nicholas Foulk, SJSU");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Playing with the kernel-Server");