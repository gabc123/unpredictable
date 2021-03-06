#ifndef UP_ERROR_H
#define UP_ERROR_H
#include <stdio.h>

// this will print the msg, and in what file , function and at what line the error oucured
// used for error printing, this helps track down where in the code the error ocurred,
// only use this for actuall errors, not just "warnings" (ex not for object already removed, but for tried to access object out of bounds)
#define UP_ERROR_MSG(str) fprintf(stderr,"\n\nError:\nFile: %s\nFunc: %s\nLine: %d\nmsg: %s \n",__FILE__,__func__,__LINE__,str)

#define UP_ERROR_MSG_STR(str1,str2) fprintf(stderr,"\n\nError:\nFile: %s\nFunc: %s\nLine: %d\nmsg: %s str: %s\n",__FILE__,__func__,__LINE__,str1,str2)
#define UP_ERROR_MSG_INT(str1,var) fprintf(stderr,"\n\nError:\nFile: %s\nFunc: %s\nLine: %d\nmsg: %s var:%d\n",__FILE__,__func__,__LINE__,str1,var)

#endif
