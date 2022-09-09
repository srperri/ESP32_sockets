#ifndef _HELPERS_H
#define _HELPERS_H
#include <stdint.h>
typedef struct{
  char type[3];
  uint32_t total_len;
  char fname[20];
  uint32_t msg_len;
} request;
void error(char*);
void request_from_char_array(request *req, char* str);
uint32_t min(uint32_t val_1, uint32_t val_2);
double dwalltime();
#endif
