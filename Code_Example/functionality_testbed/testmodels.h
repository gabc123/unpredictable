#ifndef TESTMODELS_H
#define TESTMODELS_H
#include "up_vertex.h"

struct up_mesh *meshTriangleShip();
struct up_mesh *meshPyramidShip();
struct up_mesh *up_meshMenuBackground();
struct up_mesh *up_meshBotton(float imageX, float imageY, float screenPosX, float screenPosY);
struct up_mesh *up_mesh_menu_Botton();
struct up_mesh *up_meshLoginOverlay(float maximumY, float minimumY);
struct up_mesh *up_meshQuitwindow();
struct up_mesh *up_cogWheel();
struct up_mesh *up_settingsOverlay();
struct up_mesh *up_keybindingsOverlay();
struct up_mesh *up_connectionOverlay(float Ymax, float Ymin);
struct up_mesh *up_registerOverlay(float Ymax, float Ymin);

#endif