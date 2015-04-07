#include <stdio.h>

#include "up_opengl_redirect.h"
#include "up_shader_module.h"
#include <SDL2/SDL.h>

static SDL_Window * g_openglWindow = NULL;
static SDL_Window * g_openglContext = NULL;

void UP_sdlSetup()
{
	SDL_Init(SDL_INIT_EVERYTHING);
}

//heja heja

void UP_openGLwindowSetup(int width,int height, const char *title)
{
	//We want to have 32 bit color per pixel, RGBA (red,grean,blue,alpha)
	// and set 8bit per color 
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE,8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE,8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE,8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE,8);
	SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE,32);

	// Doublebuffer lets us draw to a virtual window
	// when opengl has finnished we can call SDL_GL_SwapWindow(...)
	// and that will swap the buffer the window displayes to the virtual (now real)
	// and now opengl can draw to the old window (now virtual)
	// this prevents the users from seeing partialy rendered scences
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER,1);

	// This creates a new window , and set it to be able to handle opengl
	g_openglWindow = SDL_CreateWindow(title,SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,width,height,SDL_WINDOW_OPENGL);
	// this creates a opengl context, and binds it to our window,
	// so this only renders inside the correct context
	g_openglContext = SDL_GL_CreateContext(g_openglWindow);

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



int UP_eventHandler()
{
	int flag = 1;
	SDL_Event event;
	
	while(SDL_PollEvent(&event))
	{
		if (event.type == SDL_QUIT)
		{
			flag = 0;
		}
	}
	return flag;
}


void UP_renderBackground()
{
	glClearColor(0.0f, 0.75f, 0.22f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
}

int main(int argc, char const *argv[])
{
	int status = 1;

	printf("App is now starting\n");

	// setup , sdl and the opengl window
	UP_sdlSetup();
	printf("Sdl setup done\n");

	UP_openGLwindowSetup(400,400,"Det fungerar !!!");
	printf("opengl window setup done\n");

	struct shader_module *shaderprog;

	shaderprog = UP_Shader_new("shadertest");

	while(status)
	{
		UP_renderBackground();
		UP_shader_bind(*shaderprog);
		UP_openGLupdate();
		status = UP_eventHandler();
	}
	printf("Ended main loop\n");

	//cleanup and release all data (notice the reverse order of setup)
	UP_Shader_delete();
	UP_openGLwindowCleanup();
	UP_sdlCleanup();
	printf("All cleanup completed\n");
	return 0;
}











