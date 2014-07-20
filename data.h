
#ifndef DATA_H
#define DATA_H

#include <windows.h>
#include <tchar.h>

#define BUFSIZE		MAX_PATH
#define INDEXMAX	1000

#ifdef __cplusplus      // avoid c++ name mangling
  extern "C" {
#endif

void initdata(void);
void getdata(void);
void freedata(void);
long gettimeinterval(LARGE_INTEGER *start, LARGE_INTEGER *end);

#ifdef __cplusplus
  }
#endif 

#endif // DATA_H
