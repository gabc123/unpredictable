#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "up_error.h"
#include "up_utilities.h"
#include "up_objectReader.h"
#include "filereader/up_filereader.h"

void vertexPositionsData(char *rad, struct up_generic_list *vertexPosList);
void texturePositionsData(char *rad, struct up_generic_list *texturePosList);
void normalPositionsData(char *rad, struct up_generic_list *normalPosList);
void objFaces(char *rad, struct up_generic_list *facePosList, struct up_vertex *finalList,struct up_vec3 *vertexPos,struct up_vec2 *texturePos, struct up_vec3 *normalPos, int count);

struct up_objModel *up_loadObjModel(const char *filename)
{
    char *text;
    char *endLine = "\n";
    char *rad;
    int tmp=0;
    struct up_generic_list *vertexPosList = up_vec3_list_new(20);
    struct up_generic_list *texturePosList = up_vec2_list_new(20);
    struct up_generic_list *normalPosList = up_vec3_list_new(20);
    struct up_generic_list *facePosList = up_int_list_new(20);
    struct up_vertex *finalList = NULL;

    struct up_vec2 *tex = NULL;
    struct up_vec3 *pos = NULL;
    struct up_vec3 *norm = NULL;

    int count = 0;

    struct UP_textHandler objFile = up_loadObjFile(filename);
    text=objFile.text;
    do
    {


        rad = up_token_parser(text, &text, endLine, strlen(endLine));
        if(rad == NULL)
        {
            UP_ERROR_MSG("ERROR, obj String could not be found.");
            break;
        }
        if(rad[0]=='v')
        {
            switch (rad[1])
            {
                case ' ':
                    rad+=2; //To only read the vector positions.
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

            /**
                TODO: parse the index array
                It is posible to use up_token_parser(rad,...) and instead of looking for endline
                look for // or /, there by dividing up the 3 v/vt/vn indexes
                then use a  up_vertex_list and populate it so the v,vt,vn is inside the same one

             **/
        }

    }while(text <= objFile.text + objFile.length -1 );


    finalList = malloc(sizeof(struct up_vertex) * count );
    if(finalList == NULL)
    {
           UP_ERROR_MSG("malloc failed");
           free(tex);
           free(pos);
           free(norm);
           return NULL;
    }

    objFaces(rad, facePosList, finalList,pos,tex,norm,count);
    int i = 0;

    for(i = 0 ; i< count;i++)
    {
        finalList[i].pos = pos[i];
    }

    do
    {
        rad = up_token_parser(text, &text, endLine, strlen(endLine));
        if(rad == NULL)
        {
            UP_ERROR_MSG("ERROR, obj String could not be found.");
            break;
        }

        if (rad[0] == 'f')
        {
            rad+=2;
            objFaces(rad, facePosList, finalList,pos,tex,norm,count);
            /**
                TODO: parse the index array
                It is posible to use up_token_parser(rad,...) and instead of looking for endline
                look for // or /, there by dividing up the 3 v/vt/vn indexes
                then use a  up_vertex_list and populate it so the v,vt,vn is inside the same one

             **/
        }

    }while(text <= objFile.text + objFile.length -1 );

    return 0;
}

void up_objModelFree(struct up_objModel *object)
{

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

    int vertexIdx[3];
    int face[3];

    char *text2 = rad;
    //char *endLine2 = "/";
    char *space = " ";
    //char *rad2;
    char *facegroup;
    //int num=0;
    int i = 0;
    for(i=0;rad[i]!='\0';i++)
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

    up_int_list_add(facePosList,&face[0]);
    up_int_list_add(facePosList,&face[1]);
    up_int_list_add(facePosList,&face[2]);

    if(tmp==3)
    {
        sscanf(text2,"%d/%d/%d", &vertexIdx[0],&vertexIdx[1],&vertexIdx[2]);
        vertexIdx[0]--;
        vertexIdx[1]--;
        vertexIdx[2]--;
        finalList[vertexIdx[0]].texCoord = texturePos[vertexIdx[1]];
        finalList[vertexIdx[0]].normals = normalPos[vertexIdx[2]];

        up_int_list_add(facePosList,&face[0]);
        up_int_list_add(facePosList,&face[2]);
        up_int_list_add(facePosList,&vertexIdx[0]);

    }


        /**
        up_int_list_add(facePosList, &faceValue1);
        up_int_list_add(facePosList, &faceValue3);
        rad2 = up_token_parser(text2, &text2, endLine2, strlen(endLine2));
        sscanf(rad2,"%d", correctNumbers);
        correctNumbers - 1;
        up_int_list_add(facePosList, &correctNumbers);
        **/

}



