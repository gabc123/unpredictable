#ifndef UP_OPENGL_REDIRECT_H
#define UP_OPENGL_REDIRECT_H

// Becouse diffrent systems have diffrent path to the header file
// this checks the os and the use the correct headers

#ifdef __APPLE__

#include <OpenGL/gl3.h>
//#include <OpenGL/glew.h>  //Not needed on osx

#else

#include <GL/gl3.h>
#include <GL/glew.h>

#define TAL 22

#endif	/* __MacOSX__  */

#endif /* UP_OPENGL_REDIRECT_H */