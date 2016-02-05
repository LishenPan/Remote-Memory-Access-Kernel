
//#include <stdio.h>
//#include <string.h>
//#include <sys/mman.h>
//#include <unistd.h>
#include <linux/moduleparam.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

//#define MESSAGE_LENGTH 30
//#define ERROR -1
//#define SLEEP_TIME 1

/*typedef struct
{
	char MSG[MESSAGE_LENGTH];
	int count;
} messageObj;
void prod(messageObj *state);
void cons(messageObj *state);
void waitforchild(pid_t*);
pid_t forkChild(void (*func)(messageObj *), messageObj* state);
messageObj* createMMAP(size_t size);
void deleteMMAP(void*); 

int main()
{
	messageObj* state = createMMAP(sizeof(messageObj));
	state->count =0;
	pid_t childpid[2];
	childpid[0]= forkChild(prod, state);
	childpid[1]= forkChild(cons, state);

	waitforchild(childpid);

	deleteMMAP(state);
	exit(EXIT_SUCESS);
}

messageObj* createMMAP(size_t size)
{
	void* addr = 0;           //address of where it starts, if its 0 kernel will figure out where it goes
	int protections = PROT_READ| PROT_WRITE;
	int flags= MAP_SHARED|MAP_ANONYMOUS;
	int fd = -1;
	off_t offset = 0;

	messageObj* state= mmap(addr,size,protections,flags, fd, offset);

	if(( void *) ERROR == state)
	{
		perror("error with mmapping process");
		exit(EXIT_FAILURE);
	}
	return state;
}
void deleteMMAP(void* addr)
{
	//delete memory map
	if(ERROR == munmap(addr,sizeof(messageObj)))
	{
		perror("Error with deleting mmap");
		exit(EXIT_FAILURE);
	}
}
pid_t forkChild(void (*function)(meesageObj *),messageObj* state)
{
	pid_t childpid1;
	switch(childpid1 = fork())
	{
		case ERROR:
			perror("There was a forking error");
			exit(EXIT_FAILURE);
		case 0:
			(*function)(state);
		default:
			return childpid1;
	}
}

void waitforchild(pid_t* childpids)
{
	int status;
	while(ERROR < wait(&status))
	{
		if(!WIFEXITED(status))
		{
			kill(childpids[0],SIGKILL);
			kill(childpids[1],SIGKILL);
			break;
		}
	}
}
void prod(messageObj* state)
{
	while(1)
	{
		while(MESSAGE_LENGTH == state->count)
		{
			sleep(SLEEP_TIME);   //bad way to do this
		}
		char c = fgetc(stdin); //reads in a char
		state->MSG[state->count] = c;
		state->count++;
		if(EOF == c)
		{
			state->count = MESSAGE_LENGTH;
			exit(EXIT_SUCESS);
		}
	}
	return;
}
void cons(messageObj* state)
{
	int localcount = 0;
	while(1)
	{
        while(MESSAGE_LENGTH > state->count)
        {
        	sleep(SLEEP_TIME);    //bad way to do this
        }
        char c = state->MSG[localcount];
        if(EOF == c)
        {
        	localcount = 0;
        	state->count = 0;
        }
	}
	return;
}*/

int it[4] = {0,0,0,0};


module_param_array(it,int,NULL, S_IRUSR | S_IWUSR);

void display(void)
{
	printk(KERN_ALERT "parameter = %d", it[0]);
	printk(KERN_ALERT "parameter = %d", it[1]);
	printk(KERN_ALERT "parameter = %d", it[2]);
	printk(KERN_ALERT "parameter = %d", it[3]);
	
}
static int hello_init(void)
{
	printk(KERN_ALERT "Hi everyone My name is nick!");
	display();
	return 0; 
}
static void hello_exit(void)
{
	printk(KERN_ALERT "The struggle is fucken real!");
	return;
}

module_init(hello_init);
module_exit(hello_exit);
MODULE_LICENSE("GPL"); //general public license