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
#include "up_assets.h"

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
    
    //up_network_start_setup();
    
    int mesh_capacity = 40;
    up_mesh_start_setup(mesh_capacity);    // opengl setup, and allocate memory for mesh_capacity number of models
    up_texture_start_setup();               // opengl texture setup
    
    //this will load all the assets (modouls,texturs) specifyed in objIndex
    //be aware that index 0 is always a placeholder for modouls not found and so on
    struct up_assets *assets = up_assets_start_setup();
    
    struct up_mesh *mesh = &assets->meshArray[1];
    struct up_texture_data *texture = &assets->textureArray[1];
    
    
    struct up_modelRepresentation model = {0};
    
    model.scale=assets->scaleArray[1];
    
    // all the 4by4 matrix that are needed to place the model at the right location and with the right perspective
    up_matrix4_t transform ;//= up_matrixModel(&model.pos, &model.rot, &model.scale);
    up_matrix4_t modelMatrix;
    up_matrix4_t viewMatrix;
    up_matrix4_t perspectiveMatrix;
    
    // sets the initial location of the camera,
    // first 3 values are the camera position,
    // the next 3 value is where it looking at
    // the last 3 values is what is up and what is down
    struct up_camera cam = {{0,0,-3},{0,0,1},{0,1,0}};
    
    // generate the view matrix
    up_matrixView(&viewMatrix, &cam.eye, &cam.center, &cam.up);
    
    // this is the perspectiv configurations,
    // field of view(fov),aspect ratio,how close object can be(can not be 0!!!),and how far we can see
    struct up_perspective pers = {70.0f,screen_width/screen_hight,0.01f,1000.0f};
    
    // generate the oersoective matrix
    up_matrixPerspective(&perspectiveMatrix, pers.fov, pers.aspectRatio, pers.zNear, pers.zFar);
    
    dispMat(&perspectiveMatrix);
    
    // this is the start ship, initilazing the startin positions
    struct up_ship ship = {0};
    ship.pos.z = 10;
    
    // the ship will stand stilll at the begining
    struct shipMovement movement = {0,0,0,0};
    
    
    //up_matrix4_t identity = up_matrix4identity();
    
    // Load a shader just for the menu system (location 0)
    struct shader_module *shader_menu;
    shader_menu = UP_Shader_new("shader_menu",0);
    printf("Shader menu finnished\n");
    
    // start the menu, and display it
    status=up_menu(shader_menu);
    
    // loads the shaders for the rendering loop (location 1)
    struct shader_module *shaderprog;
    shaderprog = UP_Shader_new("shadertest",1);
    printf("Shader finnished\n");
    
    
    // starts the main game loop
    while(status)
    {
        up_updateFrameTickRate();
        status = UP_eventHandler(&ship,&movement);
        
        UP_renderBackground();                      //Clears the buffer and results an empty window.
        UP_shader_bind(shaderprog);                 // tells the gpu what shader program to use
        up_texture_bind(texture, 0);
        
        //up_newtwork_getNewMovement(&ship);          // retrive any updates from the network
        
        up_updateShipMovment(&ship);
        
        up_updatShipMatrixModel(&modelMatrix,&model,&ship); // creates the modelMatrix for the ship
        
        up_matrixView(&viewMatrix, &cam.eye, &cam.center, &cam.up); // creates the view matrix, from the camera
        
        // combinds the 3 matrix modelMatrix, viewMatrix, perspectiveMatrix, into a transformMatrix,
        // this is needed for the gpu to place the model at the right location on screen and get the right perspective
        up_getModelViewPerspective(&transform, &modelMatrix, &viewMatrix, &perspectiveMatrix);
        
        UP_shader_update(shaderprog,&transform);    // this uploads the transform to the gpu, and will now be applayed to up_draw_mesh
        
        up_shader_update_sunligth(shaderprog, &modelMatrix);    // this uploads the sun angle to the gpu
        
        up_draw_mesh(mesh); // this draws the model onto the screen , at the location transform, and with the sunlight modelMatrix
        UP_openGLupdate();  // this swaps the render and window buffer , displaying it on screen
        
    }
    printf("Ended main loop\n");
    
    //cleanup and release all data (notice the reverse order of setup)
    UP_Shader_delete();
    
    up_mesh_shutdown_deinit();
    
    up_texture_shutdown_deinit();
    up_assets_shutdown_deinit(assets);
    //up_network_shutdown_deinit();
    UP_openGLwindowCleanup();
    UP_sdlCleanup();
    printf("All cleanup completed\n");
    return 0;
}
