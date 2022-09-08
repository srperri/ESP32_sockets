#include "common.h"
#include "Arduino.h"
#include <lwip/sockets.h>
void get_file(int newsockfd, request* req){
    Serial.printf("fname : %s \n",req->fname);
    FILE* f;
    if ((f = fopen(req->fname, "rb")) == NULL){
      Serial.println("Failed to open file for reading");
      perror("Error opening file");
      return;
    }
    byte msg[req->msg_len];
    bzero(msg,req->msg_len);
    fread(msg,1, sizeof(msg),f);
    fclose(f);
    Serial.printf("mensaje a enviar:\n");
    //Serial.println(msg);
    Serial.println("----fin");
    int msg_sent_size=0;
    int n=0;
    while (n>=0 && msg_sent_size < req->msg_len){ 
         n = write(newsockfd,msg+msg_sent_size,req->msg_len-msg_sent_size);
         if (n < 0) error("ERROR reading from socket");
         printf("->%ld\n",(long unsigned)n);
         msg_sent_size+=n;
    }
    //int n = write(newsockfd,msg,sizeof(msg));

    Serial.printf("bytes sent: %lu\n",(unsigned long)msg_sent_size);
    
    Serial.printf("\nfin\n");
}
