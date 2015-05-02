#ifndef TESTMODELS_H
#define TESTMODELS_H
#include "up_vertex.h"

struct up_mesh *meshTriangleShip();
struct up_mesh *meshPyramidShip();
struct up_mesh *up_meshMenuBackground();
struct up_mesh *up_meshBotton(float imageX, float imageY, float screenPosX, float screenPosY);
struct up_mesh *up_meshLoginOverlay();

#endif