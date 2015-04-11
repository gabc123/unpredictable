#ifndef UP_ERROR_H
#define UP_ERROR_H

// this will print the msg, and in what file , function and at what line the error oucured
#define UP_ERROR_MSG(str) fprintf(stderr,"\n\nError:\nFile: %s\nFunc: %s\nLine: %d\nmsg: %s \n",__FILE__,__func__,__LINE__,str)

#endif
