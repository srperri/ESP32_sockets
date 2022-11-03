#include "helpers.h"
#include "Arduino.h"
#include <lwip/sockets.h>

void put(int sockfd, cline_req *cl_req)
{
    // ARMA EL REQUEST A ENVIAR AL SERVER CON LA OPERACION A REALIZAR
    serv_req serv_req;
    strcpy(serv_req.op, cl_req->op);
    strcpy(serv_req.fname, (strlen(cl_req->dst_fname) ? cl_req->dst_fname : cl_req->src_fname));
    serv_req.total_len = cl_req->total_len;
    serv_req.msg_len = (cl_req->msg_len ? cl_req->msg_len : cl_req->total_len);
    Serial.printf("\nserv_req:'%s %s %lu %lu'\n", serv_req.op, serv_req.fname, (unsigned long)serv_req.total_len, (unsigned long)serv_req.msg_len);

    // ENVIA EL REQUEST
    fixed_write(sockfd, &serv_req, sizeof(serv_req), 0);

    // ABRE EL ARCHIVO ORIGEN
    FILE *f;
    f = fopen(full_path(cl_req->src_fname), "rb");
    if (f == NULL)
        perror("Error opening file");

    // DEFINE VARIABLES PARA CONTAR LOS BYTES ENVIADOS, EL TAMAÑO DEL ENVIO ACTUAL, Y TIEMPOS
    uint32_t sent_size = 0;
    uint32_t msg_size;
    void *buffer = malloc(serv_req.msg_len);
    unsigned long tick, tack;

    Serial.printf("start sending \n");
    Serial.printf(" RTT - bytes \n");
    while (sent_size < serv_req.total_len)
    {
        msg_size = min(serv_req.msg_len, serv_req.total_len - sent_size);
        fread(buffer, sizeof(char), msg_size, f);
        tick = micros();
        fixed_write(sockfd, buffer, msg_size, 0);
        sent_size += msg_size;
        fixed_read(sockfd, &sent_size, sizeof(sent_size), 0);
        tack = micros();
        Serial.printf("%f - %ld \n", ((double)tack-tick)/1000, (unsigned long)msg_size);
    }
    free(buffer);
    Serial.printf("\nTotal sent bytes: %lu\n", (unsigned long)sent_size);
    fclose(f);
}
