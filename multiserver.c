#include <stdio.h>
#include <stdlib.h>  	      // strlen function
#include <string.h>           // strlen function
#include <sys/socket.h>       // socket system calls
#include <unistd.h>           // write system calls
#include <arpa/inet.h>        // inet_addr structure
#include <fcntl.h>            // O_RDONLY flag
#include <pthread.h>          // Threads, link with lpthread
#include <semaphore.h>        // Semaphore init(), sem_wait() and sem_post()

#define BUF_LEN 4096		// Buffer Length constant size

int *new_sock;			// Pointer variable to Client socket

pthread_t t_producer;		// Producer Thread Variable
pthread_t t_consumer;		// Consumer Thread Variable

sem_t buf_mutex, empty_count, fill_count;	// Mutex variables
						// buf_mutex checks buffer LOCKED(0) or UNLOCKED(1)
						// empty_count checks buffer EMPTY(0) or NOTEMPTY(1)
						// fill_count checks buffer FULL(0) or NOTFULL(1)

char buff[BUF_LEN];				// Character buffer array
int server_socket, client_sock, c;		// Client and Server Socket variable

struct sockaddr_in server, client;		// Address structure for Client and Server


int produce()
{
  // TODO:  Use open() system call to open the file and write it to a character buffer "buff"
 	char fnamebuff[256];
	int fbytes = read(client_sock,fnamebuff, 256);
	printf("File requested: %s\n",fnamebuff);
	memset(buff, 0, sizeof(buff));
	int opening = open(fnamebuff,O_RDONLY);
	if (opening < 0){
		printf("Could not open the requested file. Aborting!\n");
		close(client_sock);
		return -1;
	}
	else{
		printf("Reading file...\n");
		while (read(opening,buff,4096) > 0){
			//int wp = write(buff,readbuff,256);
			printf("Reading file into buffer!\n");
		}
	}		
}

int consume()
{
    sleep(3);

    int sock = *(int *)new_sock;
    write(sock, buff, BUF_LEN);
    printf("In consumer\n %s",buff);
    //close(sock);

    // TODO:  Read from the buffer written in produce() function and send it to client using write() system call, using the "sock" file descriptor.
}

void* producer(void *args)
{
	printf("Producer says LULW\n");
	// TODO:  Use the three mutex variables, use sem_wait() and sem_post()
	//sem_wait(&empty_count);
	sem_wait(&buf_mutex);
	sem_wait(&empty_count);
	//sem_wait(&fill_count);
	printf("Producer says KEKW\n");
	produce();
	//sem_post(&empty_count);
	sem_post(&fill_count);
	sem_post(&buf_mutex);
	return NULL;
}


void* consumer(void *args)
{
	// TODO:  Use the three mutex variables, use sem_wait() and sem_post()
	printf("Consumer says LULW\n");
	//sem_wait(&fill_count);
  	sem_wait(&buf_mutex);
    sem_wait(&fill_count);
    printf("Consumer says KEKW\n");
	consume();
    sem_post(&empty_count);
    sem_post(&buf_mutex);
    //sem_post(&fill_count);

	return NULL;
}

int main(void) 
{
	server_socket = socket(AF_INET, SOCK_STREAM, 0);    	// Create socket

	if (server_socket == -1) 				// Check create Socket ERROR=-1
    	{
	      	printf("\nCould not create Socket\n");
    	}

        puts("\nSocket created\n");				// Print Socket Created Sucessfully	

		// Prepare the sockaddr_in structure

        server.sin_family = AF_INET;				// Intialize Address Family
        server.sin_addr.s_addr = INADDR_ANY;			// Intialize Address Type
        server.sin_port = htons(5050);				//Port No :5050

		// Bind socket with sockaddr_in structure

        if(bind(server_socket, (struct sockaddr *)&server, sizeof(server))<0) 
        {
           perror("\nBind Failed. Error\n");	// Print the bind ERROR=-1 message

           return 1;
        }

        puts("\nBind Done\n");			// Bind Successful

        for (size_t i = 0; i < BUF_LEN; i++) 	// Intialize Buffer with 0
        {
           buff[i] = 0;
        }

        listen(server_socket, 3);		// Server Listens, Backlog=3 Clients

        c = sizeof(struct sockaddr_in);

        // Accept and incoming connection

        puts("Waiting for incoming connections...");

        client_sock=accept(server_socket,(struct sockaddr *)&client,(socklen_t *)&c); // Server accepts connection

        new_sock = malloc(1);			// Allocate memory to pointer variable to Client socket

       *new_sock = client_sock;			// Assigning Client socket variable to newly created pointer variable.

       int err;					// Error variable

       sem_init(&buf_mutex, 0, 1);		// Intialize buf_mutex to 1(BUFFER UNLOCKED/FREE), 0 means Threads
       sem_init(&fill_count, 0, 0);		// Intialize fill_count to 0 (BUFFER FULL) 1 -> BUFF empty
       //sem_init(&empty_count, 0, BUF_LEN); 	// Intialize empty_count to BUF_LEN=4096 (EMPTY)
       sem_init(&empty_count, 0, 1);

    // TODO: Create 2 threads t_producer and t_consumer, with producer() and consumer() as their callback functions.
    // TODO: Join the 2 threads
    	int prod, cons;
    	prod = pthread_create(&t_producer, NULL, producer, NULL);
    	cons = pthread_create(&t_consumer, NULL, consumer, NULL);
    	if(prod){
         	fprintf(stderr,"Error - Producer pthread_create() return code: %d\n",prod);
         	exit(EXIT_FAILURE);
     	}
     	else{
     		printf("Producer thread created!\n");
     	}
     	if(cons){
         	fprintf(stderr,"Error - Consumer pthread_create() return code: %d\n",cons);
         	exit(EXIT_FAILURE);
     	}
     	else{
     		printf("Consumer thread created!\n");
     	}
     	
     	pthread_join(t_producer,NULL);
     	pthread_join(t_consumer,NULL);
     	printf("Threads have joined!\n");
		close(server_socket); 		// Server Wait/Join for Producer Thread to complete
        close(client_sock);		// Server Wait/Join for Consumer Thread to complete

	return 0;
}

