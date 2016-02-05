
#include <linux/module.h> // all modules
#include <linux/init.h>    //entry and exit
#include <linux/kernel.h>   //printk


#include <linux/net.h>
#include <net/sock.h>
#include <linux/tcp.h>
#include <linux/in.h>
#include <asm/uaccess.h>
#include <linux/file.h>
#include <linux/socket.h>
//#include <linux/smp_lock.h>
#include <linux/slab.h>

struct params {
    /* Destination IP address */
    unsigned char destip[4];
    /* Source IP address */
    unsigned char srcip[4];
    /* Source file - file to be downloaded from
       the server */
    char src[64];
    /* Destination file - local file where the
       downloaded file is copied */
    char dst[64];
    char user[16]; /* Username */
    char pass[64]; /* Password */
};
asmlinkage int my_sys_call(struct params __user *pm)
{
    struct sockaddr_in saddr, daddr;
    struct socket *control= NULL;
    struct socket *data = NULL;
    struct socket *new_sock = NULL;

    int r = -1;
    char *response = kmalloc(SNDBUF, GFP_KERNEL);
    char *reply = kmalloc(RCVBUF, GFP_KERNEL);

    struct params pmk;

    if(unlikely(!access_ok(VERIFY_READ,
                 pm, sizeof(pm))))
        return -EFAULT;
    if(copy_from_user(&pmk, pm,
       sizeof(struct params)))
        return -EFAULT;
    if(current->uid != 0)
        return r;

    r = sock_create(PF_INET, SOCK_STREAM,
                    IPPROTO_TCP, &control);

    memset(&servaddr,0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr.s_addr =
        htonl(create_address(128, 196, 40, 225));

    r = control->ops->connect(control,
             (struct sockaddr *) &servaddr,
             sizeof(servaddr), O_RDWR);
    read_response(control, response);
    sprintf(temp, "USER %s\r\n", pmk.user);
    send_reply(control, temp);
    read_response(control, response);
    sprintf(temp, "PASS %s\r\n", pmk.pass);
    send_reply(control, temp);
    read_response(control, response);
    r = sock_create(PF_INET, SOCK_STREAM,
                IPPROTO_TCP, &data);
memset(&claddr,0, sizeof(claddr));
claddr.sin_family = AF_INET;
claddr.sin_port = htons(EPH_PORT);
clddr.sin_addr.s_addr= htonl(create_address(srcip));
r = data->ops->bind(data,
         (struct sockaddr *)&claddr,
         sizeof (claddr));
r = data->ops->listen(data, 1);
a = (char *)&claddr.sin_addr;
p = (char *)&claddr.sin_port;

send_reply(control, reply);
read_response(control, response);

strcpy(reply, "RETR ");
strcat(reply, src);
strcat(reply, "\r\n");

send_reply(control, reply);
read_response(control, response);
new_sock = sock_alloc();
new_sock->type = data->type;
new_sock->ops = data->ops;

r = data->ops->accept(data, new_sock, 0);
new_sock->ops->getname(new_sock,
    (struct sockaddr *)address, &len, 2);
    if((total_written = write_to_file(pmk.dst,
            new_sock, response)) < 0)
    goto err3;
return;
}

static void send_reply(struct socket *sock, char *str)
{
    send_sync_buf(sock, str, strlen(str),
                  MSG_DONTWAIT);
}


static int send_sync_buf (struct socket *sock, const char *buf,const size_t length, unsigned long flags)
{
    struct msghdr msg;
    struct iovec iov;
    int len, written = 0, left = length;
    mm_segment_t oldmm;

    msg.msg_name     = 0;
    msg.msg_namelen  = 0;
    msg.msg_iov      = &iov;
    msg.msg_iovlen   = 1;
    msg.msg_control  = NULL;
    msg.msg_controllen = 0;
    msg.msg_flags    = flags;

    oldmm = get_fs(); set_fs(KERNEL_DS);

repeat_send:
    msg.msg_iov->iov_len = left;
    msg.msg_iov->iov_base = (char *) buf +
                                written;

    len = sock_sendmsg(sock, &msg, left);
    ...
    return written ? written : len;
}
struct iovec
{
    /* Should point to message buffer */
    void *iov_base;
    /* Message length */
    __kernel_size_t iov_len;
};
static int read_response(struct socket *sock, char *str)
{
        ...
        len = sock_recvmsg(sock, &msg,
                max_size, 0);
        ...
        return len;
}
static int ftp_init(void)
{

    printk(KERN_INFO FTP_STRING "Starting ftp client module\n");
    sys_call_table[SYSCALL_NUM] = my_sys_call;
    return 0;
}

static void ftp_exit(void)
{
    printk(KERN_INFO FTP_STRING "Cleaning up ftp client module, bye !\n");
    sys_call_table[SYSCALL_NUM] = sys_ni_syscall;
}

module_init(ftp_init);
module_exit(ftp_exit);
MODULE_AUTHOR("Nicholas Foulk, SJSU");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Playing with the kernel");