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
#include "up_camera_module.h"
#include "up_updateObjectMatrix.h"
#include "up_error.h"
#include "up_render_engine.h"
#include "up_star_system.h"
#include "up_healthbar.h"
#include "up_music.h"
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



    int mesh_capacity = 500;
    up_mesh_start_setup(mesh_capacity);    // opengl setup, and allocate memory for mesh_capacity number of models
    up_texture_start_setup();               // opengl texture setup

    int max_ship_count = 40;
    int max_projectile_count = 200;
    int max_enviroment_count = 500;
    int max_others_count = 20;

    up_unit_start_setup(max_ship_count, max_projectile_count, max_enviroment_count, max_others_count);

    int totalNumObjects = max_ship_count + max_projectile_count + max_enviroment_count + max_others_count;
    struct up_transformationContainer *transformationArray = malloc(sizeof(struct up_transformationContainer)*totalNumObjects);
    if (transformationArray == NULL) {
        UP_ERROR_MSG("transformation array malloc failure");

    }


    // Load a shader just for the menu system (location 0)
    struct shader_module *shader_menu;
    shader_menu = UP_Shader_new("shader_menu",0);
    printf("Shader menu finnished\n");

    //Init sound
    struct soundLib *sound= up_setupSound();

    // start the menu, and display it
    status=up_menu(shader_menu, sound);

    //this will load all the assets (modouls,texturs) specifyed in objIndex
    //be aware that index 0 is always a placeholder for modouls not found and so on
    struct up_assets *assets = up_assets_start_setup();
    printf("past assets\n");
    //up_generate_asteroidBelt(idensity,maxAngle,float minAngle,float outerEdge,float innerEdge,float maxHeight,float minHeight)

    // this is the start ship, initilazing the startin positions
    struct up_objectInfo tmp_ship = {0};
    tmp_ship.pos.x = 440;
    tmp_ship.pos.z = 40;
    tmp_ship.modelId = 1;
    tmp_ship.objectId.type = up_ship_type;
    tmp_ship.turnSpeed = 1;
    tmp_ship.acceleration = 5;
    tmp_ship.scale = assets->scaleArray[1];

    int shipIndex = 0;
    int shipIndex_tmp = 0;
    shipIndex_tmp = up_unit_add(up_ship_type,tmp_ship);
    shipIndex_tmp = up_unit_add(up_ship_type,tmp_ship);
    shipIndex_tmp = up_unit_add(up_ship_type,tmp_ship);
    shipIndex_tmp = up_unit_add(up_ship_type,tmp_ship);
    shipIndex_tmp = up_unit_add(up_ship_type,tmp_ship);
    shipIndex = shipIndex_tmp;

    struct up_objectInfo stillObj = {0};
    stillObj.pos.z = 30;
    stillObj.scale = assets->scaleArray[2];
    stillObj.modelId = 2;
    stillObj.objectId.type = up_others_type;

    struct up_objectInfo *ship = up_unit_objAtIndex(up_ship_type,shipIndex);

    up_unit_add(up_environment_type,stillObj);

    up_generate_sun();

    up_generate_asteroidBelt(300, 2*M_PI, 0, 500, 440, 50, 30);

    up_generate_randomize_satellite(40);        //satellite
    up_generate_randomize_spaceMine(80);        //space mine

    
    struct up_font_assets *font_assets = up_font_start_setup();  //load font to inteface startup
    //up_matrix4_t transform2 ;//= up_matrixModel(&model.pos, &model.rot, &model.scale);

    // all the 4by4 matrix that are needed to place the model at the right location and with the right perspective
    //up_matrix4_t transform ;//= up_matrixModel(&model.pos, &model.rot, &model.scale);
    //up_matrix4_t modelMatrix;
    up_matrix4_t viewMatrix;
    up_matrix4_t perspectiveMatrix;

    // sets the initial location of the camera,
    // first 3 values are the camera position,
    // the next 3 value is where it looking at
    // the last 3 values is what is up and what is down
    struct up_camera cam = {{0,0,30},{0,0,1},{0,1,0}};

    // generate the view matrix
    up_matrixView(&viewMatrix, &cam.eye, &cam.center, &cam.up);

    // this is the perspectiv configurations,
    // field of view(fov),aspect ratio,how close object can be(can not be 0!!!),and how far we can see
    // and not smalle then 1 , wierd results happens then
    struct up_perspective pers = {70.0f,screen_width/screen_hight,1.0f,1000.0f};

    // generate the oersoective matrix
    up_matrixPerspective(&perspectiveMatrix, pers.fov, pers.aspectRatio, pers.zNear, pers.zFar);

    dispMat(&perspectiveMatrix);


    // the ship will stand stilll at the begining
    //struct shipMovement movement = {0,0,0,0};

    up_health_bar_t healthBar;
    healthBar = healthbar_creation();

    //up_matrix4_t identity = up_matrix4identity();



    // loads the shaders for the rendering loop (location 1)
    struct shader_module *shaderprog;
    shaderprog = UP_Shader_new("shadertest",1);
    printf("Shader finnished\n");

    // loads skybox shaders and fill out the structure
    up_skyBox_t skyBox;
    skyBox.textureId = up_cubeMapTexture_load();
    skyBox.skyBox = UP_Shader_new("skybox",2);
    skyBox.mesh = &assets->meshArray[3];


    struct up_objectInfo *objectArray = NULL;
    int numObjects = 0;
    struct up_eventState currentEvent = {0};
    struct up_actionState shipAction = {0};
    shipAction.objectID.idx = shipIndex;
    shipAction.objectID.type = up_ship_type;

    up_weaponCoolDown_start_setup(&currentEvent);
    printf("out of weapon\n");
    struct up_objectInfo in_cam[500];
    struct up_eventState funkarEj = {0};
    // starts the main game loop
    up_matrix4_t viewPerspectivMatrix;


    struct up_actionState network_states_data[10];
    int max_network_states = 10;
    int i = 0;
    struct up_actionState noState = {0};
    for (i = 0; i < max_network_states; i++) {
        network_states_data[i] = noState;
    }
    int network_state_recived = 0;
    Pthread_listen_datapipe_t *connection_data = up_network_start_setup();


    while(status)
    {
        up_updateFrameTickRate();
        status = UP_eventHandler(&currentEvent,&shipAction);


        //upnewtwork_getNewMovement(&ship);          // retrive any updates from the network

        //up_newtwork_getNewMovement(&ship);          // retrive any updates from the network

        //up_updatShipMatrixModel(&modelMatrix,&model,ship); // creates the modelMatrix for the ship
        //up_updateShipMovment(ship);
        up_network_sendNewMovement(&shipAction, connection_data);
        network_state_recived = up_network_getNewMovement(network_states_data, max_network_states);

        up_update_actions(&shipAction, network_states_data, network_state_recived,&funkarEj, sound);
        up_updateMovements();
        up_checkCollision();
        moveHealthBar(shipIndex,healthBar);

        up_update_camera(&cam, ship);

        up_matrixView(&viewMatrix, &cam.eye, &cam.center, &cam.up); // creates the view matrix, from the camera

        objectArray = up_ObjectsInView(in_cam,&numObjects, &cam);
        //objectArray = up_unit_getAllObj(&numObjects);

        numObjects = (totalNumObjects > numObjects) ? numObjects : totalNumObjects;

        up_getViewPerspective(&viewPerspectivMatrix,&viewMatrix,&perspectiveMatrix);

        up_updateMatrix(transformationArray, &viewPerspectivMatrix, objectArray, numObjects);
        
        up_gamePlayInterface(font_assets,shader_menu);

        up_render_scene(transformationArray, objectArray, numObjects,&viewPerspectivMatrix, shaderprog, assets,&skyBox);


    }
    printf("Ended main loop\n");

    //cleanup and release all data (notice the reverse order of setup)
    UP_Shader_delete();
    up_unit_shutdown_deinit();
    up_mesh_shutdown_deinit();
    up_freeSound(sound);

    up_texture_shutdown_deinit();
    up_assets_shutdown_deinit(assets);
    //up_network_shutdown_deinit();
    up_network_shutdown_deinit(connection_data);
    UP_openGLwindowCleanup();
    UP_sdlCleanup();
    printf("All cleanup completed\n");
    return 0;
}
