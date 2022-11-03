#ifndef _HELPERS_H
#define _HELPERS_H
#define FNAME_LEN 20
#define SPIFFS_BASE_PATH "/spiffs"
#include <stdint.h>
#include <stddef.h>

typedef struct serv_req
{
  char op[4];
  uint32_t total_len;
  char fname[FNAME_LEN];
  uint32_t msg_len;
};

void fixed_write(int sockfd, void *buff, size_t buff_size, char verbose);

void fixed_read(int sockfd, void *buff, size_t buff_size, char verbose);

void request_from_char_array(serv_req *req, char *str);

char *full_path(char *fname);

#endif
