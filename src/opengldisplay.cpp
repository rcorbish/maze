

#include <stdio.h>

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
float vertices[] = {
    -.5, -.5, .75,
     .5, -.5, .75,
     .5,  .5, .75,
    -.5,  .5, .75
} ;

unsigned short indices[] = {  
    0, 1, 2,   // first triangle
    0, 2, 3    // second triangle
};  


void initTriangles() {

    const char *vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "uniform mat4 uProjection;\n"
    "uniform mat4 uView;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = uProjection * uView * vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}";

    const char *fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(1.0f, 0.75f, 0.2f, 1.0f);\n"
    "}";


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
    glBufferData( GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW ) ;

    glGenBuffers( 1, &EBO );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, EBO );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW );
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

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr );

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    if( player->isWallInFront( *maze ) ) {
        glDrawElements(GL_TRIANGLES, 6,  GL_UNSIGNED_SHORT, 0);
    }

    glutSwapBuffers();
}


void drawText() {
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    int w = glutGet(GLUT_WINDOW_WIDTH);
    int h = glutGet(GLUT_WINDOW_HEIGHT);
    glOrtho(0, w, 0, h, -1, 1);


    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    // 1st draw the text stuff w/o transformations
    glRasterPos2i(0, h - 20);
    char s[256];
    sprintf(s, "Hello");
    int len = (int)strlen(s);
    for (int i = 0; i < len; i++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, s[i]);
    }

    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
}

void setCameraPosition() {

    // eye is dead-centre of the cube in which player is standing
    glm::vec3 eye(
        player->getN() * 10.f + 5.f ,
        5.f ,
        player->getM() * 10.f + 5.f );

    glm::vec3 facing(eye.x, 0, eye.z );
    glm::vec3 up(0, 1, 0);

    switch( player->getDirection() ){
        case North:
            facing.z += 10.f ;
            break ;
        case South:
            facing.z-= 10.f ;
            break ;
        case West:
            facing.x-= 10.f ;
            break ;
        case East:
            facing.x += 10.f ;
            break ;
        default:
            break ;
    }
    
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


    const float zNear = 2.f ;
    const float zFar = 50.f ;
    const float aspect = (GLfloat)w / (GLfloat)h ;

    float fH = tan( 50.f * M_PI / 360.f ) * zNear;
    float fW = fH * aspect;

    glm::mat4 projection = glm::frustum( -fW, fW, -fH, fH, zNear, zFar );

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
