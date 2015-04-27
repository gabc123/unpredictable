#include "testmodels.h"
#include "up_utilities.h"
#include <stdlib.h>
#include <stdio.h>

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
    
    unsigned int indexArray[] = {0,2,1,2,3,1,0,3,2,3,0,1};
    
    // left over from debugging. fills the vertex array with pos and tex
    struct up_vertex vertex[4];
    int i = 0;
    for (i = 0; i < 4; i++) {
        vertex[i].pos = pos[i];
        vertex[i].texCoord = tex[i];
    }
    /////////////
    printf("vertex start\n");
    struct up_mesh *mesh = UP_mesh_new(vertex, sizeof(vertex)/sizeof(vertex[0]),indexArray, sizeof(indexArray)/sizeof(indexArray[0]));
    printf("Mesh finnished\n");
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
        up_vertex_list_add(list, &tmp_vertex);
    }
    struct up_vertex *vertex = up_vertex_list_transferOwnership(&list);
    
    /////////////
    printf("vertex start\n");
    struct up_mesh *mesh = UP_mesh_new(vertex, pyramid_pos_size,pyramid_indexArray, pyramid_index_size);
    printf("Mesh finnished\n");
    free(vertex);
    return mesh;
}
