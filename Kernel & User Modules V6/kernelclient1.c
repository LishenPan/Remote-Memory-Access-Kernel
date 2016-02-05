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

#ifndef VM_RESERVED
#define VM_RESERVED   (VM_DONTEXPAND | VM_DONTDUMP)
#endif

struct dentry  *file1;

struct mmap_info {
	char *data;	/* the data */
	int reference;       /* how many times it is mmapped */  	
};
/* keep track of how many times it is mmapped */

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
/* nopage is called the first time a memory area is accessed which is not in memory,
 * it does the actual mapping between kernel and user space memory
 */
//struct page *mmap_nopage(struct vm_area_struct *vma, unsigned long address, int *type)	--changed
static int mmap_fault(struct vm_area_struct *vma, struct vm_fault *vmf)
{
	struct page *page;
	struct mmap_info *info;
	/* is the address valid? */			//--changed
	/*if (address > vma->vm_end) {
		printk("invalid address\n");
		//return NOPAGE_SIGBUS;
		return VM_FAULT_SIGBUS;
	}
	/* the data is in vma->vm_private_data */
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
	/* type is the page fault type */
	/*if (type)
		*type = VM_FAULT_MINOR;
	*/
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
	memcpy(info->data, "hello from kernel this is file: ", 32);
	memcpy(info->data + 32, filp->f_dentry->d_name.name, strlen(filp->f_dentry->d_name.name));
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

printk("Client:Entering SendBuffer Client\n");


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

printk("Client:message sent\n");
printk("Client Message Contents: %s", Buffer);

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
oldfs = get_fs(); set_fs(KERNEL_DS);
len = sock_recvmsg(sock,&msg,Length,0/*MSG_DONTWAIT*/); // let it wait if there is no message
set_fs(oldfs);

// if ((len!=-EAGAIN)&&(len!=0))
// printk("RecvBuffer Recieved %i bytes \n",len);

return len;
}


	/*
	Sets up a server-side socket

	1. Create a new socket
	2. Bind the address to the socket
	3. Start listening on the socket
	*/

	struct socket* set_up_server_socket(int port_no) {
	struct socket *sock;
	struct sockaddr_in sin;

	int error;


	/* First create a socket */
	error = sock_create(PF_INET,SOCK_STREAM,IPPROTO_TCP,&sock) ;
	if (error<0)
	printk("Client:Error during creation of socket; terminating\n");



	/* Now bind the socket */
	printk("Client:successfully set up socket, now binding socket\n");
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = INADDR_ANY;
	sin.sin_port = htons(port_no);

	error = sock->ops->bind(sock,(struct sockaddr*)&sin,sizeof(sin));
	if (error<0)
	{
	printk("Client:Error binding socket \n");
	return 0;
	}
	printk("Client:successfully bind socket, now preparing to listen \n");
	/* Now, start listening on the socket */
	error=sock->ops->listen(sock,32);
	
	if (error!=0)
	{
	printk("Client:Error listening on socket \n");
	}
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

struct socket* server_accept_connection(struct socket *sock)
 {

	struct socket* newsock;
	int error;

	/* Before accept: Clone the socket */

	error = sock_create(PF_INET,SOCK_STREAM,IPPROTO_TCP,&newsock);

	if (error<0)
	{
	printk("Client:Error during creation of the other socket; terminating\n");
	}

	newsock->type = sock->type;
	newsock->ops=sock->ops;

	/* Do the actual accept */

	error = newsock->ops->accept(sock,newsock,0);


	if (error<0) 
	{
	printk("Client:Error accepting socket\n") ;
	return 0;
	}
        return newsock;
 }


struct socket* set_up_client_socket(unsigned int IP_addr, int port_no){
struct socket * clientsock;
struct sockaddr_in sin;
int error, i;
//unsigned int ip_addr;
//ip_addr = 168430082;
/* First create a socket */
error = sock_create(PF_INET,SOCK_STREAM,IPPROTO_TCP,&clientsock);
if (error<0) {
printk("Client:Error during creation of socket; terminating\n");
return 0;
}

/* Now bind and connect the socket */
sin.sin_family = AF_INET;
//sin.sin_addr.s_addr = htonl(INADDR_ANY);
sin.sin_addr.s_addr = htonl(IP_addr);
sin.sin_port = htons(port_no);



for(i=0;i<10;i++) {
error = clientsock->ops->connect(clientsock,(struct sockaddr*)&sin,sizeof(sin),0);
if (error<0) {
printk("Client:Error connecting client socket to server: %i, retrying .. %d \n",error, i);
if(i==10-1) {
printk("Client:Giving Up!\n"); return 0;
}
}
else 
{
break; //connected
}

}

return clientsock;

}

static int module7_init(void)
{
int port= 13394;
unsigned int IP_addr= 168430083;
struct socket* socketz;
size_t sendings;

char *sendbuff;
size_t length;
sendbuff= "hi i am thinh";

file1 = debugfs_create_file("mmap_example1", 0644, NULL, NULL, &my_fops);
socketz=set_up_client_socket(IP_addr,port);
length=strlen(sendbuff);
sendings= SendBuffer(socketz, sendbuff, length);

return 0;
}
static void module7_exit(void)
{
	debugfs_remove(file1);
	printk(KERN_ALERT "Client:The struggle is fucken real!");
	return;
}

module_init(module7_init);
module_exit(module7_exit);
MODULE_AUTHOR("thinh");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("blahblah");

