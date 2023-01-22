
#include "opengldisplay.hpp"

#include <clocale>

#define GL_GLEXT_PROTOTYPES
#include <GL/glut.h>
#include <GL/freeglut_ext.h> 
#include <glm/glm.hpp>  
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


void displayinit( int argc, char **argv ) {
  setlocale( LC_ALL, "" ) ;

  glutInit(&argc,argv);
 
  //  Request double buffered true color window with Z-buffer
  glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB );
 
  // Create window
  glutCreateWindow( "Maze" );

  glutReshapeFunc(reshape);
  glutMouseFunc(mousebutton);
  glutMotionFunc(mousemove) ;
  glutKeyboardFunc(keyboard);

  glutReshapeWindow( 500, 500 );
 
  // Enable Z-buffer depth test
  glEnable(GL_DEPTH_TEST);

  initTriangles() ;

  // Callback functions
  glutDisplayFunc(display);

  // Pass control to GLUT for events
  glutMainLoop(); 
}

