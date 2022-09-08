#ifndef _COMMON_H
#define _COMMON_H
typedef struct request{
  char type[3];
  long total_len;
  char fname[20];
  long msg_len;
};
void error(char*);
void request_from_char_array(request *req, char* str);
#endif
