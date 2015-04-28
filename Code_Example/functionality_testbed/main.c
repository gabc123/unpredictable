#include <stdio.h>
#include "up_initGraphics.h"
#include "up_utilities.h"
#include "up_texture_module.h"
#include "up_ship.h"
#include "up_objectReader.h"
#include "up_shader_module.h"
#include "up_network_module.h"
#include "up_menu.h"
#include "up_camera_module.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif


int main(int argc, char const *argv[])
{
	int status = 1;

	printf("App is now starting\n");

	// setup , sdl and the opengl window
	UP_sdlSetup();
	printf("Sdl setup done\n");
    int screen_width = 1280;
    int screen_hight = 800;
	UP_openGLwindowSetup(screen_width,screen_hight,"Det fungerar !!!");
	printf("opengl window setup done\n");


	up_mesh_start_setup(4);
    up_texture_start_setup();

    //struct up_mesh *mesh = meshTriangleShip();
    //struct up_mesh *mesh = meshPyramidShip();

    struct up_objModel *testObj = up_loadObjModel("space_mats2.obj");
    //struct up_objModel *testObj = up_loadObjModel("fighter.obj");

    struct up_mesh *mesh = UP_mesh_new(testObj->vertex, testObj->vertex_length, testObj->indexArray, testObj->index_length);

    up_objModelFree(testObj);

	struct shader_module *shaderprog;
	shaderprog = UP_Shader_new("shadertest",0);
	printf("Shader finnished\n");
    struct up_texture_data *texture = up_load_texture("lala.png");
    
    //struct up_texture_data *texture = up_load_texture("fighter.png");


    struct up_modelRepresentation model;

    
    
    
    model.pos.x=0;
    model.pos.y=0;
    model.pos.z=1;

    model.rot.x=0;
    model.rot.y=0;
    model.rot.z=0;

    model.scale.x=0.1;
    model.scale.y=0.1;
    model.scale.z=0.1;

    
    
    /*
     void up_matrixModel(up_matrix4_t *modelMatrix, struct up_vec3 *pos,struct up_vec3 *rotation,struct up_vec3 *scale);
     void up_matrixView(up_matrix4_t *matrixView, struct up_vec3 *eye, struct up_vec3 *center,struct up_vec3 *UP);

     void up_matrixPerspective(up_matrix4_t *perspective, GLdouble fov,GLdouble aspectRatio,GLdouble zNear,GLdouble zFar);


     void up_getModelViewPerspective(up_matrix4_t *mvp,
     up_matrix4_t *modelMatrix,
     up_matrix4_t *matrixView,
     up_matrix4_t *perspective);
     */

    up_matrix4_t transform ;//= up_matrixModel(&model.pos, &model.rot, &model.scale);
    up_matrix4_t modelMatrix;
    up_matrix4_t viewMatrix;
    up_matrix4_t perspectiveMatrix;


    struct up_camera cam = {{0,0,-3},{0,0,1},{0,1,0}};

    up_matrixView(&viewMatrix, &cam.eye, &cam.center, &cam.up);

    struct up_perspective pers = {70.0f,screen_width/screen_hight,0.01f,1000.0f};


    up_matrixPerspective(&perspectiveMatrix, pers.fov, pers.aspectRatio, pers.zNear, pers.zFar);
    dispMat(&perspectiveMatrix);
    struct up_ship ship;
    ship.pos.x = 0;
    ship.pos.y = 0;
    ship.pos.z = 10;

    ship.dir.x = 0;
    ship.dir.y = 0;
    ship.dir.z = 0;

    ship.speed = 0;

    struct shipMovement movement = {0,0,0,0};


    //up_matrix4_t sunModelMatrix;

    
    //up_matrix4_t identity = up_matrix4identity();
    //up_network_start_setup();
    
    // Load a shader just for the menu system
    struct shader_module *shader_menu;
    shader_menu = UP_Shader_new("shader_menu",1);
    printf("Shader menu finnished\n");
    
    status=up_menu(shader_menu);
    
    while(status)
	{
        up_updateFrameTickRate();
        status = UP_eventHandler(&ship,&movement);

		UP_renderBackground();                      //Clears the buffer and results an empty window.
		UP_shader_bind(shaderprog);                 //
        up_texture_bind(texture, 0);

        //up_newtwork_getNewMovement(&ship);
        
        up_updateShipMovment(&ship);
        up_updatShipMatrixModel(&modelMatrix,&model,&ship);

        
        //up_matrixModel(&sunModelMatrix,&sun.pos, &sun.rot, &sun.scale);
        
        up_matrixView(&viewMatrix, &cam.eye, &cam.center, &cam.up);

        up_getModelViewPerspective(&transform, &modelMatrix, &viewMatrix, &perspectiveMatrix);
        //up_getModelViewPerspective(&transform, &modelMatrix, &viewMatrix, &identity);
        //dispMat(&transform);
        UP_shader_update(shaderprog,&transform);
        
        
        //up_matrixView(&sunViewMatrix, &cam.center, &cam.eye, &cam.up);
        //up_getModelViewPerspective(&sunMatrix, &identity, &identity, &identity);
        up_shader_update_sunligth(shaderprog, &modelMatrix);
        up_draw_mesh(mesh);
		UP_openGLupdate();

        //transform = up_matrixModel(&model.pos, &model.rot, &model.scale);
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
