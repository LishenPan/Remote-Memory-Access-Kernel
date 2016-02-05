#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/socket.h>
#include <linux/net.h>
#include <net/sock.h>
#include <linux/string.h>
//mmap headers
#include <linux/fs.h>
#include <linux/debugfs.h>
#include <linux/slab.h>
#include <linux/mm.h>  /* mmap related stuff */
#include <linux/string.h>
#ifndef VM_RESERVED
#define VM_RESERVED   (VM_DONTEXPAND | VM_DONTDUMP)
#endif

//file operation
#include <asm/uaccess.h>  /* for put_user */
#include <asm/errno.h>

//timer file
#include <linux/timer.h>
//netlink
#include <net/sock.h>
#include <linux/netlink.h>
#include <linux/skbuff.h>

#include <asm/uaccess.h>  /* for put_user */
#include <asm/errno.h>


  
struct dentry  *file1;
	struct socket* socketz;

	char *Buffer;
	struct sock *nl_sk = NULL;

	char * netlinkbuff;
	int lens2;
struct mmap_info 
{
	char *data;	/* the data */
	int reference;       /* how many times it is mmapped */  	
};
/* keep track of how many times it is mmapped */

	struct socket* serversocket;
	struct socket* newsocks;


void mmap_open(struct vm_area_struct *vma)
{
	struct mmap_info *info = (struct mmap_info *)vma->vm_private_data;
	info->reference++;
}

void mmap_close(struct vm_area_struct *vma)
{
	struct mmap_info *info = (struct mmap_info *)vma->vm_private_data;
	info->reference--;
}

static int mmap_fault(struct vm_area_struct *vma, struct vm_fault *vmf)
{
	struct page *page;
	struct mmap_info *info;
	
	info = (struct mmap_info *)vma->vm_private_data;
	if (!info->data) {
		printk("no data\n");
		return NULL;	
	}

	/* get the page */
	page = virt_to_page(info->data);
	
	/* increment the reference count of this page */
	get_page(page);
	vmf->page = page;					//--changed
	printk("\n mm-fault, virtual address %p \n page address %p",vmf->virtual_address, page);	

	return 0;
}

struct vm_operations_struct mmap_vm_ops = {
	.open =     mmap_open,
	.close =    mmap_close,
	.fault =    mmap_fault,
	//.nopage =   mmap_nopage,				//--changed
};

int my_mmap(struct file *filp, struct vm_area_struct *vma)
{
	vma->vm_ops = &mmap_vm_ops;
	vma->vm_flags |= VM_RESERVED;
	/* assign the file private data to the vm private data */
	vma->vm_private_data = filp->private_data;
	mmap_open(vma);
	return 0;
}

int my_close(struct inode *inode, struct file *filp)
{
	struct mmap_info *info = filp->private_data;
	/* obtain new memory */
	free_page((unsigned long)info->data);
    	kfree(info);
	filp->private_data = NULL;
	return 0;
}

int my_open(struct inode *inode, struct file *filp)
{
	struct mmap_info *info = kmalloc(sizeof(struct mmap_info), GFP_KERNEL);
	/* obtain new memory */
    	info->data = (char *)get_zeroed_page(GFP_KERNEL);
	memcpy(info->data, Buffer, 1024);
	memcpy(info->data + 1024, filp->f_dentry->d_name.name, strlen(filp->f_dentry->d_name.name));
	/* assign this info struct to the file */
	filp->private_data = info;
	return 0;
}

static const struct file_operations my_fops = {
	.open = my_open,
	.release = my_close,
	.mmap = my_mmap,
};
/*
Sendbuffer sends "Length" bytes from "Buffer" through the socket "sock".
*/
size_t SendBuffer(struct socket *sock, const char *Buffer, size_t Length)
{

	struct msghdr msg;
	mm_segment_t oldfs; // mm_segment_t is just a long
	struct iovec iov; // structure containing a base addr. and length
	int len2;

	printk("Server:Entering SendBuffer\n");


	msg.msg_name = 0;
	msg.msg_namelen = 0;
	msg.msg_iov = &iov;
	msg.msg_iovlen = 1; //point to be noted
	msg.msg_control = NULL;
	msg.msg_controllen = 0;

	msg.msg_flags = MSG_NOSIGNAL;//0/*MSG_DONTWAIT*/;

	iov.iov_base = (char*) Buffer; // as we know that iovec is
	iov.iov_len = (__kernel_size_t) Length; // nothing but a base addr and length



	// #define get_fs() (current_thread_info()->addr_limit)
	// similar for set_fs;
	/*
	Therefore this line sets the "fs" to KERNEL_DS and saves its old value
	*/
	oldfs = get_fs(); set_fs(KERNEL_DS);

	/* Actual Sending of the Message */
	len2 = sock_sendmsg(sock,&msg,(size_t)(Length));

	/* retrieve the old value of fs (whatever it is)*/
	set_fs(oldfs);

	return len2;
}


/*
Recieves data from the socket "sock" and puts it in the 'Buffer'.
Returns the length of data recieved

The Calling function must do a:
Buffer = (char*) get_free_page(GFP_KERNEL);
or a kmalloc to allocate kernel's memory
(or it can use the kernel's stack space [very small] )

*/


size_t RecvBuffer(struct socket *sock, const char *Buffer, size_t Length)
{
	struct msghdr msg;
	struct iovec iov;

	int len;
	mm_segment_t oldfs;
	printk("Server:Entering Rec. Buffer \n");
	/* Set the msghdr structure*/
	msg.msg_name = 0;
	msg.msg_namelen = 0;
	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;
	msg.msg_control = NULL;
	msg.msg_controllen = 0;
	msg.msg_flags = 0;

	/* Set the iovec structure*/
	iov.iov_base = (void *) &Buffer[0];
	iov.iov_len = (size_t)Length;

	/* Recieve the message */
	oldfs = get_fs(); 
	set_fs(KERNEL_DS);
	len = sock_recvmsg(sock,&msg,Length,0/*MSG_DONTWAIT*/); // let it wait if there is no message
	set_fs(oldfs);
	//printk("Server msg received:- %s  \n", Buffer);
	if ((len!=-EAGAIN)&&(len!=0))
        {
	printk("Server:RecvBuffer Recieved %i bytes \n",len);
        }
	//printk("Server:finish recvbuffer\n");
	return len;

}



/*
Sets up a server-side socket

1. Create a new socket
2. Bind the address to the socket
3. Start listening on the socket
*/

struct socket* set_up_server_socket(int port_no) 
{
	struct socket *sock;
	struct sockaddr_in sin;
	unsigned int serv_addr;
	int error, a;


	serv_addr = 3232250007;
	/* First create a socket */
	error = sock_create(PF_INET,SOCK_STREAM,IPPROTO_TCP,&sock) ;
	if (error<0)
	printk("Server:Error during creation of socket; terminating\n");


	/* Now bind the socket */
	printk("Server:successfully set up socket, now binding socket\n");
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = htonl(serv_addr);
	sin.sin_port = htons(port_no);

	a = sock->ops->bind(sock,(struct sockaddr*)&sin,sizeof(sin));
	if (a<0)
	{
	printk("Server:Error binding socket \n");
	return 0;
	}
	printk("Server:successfully bind socket, now preparing to listen \n");
	/* Now, start listening on the socket */
	error=sock->ops->listen(sock,32);

	if (error!=0)
	printk("Server:Error listening on socket \n");

	/* Now start accepting */
	// Accepting is performed by the function server_accept_connection

	return sock;
}


/*

Accepts a new connection (server calls this function)

1. Create a new socket
2. Call socket->ops->accept
3. return the newly created socket

*/

struct socket* server_accept_connection(struct socket* sock) 
{
	struct socket* newsock;
	int error;

	/* Before accept: Clone the socket */

	error = sock_create(PF_INET,SOCK_STREAM,IPPROTO_TCP,&newsock);
	if (error<0)
	printk("Server:Error during creation of the other socket; terminating\n");

	newsock->type = sock->type;
	newsock->ops=sock->ops;

	/* Do the actual accept */

	error = newsock->ops->accept(sock,newsock,0);


	if (error<0) 
	{
	printk("Server:Error accepting socket\n") ;
	return 0;
	}
	return newsock;
}


struct socket* set_up_client_socket( int IP_addr, int port_no)
{

	struct socket * clientsock;
	struct sockaddr_in sin;
	int error, i;

	/* First create a socket */
	error = sock_create(PF_INET,SOCK_STREAM,IPPROTO_TCP,&clientsock);
	if (error<0) 
	{
	printk("Server:Error during creation of socket; terminating\n");
	return 0;
	}

	/* Now bind and connect the socket */
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = htonl(IP_addr);
	sin.sin_port = htons(port_no);

	for(i=0;i<10;i++) 
	{
	error = clientsock->ops->connect(clientsock,(struct sockaddr*)&sin,sizeof(sin),0);
	if (error<0) 
	{
	printk("Server:Error connecting client socket to server: %i, retrying .. %d \n",error, i);
	if(i==10-1) 
	{
	printk("Server:Giving Up!\n"); return 0;
	}
	else 
	break; //connected
	}
}

return clientsock;

}


static void netlinkmsg(struct sk_buff *skb)
{

    struct nlmsghdr *nlh;
    int pid;
    struct sk_buff *skb_out;
    int msg_size;
    char *msg = "messaged recieved";
    int res;
    int sendings;

    printk(KERN_INFO "Entering: %s\n", __FUNCTION__);

    msg_size = strlen(msg);

    nlh = (struct nlmsghdr *)skb->data;
    printk(KERN_INFO "Netlink received msg payload: %s\n", (char *)nlmsg_data(nlh));


   

	netlinkbuff= (char *)nlmsg_data(nlh);
	lens2= strlen(netlinkbuff);
	pid = nlh->nlmsg_pid; /*pid of sending process */

    skb_out = nlmsg_new(msg_size, 0);

    if (!skb_out)
    {

        printk(KERN_ERR "Failed to allocate new skb\n");
        return;

    }
    nlh = nlmsg_put(skb_out, 0, 0, NLMSG_DONE, msg_size, 0);
    NETLINK_CB(skb_out).dst_group = 0; /* not in mcast group */
    strncpy(nlmsg_data(nlh), msg, msg_size);

    res = nlmsg_unicast(nl_sk, skb_out, pid);

    if (res < 0)
    {
        printk(KERN_INFO "Error while sending bak to user\n");
    }
        sendings = SendBuffer(newsocks,  netlinkbuff, lens2);
   if(sendings<0)
   {
   	printk("error sending ");

   }
   else
   	printk("sent complete");

   
}


static int module7_init(void)
{
	int port= 13394;

	size_t recieving;
   
	struct netlink_kernel_cfg cfg = {
                .groups = 1,
                .input = netlinkmsg,
        };
        nl_sk = netlink_kernel_create(&init_net, 3, &cfg);

	Buffer = (char*)__get_free_page(GFP_KERNEL);
 
	serversocket= set_up_server_socket(port);
	newsocks= server_accept_connection(serversocket); 

	

	recieving = RecvBuffer(newsocks, Buffer, 1024);
	if (recieving<=0)
	{
	printk(KERN_INFO "Server:error when receiving message\n");
	}
	else
	{
	printk(KERN_INFO "Server received msg: %s  \n",Buffer);

	}


file1 = debugfs_create_file("mmap_example1", 0644, NULL, NULL, &my_fops);
	if (file1 == NULL)
	{
   	 	printk(KERN_ALERT "Error occurred\n");
   	}
	else
	{
   		printk("The creation of the file should has occured\n");
    		printk("The pointer value of the create file %p \n", &file1);
	}


	
	
 	
  return 0;
}
static void module7_exit(void)
{

sock_release(serversocket);
sock_release(newsocks);
	debugfs_remove(file1);
 	netlink_kernel_release(nl_sk);
	printk(KERN_ALERT "Server exiting \n");
	return;
}


module_init(module7_init);
module_exit(module7_exit);
MODULE_AUTHOR("thinh");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("blahblah");

