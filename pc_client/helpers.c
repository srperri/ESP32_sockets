//#include "Arduino.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include "helpers.h"

void error(char *str){
    perror(str);
    exit(1);
}

void request_from_char_array(request *req, char* str){
    char *s = strtok(str, " ");
    strcpy(req->type,s);//ver si recibe argumentos erroneos
    s = strtok(NULL, " ");
    if (s!=NULL){
      strcpy(req->fname,"/spiffs/");
      strcat(req->fname,s);
      s = strtok(NULL, " ");
      if (s!=NULL){
        req->total_len=atol(s);
        s = strtok(NULL, " ");
        if (s!=NULL){
          req->msg_len=atol(s);
        }
      } 
    }
}
uint32_t min(uint32_t val_1, uint32_t val_2){
  return (val_1<=val_2?val_1:val_2);
}

double dwalltime(){
	double sec;
	struct timeval tv;

	gettimeofday(&tv,NULL);
	sec = tv.tv_sec + tv.tv_usec/1000000.0;
	return sec;
}
// void error(String str){
//     int len = str.length() + 1; 
//     char msg[len];
//     str.toCharArray(msg, len);
//     perror(msg);
//     exit(1);
// }