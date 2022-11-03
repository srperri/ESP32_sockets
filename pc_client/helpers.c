#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
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
        printf("reading");
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
