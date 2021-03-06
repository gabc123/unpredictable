#include "up_math.h"
#include "up_assets.h"
#include "up_graphics_setup.h"
#include "up_graphics_update.h"
#include <stdio.h>
#include <stdlib.h>
#include "up_error.h"

// based on tutorials from
//https://youtu.be/csKrVBWCItc?list=PLEETnX-uPtBXT9T-hD0Bj31DSnwio-ywh
//http://ogldev.atspace.co.uk/www/tutorial17/tutorial17.html
//http://ogldev.atspace.co.uk/www/tutorial18/tutorial18.html
//https://youtu.be/0jML5fMBDGg?list=PLEETnX-uPtBXT9T-hD0Bj31DSnwio-ywh

struct internal_object_tracker
{
	struct up_mesh *mesh;
	int mesh_count;	// how many meshes we currently have
	int mesh_size;	// the maximum number of meshes
};


static struct internal_object_tracker internal_state;
// magnus
// allocate internal memory to keep track of all meshes in the game
void up_mesh_start_setup(int maximum_meshes)
{
	struct up_mesh *mesh = malloc(sizeof(struct up_mesh) * maximum_meshes);
	if(mesh == NULL)
	{
		UP_ERROR_MSG("malloc failwhen allocating mesh \n");
	}
	internal_state.mesh = mesh;
	internal_state.mesh_count = 0;
	internal_state.mesh_size = maximum_meshes;
}


// magnus
void up_mesh_shutdown_deinit()
{
	// TODO: fix for more buffers
	glDeleteBuffers(MESH_BUFFER_COUNT,internal_state.mesh[0].vertexArrayBuffer);
	glDeleteVertexArrays(1,&(internal_state.mesh[0].vertexArrayObj));

	///
	internal_state.mesh_size = 0;
	internal_state.mesh_count = 0;
	free(internal_state.mesh);
}

//magnus
struct up_mesh *UP_mesh_new(struct up_vertex *vertex, int vertex_count,unsigned int *indexArray,int index_count)
{
	
    printf("Internal mesh_count :%d/%d \n",internal_state.mesh_count,internal_state.mesh_size);
    printf("Vertex count %d Loading", vertex_count);
    if(internal_state.mesh_count >= internal_state.mesh_size)
	{
		UP_ERROR_MSG("Full mesh \n");
	}
	//struct up_mesh *mesh = malloc(sizeof(struct up_mesh));
	struct up_mesh *mesh =&(internal_state.mesh[internal_state.mesh_count]);
	mesh->vertex_count = vertex_count;
    mesh->index_count = index_count;
    mesh->drawMode = GL_TRIANGLES;
    printf(".");



    // we want to load position , texture, and normals, seperatly ,
    // allocate room
    
    struct up_vec3 *positions = malloc(sizeof(struct up_vec3) * mesh->vertex_count);
    if (positions == NULL) {
        UP_ERROR_MSG("malloc failed, positions");
        return NULL;
    }
    printf(".");

    struct up_vec2 *textureCoord = malloc(sizeof(struct up_vec2) * mesh->vertex_count);
    if (textureCoord == NULL) {
        free(positions);
        UP_ERROR_MSG("malloc failed, textureCoord");
        return NULL;
    }
    printf(".");

    struct up_vec3 *normals = malloc(sizeof(struct up_vec3) * mesh->vertex_count);
    if (normals == NULL) {
        free(positions);
        free(textureCoord);
        UP_ERROR_MSG("malloc failed, textureCoord");
        return NULL;
    }
    printf(".");
    
    // transfer the relevent data from vertex into the allocated memory
    int i = 0;
    for (i = 0; i< mesh->vertex_count; i++) {
        positions[i] = vertex[i].pos;
        // Thx to SDL_image, the texture will appeare upsidedown
        // this correct that problem
        textureCoord[i].x = vertex[i].texCoord.x;
        textureCoord[i].y = 1 - vertex[i].texCoord.y;
        normals[i] = vertex[i].normals;
    }
    // tell the gpu to make room, and generate ids to acces it
    GLuint vertexBufferId[MESH_BUFFER_COUNT];
    printf(".");
    glGenVertexArrays(1, &mesh->vertexArrayObj);
    printf(".");
    glBindVertexArray(mesh->vertexArrayObj);

    
	glGenBuffers(MESH_BUFFER_COUNT, vertexBufferId);

    // now trensfer the ids, for the diffrent buffers on the gpu for this model
    // this is done to acces later when we are going to use them
    mesh->positionId = vertexBufferId[0];
    mesh->textureCoordId = vertexBufferId[1];
    mesh->normalId = vertexBufferId[2];
    mesh->indicesId = vertexBufferId[3];
    
    // binding the position
	glBindBuffer(GL_ARRAY_BUFFER, mesh->positionId);
	glBufferData(GL_ARRAY_BUFFER, mesh->vertex_count * sizeof(positions[0]), &positions[0],GL_STATIC_DRAW);

    //vilken attribuit array, hurmånga element, vad för typ, hur mycket som ska skippas mellan varje
    // och vilken offset från starten
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);


    // binding the texture coordenates
    glBindBuffer(GL_ARRAY_BUFFER, mesh->textureCoordId);
    glBufferData(GL_ARRAY_BUFFER, mesh->vertex_count * sizeof(textureCoord[0]), &textureCoord[0],GL_STATIC_DRAW);

    //vilken attribuit array, hurmånga element, vad för typ, hur mycket som ska skippas mellan varje
    // och vilken offset från starten
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
    
    // binding the texture coordenates
    glBindBuffer(GL_ARRAY_BUFFER, mesh->normalId);
    glBufferData(GL_ARRAY_BUFFER, mesh->vertex_count * sizeof(normals[0]), &normals[0],GL_STATIC_DRAW);
    
    //vilken attribuit array, hurmånga element, vad för typ, hur mycket som ska skippas mellan varje
    // och vilken offset från starten
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
    
    // binding the indecies
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->indicesId);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->index_count * sizeof(indexArray[0]), &indexArray[0],GL_STATIC_DRAW);

    glBindVertexArray(0);


	internal_state.mesh_count++;
    free(positions);
    free(textureCoord);
    free(normals);
    printf("\n");
	//printf(" mesh_count :%d \n",internal_state.mesh_count);
    return mesh;
}

// magnus
// used to enable diffrent draw modes
void up_mesh_draw_speciall(struct up_mesh *mesh,int drawMode)
{
    glBindVertexArray(mesh->vertexArrayObj);
    
    //glDrawArrays(GL_TRIANGLES,0,mesh->vertex_count);
    glDrawElementsBaseVertex(drawMode, mesh->index_count, GL_UNSIGNED_INT, 0, 0);
    //glDrawElementsBaseVertex(mesh->drawMode, mesh->index_count, GL_UNSIGNED_INT, 0, 0);
    
    glBindVertexArray(0);	//fri gör
}
//magnus

void up_mesh_draw(struct up_mesh *mesh)
{
	glBindVertexArray(mesh->vertexArrayObj);

	//glDrawArrays(GL_TRIANGLES,0,mesh->vertex_count);
    glDrawElementsBaseVertex(GL_TRIANGLES, mesh->index_count, GL_UNSIGNED_INT, 0, 0);
    //glDrawElementsBaseVertex(mesh->drawMode, mesh->index_count, GL_UNSIGNED_INT, 0, 0);

	glBindVertexArray(0);	//fri gör
}








