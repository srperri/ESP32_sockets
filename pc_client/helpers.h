#ifndef _HELPERS_H
#define _HELPERS_H
#define FNAME_LEN 20
#include <stdint.h>

typedef struct
{
  char type[3];
  uint32_t total_len;
  char fname[FNAME_LEN];
  uint32_t msg_len;
} serv_req;

void error(char *);

void fixed_write(int sockfd, const void *buff, size_t buff_size, char verbose);

void fixed_read(int sockfd, const void *buff, size_t buff_size, char verbose);

void serv_req_from_char_array(serv_req *req, char *str);

uint32_t min(uint32_t val_1, uint32_t val_2);

double dwalltime();

#endif
