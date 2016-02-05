#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/socket.h>
#include <linux/net.h>
#include <net/sock.h>
#include <linux/string.h>

//clientside
#define PORT 9339
#define SNDBUF 1024


static int the_socket2(void)
{
	struct socket *socketinfo2;
	struct sockaddr_in *client_address;
	struct msghdr *sock_msg;
	struct iovec iov;
	
	int a;
	int abc;
	int size;
    int send;
 	char *buffer;


	client_address=(struct sockaddr_in*) kmalloc(sizeof(struct sockaddr_in),GFP_KERNEL);
	//iov=(struct iovec*)kmalloc(sizeof(struct iovec),GFP_KERNEL);
	sock_msg=(struct msghdr*)kmalloc(sizeof(struct msghdr),GFP_KERNEL);
	buffer=(char*)kmalloc(((sizeof(char))*200),GFP_KERNEL);
	//create a socket
	
	a = sock_create_kern(PF_INET, SOCK_STREAM,IPPROTO_TCP, &socketinfo2);
	
	printk(KERN_ALERT "client sock %d\n",a); 

	if(a < 0) 
	{
			printk(KERN_ERR "creating socket failed");
			return -1;
	}
	memset(client_address,0,sizeof(struct sockaddr_in));

	client_address->sin_family = AF_INET;
	client_address->sin_addr.s_addr =htonl(INADDR_ANY);
	client_address->sin_port = htons(PORT);

	abc = socketinfo2->ops->connect(socketinfo2,(struct sockaddr*)&client_address, sizeof(client_address), 0);

    if(abc < 0)
    {
    	printk(KERN_ERR "Connecting failed");
    	return -1;
    }

    memset(sock_msg,0,sizeof(struct msghdr));
    memset(&iov,0,sizeof(iov));

	iov.iov_base=buffer;
	size=200;
	//iov.iov_len=size;
	// sock_msg->msg_name=cl_addr;
	// sock_msg->msg_namelen=sizeof(struct sockaddr_in);
	// sock_msg->msg_iov=iov;
	// sock_msg->msg_iovlen=1;
	// sock_msg->msg_control=NULL;
	// sock_msg->msg_controllen=0;
	// sock_msg->msg_flags=0;
    
    send = sock_sendmsg(socketinfo2,sock_msg,size);

    if(send < 0)
    {
    	printk(KERN_ERR "Sending failed");
    	sock_release((struct socket *)socketinfo2);
    	return -1;
    }

  return 0;
}


static int module5_init(void)
{
	the_socket2();
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
MODULE_DESCRIPTION("Playing with the kernel-Client");