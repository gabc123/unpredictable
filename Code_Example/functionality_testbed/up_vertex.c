#include "up_vertex.h"
#include <stdio.h>
#include <stdlib.h>
#include "up_error.h"

struct internal_object_tracker
{
	struct up_mesh *mesh;
	int mesh_count;	// how many meshes we currently have
	int mesh_size;	// the maximum number of meshes
};


static struct internal_object_tracker internal_state;

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

struct up_mesh *UP_mesh_new(struct up_vertex *vertex, int vertex_count,unsigned int *indexArray,int index_count)
{
	printf("Vertex count %d\n", vertex_count);
    printf("Internal internal_state.mesh_count :%d \n",internal_state.mesh_count);
    printf("Internal internal_state.mesh_size :%d \n",internal_state.mesh_size);
	if(internal_state.mesh_count >= internal_state.mesh_size)
	{
		UP_ERROR_MSG("Full mesh \n");
	}
	//struct up_mesh *mesh = malloc(sizeof(struct up_mesh));
	struct up_mesh *mesh =&(internal_state.mesh[internal_state.mesh_count]);
	mesh->vertex_count = vertex_count;
    mesh->index_count = index_count;
    printf("mesh->vertex_count:%d \n",mesh->vertex_count);



    struct up_vec3 *positions = malloc(sizeof(struct up_vec3) * mesh->vertex_count);
    if (positions == NULL) {
        UP_ERROR_MSG("malloc failed, positions");
        return NULL;
    }
    printf("after pos coord malloc\n");

    struct up_vec2 *textureCoord = malloc(sizeof(struct up_vec2) * mesh->vertex_count);
    if (textureCoord == NULL) {
        UP_ERROR_MSG("malloc failed, textureCoord");
        return NULL;
    }
    printf("after text coord malloc\n");

    int i = 0;
    for (i = 0; i< mesh->vertex_count; i++) {
        positions[i] = vertex[i].pos;
        // Thx to SDL_image, the texture will appeare upsidedown
        // this correct that problem
        textureCoord[i].x = vertex[i].texCoord.x;
        textureCoord[i].y = 1 - vertex[i].texCoord.y;
    }
    printf("after vertex segmetion malloc\n");
    glGenVertexArrays(1, &(mesh->vertexArrayObj));
    printf("after genvertex \n");
    glBindVertexArray(mesh->vertexArrayObj);

	glGenBuffers(MESH_BUFFER_COUNT, mesh->vertexArrayBuffer);

    // binding the position
	glBindBuffer(GL_ARRAY_BUFFER, mesh->vertexArrayBuffer[MESH_BUFFER_VB]);
	glBufferData(GL_ARRAY_BUFFER, mesh->vertex_count * sizeof(positions[0]), &positions[0],GL_STATIC_DRAW);

    //vilken attribuit array, hurmånga element, vad för typ, hur mycket som ska skippas mellan varje
    // och vilken offset från starten
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);


    // binding the texture coordenates
    glBindBuffer(GL_ARRAY_BUFFER, mesh->vertexArrayBuffer[MESH_BUFFER_TXB]);
    glBufferData(GL_ARRAY_BUFFER, mesh->vertex_count * sizeof(textureCoord[0]), &textureCoord[0],GL_STATIC_DRAW);

    //vilken attribuit array, hurmånga element, vad för typ, hur mycket som ska skippas mellan varje
    // och vilken offset från starten
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);

    // binding the indecies
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->vertexArrayBuffer[MESH_BUFFER_INDEX]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->index_count * sizeof(indexArray[0]), &indexArray[0],GL_STATIC_DRAW);

    glBindVertexArray(0);


	internal_state.mesh_count++;
    free(positions);
    free(textureCoord);
	printf("Internal internal_state.mesh_count :%d \n",internal_state.mesh_count);
    return mesh;
}


void up_draw_mesh(struct up_mesh *mesh)
{
	glBindVertexArray(mesh->vertexArrayObj);

	//glDrawArrays(GL_TRIANGLES,0,mesh->vertex_count);
    glDrawElementsBaseVertex(GL_TRIANGLES, mesh->index_count, GL_UNSIGNED_INT, 0, 0);

	glBindVertexArray(0);	//fri gör
}








