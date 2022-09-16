#include "helpers.h"
#include "Arduino.h"
#include <lwip/sockets.h>

void put(int sockfd, serv_req *req)
{
    // ABRE UN ARCHIVO PARA ESCRIBIR LO QUE RECIBE
    Serial.printf("Opening file %s\n", req->fname);
    FILE *f;
    f = fopen(full_path(req->fname), "wb");
    if (f == NULL)
        perror("Error opening file");

    // DEFINE VARIABLES PARA CONTAR LOS BYTES RECIBIDOS, EL TAMAÑO MAXIMO POR ENVIO
    uint32_t received_size = 0;
    uint32_t msg_size;
    void *buffer = malloc(req->msg_len);

    Serial.println("Start sending");
    // MIENTRAS NO RECIBIO TODO
    while (received_size < req->total_len)
    {
        msg_size = (req->total_len - received_size > req->msg_len ? req->msg_len : req->total_len - received_size);
        fixed_read(sockfd, buffer, msg_size, 1);
        received_size += msg_size;
        fixed_write(sockfd, &received_size, sizeof(received_size), 0); // ojo verificar si sizeof(long) depende de la arquitectura
        fwrite(buffer, sizeof(byte), msg_size, f);
    }
    fclose(f);
    free(buffer);
}
