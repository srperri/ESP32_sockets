#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include "Arduino.h"
#include <lwip/sockets.h>
#include "helpers.h"

void error(char *str)
{
    Serial.println(str);
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

void fixed_read(int sockfd, void *buff, size_t buff_size, char verbose)
{
    int n, count = 0;
    if (verbose)
        printf("reading ");
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

void cline_req_from_char_array(cline_req *req, char *str)
{
    bzero((char *)req, sizeof(cline_req));
    // 1:hostname 2:port 3:[GET|PUT] 4:src_filename 5:<bytes> 6:<dst_filename> 7:<msg_bytes>\n", argv[0]);
    char *s = strtok(str, " ");
    if (s != NULL)
    {
        Serial.printf("host = <%s>", s);
        strcpy(req->host, s); // ver si recibe argumentos erroneos
        s = strtok(NULL, " ");
        if (s != NULL)
        {
            Serial.printf("port = <%s>", s);
            req->port = atol(s);
            s = strtok(NULL, " ");
            if (s != NULL)
            {
                Serial.printf("op = <%s>", s);
                strcpy(req->op, s); // ver si recibe argumentos erroneos
                s = strtok(NULL, " ");
                if (s != NULL)
                {
                    Serial.printf("src_fname = <%s>", s);
                    strcpy(req->src_fname, s);
                    s = strtok(NULL, " ");
                    if (s != NULL)
                    {
                        Serial.printf("total_len = <%s>", s);
                        req->total_len = atol(s);
                        s = strtok(NULL, " ");
                        if (s != NULL)
                        {
                            Serial.printf("dst_fname = <%s>", s);
                            strcpy(req->dst_fname, s);
                            s = strtok(NULL, " ");
                            if (s != NULL)
                            {
                                Serial.printf("msg_len = <%s>", s);
                                req->msg_len = atol(s);
                            }
                        }
                    }
                }
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
// void error(String str){
//     int len = str.length() + 1;
//     char msg[len];
//     str.toCharArray(msg, len);
//     perror(msg);
//     exit(1);
// }