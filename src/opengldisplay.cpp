

#include <stdio.h>
#include <vector>

#define GL_GLEXT_PROTOTYPES
#include <GL/freeglut_std.h>
#include <GL/freeglut_ext.h>
#include <GL/glut.h>
#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "opengldisplay.hpp"

using namespace std ;

float anglex;
float angley;
float radius;

float prev_x;
float prev_y;

float t = 1000000.f;

unsigned int VAO;
unsigned int VBO;
GLuint EBO;

unsigned int vertexShader;
unsigned int fragmentShader;
unsigned int shaderProgram;
int numberOfWalls ;

constexpr int FloatsPerVertex = 6 ;
constexpr int VerticesPerGridPoint = 2 ;
constexpr int TrianglesPerWall = 2 ;


void initTriangles() {

    const char *vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "layout (location = 1) in vec3 aCol;\n"
    "uniform mat4 uProjection;\n"
    "uniform mat4 uView;\n"
    "out vec4 vColor;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = uProjection * uView * vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "   vColor = vec4( aCol.x, aCol.y, aCol.z, 1.0);\n"
    "}";

    const char *fragmentShaderSource = "#version 330 core\n"
    "out vec4 vFragColor;\n"
    "in vec4 vColor;\n"
    "void main()\n"
    "{\n"
    "   vFragColor = vColor;\n"
    "}";


    // array of a 2 vertices at each corner of grid
    // 1 at floor level and 1 at ceiling
    vector<float> allVertices;  
    int numGridPoints = (maze->M+1) * (maze->N+1);
    allVertices.reserve( numGridPoints * FloatsPerVertex * VerticesPerGridPoint + (FloatsPerVertex*4) );

    float r = static_cast <float>( rand() ) / static_cast <float>( RAND_MAX ) ; 
    float g = static_cast <float>( rand() ) / static_cast <float>( RAND_MAX ) ; 
    float b = static_cast <float>( rand() ) / static_cast <float>( RAND_MAX ) ;

    for( int m=0 ; m<=maze->M ; m++ ) {
        for( int n=0 ; n<=maze->N ; n++ ) {
            r += static_cast <float>( rand() ) / static_cast <float>( RAND_MAX ) / 50.f ; 
            g += static_cast <float>( rand() ) / static_cast <float>( RAND_MAX ) / 50.f ; 
            b += static_cast <float>( rand() ) / static_cast <float>( RAND_MAX ) / 50.f ;
            if( r>= 1.f ) r -= 1.f ;
            if( g>= 1.f ) g -= 1.f ;
            if( b>= 1.f ) b -= 1.f ;

            allVertices.push_back( 10.f*n ) ;   // x
            allVertices.push_back( 0.f ) ;      // y
            allVertices.push_back( 10.f*m ) ;   // z
            allVertices.push_back( r ) ;        // color r
            allVertices.push_back( g ) ;        // color g
            allVertices.push_back( b ) ;        // color b

            r += static_cast <float>( rand() ) / static_cast <float>( RAND_MAX ) / 50.f ; 
            g += static_cast <float>( rand() ) / static_cast <float>( RAND_MAX ) / 50.f ; 
            b += static_cast <float>( rand() ) / static_cast <float>( RAND_MAX ) / 50.f ;
            if( r>= 1.f ) r -= 1.f ;
            if( g>= 1.f ) g -= 1.f ;
            if( b>= 1.f ) b -= 1.f ;

            allVertices.push_back( 10.f*n ) ;   // x
            allVertices.push_back( 10.f ) ;     // y
            allVertices.push_back( 10.f*m ) ;   // z
            allVertices.push_back( r ) ;        // color
            allVertices.push_back( g ) ;        // color
            allVertices.push_back( b ) ;        // color
        }
    }

    int startOfFloor = numGridPoints * 2;
    // define floor == full plate over floor
    allVertices.push_back( 0.f ) ;   // x
    allVertices.push_back( 0.f ) ;   // y
    allVertices.push_back( 0.f ) ;   // z
    allVertices.push_back( .2f ) ;        // color r
    allVertices.push_back( .2f ) ;        // color g
    allVertices.push_back( .2f ) ;        // color b

    allVertices.push_back( 10.f*maze->N ) ;   // x
    allVertices.push_back( 0.f ) ;            // y
    allVertices.push_back( 0.f ) ;   // z
    allVertices.push_back( .5f ) ;        // color r
    allVertices.push_back( .5f ) ;        // color g
    allVertices.push_back( .5f ) ;        // color b

    allVertices.push_back( 10.f*maze->N ) ;   // x
    allVertices.push_back( 0.f ) ;            // y
    allVertices.push_back( 10.f*maze->M ) ;   // z
    allVertices.push_back( .8f ) ;        // color r
    allVertices.push_back( .8f ) ;        // color g
    allVertices.push_back( .8f ) ;        // color b

    allVertices.push_back( 0 ) ;   // x
    allVertices.push_back( 0.f ) ;            // y
    allVertices.push_back( 10.f*maze->M ) ;   // z
    allVertices.push_back( .5f ) ;        // color r
    allVertices.push_back( .5f ) ;        // color g
    allVertices.push_back( .5f ) ;        // color b

    // define ceiling == full plate over floor
    allVertices.push_back( 0.f ) ;   // x
    allVertices.push_back( 10.f ) ;   // y
    allVertices.push_back( 0.f ) ;   // z
    allVertices.push_back( .2f ) ;        // color r
    allVertices.push_back( .2f ) ;        // color g
    allVertices.push_back( .4f ) ;        // color b

    allVertices.push_back( 10.f*maze->N ) ;   // x
    allVertices.push_back( 10.f ) ;            // y
    allVertices.push_back( 0.f ) ;   // z
    allVertices.push_back( .2f ) ;        // color r
    allVertices.push_back( .2f ) ;        // color g
    allVertices.push_back( .4f ) ;        // color b

    allVertices.push_back( 10.f*maze->N ) ;   // x
    allVertices.push_back( 10.f ) ;            // y
    allVertices.push_back( 10.f*maze->M ) ;   // z
    allVertices.push_back( .2f ) ;        // color r
    allVertices.push_back( .2f ) ;        // color g
    allVertices.push_back( .4f ) ;        // color b

    allVertices.push_back( 0 ) ;   // x
    allVertices.push_back( 10.f ) ;            // y
    allVertices.push_back( 10.f*maze->M ) ;   // z
    allVertices.push_back( .2f ) ;        // color r
    allVertices.push_back( .2f ) ;        // color g
    allVertices.push_back( .4f ) ;        // color b

    //
    //   Defines points for East-West wall and North-South wall
    //
    //      x .. E
    //      .
    //      .
    //      S
    //
    vector<unsigned short> allIndices;      
    allIndices.reserve( (maze->M) * (maze->N) * 12 + 6 ); // max number of walls

    allIndices.push_back( startOfFloor ) ;
    allIndices.push_back( startOfFloor+3 ) ;
    allIndices.push_back( startOfFloor+1 ) ;
    allIndices.push_back( startOfFloor+1 ) ;
    allIndices.push_back( startOfFloor+3 ) ;
    allIndices.push_back( startOfFloor+2 ) ;

    allIndices.push_back( startOfFloor+4 ) ;
    allIndices.push_back( startOfFloor+7 ) ;
    allIndices.push_back( startOfFloor+5 ) ;
    allIndices.push_back( startOfFloor+5 ) ;
    allIndices.push_back( startOfFloor+7 ) ;
    allIndices.push_back( startOfFloor+6 ) ;

    numberOfWalls = 2 ;
    unsigned short row_length = 2 * (maze->N+1) ;

    for( unsigned short m=0 ; m<maze->M ; m++ ) {
        for( unsigned short n=0 ; n<maze->N ; n++ ) {
         
            unsigned short x0 = m * row_length + n*2 ;
            unsigned short x1 = x0 + 2 ;
            unsigned short x2 = x0 + row_length ;

            if( maze->isWallInFront( North, m, n ) ) {
                allIndices.push_back( x0+1 ) ;
                allIndices.push_back( x0 ) ;
                allIndices.push_back( x1+1 ) ;
                allIndices.push_back( x1 ) ;
                allIndices.push_back( x0 ) ;
                allIndices.push_back( x1+1 ) ;
                numberOfWalls++ ;
            }
            
            if( maze->isWallInFront( West, m, n ) ) {
                allIndices.push_back( x0 ) ;
                allIndices.push_back( x0+1 ) ;
                allIndices.push_back( x2 ) ;
                allIndices.push_back( x2 ) ;
                allIndices.push_back( x0+1 ) ;
                allIndices.push_back( x2+1 ) ;
                numberOfWalls++ ;
            }
        }
    }

    for( unsigned short m=0 ; m<maze->M ; m++ ) {
        int n = maze->N ;         
        unsigned short x0 = m * row_length + n*2 ;
        unsigned short x1 = x0 + 2 ;
        unsigned short x2 = x0 + row_length ;

        if( maze->isWallInFront( West, m, n ) ) {
            allIndices.push_back( x0 ) ;
            allIndices.push_back( x0+1 ) ;
            allIndices.push_back( x2 ) ;
            allIndices.push_back( x2 ) ;
            allIndices.push_back( x0+1 ) ;
            allIndices.push_back( x2+1 ) ;
            numberOfWalls++ ;
        }
    }    


    for( unsigned short n=0 ; n<maze->N ; n++ ) {
        int m = maze->M ;
        unsigned short x0 = m * row_length + n*2 ;
        unsigned short x1 = x0 + 2 ;
        unsigned short x2 = x0 + row_length ;

        if( maze->isWallInFront( North, m, n ) ) {
            allIndices.push_back( x0+1 ) ;
            allIndices.push_back( x0 ) ;
            allIndices.push_back( x1+1 ) ;
            allIndices.push_back( x1 ) ;
            allIndices.push_back( x0 ) ;
            allIndices.push_back( x1+1 ) ;
            numberOfWalls++ ;   
        }            
    }

    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);

    int  success;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if(!success) {
        char infoLog[512];
        glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
        std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if(!success) {
        char infoLog[512];
        glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
        std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if(!success) {
        char infoLog[512];
        glGetShaderInfoLog(shaderProgram, 512, nullptr, infoLog);
        std::cerr << "ERROR::PROGRAM::LINKER_FAILED\n" << infoLog << std::endl;
    }

    glGenBuffers( 1, &VBO ) ;
    glBindBuffer( GL_ARRAY_BUFFER, VBO ) ;
    glBufferData( GL_ARRAY_BUFFER, sizeof(float) * allVertices.size(), allVertices.data(), GL_STATIC_DRAW ) ;

    glGenBuffers( 1, &EBO );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, EBO );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned short) * allIndices.size(), allIndices.data(), GL_STATIC_DRAW );

    setCameraPosition();
}


// ----------------------------------------------------------
// display() Callback function
// ----------------------------------------------------------
void display() {
    t += dt;

    if (t > MaxClock) {
        t = 0;
    }
    //  Clear screen and Z-buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(shaderProgram);

    GLsizei stride = FloatsPerVertex*sizeof(float) ;
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, nullptr );
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)) );

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glDrawElements(GL_TRIANGLES, 3 * TrianglesPerWall * numberOfWalls, GL_UNSIGNED_SHORT, 0);

    glutSwapBuffers();
}


void setCameraPosition() {

    // eye is dead-centre of the cube in which player is standing
    glm::vec3 eye(
        player->getN() * 10.f + 5.f ,
        5.f ,
        player->getM() * 10.f + 5.f );

    glm::vec3 facing(eye.x, 5.f, eye.z );
    glm::vec3 up(0, 1, 0);

    switch( player->getDirection() ){
        case North:
            facing.z -= 10.f ;
            break ;
        case South:
            facing.z += 10.f ;
            break ;
        case West:
            facing.x -= 10.f ;
            break ;
        case East:
            facing.x += 10.f ;
            break ;
        default:
            break ;
    }

    // eye.x = maze->N * 5.f ;
    // eye.y = 100.f ;
    // eye.z = maze->N * 5.f ;
    // facing.x = eye.x ;
    // facing.y = 0.f ;
    // facing.z = eye.z ;
    // up.x = 0.f;
    // up.y = 0.f;
    // up.z = -1.f;

    
    glm::mat4 view = glm::lookAt(eye, facing, up) ;

    int location = glGetUniformLocation(shaderProgram, "uView" );
    glProgramUniformMatrix4fv(shaderProgram, location, 1, GL_FALSE, &view[0][0] );

    cout 
        << *player << " | "
        << eye.x << ',' << eye.y << ',' << eye.z << " --> " 
        << facing.x << ',' << facing.y << ',' << facing.z << "  " 
        << ( player -> isWallInFront( *maze ) ? "Wall" : "Path" )
        << endl ;
}


void reshape(int w, int h) {
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);

    const float zNear = .5f ;
    const float zFar = 150.f ;
    const float aspect = (GLfloat)w / (GLfloat)h ;

    float fovRadians = glm::radians( 105.f ) ; // 85' in radians
    glm::mat4 projection = glm::perspective( fovRadians, aspect, zNear, zFar );

    int location = glGetUniformLocation(shaderProgram, "uProjection" );
    glProgramUniformMatrix4fv(shaderProgram, location, 1, GL_FALSE, &projection[0][0] );
}


void mousebutton(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON) {
        if (state == GLUT_DOWN) {
            prev_x = x;
            prev_y = y;
        }
    }

    if (button == 3) {
        radius += 0.2f;
    }
    if (button == 4) {
        radius -= 0.2f;
    }
}

void mousemove(int x, int y) {
    float dx = (x - prev_x) / 100.f;
    float dy = (y - prev_y) / 100.f;
    angley = dx;
    anglex = dy;
}

void reset() {
    anglex = 0.f;
    angley = 0.f;
    radius = 3.f;
}

void keyboard(unsigned char key, int x, int y) {
    //-------- reset -------
    if (key == 'r') {
        player->reset() ;
        reset();
    } else if (key == '[') {
        player->turn( Left ) ;
    } else if (key == ']') {
        player->turn( Right ) ;
    } else if (key == 'w') {
        player->step(*maze);
    } else if (key == 'q') {
        exit(0);
    }

    setCameraPosition();    // set model view
    glutPostRedisplay();
}
