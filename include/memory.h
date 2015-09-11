#ifndef  CUBE_MEMORY_H
#define  CUBE_MEMORY_H

void * Memcpy(void * dest,void * src, unsigned int count);
void * Memset(void * s,int c, int n);
int    Memcmp(const void *s1,const void *s2,int n);

char * Strcpy(char *desc,const char *src);
char * Strncpy(char *desc,const char *src,int n);
int    Strcmp(const char *s1,const char *s2);
int    Strncmp(const char *s1,const char *s2,int n);
char * Strcat(char * desc,const char *src);
char * Strncat(char * desc,const char *src,int n);

#endif
