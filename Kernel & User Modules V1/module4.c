#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>




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