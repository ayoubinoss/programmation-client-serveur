/* Usage : streamclient /usr/local/mysock "Hello World !" */
#include <sys/types.h> 
#include <sys/socket.h> 
#include <string.h>
#include <netdb.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>

#define TIME_OUT 10

void readWithTimeOut(int, char*, long, char*, long);
void readWithACK(int , char*, long);
void writeWithACK(int, char*, long);
int isTimeOut(int time) {
	return time>=TIME_OUT?1:0;
}


int main(int argc, char *argv[]) {
int sd; 

struct sockaddr_in serveraddr;
char response[256];
char request[256];
char id[256];
char destinataire[256];
int i;


//definition de la socket
sd=socket(AF_INET, SOCK_STREAM,0);
if(sd<0){
	perror("erreur de creation du socket");
	exit(1);
}

//connexion au serveur
serveraddr.sin_family=AF_INET;
serveraddr.sin_port=1501;
serveraddr.sin_addr.s_addr=INADDR_ANY;
if(connect(sd, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) <0){
	perror("erreur de connexion");
	exit(1);
}

printf("Donnez votre identifiant: ");
gets(id);
write(sd, id, sizeof(id));
printf("***********************************************\n");
printf("Voulez vous commencer une communication?0/1");
scanf("%d",&i);
getchar();
write(sd, &i, sizeof(i));
if (i){
	printf("Donnez le nom du destinataire:");
	gets(destinataire);
	write(sd, destinataire, sizeof(destinataire));
	do{
		printf("%s: ",id);
		gets(request);
		/*send a message and wait for a positif ACK*/
		writeWithACK(sd, request,sizeof(request));
		/*read a message and send an ACK for control*/
		readWithACK(sd, response,sizeof(response));
		printf("%s: %s\n", destinataire,response);
	} while(strcmp(request,"bye") != 0 || strcmp("bye",response) != 0);
}
else{
	char destinateur[256];
	read(sd, destinateur,sizeof(destinateur));
	do{
		readWithACK(sd, response,sizeof(response));
		printf("%s: %s\n",destinateur, response);
		printf("%s:",id);
		gets(request);
		writeWithACK(sd, request, sizeof(request));
	} while(strcmp(request,"bye") != 0 || strcmp("bye",response) != 0);
}	
close(sd);
}

void writeWithACK(int descriptor,char *request, long size) {
	char resp[256];
	do{
		write(descriptor, request, size);
		readWithTimeOut(descriptor, resp,sizeof(resp), request, size);
		if(strcmp(resp,"00000000") == 0) {
 			printf("[-]negatif ACK, resend message..");
		}
	 	else {
	 		printf("\n[+]positif ACK.\n");
	 	}
	}while(strcmp(resp, "00000000") == 0);
}
/**
*
*/
void readWithACK(int descriptor, char *response, long size) {
	char request[256];
	int ack,temps;
	do{
		read(descriptor, response, size);
		
		srand(time(0));
		ack = rand()%2;
		
		srand(time(0));
		temps = 3;//rand()%20;
		
		//printf("sleep %d seconds..\n",temps);
		//sleep(temps);
		
		if(isTimeOut(temps) == 0) {
			if(ack == 0) strcpy(request, "00000000");
			else strcpy(request,"11111111");
			
			write(descriptor, request, sizeof(request));
		}
		
	}while( ack == 0 || isTimeOut(temps)==1 );
}

void readWithTimeOut(int descriptor, char *response, long size, char* request, long sizeReq) {
	
	void handler(int signal) {
		printf("Timeout, retransmisiion de message..\n");
		write(descriptor, request, sizeReq);
		printf("wait for another ACK..\n");
		alarm(10);
	}
	signal(SIGALRM, handler);
	alarm(10);
	read(descriptor, response, size);
	alarm(0);

}
