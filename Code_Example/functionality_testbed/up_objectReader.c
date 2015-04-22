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

struct up_objModel *up_loadObjModel(const char *filename)
{
    char *text;
    char *endLine = "\n";
    char *rad;
    struct up_generic_list *vertexPosList = up_vec3_list_new(20);
    struct up_generic_list *texturePosList = up_vec2_list_new(20);
    struct up_generic_list *normalPosList = up_vec3_list_new(20);

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
    fprintf(stderr, "Vertex. X of obj: %f, Y of obj: %f, Z of obj: %f\n",vertexPositions.x,vertexPositions.y,vertexPositions.z);
}

void texturePositionsData(char *rad, struct up_generic_list *texturePosList)
{
    struct up_vec2 texturePositions;

    sscanf(rad,"%f %f",&texturePositions.x,&texturePositions.y);
    fprintf(stderr, "Texture. X of obj: %f, Y of obj: %f\n",texturePositions.x,texturePositions.y);
}

void normalPositionsData(char *rad, struct up_generic_list *normalPosList)
{
    struct up_vec3 normalPositions;

    sscanf(rad,"%f %f %f",&normalPositions.x,&normalPositions.y, &normalPositions.z);
    fprintf(stderr, "normal. X of obj: %f, Y of obj: %f, Z of obj: %f\n",normalPositions.x,normalPositions.y,normalPositions.z);
}
