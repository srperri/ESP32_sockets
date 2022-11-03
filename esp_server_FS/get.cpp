#include "helpers.h"
#include "Arduino.h"
#include <lwip/sockets.h>

void get(int sockfd, serv_req *req)
{
    // ABRE EL ARCHIVO PARA LEER LO QUE VA A ENVIAR
    Serial.printf("Opening file %s\n", req->fname);
    FILE *f;
    f = fopen(full_path(req->fname), "rb");
    if (f == NULL)
        perror("Error opening file");

    // DEFINE VARIABLES PARA CONTAR LOS BYTES ENVIADOS, EL TAMAÑO DEL MENSAJE Y LA PORCION ENVIADA
    uint32_t sent_size = 0;
    uint32_t received_size = 0;
    uint32_t msg_size;
    void *buffer = malloc(req->msg_len);

    Serial.println("Start receiving");
    // MIENTRAS NO ENVIO TODO
    while (sent_size < req->total_len)
    {
        msg_size = (req->total_len - sent_size > req->msg_len ? req->msg_len : req->total_len - sent_size);
        fread(buffer, sizeof(byte), msg_size, f);
        fixed_write(sockfd, buffer, msg_size, 1);
        sent_size += msg_size;
        fixed_read(sockfd, &received_size, sizeof(received_size), 0); 
    }
    fclose(f);
    free(buffer);

    // int n = write(newsockfd,msg,sizeof(msg));

    Serial.printf("bytes sent: %lu\n", (unsigned long)sent_size);

    Serial.printf("\nfin\n");
}
