#ifndef _HELPERS_H
#define _HELPERS_H
#define FNAME_LEN 20
#define SPIFFS_BASE_PATH "/spiffs"
#include <stdint.h>
#include <stddef.h>

typedef struct cline_req
{
  char host[20];
  uint16_t port;
  char op[4];
  char src_fname[FNAME_LEN];
  uint32_t total_len;
  char dst_fname[FNAME_LEN];
  uint32_t msg_len;
};

typedef struct serv_req
{
  char op[4];
  uint32_t total_len;
  char fname[FNAME_LEN];
  uint32_t msg_len;
};
void error(char *);

void fixed_write(int sockfd, const void *buff, size_t buff_size, char verbose);

void fixed_read(int sockfd, void *buff, size_t buff_size, char verbose);

void cline_req_from_char_array(cline_req *req, char *str);

char *full_path(char *fname);

#endif
