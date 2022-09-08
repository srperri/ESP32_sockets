//#include "Arduino.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
// void error(String str){
//     int len = str.length() + 1; 
//     char msg[len];
//     str.toCharArray(msg, len);
//     perror(msg);
//     exit(1);
// }