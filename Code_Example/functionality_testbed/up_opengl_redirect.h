#ifndef UP_OPENGL_REDIRECT_H
#define UP_OPENGL_REDIRECT_H

// Becouse diffrent systems have diffrent path to the header file
// this checks the os and the use the correct headers

#ifdef __APPLE__

#include <OpenGL/gl3.h>
//#include <OpenGL/glew.h>  //Not needed on osx

#elif __linux

#include <GL/glew.h>
#include <GL/gl.h>

#else
#include <GL/glew.h>
#include <GL/gl.h>

#endif	/* __MacOSX__  */

#endif /* UP_OPENGL_REDIRECT_H */
