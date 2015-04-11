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

struct up_mesh *UP_mesh_new(struct up_vertex *vertex, int vertex_count)
{
	printf("Vertex count %d\n", vertex_count);
    printf("Internal internal_state.mesh_count :%d \n",internal_state.mesh_count);
    printf("Internal internal_state.mesh_size :%d \n",internal_state.mesh_size);
	if(internal_state.mesh_count >= internal_state.mesh_size)
	{
		UP_ERROR_MSG("Full mesh \n");
	}
	struct up_mesh *mesh = &(internal_state.mesh[internal_state.mesh_count]);
	mesh->vertex_count = vertex_count;

    printf("mesh->vertex_count:%d \n",mesh->vertex_count);

	glGenVertexArrays(1, &(mesh->vertexArrayObj));
	printf("after genvertex \n");
	glBindVertexArray(mesh->vertexArrayObj);

	glGenBuffers(MESH_BUFFER_COUNT, mesh->vertexArrayBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->vertexArrayBuffer[MESH_BUFFER_VB]);

    printf("before glbuffer bind \n");
	glBufferData(GL_ARRAY_BUFFER, mesh->vertex_count * sizeof(vertex[0]), vertex,GL_STATIC_DRAW);
    printf("after glbuffer bind \n");
	glEnableVertexAttribArray(0);
	//vilken attribuit array, hurmånga element, vad för typ, hur mycket som ska skippas mellan varje
	// och vilken offset från starten
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);


	glBindVertexArray(0);
	internal_state.mesh_count++;
	printf("Internal internal_state.mesh_count :%d \n",internal_state.mesh_count);
    return mesh;
}


void up_draw_mesh(struct up_mesh *mesh)
{
	glBindVertexArray(mesh->vertexArrayObj);

	glDrawArrays(GL_TRIANGLES,0,mesh->vertex_count);

	glBindVertexArray(0);	//fri gör
}








