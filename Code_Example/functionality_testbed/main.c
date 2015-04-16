#include <stdio.h>
#include "up_sdl_redirect.h"
#include "up_opengl_redirect.h"
#include "up_shader_module.h"
#include "up_texture_module.h"
#include "up_vertex.h"
#include "up_matrixTransforms.h"
#include "up_error.h"
#include "up_modelRepresentation.h"
#include <math.h>

#define M_PI 3.14159265358979323846

static SDL_Window * g_openglWindow = NULL;
static SDL_Window * g_openglContext = NULL;

void UP_sdlSetup()
{
	if(SDL_Init(SDL_INIT_EVERYTHING)!=0)
    {
        UP_ERROR_MSG_STR("SDL_INIT failed, we are all doomed!!\n",SDL_GetError());
    }
}

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

//on linux and windows glew needs to be installed
#ifndef __APPLE__
	GLenum res = glewInit();
	if(res != GLEW_OK)
	{
        UP_ERROR_MSG("glew init error");
	}
#endif

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


int UP_eventHandler(struct up_modelRepresentation *ship)
{
	int flag = 1;
	SDL_Event event;

	while(SDL_PollEvent(&event))
	{
		if (event.type == SDL_QUIT)
		{
			flag = 0;
		}

        else if(event.type == SDL_KEYDOWN){
            switch (event.key.keysym.sym) {
                case SDLK_UP:
                    ship->pos.y+=0.05;
                    ship->rot.z=0;
                    if(ship->pos.y>=0.9)
                        ship->pos.y=-1;
                    break;
                case SDLK_DOWN:
                    ship->pos.y -=0.05;
                    ship->rot.z=M_PI;
                    if(ship->pos.y<=-0.9)
                        ship->pos.y=1;
                    break;
                case SDLK_LEFT:
                    ship->pos.x -=0.05;
                    ship->rot.z=1.5*M_PI;
                    if(ship->pos.x<=-0.9)
                        ship->pos.x=1;
                    break;
                case SDLK_RIGHT:
                    ship->pos.x +=0.05;
                    ship->rot.z=M_PI/2;
                    if(ship->pos.x>=0.9)
                        ship->pos.x=-1;
                    break;
                case SDLK_SPACE:
                    break;

                default:
                    break;
            }
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

	UP_openGLwindowSetup(1280,800,"Det fungerar !!!");
	printf("opengl window setup done\n");



	up_mesh_start_setup(4);
    up_texture_start_setup();

    /// setup the vertexs and the tex coords, this is done like this for debbuing resons
    // texture coordinates, 0,0 is bottom left, 1,1 is top right
    struct up_vec2 tex[] = {{0.0f, 0.0f},
                            {0.2f, 1.0f},
                            {1.0f, 0.0f}
    };

    // this is the posisions of the vertexes
    struct up_vec3 pos[] = {{-0.5f, -0.5f, 0.0f},
                            {0.0f, 0.5f, 0.0f},
                            {0.5f, -0.5f, 0.0f}
    };


    // left over from debugging. fills the vertex array with pos and tex
    struct up_vertex vertex[3];
    int i = 0;
    for (i = 0; i < 3; i++) {
        vertex[i].pos = pos[i];
        vertex[i].texCoord = tex[i];
    }
    /////////////

	printf("vertex start\n");
	struct up_mesh *mesh = UP_mesh_new(vertex, sizeof(vertex)/sizeof(vertex[0]));
	printf("Mesh finnished\n");

	struct shader_module *shaderprog;
	shaderprog = UP_Shader_new("shadertest");
	printf("Shader finnished\n");
    struct up_texture_data *texture = up_load_texture("lala.png");

    /*
    struct up_vec3 model_pos = {0,0.5,0};
    struct up_vec3 model_rot = {0,0,1.5};
    struct up_vec3 model_scale = {1,1,1};
    */

    struct up_modelRepresentation model;

    model.pos.x=0;
    model.pos.y=0.5;
    model.pos.z=0;

    model.rot.x=0;
    model.rot.y=0;
    model.rot.z=1.5;

    model.scale.x=1;
    model.scale.y=1;
    model.scale.z=1;

    up_matrix4_t transform = up_matrixModel(&model.pos, &model.rot, &model.scale);


	while(status)
	{
		UP_renderBackground();                      //Clears the buffer and results an empty window.
		UP_shader_bind(shaderprog);                 //
        up_texture_bind(texture, 0);

        UP_shader_update(shaderprog,&transform);
        up_draw_mesh(mesh);
		UP_openGLupdate();
		status = UP_eventHandler(&model);
        transform = up_matrixModel(&model.pos, &model.rot, &model.scale);
	}
	printf("Ended main loop\n");

	//cleanup and release all data (notice the reverse order of setup)
	UP_Shader_delete();

	up_mesh_shutdown_deinit();

    up_texture_shutdown_deinit();
	UP_openGLwindowCleanup();
	UP_sdlCleanup();
	printf("All cleanup completed\n");
	return 0;
}
