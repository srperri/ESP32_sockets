#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include "Arduino.h"
#include <lwip/sockets.h>
#include "helpers.h"

void fixed_write(int sockfd, void *buff, size_t buff_size, char verbose)
{
    int n, count = 0;
    if (verbose)
        printf("writting ");
    while (count < buff_size)
    {
        n = write(sockfd, buff + count, buff_size - count);
        if (n < 0)
            perror("ERROR writing to socket");
        count += n;
        if (verbose)
            printf("->%ld", (long unsigned)n);
    }
    if (verbose)
        printf("\n");
}

void fixed_read(int sockfd, void *buff, size_t buff_size, char verbose)
{
    int n, count = 0;
    if (verbose)
        printf("reading ");
    while (count < buff_size)
    {
        n = read(sockfd, buff + count, buff_size - count);
        if (n < 0)
            perror("ERROR reading from socket");
        count += n;
        if (verbose)
            printf("->%ld", (long unsigned)n);
    }
    if (verbose)
        printf("\n");
}

void request_from_char_array(serv_req *req, char *str)
{
    char *s = strtok(str, " ");
    strcpy(req->op, s); // ver si recibe argumentos erroneos
    s = strtok(NULL, " ");
    if (s != NULL)
    {
        strcpy(req->fname, "/spiffs/");
        strcat(req->fname, s);
        s = strtok(NULL, " ");
        if (s != NULL)
        {
            req->total_len = atol(s);
            s = strtok(NULL, " ");
            if (s != NULL)
            {
                req->msg_len = atol(s);
            }
        }
    }
}
char *full_path(char *fname)
{
    static char full_path_fname[sizeof(SPIFFS_BASE_PATH) + 1 + FNAME_LEN];
    strcpy(full_path_fname, SPIFFS_BASE_PATH);
    strcat(full_path_fname, "/");
    strcat(full_path_fname, fname);
    return full_path_fname;
}
