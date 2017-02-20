#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>

void error(char *msg)
{
	perror(msg);
	exit(0);
}

int main(int argc, char *argv[])
{
	int sockfd, portno, n;
	struct sockaddr_in serv_addr;
	struct hostent *server;
	char buffer[256];
    char cash[256];
   	if(argc < 3){
        fprintf(stderr, "usage %s hostname port\n", argv[0]);
        exit(0);
    }
    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0)
        error("ERROR opening socket");
    server = gethostbyname(argv[1]);
    if(server == NULL){
        fprintf(stderr, "ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
          (char *)&serv_addr.sin_addr.s_addr,
          server->h_length);
    serv_addr.sin_port = htons(portno);
    if(connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr))<0)
        error("ERROR connecting");
    printf("Welcome to CSUEB Bank:\n\n");
    printf("Please enter an account number: ");
    bzero(buffer, 256);
    fgets(buffer,255,stdin);
    n = write(sockfd, buffer, strlen(buffer));
    if (n<0)
        error("ERROR writing to socket");
    bzero(buffer, 256);
    n = read(sockfd, buffer, 255); //enter password
    if(n<0)
        error("ERROR reading from socket");
//**********Account verification******************
    if(n){
        printf("%s ", buffer);
        scanf("%s", buffer);
        
        n = write(sockfd, buffer, strlen(buffer));
        if (n<0)
            error("ERROR writing to socket");
    }else{
        printf("Account number is wrong! Try again.\n");
        close(sockfd);
        exit(0);
    }
//************Account Transaction: Balance, Widthraw and deposit*********
    bzero(buffer, 256);
    n = read(sockfd, buffer, 255); //transaction quote from server
    if(n<0) error("ERROR reading from socket");
    if(n){
        printf("%s ", buffer);
        scanf("%s %s", buffer, cash);

        if(!strcmp(buffer, "Balance")){
            n = write(sockfd, buffer, strlen(buffer)); //Client->Server
            if (n<0)
                error("ERROR writing to socket");
            bzero(buffer, 256);
            n = read(sockfd, buffer, 255); //Balance info from server
            if(n<0) error("ERROR reading from socket");
            printf(" %s ", buffer);
            printf("\n");
            close(sockfd);
        }
        else if(!strcmp(buffer, "Deposit"))
        {
            n = write(sockfd, buffer, strlen(buffer));//Client->Server
            if (n<0)
                error("ERROR writing to socket");
            n = write(sockfd, cash, strlen(cash));//Client->Server
            if (n<0)
                error("ERROR writing to socket");
            bzero(buffer, 256);
            n = read(sockfd, buffer, 255); // Balance info from server
            if(n<0) error("ERROR reading from socket");
            printf("%s\n", buffer);
            close(sockfd);
            
        }
        else if(!strcmp(buffer, "Withdraw"))
        {
            n = write(sockfd, buffer, strlen(buffer));//Client->Server
            if (n<0)
                error("ERROR writing to socket");
            bzero(buffer, 256);
            n = write(sockfd, cash, strlen(cash)); //Client->Server
            if (n<0)
                error("ERROR writing to socket");
            n = read(sockfd, buffer, 255); // Balance info from server
            if(n<0) error("ERROR reading from socket");
            printf("%s\n", buffer);
         
            
                close(sockfd);
        }
    }else{
        printf("Password entered for the account does not match! Try Again\n");
        close(sockfd);
        exit(0);
    }
    close(sockfd);
    return 0;
}
