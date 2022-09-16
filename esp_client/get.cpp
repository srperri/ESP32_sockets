#include "helpers.h"
#include "Arduino.h"
#include <lwip/sockets.h>

void get(int sockfd, cline_req *cl_req)
{
    // ARMA EL REQUEST A ENVIAR AL SERVER CON LA OPERACION A REALIZAR
    serv_req serv_req;
    strcpy(serv_req.op, cl_req->op);
    strcpy(serv_req.fname, cl_req->src_fname);
    serv_req.total_len = cl_req->total_len;
    serv_req.msg_len = (cl_req->msg_len ? cl_req->msg_len : cl_req->total_len);
    Serial.printf("\nserv_req:'%s %s %lu %lu'\n", serv_req.op, serv_req.fname, (unsigned long)serv_req.total_len, (unsigned long)serv_req.msg_len);

    // ENVIA EL REQUEST
    fixed_write(sockfd, &serv_req, sizeof(serv_req), 0);

    // ABRE EL ARCHIVO PARAMETRO #6:dst_filename
    FILE *f;
    f = fopen(full_path(strlen(cl_req->dst_fname) ? cl_req->dst_fname : cl_req->src_fname), "wb");
    if (f == NULL)
        error("Error opening file");

    // DEFINE VARIABLES PARA CONTAR LOS BYTES ENVIADOS, EL TAMAÑO DEL ENVIO ACTUAL, Y TIEMPOS
    uint32_t received_size = 0;
    uint32_t msg_size;
    void *buffer = malloc(serv_req.msg_len);

    Serial.printf("start receiving \n");
    while (received_size < serv_req.total_len)
    {
        msg_size = min(serv_req.msg_len, serv_req.total_len - received_size);
        fixed_read(sockfd, buffer, msg_size, 1);
        received_size += msg_size;
        fixed_write(sockfd, &received_size, sizeof(received_size), 0);
        fwrite(buffer, sizeof(char), msg_size, f);
    }
    free(buffer);
    Serial.printf("\nTotal received bytes: %lu\n", (unsigned long)received_size);
    fclose(f);
}
