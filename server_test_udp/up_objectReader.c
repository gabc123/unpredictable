#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "up_error.h"
#include "up_utilities.h"
#include "up_objectReader.h"
#include "up_filereader.h"

void vertexPositionsData(char *rad, struct up_generic_list *vertexPosList);
void texturePositionsData(char *rad, struct up_generic_list *texturePosList);
void normalPositionsData(char *rad, struct up_generic_list *normalPosList);
void objFaces(char *rad, struct up_generic_list *facePosList, struct up_vertex *finalList,struct up_vec3 *vertexPos,struct up_vec2 *texturePos, struct up_vec3 *normalPos, int count);



struct up_objModel *up_loadObjModel(const char *filename)
{
    /*
    Creates lists to save the different values in, one for coordinates, one for textures, one for the normals
    and one for the faces that holds the combination keys. Later on this function sends the values in to a list
    that matches the coordinates, textures and normals to make up a vertex. The function then determines wether
    there are three or four vertex positions to combine into a polygon.

    */

    //Variables to be used by the token parser to determine what to read in.
    char *text;
    char *endLine = "\n";  //we need to take one line at a time. thus '\n'
    char *rad;

    //These are the lists mentioned in the main description
    struct up_generic_list *vertexPosList = up_vec3_list_new(20);
    struct up_generic_list *texturePosList = up_vec2_list_new(20);
    struct up_generic_list *normalPosList = up_vec3_list_new(20);
    struct up_generic_list *facePosList = up_uint_list_new(20);
    struct up_vertex *finalList = NULL;

    struct up_vec2 *tex = NULL;
    struct up_vec3 *pos = NULL;
    struct up_vec3 *norm = NULL;

    int count = 0;

    printf("%s\n", filename);
    struct UP_textHandler objFile = up_loadObjFile(filename);
    if (objFile.text == NULL) {
        UP_ERROR_MSG_STR("Obj faild to load", filename);
        return NULL;
    }

    text=objFile.text;
    do
    {

        rad = up_token_parser(text, &text, endLine, strlen(endLine));
        if(rad == NULL)
        {
            //UP_ERROR_MSG("ERROR, obj String could not be found.");
            printf("vertex read completed for %s\n",filename);
            break;
        }
        if(rad[0]=='v')
        {
            switch (rad[1])
            {
                case ' ':
                    rad+=2; //To only read the vector positions. And removes the v and space.
                    vertexPositionsData(rad, vertexPosList);
                    break;
                case 't':
                    rad+=3;
                    texturePositionsData(rad, texturePosList);
                    break;
                case 'n':
                    rad+=3;
                    normalPositionsData(rad, normalPosList);
                    break;
                default:
                    //Unknown followup letter after the v on this specific line.
                    UP_ERROR_MSG_STR("Unknown identifier, look in obj file ", filename);
                    break;

            }

        }else if (rad[0] == 'f')
        {

            count = up_vec3_list_count(vertexPosList);
            tex = up_vec2_list_transferOwnership(&texturePosList);
            pos = up_vec3_list_transferOwnership(&vertexPosList);
            norm = up_vec3_list_transferOwnership(&normalPosList);
            rad+=2;
            break;

        }

    }while(text <= objFile.text + objFile.length -1 );

    // safty check
    finalList = NULL;
    if (count != 0) {
        finalList = malloc(sizeof(struct up_vertex) * count );
    }
    if(finalList == NULL)
    {
           UP_ERROR_MSG("malloc failed");
           free(tex);
           free(pos);
           free(norm);
           up_textHandler_free(&objFile);
           return NULL;
    }


    int i = 0;

    for(i = 0 ; i< count;i++)
    {
        finalList[i].pos = pos[i];
        //printf("%d: %f %f %f\n",i,pos[i].x, pos[i].y, pos[i].z);
    }
    objFaces(rad, facePosList, finalList,pos,tex,norm,count);
    do
    {
        rad = up_token_parser(text, &text, endLine, strlen(endLine));
        if(rad == NULL)
        {
            //UP_ERROR_MSG("ERROR, obj String could not be found.");
            printf("faces index read completed for %s\n",filename);
            break;
        }

        if (rad[0] == 'f')
        {
            rad+=2;
            objFaces(rad, facePosList, finalList,pos,tex,norm,count);
        }

    }while(text <= objFile.text + objFile.length -1 );

    int num_face = up_uint_list_count(facePosList);
    unsigned int *face_indexList = up_uint_list_transferOwnership(&facePosList);
/*
    for (i =0; i<num_face; i+=3) {
       printf("%d Face: idx: %d %d %d\n",i,face_indexList[i],face_indexList[i+1],face_indexList[i+2]);
    }
*/
    struct up_objModel *result = malloc(sizeof(struct up_objModel));
    if (result == NULL) {
        UP_ERROR_MSG("malloc fail");
    }else
    {
        result->index_length = num_face;
        result->indexArray = face_indexList;
        result->vertex_length = count;
        result->vertex = finalList;
    }

    

    free(pos);
    free(tex);
    free(norm);
    up_textHandler_free(&objFile);
    printf("Model loaded %s\n",filename);
    return result;
}

void up_objModelFree(struct up_objModel *object)
{
    object->index_length = 0;
    object->vertex_length = 0;
    free(object->indexArray);
    free(object->vertex);
}


void vertexPositionsData(char *rad, struct up_generic_list *vertexPosList)
{
    struct up_vec3 vertexPositions;

    sscanf(rad,"%f %f %f",&vertexPositions.x,&vertexPositions.y, &vertexPositions.z);
    //fprintf(stderr, "Vertex. X of obj: %f, Y of obj: %f, Z of obj: %f\n",vertexPositions.x,vertexPositions.y,vertexPositions.z);
    up_vec3_list_add(vertexPosList,&vertexPositions);
}

void texturePositionsData(char *rad, struct up_generic_list *texturePosList)
{
    struct up_vec2 texturePositions;

    sscanf(rad,"%f %f",&texturePositions.x,&texturePositions.y);
    //fprintf(stderr, "Texture. X of obj: %f, Y of obj: %f\n",texturePositions.x,texturePositions.y);
    up_vec2_list_add(texturePosList,&texturePositions);
}

void normalPositionsData(char *rad, struct up_generic_list *normalPosList)
{
    struct up_vec3 normalPositions;

    sscanf(rad,"%f %f %f",&normalPositions.x,&normalPositions.y, &normalPositions.z);
    //fprintf(stderr, "normal. X of obj: %f, Y of obj: %f, Z of obj: %f\n",normalPositions.x,normalPositions.y,normalPositions.z);
    up_vec3_list_add(normalPosList,&normalPositions);
}



void objFaces(char *rad, struct up_generic_list *facePosList, struct up_vertex *finalList,struct up_vec3 *vertexPos,struct up_vec2 *texturePos, struct up_vec3 *normalPos, int count)
{

    int tmp=0;

    //int faceValue1,faceValue3 = 0;//reuse 1,3 with fourth face to create a new polygon

    unsigned int vertexIdx[3];
    unsigned int face[3];

    char *text2 = rad;
    //char *endLine2 = "/";
    char *space = " ";
    //char *rad2;
    char *facegroup;
    //int num=0;
    int i = 0;
    for(i=0;(rad[i]!='\0') && (rad[i] != '\n');i++)
    {
        if (rad[i]==' ')
        {
            tmp++;
        }
    }

    int k =0;
    for(k=0;k<3;k++)
    {
        facegroup = up_token_parser(text2, &text2, space, strlen(space));
        if(facegroup == NULL && k == 2)
        {
            facegroup = text2;
        }
        sscanf(facegroup,"%d/%d/%d", &vertexIdx[0],&vertexIdx[1],&vertexIdx[2]);

        vertexIdx[0]--;
        vertexIdx[1]--;
        vertexIdx[2]--;

        finalList[vertexIdx[0]].texCoord = texturePos[vertexIdx[1]];
        finalList[vertexIdx[0]].normals = normalPos[vertexIdx[2]];

        face[k] = vertexIdx[0];
    }

    up_uint_list_add(facePosList,&face[0]);
    up_uint_list_add(facePosList,&face[1]);
    up_uint_list_add(facePosList,&face[2]);

    if(tmp==3)
    {
        sscanf(text2,"%d/%d/%d", &vertexIdx[0],&vertexIdx[1],&vertexIdx[2]);
        //C arrays starts at 0, objfileintex starts at 1. Those needs to be synched
        vertexIdx[0]--;
        vertexIdx[1]--;
        vertexIdx[2]--;
        finalList[vertexIdx[0]].texCoord = texturePos[vertexIdx[1]];
        finalList[vertexIdx[0]].normals = normalPos[vertexIdx[2]];

        up_uint_list_add(facePosList,&face[0]);
        up_uint_list_add(facePosList,&face[2]);
        up_uint_list_add(facePosList,&vertexIdx[0]);
        //fprintf(stderr, "\n%d %d %d", face[0],face[2],vertexIdx[0]);


    }

   // fprintf(stderr, "\n");
        /**
        up_uint_list_add(facePosList, &faceValue1);
        up_uint_list_add(facePosList, &faceValue3);
        rad2 = up_token_parser(text2, &text2, endLine2, strlen(endLine2));
        sscanf(rad2,"%d", correctNumbers);
        correctNumbers - 1;
        up_uint_list_add(facePosList, &correctNumbers);
        **/

}
