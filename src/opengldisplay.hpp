
#pragma once

#include "maze.hpp"
#include "player.hpp"

constexpr float IMG_LIMIT = 100.0;
constexpr float MaxClock = 150.f;
constexpr float dt = 1.f;

extern Maze *maze ;
extern Player *player ;

void displayinit( int argc, char **argv ) ;
void initTriangles() ;

void setCameraPosition();
void drawText() ;
void display() ;
void keyboard(unsigned char key, int x, int y) ;
void reset() ;
void mousemove(int x, int y) ;
void mousebutton(int button, int state, int x, int y) ;
void reshape(int w,int h) ;
void handleSpecialKeyReleased(int key, int x, int y);
void handleSpecialKeypress(int key, int x, int y);
