#include "testmodels.h"
#include "up_utilities.h"
#include <stdlib.h>
#include <stdio.h>
#include "up_modelRepresentation.h"
#include "up_matrixTransforms.h"
#include "up_error.h"

/*fallback mesh*/
//Sebastian

struct up_mesh *meshTriangleShip()
{
    /// setup the vertexs and the tex coords, this is done like this for debbuging reasons
    // texture coordinates, 0,0 is bottom left, 1,1 is top right
    struct up_vec2 tex[] = {
        {0.0f, 0.0f},
        {0.2f, 1.0f},
        {1.0f, 0.0f},
        {0.5f, 0.5f}
    };

    // this is the posisions of the vertexes
    struct up_vec3 pos[] = {
        {-0.5f, -0.5f, 0.0f},
        {0.0f, 0.5f, 0.0f},
        {0.5f, -0.5f, 0.0f},
        {0.0f, 0.0f, 1.0f}
    };

//    unsigned int indexArray[] = {0,2,1,2,3,1,0,3,2,3,0,1};
    unsigned int indexArray[] = {1,2,0,1,3,2,2,3,0,1,0,3};
    // left over from debugging. fills the vertex array with pos and tex an normals
    struct up_vertex vertex[4];
    int i = 0;
    for (i = 0; i < 4; i++) {
        vertex[i].pos = pos[i];
        vertex[i].texCoord = tex[i];
        vertex[i].normals.x = 0;
        vertex[i].normals.y = 0;
        vertex[i].normals.z = 0;
    }
    /////////////
    printf("vertex start: ");
    struct up_mesh *mesh = UP_mesh_new(vertex, sizeof(vertex)/sizeof(vertex[0]),indexArray, sizeof(indexArray)/sizeof(indexArray[0]));
    //printf("Mesh finnished\n");
    return mesh;
}

struct up_mesh *meshPyramidShip()
{
    struct up_vec3 pyramid_pos[] = {
        {-0.5f, -0.5f, -0.5f},
        {0.0f, 0.5f, 0.0f},
        {0.5f, -0.5f, -0.5f},
        {0.0f, -0.5f, 0.5f},
    };
    unsigned int pyramid_pos_size = sizeof(pyramid_pos)/sizeof(pyramid_pos[0]);

    struct up_vec2 pyramid_tex[] = {
        {0.0f, 0.0f},
        {0.2f, 1.0f},
        {1.0f, 0.0f},
        {0.2f, 1.0f}
    };
    // unsigned int pyramid_tex_size = sizeof(pyramid_tex)/sizeof(pyramid_tex[0]);

    unsigned int pyramid_indexArray[] = {0,1,2,3,2,4,4,2,1};
    unsigned int pyramid_index_size =sizeof(pyramid_indexArray)/sizeof(pyramid_indexArray[0]);

    struct up_generic_list *list = up_vertex_list_new(pyramid_pos_size);
    int i = 0;

    struct up_vertex tmp_vertex;
    for (i = 0; i < pyramid_pos_size; i++) {
        tmp_vertex.pos = pyramid_pos[i];
        tmp_vertex.texCoord = pyramid_tex[i];

        tmp_vertex.normals.x = 0;
        tmp_vertex.normals.y = 0;
        tmp_vertex.normals.z = 0;

        up_vertex_list_add(list, &tmp_vertex);
    }
    struct up_vertex *vertex = up_vertex_list_transferOwnership(&list);

    /////////////
    printf("vertex start: ");
    struct up_mesh *mesh = UP_mesh_new(vertex, pyramid_pos_size,pyramid_indexArray, pyramid_index_size);
    //printf("Mesh finnished\n");
    free(vertex);
    return mesh;
}


//Gjord av Joachim

struct up_mesh *up_meshMenuBackground()
{
    /// setup the vertexs and the tex coords, this is done like this for debbuging reasons
    // texture coordinates, 0,0 is bottom left, 1,1 is top right
    struct up_vec2 tex[] = {
        {0.0f, 0.0f},
        {1.0f, 0.0f},
        {0.0f, 1.0f},
        {1.0f, 1.0f}
    };

    // this is the posisions of the vertexes
    struct up_vec3 pos[] = {
        {-1.0f, -1.0f, 0.5f},   // bottomleft 0
        {1.0f, -1.0f, 0.5f},   //bottom right 1
        {-1.0f, 1.0f, 0.5f},  //topleft 2
        {1.0f, 1.0f, 0.5f}   //topright 3
    };


    //unsigned int indexArray[] = {0,2,1,2,3,1};  //binds togheter two triangels into one square
    unsigned int indexArray[] = {1,2,0,1,3,2};  //binds togheter two triangels into one square

    // left over from debugging. fills the vertex array with pos and tex
    struct up_vertex vertex[4];
    int i = 0;
    for (i = 0; i < 4; i++) {
        vertex[i].pos = pos[i];
        vertex[i].texCoord = tex[i];
        vertex[i].normals.x = 0;
        vertex[i].normals.y = 0;
        vertex[i].normals.z = 0;
    }
    /////////////
    printf("vertex start: ");
    struct up_mesh *mesh = UP_mesh_new(vertex, sizeof(vertex)/sizeof(vertex[0]),indexArray, sizeof(indexArray)/sizeof(indexArray[0]));
    //printf("Mesh finnished\n");
    return mesh;
}

struct up_mesh *up_meshBotton(float imageX, float imageY, float screenPosX, float screenPosY)
{
    /// setup the vertexs and the tex coords, this is done like this for debbuging reasons
    // texture coordinates, 0,0 is bottom left, 1,1 is top right
    
    imageX=imageX*0.5;
    imageY=imageY*0.5;
    
    // screenPosX=screenPosX*0.01;
    //screenPosY=screenPosY*0.01;
    
    struct up_vec2 tex[] = {
        {0.0f + imageX, 0.0f + imageY},
        {1.0f + imageX, 0.0f + imageY},
        {0.0f + imageX, 0.5f + imageY},
        {1.0f + imageX, 0.5f + imageY}
    };
    
    // this is the posisions of the vertexes
    struct up_vec3 pos[] = {
        {-0.28f + screenPosX, -0.1f + screenPosY, 0.1f},   // bottomleft 0
        {0.28f + screenPosX, -0.1f + screenPosY, 0.1f},   //bottom right 1
        {-0.28f + screenPosX, 0.1f + screenPosY, 0.1f},  //topleft 2
        {0.28f + screenPosX, 0.1f + screenPosY, 0.1f}   //topright 3
    };
    
    
    unsigned int indexArray[] = {1,2,0,1,3,2};  //binds togheter two triangels into one square
    
    // left over from debugging. fills the vertex array with pos and tex
    struct up_vertex vertex[4];
    int i = 0;
    for (i = 0; i < 4; i++) {
        vertex[i].pos = pos[i];
        vertex[i].texCoord = tex[i];
        vertex[i].normals.x = 0;
        vertex[i].normals.y = 0;
        vertex[i].normals.z = 0;
    }
    /////////////
    printf("vertex start: ");
    struct up_mesh *mesh = UP_mesh_new(vertex, sizeof(vertex)/sizeof(vertex[0]),indexArray, sizeof(indexArray)/sizeof(indexArray[0]));
    //printf("Mesh finnished\n");
    return mesh;
}


// magnus test
struct up_mesh *up_mesh_menu_Botton()
{


    struct up_vec2 tex[] = {
        {0.0f, 0.0f},
        {1.0f, 0.0f},
        {0.0f, 1.0f},
        {1.0f, 1.0f}
    };

    // this is the posisions of the vertexes
    struct up_vec3 pos[] = {
        {-1.0f, -1.0f, 0.0f},   // bottomleft 0
        {1.0f, -1.0f, 0.0f},   //bottom right 1
        {-1.0f, 1.0f, 0.0f},  //topleft 2
        {1.0f, 1.0f, 0.0f}   //topright 3
    };


    unsigned int indexArray[] = {1,2,0,1,3,2};  //binds togheter two triangels into one square

    // left over from debugging. fills the vertex array with pos and tex
    struct up_vertex vertex[4];
    int i = 0;
    for (i = 0; i < 4; i++) {
        vertex[i].pos = pos[i];
        vertex[i].texCoord = tex[i];
        vertex[i].normals.x = 0;
        vertex[i].normals.y = 0;
        vertex[i].normals.z = 0;
    }
    /////////////
    printf("vertex start: ");
    struct up_mesh *mesh = UP_mesh_new(vertex, sizeof(vertex)/sizeof(vertex[0]),indexArray, sizeof(indexArray)/sizeof(indexArray[0]));
    //printf("Mesh finnished\n");
    return mesh;
}

struct up_mesh *up_meshLoginOverlay(float maximumY, float minimumY)
{
    /// setup the vertexs and the tex coords, this is done like this for debbuging reasons
    // texture coordinates, 0,0 is bottom left, 1,1 is top right
    struct up_vec2 tex[] = {
        {0.0f, minimumY},
        {1.0f, minimumY},
        {0.0f, maximumY},
        {1.0f, maximumY}
    };

    // this is the posisions of the vertexes
    struct up_vec3 pos[] = {
        {-0.28f, -0.3f, 0.2f},   // bottomleft 0
        {0.28f, -0.3f, 0.2f},   //bottom right 1
        {-0.28f, 0.2f, 0.2f},  //topleft 2
        {0.28f, 0.2f, 0.2f}   //topright 3
    };


    unsigned int indexArray[] = {1,2,0,1,3,2};  //binds togheter two triangels into one square

    // left over from debugging. fills the vertex array with pos and tex
    struct up_vertex vertex[4];
    int i = 0;
    for (i = 0; i < 4; i++) {
        vertex[i].pos = pos[i];
        vertex[i].texCoord = tex[i];
        vertex[i].normals.x = 0;
        vertex[i].normals.y = 0;
        vertex[i].normals.z = 0;
    }
    /////////////
    printf("vertex start: ");
    struct up_mesh *mesh = UP_mesh_new(vertex, sizeof(vertex)/sizeof(vertex[0]),indexArray, sizeof(indexArray)/sizeof(indexArray[0]));
    //printf("Mesh finnished\n");
    return mesh;
}

struct up_mesh *up_meshQuitwindow()
{
    /// setup the vertexs and the tex coords, this is done like this for debbuging reasons
    // texture coordinates, 0,0 is bottom left, 1,1 is top right
    struct up_vec2 tex[] = {
        {0.0f, 0.0f},
        {1.0f, 0.0f},
        {0.0f, 1.0f},
        {1.0f, 1.0f}
    };

    // this is the posisions of the vertexes
    struct up_vec3 pos[] = {
        {-0.28f, -0.3f, 0.2f},   // bottomleft 0
        {0.28f, -0.3f, 0.2f},   //bottom right 1
        {-0.28f, 0.2f, 0.2f},  //topleft 2
        {0.28f, 0.2f, 0.2f}   //topright 3
    };


    unsigned int indexArray[] = {1,2,0,1,3,2};  //binds togheter two triangels into one square

    // left over from debugging. fills the vertex array with pos and tex
    struct up_vertex vertex[4];
    int i = 0;
    for (i = 0; i < 4; i++) {
        vertex[i].pos = pos[i];
        vertex[i].texCoord = tex[i];
        vertex[i].normals.x = 0;
        vertex[i].normals.y = 0;
        vertex[i].normals.z = 0;
    }
    /////////////
    printf("vertex start: ");
    struct up_mesh *mesh = UP_mesh_new(vertex, sizeof(vertex)/sizeof(vertex[0]),indexArray, sizeof(indexArray)/sizeof(indexArray[0]));
    //printf("Mesh finnished\n");
    return mesh;
}

struct up_mesh *up_cogWheel()
{
    /// setup the vertexs and the tex coords, this is done like this for debbuging reasons
    // texture coordinates, 0,0 is bottom left, 1,1 is top right
    struct up_vec2 tex[] = {
        {0.0f, 0.0f},
        {1.0f, 0.0f},
        {0.0f, 1.0f},
        {1.0f, 1.0f}
    };
    
    // this is the posisions of the vertexes
    struct up_vec3 pos[] = {
        {1.5f, -2.1f, 0.2f},   // bottomleft 0
        {2.1f, -2.1f, 0.2f},   //bottom right 1
        {1.5f, -1.6f, 0.2f},  //topleft 2
        {2.1f, -1.6f, 0.2f}   //topright 3

    };
    
    
    unsigned int indexArray[] = {1,2,0,1,3,2};  //binds togheter two triangels into one square
    
    // left over from debugging. fills the vertex array with pos and tex
    struct up_vertex vertex[4];
    int i = 0;
    for (i = 0; i < 4; i++) {
        vertex[i].pos = pos[i];
        vertex[i].texCoord = tex[i];
        vertex[i].normals.x = 0;
        vertex[i].normals.y = 0;
        vertex[i].normals.z = 0;
    }
    /////////////
    printf("vertex start: ");
    struct up_mesh *mesh = UP_mesh_new(vertex, sizeof(vertex)/sizeof(vertex[0]),indexArray, sizeof(indexArray)/sizeof(indexArray[0]));
    //printf("Mesh finnished\n");
    return mesh;
}

struct up_mesh *up_settingsOverlay()
{
    /// setup the vertexs and the tex coords, this is done like this for debbuging reasons
    // texture coordinates, 0,0 is bottom left, 1,1 is top right
    struct up_vec2 tex[] = {
        {0.0f, 0.0f},
        {1.0f, 0.0f},
        {0.0f, 1.0f},
        {1.0f, 1.0f}
    };
    
    // this is the posisions of the vertexes
    struct up_vec3 pos[] = {
        {-0.2f, -0.3f, 0.2f},   // bottomleft 0
        {0.2f, -0.3f, 0.2f},   //bottom right 1
        {-0.2f, 0.3f, 0.2f},  //topleft 2
        {0.2f, 0.3f, 0.2f}   //topright 3
    };
    
    
    unsigned int indexArray[] = {1,2,0,1,3,2};  //binds togheter two triangels into one square
    
    // left over from debugging. fills the vertex array with pos and tex
    struct up_vertex vertex[4];
    int i = 0;
    for (i = 0; i < 4; i++) {
        vertex[i].pos = pos[i];
        vertex[i].texCoord = tex[i];
        vertex[i].normals.x = 0;
        vertex[i].normals.y = 0;
        vertex[i].normals.z = 0;
    }
    /////////////
    printf("vertex start: ");
    struct up_mesh *mesh = UP_mesh_new(vertex, sizeof(vertex)/sizeof(vertex[0]),indexArray, sizeof(indexArray)/sizeof(indexArray[0]));
    //printf("Mesh finnished\n");
    return mesh;
}

struct up_mesh *up_keybindingsOverlay()
{
    /// setup the vertexs and the tex coords, this is done like this for debbuging reasons
    // texture coordinates, 0,0 is bottom left, 1,1 is top right
    struct up_vec2 tex[] = {
        {0.0f, 0.0f},
        {1.0f, 0.0f},
        {0.0f, 1.0f},
        {1.0f, 1.0f}
    };
    
    // this is the posisions of the vertexes
    struct up_vec3 pos[] = {
        {-0.36f, -0.89f, 0.2f},   // bottomleft 0
        {0.36f, -0.89f, 0.2f},   //bottom right 1
        {-0.36f, 0.72f, 0.2f},  //topleft 2
        {0.36f, 0.72f, 0.2f}   //topright 3
    };
    
    
    unsigned int indexArray[] = {1,2,0,1,3,2};  //binds togheter two triangels into one square
    
    // left over from debugging. fills the vertex array with pos and tex
    struct up_vertex vertex[4];
    int i = 0;
    for (i = 0; i < 4; i++) {
        vertex[i].pos = pos[i];
        vertex[i].texCoord = tex[i];
        vertex[i].normals.x = 0;
        vertex[i].normals.y = 0;
        vertex[i].normals.z = 0;
    }
    /////////////
    printf("vertex start: ");
    struct up_mesh *mesh = UP_mesh_new(vertex, sizeof(vertex)/sizeof(vertex[0]),indexArray, sizeof(indexArray)/sizeof(indexArray[0]));
    //printf("Mesh finnished\n");
    return mesh;
}


struct up_mesh *up_connectionOverlay(float Ymax, float Ymin)
{
    /// setup the vertexs and the tex coords, this is done like this for debbuging reasons
    // texture coordinates, 0,0 is bottom left, 1,1 is top right
    struct up_vec2 tex[] = {
        {0.0f, Ymin},
        {1.0f, Ymin},
        {0.0f, Ymax},
        {1.0f, Ymax}
    };
    
    // this is the posisions of the vertexes
    struct up_vec3 pos[] = {
        {-0.35f, -0.2f, 0.2f},   // bottomleft 0
        {0.35f, -0.2f, 0.2f},   //bottom right 1
        {-0.35f, 0.2f, 0.2f},  //topleft 2
        {0.35f, 0.2f, 0.2f}   //topright 3
    };
    
    
    unsigned int indexArray[] = {1,2,0,1,3,2};  //binds togheter two triangels into one square
    
    // left over from debugging. fills the vertex array with pos and tex
    struct up_vertex vertex[4];
    int i = 0;
    for (i = 0; i < 4; i++) {
        vertex[i].pos = pos[i];
        vertex[i].texCoord = tex[i];
        vertex[i].normals.x = 0;
        vertex[i].normals.y = 0;
        vertex[i].normals.z = 0;
    }
    /////////////
    printf("vertex start: ");
    struct up_mesh *mesh = UP_mesh_new(vertex, sizeof(vertex)/sizeof(vertex[0]),indexArray, sizeof(indexArray)/sizeof(indexArray[0]));
    //printf("Mesh finnished\n");
    return mesh;
}

struct up_mesh *up_registerOverlay(float Ymax, float Ymin)
{
    /// setup the vertexs and the tex coords, this is done like this for debbuging reasons
    // texture coordinates, 0,0 is bottom left, 1,1 is top right
    struct up_vec2 tex[] = {
        {0.0f, Ymin},
        {1.0f, Ymin},
        {0.0f, Ymax},
        {1.0f, Ymax}
    };
    
    // this is the posisions of the vertexes
    struct up_vec3 pos[] = {
        {-0.35f, -0.2f, 0.2f},   // bottomleft 0
        {0.35f, -0.2f, 0.2f},   //bottom right 1
        {-0.35f, 0.2f, 0.2f},  //topleft 2
        {0.35f, 0.2f, 0.2f}   //topright 3
    };
    
    
    unsigned int indexArray[] = {1,2,0,1,3,2};  //binds togheter two triangels into one square
    
    // left over from debugging. fills the vertex array with pos and tex
    struct up_vertex vertex[4];
    int i = 0;
    for (i = 0; i < 4; i++) {
        vertex[i].pos = pos[i];
        vertex[i].texCoord = tex[i];
        vertex[i].normals.x = 0;
        vertex[i].normals.y = 0;
        vertex[i].normals.z = 0;
    }
    /////////////
    printf("vertex start: ");
    struct up_mesh *mesh = UP_mesh_new(vertex, sizeof(vertex)/sizeof(vertex[0]),indexArray, sizeof(indexArray)/sizeof(indexArray[0]));
    //printf("Mesh finnished\n");
    return mesh;
}


struct up_mesh *up_shipSelection(float Ymax, float Ymin, float Xposmax, float Xposmin)
{
    /// setup the vertexs and the tex coords, this is done like this for debbuging reasons
    // texture coordinates, 0,0 is bottom left, 1,1 is top right
    struct up_vec2 tex[] = {
        {0.0f, Ymin},
        {1.0f, Ymin},
        {0.0f, Ymax},
        {1.0f, Ymax}
    };
    
    // this is the posisions of the vertexes
    struct up_vec3 pos[] = {
        {Xposmin, -0.2f, 0.2f},   // bottomleft 0
        {Xposmax, -0.2f, 0.2f},   //bottom right 1
        {Xposmin, 0.2f, 0.2f},  //topleft 2
        {Xposmax, 0.2f, 0.2f}   //topright 3
    };
    
    
    unsigned int indexArray[] = {1,2,0,1,3,2};  //binds togheter two triangels into one square
    
    // left over from debugging. fills the vertex array with pos and tex
    struct up_vertex vertex[4];
    int i = 0;
    for (i = 0; i < 4; i++) {
        vertex[i].pos = pos[i];
        vertex[i].texCoord = tex[i];
        vertex[i].normals.x = 0;
        vertex[i].normals.y = 0;
        vertex[i].normals.z = 0;
    }
    /////////////
    printf("vertex start: ");
    struct up_mesh *mesh = UP_mesh_new(vertex, sizeof(vertex)/sizeof(vertex[0]),indexArray, sizeof(indexArray)/sizeof(indexArray[0]));
    //printf("Mesh finnished\n");
    return mesh;
}