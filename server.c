#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>

void error(char *msg)
{
    perror(msg);
    exit(1);
}

struct bankaccount
{
    int acctnum;
    char password[8];
    double balance;
};

int main(int argc, char *argv[])
{
    int sockfd, newsockfd, portno, clilen;
    char buffer[256];
    struct sockaddr_in serv_addr, cli_addr;
    int n;
    int accountnumber;
    char transaction[256];
    double money;
    struct bankaccount account[3]; // 3 test accounts
    
    //1st account holder info
    account[0].acctnum = 1234;
    strcpy(account[0].password, "abcd");
    account[0].balance = 122.5;
    //2nd account holder info
    account[1].acctnum = 4567;
    strcpy(account[1].password, "efgh");
    account[1].balance = 133.5;
    //3rd account holder info
    account[2].acctnum = 8910;
    strcpy(account[2].password, "ijkl");
    account[2].balance = 144.5;
   
    if (argc < 2) {
        fprintf(stderr,"ERROR, no port provided\n");
        exit(1);
    }
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");
    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    if (bind(sockfd, (struct sockaddr *) &serv_addr,
             sizeof(serv_addr)) < 0)
        error("ERROR on binding");
    listen(sockfd,5);
    clilen = sizeof(cli_addr);
    
    
    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
    if (newsockfd < 0)
        error("ERROR on accept");
    bzero(buffer,256);
    n = read(newsockfd,buffer,255);
    if (n < 0) error("ERROR reading from socket");
    accountnumber = atoi(buffer);
   
    for (int i = 0; i<3; i++) {
        if(account[i].acctnum == accountnumber){
            n = write(newsockfd,"Please enter password: ",36);
            if (n < 0) error("ERROR writing to socket");
            bzero(buffer,256);
            n = read(newsockfd,buffer,255);//password info
            
            if(!(strcmp(account[i].password, buffer))){
                
                n = write(newsockfd,"Please enter a transaction: ", 28);
                if (n < 0) error("ERROR writing to socket");
                bzero(buffer,256);
                n = read(newsockfd,buffer,255);//transaction info from client
               

                if(!strcmp(buffer, "Balance"))
                {
                    
                    bzero(buffer,7);
                    sprintf(buffer, "Total Balance: %.2f", account[i].balance);//copy to buffer
                    n = write(newsockfd, buffer, 255);
                    if (n < 0) error("ERROR writing to socket");
                    close(newsockfd);
                    
                }
                else if(!strcmp(buffer, "Deposit"))
                {
                    
                    bzero(buffer,256); // Read cash from client
                    n = read(newsockfd,buffer,255);
                    
                    account[i].balance += atof(buffer);
                    bzero(buffer,256);
                    sprintf(buffer, "After Deposit, Total Balance: %.2f", account[i].balance);//copy to buffer
                    n = write(newsockfd, buffer, 255);
                    if (n < 0) error("ERROR writing to socket");
                    
                    close(newsockfd);
                }
                else if(!strcmp(buffer, "Withdraw"))
                {
                    printf("Withdrawing\n");
                    
                    bzero(buffer,256); // Read cash from client
                    n = read(newsockfd,buffer,255);
                    printf("%s\n", buffer);
                    money = atof(buffer);
                    if(account[i].balance > money)
                    {
                        account[i].balance -= money;
                        bzero(buffer,256);
                        sprintf(buffer, "After Withdrawal, Total Balance: %.2f", account[i].balance);//copy to buffer
                        n = write(newsockfd, buffer, 255);
                        if (n < 0) error("ERROR writing to socket");
                        close(newsockfd);
                    }
                    else
                    {
                        n = write(newsockfd,"Sorry, transaction failed due to insufficient funds", 70);
                        if (n < 0) error("ERROR writing to socket");
                    close(newsockfd);
                    }
                }
            }
        }
    }
    printf("");
    close(newsockfd);
    
    return 0;
}
