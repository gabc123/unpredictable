#include <stdio.h>


#include "up_utilities.h"


#include "up_network_module.h"
#include "up_menu.h"
#include "up_camera_module.h"

#include "up_camera_module.h"
#include "up_error.h"
#include "up_render_engine.h"
#include "up_star_system.h"

#include "up_interface.h"

#include "up_music.h"

#include "up_game_tools.h"

#include "up_assets.h"

#include "up_graphics_setup.h"
#include "up_graphics_update.h"

#include "up_player.h"
#include "up_control_events.h"
#include "up_collisions.h"

#include "up_object_actions.h"
#include "up_object_handler.h"

#include "up_updateObjectMatrix.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif


// magnus ,
int main(int argc, char const *argv[])
{
    int status = 1;

    printf("App is now starting\n");

    // setup , sdl and the opengl window
    UP_sdlSetup();
    up_system_check();
    printf("Sdl setup done\n");
    int screen_width = 1280;
    int screen_hight = 800;
    UP_openGLwindowSetup(screen_width,screen_hight,"UNPREDICTABLE");
    printf("opengl window setup done\n");

    //up_network_start_setup();

    int mesh_capacity = 500;
    up_mesh_start_setup(mesh_capacity);    // opengl setup, and allocate memory for mesh_capacity number of models
    up_texture_start_setup();               // opengl texture setup

    int max_ship_count = 70;
    int max_projectile_count = 300;
    int max_enviroment_count = 500;
    int max_others_count = 200;
    struct up_allCollisions allcollisions = {0};

    // loads the shaders for the rendering loop (location 1)
    struct up_shader_module *shaderprog;
    shaderprog = up_shader_new("shadertest",1);
    printf("Shader finnished\n");

    // if we failed to load the primary shaders, there is no game to play
    // so do program cleanup and exit
    if (shaderprog == NULL) {
        UP_ERROR_MSG("Failed to load primary shader aborting program");
        up_shader_deinit();
        up_mesh_shutdown_deinit();
        up_texture_shutdown_deinit();
        UP_openGLwindowCleanup();
        UP_sdlCleanup();
        return 0;
    }
    // Load a shader just for the menu system (location 0)
    struct up_shader_module *shader_menu;
    shader_menu = up_shader_new("shader_menu",0);
    if (shader_menu == NULL) {
        // we use the primary shader as a fallback
        shader_menu = shaderprog;
    }
    printf("Shader menu finnished\n");


    up_unit_start_setup(max_ship_count, max_projectile_count, max_enviroment_count, max_others_count);

    int totalNumObjects = max_ship_count + max_projectile_count + max_enviroment_count + max_others_count;
    struct up_transformationContainer *transformationArray = malloc(sizeof(struct up_transformationContainer)*totalNumObjects);
    if (transformationArray == NULL) {
        UP_ERROR_MSG("transformation array malloc failure");
    }


    struct up_network_datapipe *connection_data = up_network_start_gameplay_setup();


    //Init sound
    struct soundLib *sound= up_setupSound();

    struct up_key_map *keymap = up_key_remapping_setup();
    struct up_font_assets *font_assets = up_font_start_setup();  //load font to inteface startup

    struct up_network_datapipe *account_connection = up_network_start_account_setup();
    // start the menu, and display it
    
    struct up_map_data mapData = {0};   // gets filled out from the login sequance in menu
    mapData.mapSeed = 42;
    mapData.numPlayersOnline = 4;
    mapData.playeModel = 18;
    mapData.playerIndex = 1;
    status = 2;
    status=up_menu(shader_menu, sound,keymap,font_assets,account_connection,&mapData);
    
    
    up_network_shutdown_deinit(account_connection);
    
    //this will load all the assets (modouls,texturs) specifyed in objIndex
    //be aware that index 0 is always a placeholder for modouls not found and so on
    struct up_assets *assets = up_assets_start_setup();
    printf("past assets\n");
    //up_generate_asteroidBelt(idensity,maxAngle,float minAngle,float outerEdge,float innerEdge,float maxHeight,float minHeight)



    // this is the backup ship incase something went wrong, initilazing the startin positions
    struct up_objectInfo tmp_ship = up_asset_createObjFromId(mapData.playeModel);
    tmp_ship.pos.x = 440;
    tmp_ship.pos.z = 40;
    tmp_ship.dir.x = 0.03;
    tmp_ship.dir.y = 1.0;
    tmp_ship.angle = 0.0;
    tmp_ship.modelId = mapData.playeModel;
    tmp_ship.objectId.type = up_ship_type;
    tmp_ship.turnSpeed = 1;
    tmp_ship.acceleration = 5;
    //tmp_ship.scale = assets->scaleArray[1];

    int shipIndex = mapData.playerIndex;
    up_server_unit_setObjAtindex(up_ship_type, tmp_ship, shipIndex);


    
    up_network_sendChangeModel(mapData.playeModel, mapData.playerIndex, connection_data);

    struct up_objectInfo *ship = up_unit_objAtIndex(up_ship_type,shipIndex);
    if (ship == NULL) {
        UP_ERROR_MSG("ship do not exist, use loner ship");
        ship = &tmp_ship;
    }

    up_generate_map(mapData.mapSeed);
    
    struct up_shader_module *sunShader = up_shader_new("shader_sun", 3);
    struct up_sun theSun = up_sun_create(sunShader);



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
    struct up_camera cam = {{-500,10,30},{440,0,40},{0,0,-1}};
    up_cam_zoom(-3);
    up_update_camera(&cam, ship);

    // generate the view matrix
    up_matrixView(&viewMatrix, &cam.eye, &cam.center, &cam.up);

    // this is the perspectiv configurations,
    // field of view(fov),aspect ratio,how close object can be(can not be 0!!!),and how far we can see
    // and not smalle then 1 , wierd results happens then
    struct up_perspective pers = {70.0f,screen_width/screen_hight,1.0f,1000.0f};

    // generate the oersoective matrix
    up_matrixPerspective(&perspectiveMatrix, pers.fov, pers.aspectRatio, pers.zNear, pers.zFar);

    dispMat(&perspectiveMatrix);




    //up_health_bar_t healthBar;
   // healthBar = healthbar_creation();

   // up_stats_index_t interface_info = up_create_statsObject();

    // loads skybox shaders and fill out the structure
    struct up_skyBox skyBox;
    skyBox.textureId = up_cubeMapTexture_load();
    skyBox.skyBox = up_shader_new("skybox",2);
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

    // starts the main game loop
    up_matrix4_t viewPerspectivMatrix;

    int map_maxPlayers = 20;
    struct up_actionState noState = {0};

    struct up_actionState *network_states_data = malloc(sizeof(struct up_actionState)*map_maxPlayers);
    if (network_states_data == NULL) {
        UP_ERROR_MSG("failed network_state malloc");
        network_states_data = &noState;
        map_maxPlayers = 0;
    }

    int i = 0;
    for (i = 0; i < map_maxPlayers; i++) {
        network_states_data[i] = noState;
    }
    int network_state_recived = 0;
    
    

    struct up_player_stats player_stats = {0};

    struct up_interface_game interface = {0};
    
    up_player_setup(&player_stats, currentEvent.flags);
    up_interface_creation(&interface, &player_stats);

    //SDL_Delay(200);//so the data have been updated
    printf("starting main loop\n");

    network_state_recived = up_network_getNewStates(network_states_data, map_maxPlayers,shipIndex,&player_stats,connection_data);
    
    ship = up_unit_objAtIndex(up_ship_type,shipIndex);
    if (ship == NULL) {
        UP_ERROR_MSG("ship do not exist, use loner ship");
        ship = &tmp_ship;
    }
    
    while(status)
    {
        up_updateFrameTickRate();
        status = UP_eventHandler(&currentEvent,&shipAction,keymap);

        up_network_sendNewMovement(&shipAction, connection_data);
        network_state_recived = up_network_getNewStates(network_states_data, map_maxPlayers,shipIndex,&player_stats,connection_data);

        up_update_actions(&shipAction, network_states_data, map_maxPlayers,&currentEvent, sound);
        
        up_updateMovements();
        up_checkCollision(&allcollisions);
        
        up_update_playerStats(&allcollisions, &player_stats,&currentEvent.flags, shipIndex);
        up_handleCollision(&allcollisions,&player_stats,&currentEvent.flags);

        up_update_sun(&theSun);
        
        up_update_camera(&cam, ship);
        
        up_interface_update(&interface, &player_stats);
       // up_moveHealthBar(shipIndex,healthBar,&player_stats, &cam);
        //up_interface_placement(&cam,interface_info);
        up_radar_search(&interface,assets,ship);

        up_matrixView(&viewMatrix, &cam.eye, &cam.center, &cam.up); // creates the view matrix, from the camera

        objectArray = up_ObjectsInView(in_cam,&numObjects, &cam);
        //objectArray = up_unit_getAllObj(&numObjects);

        numObjects = (totalNumObjects > numObjects) ? numObjects : totalNumObjects;

        up_getViewPerspective(&viewPerspectivMatrix,&viewMatrix,&perspectiveMatrix);

        up_updateMatrix(transformationArray, &viewPerspectivMatrix, objectArray, numObjects);


        UP_renderBackground();                      //Clears the buffer and results an empty window. to prep for render

        up_render_scene(transformationArray, objectArray, numObjects,&viewPerspectivMatrix, shaderprog, assets);

        
        up_skybox_render(&skyBox,&cam,&viewPerspectivMatrix);
        
        up_render_sun(&theSun, &cam, &viewPerspectivMatrix, assets);
        
        
        up_interface_gamePlay(assets,font_assets,shader_menu,&interface);


        UP_openGLupdate();

    }
    up_network_exitProg(mapData.userName, shipIndex, connection_data);
    printf("Ended main loop\n");
    free(transformationArray);
    //cleanup and release all data (notice the reverse order of setup)
    up_shader_deinit();
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
