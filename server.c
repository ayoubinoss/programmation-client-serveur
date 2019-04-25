/* Usage : streamserveur /usr/local/mysock */ 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <string.h>
#include <netdb.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>

#define TIME_OUT 10

	int sd, ns, nb,nl; 
	char request[256]; 
	char response[256];
	
	char client[10][256]; 
	int  cssd[10];
	int occupe[10];
	int  nbrCli=-1;

	char secondMsg[256];
	int descriptor;
void handler(int signal) {
	printf("retranssmition de la trame..\n");
	write(descriptor, secondMsg, sizeof(secondMsg));
	printf("wait for another ACK..\n");
	alarm(20);
}

int extractDescriptor(char destinataire[256]){
for(int i=0;i<=nbrCli;i++){
 if (strcmp(destinataire,client[i])==0)
          return cssd[i];	
	}
}

int estOccupe(char destinataire[256]){
for(int i=0;i<=nbrCli;i++){
 if (strcmp(destinataire,client[i])==0)
          return occupe[i];	
	}
 return 0;
}

void liberer(char id[256]){
for(int i=0;i<=nbrCli;i++){
 if (strcmp(id,client[i])==0)
           occupe[i]=0;	
	}
}

void occuper(char id[256]){
for(int i=0;i<=nbrCli;i++){
 if (strcmp(id,client[i])==0)
           occupe[i]=1;	
	}
}

void readWithACK(int , char *, long);
void writeWithACK(int , char* , long);
void readWithTimeOut(int , char* , long, char *, long);

int main(int argc, char *argv[]){


//definition du socket du serveur 		
struct sockaddr_in serveraddr; 
sd=socket(AF_INET, SOCK_STREAM,0);
if (sd<0){
	perror("erreur de creation du socket");
	exit(1);
	}
serveraddr.sin_family=AF_INET;
serveraddr.sin_port=1501;
serveraddr.sin_addr.s_addr=INADDR_ANY;
nb=bind(sd, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
if(nb<0){
	perror("erreur d'attachement d'adresse");
 	exit(1);
 	}

nl=listen(sd,10);
if (nl<0){
	perror("erreur d'ecoute");
	exit(1);
	}
	/*char *str;
	char string[256];
	printf("str=%d\nstring=%d\n",sizeof(str), sizeof(string));
	*/
while(1)
{
	ns=accept(sd,0,0);
	printf("Nouvelle demande de connexion... ");
	nbrCli++;
	cssd[nbrCli]=ns;
	char id[256];
	read(ns, id, sizeof(id));
	printf("le nom de l'utilisateur est = %s \n",id);
	strcpy(client[nbrCli],id);
	int pid=fork();	//lancement d'un sous process
	if(pid==0){  
		//signal(SIGALRM, handler);
		int chat;	
		read(ns, &chat, sizeof(chat));
    	if(chat){	
			char destinataire[256];			
            read(ns, destinataire, sizeof(destinataire));
            if (!estOccupe(destinataire))
            {
            	occuper(id);
            	occuper(destinataire);
            	printf("destinataire= %s est disponible pour chatter avec %s  \n",destinataire,id);
            	int csd;
            	csd=extractDescriptor(destinataire);
            	write(csd,id,sizeof(id));
            	do{
		 			readWithACK(ns, request, sizeof(request));
		 			/*[BEGIN] information for handler function*/
		 			strcpy(secondMsg, request);
		 			descriptor = csd;
		 			/*[END]*/
		 			writeWithACK(csd,request, sizeof(request));
		 			printf("client ==>destinataire= %s \n",request);
					readWithACK(csd, response, sizeof(response));
		 			writeWithACK(ns,response, sizeof(response));
		 			printf("destinataire==>client= %s \n",response);
	  			 }while(strcmp(request,"bye") != 0 || strcmp(response,"bye") != 0);
	  	   	}
	  	   	else{
	  	   	     liberer(id);
	  	   	     printf("veuillez essayer plustard");
	  	   	}
	  	   	close(ns);
	    }
      }
 } 
close(sd);
}

int isTimeOut(int time) {
	return time > TIME_OUT ? 1 : 0;
}

void readWithACK(int descriptor, char *res, long size){
	int ack,temps;
	char req[256];
	do{
		read(descriptor, res, size);
		
		/*change random function with CRC function*/
		srand(time(0));
		ack = rand()%2;
		srand(time(0));
		temps = rand()%20;
		/*printf("sleep %d seconds.\n",temps);
		sleep(temps);*/

		if(isTimeOut(temps) == 0) {
			if(ack == 0) strcpy(req, "00000000");
			else strcpy(req,"11111111");
			write(descriptor, req, sizeof(req));
		}
		
	}while(ack == 0 || isTimeOut(temps) == 1);
}
void writeWithACK(int descriptor, char* req,long size) {
	char resp[256];
	do{
		
		write(descriptor, req, size);
		/**/
		
		readWithTimeOut(descriptor, resp,sizeof(resp), req, size);
	
		if(strcmp(resp,"00000000") == 0) 
 			printf("[-]negatif ACK, resend message..\n");
	 	else 
	 		printf("[+]positif ACK.\n");
	}while(strcmp(resp, "00000000") == 0);
}

void readWithTimeOut(int descriptor, char *res, long size,char *req, long sizeReq) {
	
	void handler(int signal) {
		printf("Timeout, retransmisiion de message..\n");
		write(descriptor, req, sizeReq);
		printf("wait for another ACK..\n");
		alarm(10);
	}
	signal(SIGALRM, handler);
	alarm(10);
	read(descriptor, res, size);
	alarm(0);

}
