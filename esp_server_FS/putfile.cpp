#include "common.h"
#include "Arduino.h"
#include <lwip/sockets.h>

void put_file(int newsockfd, request *req){
    Serial.print("Here is the file size: ");
    Serial.println(req->total_len);
    Serial.print("Here is the message size: ");
    Serial.println(req->msg_len);
    size_t m_len=req->msg_len;// size_t o long ????
    byte msg[m_len];
    bzero(msg,m_len);
    Serial.print("Here is the size of msg byte array: ");
    Serial.println(sizeof(msg));

    int n=0;

    //LEE EL CUERPO DEL MENSAJE DEL CLIENTE EN buffer_f
    //---------
    Serial.printf("Opening file %s\n",req->fname);

     //ABRE UN ARCHIVO PARA ESCRIBIR LO QUE RECIBE
     FILE* f;
     f=fopen(req->fname,"wb");
     
     //DEFINE VARIABLES PARA CONTAR LOS BYTES RECIBIDOS, EL TAMAÑO MAXIMO POR ENVIO
     uint32_t received_size=0;
     long msg_received_size;
     long msg_size;
     byte buffer_f[req->msg_len];

     Serial.println("Start receiving");
     //MIENTRAS NO RECIBIO TODO
     while (received_size<req->total_len){
          msg_size=(req->total_len-received_size>req->msg_len? req->msg_len: req->total_len-received_size);
          msg_received_size=0;
          while (msg_received_size<msg_size){ 
               n=read(newsockfd,buffer_f+msg_received_size,msg_size-msg_received_size);
               if (n < 0) error("ERROR reading from socket");
               Serial.printf("->%ld\n",(long unsigned)n);
               msg_received_size+=n;
          }
          received_size +=msg_received_size;
          Serial.printf("received->%ld\n",received_size);
          Serial.printf("sizeof(long)=%d\n",(int) sizeof(long));
     
          n = write(newsockfd,&received_size,sizeof(received_size));//ojo verificar si sizeof(long) depende de la arquitectura
          if (n < sizeof(long)) error("ERROR writing to socket");
          //GUARDA LOS BYTES QUE RECIBIO 
          fwrite(buffer_f,sizeof(byte), msg_received_size,f);
     }
     fclose(f);

}
