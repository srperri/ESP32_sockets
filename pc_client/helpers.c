#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include "helpers.h"

void error(char *str)
{
    perror(str);
    exit(1);
}

void fixed_write(int sockfd, const void *buff, size_t buff_size, char verbose)
{
    int n, count = 0;
    if (verbose)
        printf("writting ");
    while (count < buff_size)
    {
        n = write(sockfd, buff + count, buff_size - count);
        if (n < 0)
            error("ERROR writing to socket");
        count += n;
        if (verbose)
            printf("->%ld", (long unsigned)n);
    }
    if (verbose)
        printf("\n");
}

void fixed_read(int sockfd, const void *buff, size_t buff_size, char verbose)
{
    int n, count = 0;
    if (verbose)
        printf("reading");
    while (count < buff_size)
    {
        n = read(sockfd, buff + count, buff_size - count);
        if (n < 0)
            error("ERROR reading from socket");
        count += n;
        if (verbose)
            printf("->%ld", (long unsigned)n);
    }
    if (verbose)
        printf("\n");
}

void request_from_char_array(serv_req *s_req, char *str)
{
    char *s = strtok(str, " ");
    strcpy(s_req->type, s); // ver si recibe argumentos erroneos
    s = strtok(NULL, " ");
    if (s != NULL)
    {
        strcpy(s_req->fname, "/spiffs/");
        strcat(s_req->fname, s);
        s = strtok(NULL, " ");
        if (s != NULL)
        {
            s_req->total_len = atol(s);
            s = strtok(NULL, " ");
            if (s != NULL)
            {
                s_req->msg_len = atol(s);
            }
        }
    }
}
uint32_t min(uint32_t val_1, uint32_t val_2)
{
    return (val_1 <= val_2 ? val_1 : val_2);
}

double dwalltime()
{
    double sec;
    struct timeval tv;

    gettimeofday(&tv, NULL);
    sec = tv.tv_sec + tv.tv_usec / 1000000.0;
    return sec;
}
// void error(String str){
//     int len = str.length() + 1;
//     char msg[len];
//     str.toCharArray(msg, len);
//     perror(msg);
//     exit(1);
// }