#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/time.h>
#include <sys/msg.h>
#include <errno.h>


#define SIG_DT 44 
#define MSG_SIZE 256
#define SERVER "192.168.100.1"


int controller_no;

int sockfd,portno,n,sockfd_udp,portno_udp;
struct sockaddr_in serv_addr,serv_addr_udp;
struct hostent *server,*server_udp;
char buffer[256],buffer_udp[256];
//char * dataptr;
char data[256];
static char alive_msg[256];

struct itimerval old;
struct itimerval new;

int controller_no;
int total_spaces_in_level;
static int total_free_in_level;
static int total_occupied_in_level;
int total_zones;
//int *total_spaces_in_zone;


int total_detectors;
int entry_detectors[10][10];
int exit_detectors[10][10];


static struct zone_data{
	int total_space;
	int total_free;
	int total_occupied;
	int total_entry;
	int total_exit;
}zone_data_arr[10];

struct zone_data * zone_data_ptr=zone_data_arr;


static int a[20];
int *detector_ptr[20];

struct ipc_msg_st ipc_data;
int msgid;
char ipc_msg_buffer[256];
struct ipc_msg_st
{
	long int ipc_msg_type;
	char ipc_msg[MSG_SIZE];
};

void send_data_to_udp_server(char arr[256])
{
 sockfd_udp = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
        if (socket <0)
        error("Error opening socket");
//	printf("UDP socket created %d\n",sockfd_udp);
        if(server == NULL)
        {
                fprintf(stderr,"ERROR, no such host\n");
                exit(0);
        }
//      bzero((char*) & serv_addr,sizeof(serv_addr_udp));
        memset ((char*)&serv_addr_udp,0,sizeof(serv_addr_udp));
        serv_addr_udp.sin_family = AF_INET;
//      bcopy((char *)server_udp->h_addr,
  //      (char *)&serv_addr_udp.sin_addr.s_addr,
   //     server_udp->h_length);
        serv_addr_udp.sin_port = htons(portno_udp);

        if(inet_aton(SERVER,&serv_addr_udp.sin_addr) == 0)
        {
                fprintf(stderr, "inet_aton() failed\n");
                exit(1);
        }
 sendto(sockfd_udp,arr,strlen(arr),0,(struct sockaddr*)&serv_addr_udp,sizeof(serv_addr_udp));
        bzero(buffer_udp,256);
        close(sockfd_udp);



}

int send_data_ipc(char ipc_buffer[256])
	{
	ipc_data.ipc_msg_type=1;
	strcpy(ipc_data.ipc_msg,ipc_buffer);
	if(msgsnd(msgid,(void*)&ipc_data,MSG_SIZE,0) == -1)
		{
			fprintf(stderr,"msgsnd failed\n");
			exit(EXIT_FAILURE);
		}
	}
int update_zone(int zone_number,char* operation)
{
	if ((*operation) =='+')
	{
	(zone_data_ptr+zone_number)->total_occupied++;
	total_occupied_in_level++;
	}

	else
	{
	(zone_data_ptr+zone_number)->total_occupied--;
	total_occupied_in_level--;
	}

	(zone_data_ptr+zone_number)->total_free = ((zone_data_ptr+zone_number)->total_space)-((zone_data_ptr+zone_number)->total_occupied);

	if((zone_data_ptr+zone_number)->total_free <= 0)
	return -1;

	else if ((zone_data_ptr+zone_number)->total_free >= (zone_data_ptr+zone_number)->total_space)
	return 0;

	else
	return ((zone_data_ptr+zone_number)->total_free); 
	
}


int update_level(void)
{

	total_free_in_level= total_spaces_in_level-total_occupied_in_level;
	
	if (total_free_in_level <=0)	
	return -1;
	
	else if (total_free_in_level >= total_spaces_in_level)
	return 0;
	
	else 
	return (total_free_in_level);

}

void send_data(char arr[256])

{
	
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0)
        error("ERROR opening socket");

   	if (server == NULL)
	{
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    	}
        bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy((char *)server->h_addr,
	(char *)&serv_addr.sin_addr.s_addr,
	server->h_length);
	serv_addr.sin_port = htons(portno);
	
	    	
	if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
        error("ERROR connecting");
    	n = write(sockfd,arr,strlen(arr));
  
  	if (n < 0)
        error("ERROR writing to socket");

    	bzero(buffer,256);
    	n = read(sockfd,buffer,255);

    	if (n < 0)
        error("ERROR reading from socket");

//    	printf("%s\n",buffer);
    	close(sockfd);


}

void send_zone(int zone_number,int free_space)
{

	if(free_space == -1)
	{
		snprintf(data,256,"L-%dZ-%d  Full",controller_no,(zone_number+1));
		//printf("Zone%d is Full\n",(zone_number+1));
	//	send_data(data);
		send_data_ipc(data);
	}
       	else if(free_space == 0)
	{
		snprintf(data,256,"L-%dZ-%d Empty",controller_no,(zone_number+1));
		//printf("Zone%d is Empty\n",(zone_number+1));
	//	send_data(data);
		send_data_ipc(data);
	}
	else
	{
		snprintf(data,256,"L-%dZ-%d free space-%d",controller_no,(zone_number+1),free_space);
	//	printf("%d free space in zone %d\n", free_space,(zone_number+1));
	//	send_data(data);
		send_data_ipc(data);
	}
}

void send_level(int free_space)
{
	if(free_space==-1)
	{
		snprintf(data,256,"L-%d Full",controller_no);
		//printf("Level %d Full\n",controller_no);
	//	send_data(data);
		send_data_ipc(data);
	}
	else if(free_space == 0)
	{
		snprintf(data,256,"L-%d Empty",controller_no);
		//printf("Level %d is Empty\n",controller_no);
	//	send_data(data);
		send_data_ipc(data);
	}
	else
	{
		snprintf(data,256,"L-%d free space-%d",controller_no,free_space);
		//printf("%d free space in Level %d\n", free_space,controller_no);
	//	send_data(data);
		send_data_ipc(data);
	}

}

void catch_alarm(int sig)
{
	send_data_to_udp_server(alive_msg);
}

void receiveData(int n, siginfo_t *info, void *unused) {
        
	int detector,i,j,zone_free,level_free;
	int signal_data = info->si_int;
	*detector_ptr[(signal_data-1)]+=1;
	
	//printf("received signal %d\n",signal_data);
	if(signal_data%2!=0)
	{	
		detector=(signal_data+1)/2;
		
//		printf("detector%d forward \n",detector);		
		for(i=0;i<total_zones;i++)
		{
			for(j=0;j<(zone_data_ptr+i)->total_entry;j++)
			{	
				if(detector == entry_detectors[i][j])
					{
//					printf("detector%d is entry%d for zone %d\n",detector,(j+1),(i+1)); 
					zone_free = update_zone(i,"+");
					//printf ("zone_free value %d\n",zone_free);
					
					send_zone(i,zone_free);
					}

			}
		}
		
		for(i=0;i<total_zones;i++)
		{
			for(j=0;j<(zone_data_ptr+i)->total_exit;j++)
			{
				if(detector == exit_detectors[i][j])
					{
					zone_free = update_zone(i,"-");
					send_zone(i,zone_free);
					}
				
			}
		
		}

	}
	else
	{
		detector=signal_data/2;
		//printf("signal_data=%d\n",signal_data);
		//printf("detector%d reverse \n",detector);
	      for(i=0;i<total_zones;i++)
                {
                        for(j=0;j<(zone_data_ptr+i)->total_entry;j++)
                        {
                                if(detector == entry_detectors[i][j])
                                        {
                                        zone_free = update_zone(i,"-");
                                       send_zone(i,zone_free);
                                        }
			
                          //    	level_free =  update_level();
			//	send_level(level_free);
                        }

                }
		
		for(i=0;i<total_zones;i++)
                {
                        for(j=0;j<(zone_data_ptr+i)->total_exit;j++)
                        {
                                if(detector == exit_detectors[i][j])
                                        {
                                        zone_free = update_zone(i,"+");
					send_zone(i,zone_free);
                                        }
                        
			}
                }

	}					
level_free = update_level();
send_level(level_free);
}




int main ( int argc, char *argv[] )
{
	
	int j,k=0;
	portno = atoi(argv[3]);
	portno_udp = atoi(argv[3]);
	server=gethostbyname(argv[2]);

	int arg_count=argc;
	int configfd;
	char buf[10];
	int i;
	
	controller_no=atoi(argv[4]);
//	printf("controller_no %d \n",controller_no);
	total_spaces_in_level=atoi(argv[5]);
//	printf("total spaces %d \n",total_spaces_in_level);
	total_zones=atoi(argv[6]);
//	printf("total zones = %d\n",total_zones);
	total_detectors=atoi(argv[7]);
//	printf("total detectors =%d\n",total_detectors);
	msgid = msgget((key_t)1234,0666|IPC_CREAT); 	
	if(msgid == -1)
	{
		fprintf(stderr,"failed to create msg queue\n");
		exit(EXIT_FAILURE);
	}

	for(i=0;i<total_zones;i++)
	{
		zone_data_ptr->total_space = atoi(argv[8+k]);k++;
		zone_data_ptr->total_entry = atoi(argv[8+k]);k++;
		zone_data_ptr->total_exit  = atoi(argv[8+k]);k++;
		for(j=0;j<zone_data_ptr->total_entry;j++)
			entry_detectors[i][j]=atoi(argv[8+k]);k++;
		for(j=0;j<zone_data_ptr->total_exit;j++)
			exit_detectors[i][j]=atoi(argv[8+k]);k++;		
		zone_data_ptr++;
}
		zone_data_ptr-=total_zones;

//	printf("Test\n");		
	for(i=0;i<(total_detectors*2);i++)
	detector_ptr[i]=&a[i];

//printf("controller No = %d\n Total Spaces In Level = %d\n Total zones = %d\n Total detectors =%d \n", controller_no,total_spaces_in_level,total_zones,total_detectors);

	
	/* setup the signal handler for SIG_DT
 	 * SA_SIGINFO -> we want the signal handler function with 3 arguments
 	 */

	struct sigaction sig;
	sig.sa_sigaction = receiveData;
	sig.sa_flags = SA_SIGINFO;
	//sig.sa_flags = SA_ONSTACK;
	sigaction(SIG_DT, &sig, NULL);

	

	/* kernel needs to know our pid to be able to send us a signal ->
 	 * we use debugfs for this -> do not forget to mount the debugfs!
 	 */
	configfd = open("/sys/kernel/debug/signalconfpid", O_WRONLY);
	if(configfd < 0) {
		perror("open");
		return -1;
	}
	sprintf(buf, "%i", getpid());
	if (write(configfd, buf, strlen(buf) + 1) < 0) {
		perror("fwrite"); 
		return -1;
	}
snprintf(alive_msg,256,"Controller-%d Alive",controller_no);
signal (SIGALRM, catch_alarm);
  new.it_interval.tv_sec = 5; 
  new.it_interval.tv_usec = 0; 
  new.it_value.tv_sec = 5;
  new.it_value.tv_usec = 0;
   
  old.it_interval.tv_sec = 0;
  old.it_interval.tv_usec = 0;
  old.it_value.tv_sec = 0;
  old.it_value.tv_usec = 0;
   
  if (setitimer (ITIMER_REAL, &new, &old) < 0)
      printf("timer init failed\n");
  //else
    //  printf("timer init succeeded\n");
while(1)
{

sleep(1000);
}

	return 0;

}

