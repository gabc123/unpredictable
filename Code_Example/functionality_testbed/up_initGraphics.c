#include "up_initGraphics.h"
#include "up_sdl_redirect.h"
#include "up_error.h"
#include "up_texture_module.h"

static SDL_Window * g_openglWindow = NULL;
static SDL_Window * g_openglContext = NULL;


void UP_sdlSetup()
{
	if(SDL_Init(SDL_INIT_EVERYTHING)!=0)
    {
        UP_ERROR_MSG_STR("SDL_INIT failed, we are all doomed!!\n",SDL_GetError());
    }
    printf("SDL init done\n");

    if (IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG) {
        UP_ERROR_MSG("SDL image init failure (PNG)");
    }
    printf("SDL image init done\n");

}

void UP_openGLwindowSetup(int width,int height, const char *title)
{
    // we need to have atleast opengl 3.2 to use all the functions we need.
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);


	//We want to have 32 bit color per pixel, RGBA (red,grean,blue,alpha)
	// and set 8bit per color
	if(SDL_GL_SetAttribute(SDL_GL_RED_SIZE,8)==-1){
        UP_ERROR_MSG_STR("set attribute red fail :-C",SDL_GetError());
	}
	if(SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE,8)==-1){
        UP_ERROR_MSG_STR("set attribute green fail :-C",SDL_GetError());
	}
	if(SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE,8)==-1){
        UP_ERROR_MSG_STR("set attribute blue fail :-C",SDL_GetError());
	}
	if(SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE,8)==-1){
        UP_ERROR_MSG_STR("set attribute alpha fail :-C",SDL_GetError());
	}
	if(SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE,32)==-1){
        UP_ERROR_MSG_STR("set attribute buffer fail :-C",SDL_GetError());
	}
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE,16);


	// Doublebuffer lets us draw to a virtual window
	// when opengl has finnished we can call SDL_GL_SwapWindow(...)
	// and that will swap the buffer the window displayes to the virtual (now real)
	// and now opengl can draw to the old window (now virtual)
	// this prevents the users from seeing partialy rendered scences
	if(SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER,1)==-1){
        UP_ERROR_MSG_STR("SDL_GLDOUBLEBUFFER fail :-C",SDL_GetError());
	}

	// This creates a new window , and set it to be able to handle opengl
	g_openglWindow = SDL_CreateWindow(title,SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,width,height,SDL_WINDOW_OPENGL);
	// this creates a opengl context, and binds it to our window,
	// so this only renders inside the correct context
	g_openglContext = SDL_GL_CreateContext(g_openglWindow);

    SDL_Delay(100);
//on linux and windows glew needs to be installed
#ifndef __APPLE__
	/*glewExperimental = GL_TRUE;
	GLenum res = glewInit();
	if(res != GLEW_OK)
	{
        UP_ERROR_MSG("glew init error");
	}*/
	//Pre-window creation.


    if(ogl_LoadFunctions() == ogl_LOAD_FAILED)
    {
        UP_ERROR_MSG("OpenGL init failed");
    }
//Call functions here.
#endif
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);   // we should change this, the model is in the wrong order

    /// print opengl information
    printf("\n\n\n\n");
    printf("\n\nGL_VENDOR: %s",glGetString(GL_VENDOR));
    printf("\n\nGL_RENDERER: %s",glGetString(GL_RENDERER));
    printf("\n\nGL_VERSION: %s",glGetString(GL_VERSION));
    printf("\n\nGL_SHADING_LANGUAGE_VERSION %s",glGetString(GL_SHADING_LANGUAGE_VERSION));
    printf("\n\nGL_EXTENSIONS: %s",glGetString(GL_EXTENSIONS));
    printf("\n\n \n\n");

}

void UP_openGLupdate()
{
	SDL_GL_SwapWindow(g_openglWindow);
}


void UP_openGLwindowCleanup()
{
	SDL_GL_DeleteContext(g_openglContext);
	SDL_DestroyWindow(g_openglWindow);
}


void UP_sdlCleanup()
{
	SDL_Quit();
}

void UP_renderBackground()
{
	glClearColor(0.0f, 0.15f, 0.42f, 1.0f);
    //glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
