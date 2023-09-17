

#include <stdio.h>
#include <vector>

#define GL_GLEXT_PROTOTYPES
#include <GL/glut.h>
#include <GL/freeglut_ext.h>
#include <GL/freeglut_std.h>

#include <glm/ext/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "opengldisplay.hpp"

using namespace std;

float prev_x;
float prev_y;

float t = 1000000.f;

GLuint VAO;
GLuint VBO;
GLuint EBO;

GLuint vertexShader;
GLuint fragmentShader;
GLuint shaderProgram;
int numberOfWalls;

constexpr GLsizei FloatsPerVertex = 6;
constexpr GLsizei VerticesPerGridPoint = 2;
constexpr GLsizei TrianglesPerWall = 2;

vector<vector<pair<int, int>>> verticals;
vector<vector<pair<int, int>>> horizontals;

void makeVertex( vector<float> &vertices, 
                const float x, const float y, const float z, 
                const float r, const float g, const float b) {
    vertices.push_back(x); 
    vertices.push_back(y); 
    vertices.push_back(z); 
    vertices.push_back(r); 
    vertices.push_back(g); 
    vertices.push_back(b); 
}


void makeIndex( vector<GLushort> &allIndices, 
                const GLushort a, const GLushort b, const GLushort c,
                const GLushort d, const GLushort e, const GLushort f) {
    allIndices.push_back(a) ;
    allIndices.push_back(b) ;
    allIndices.push_back(c) ;
    allIndices.push_back(d) ;
    allIndices.push_back(e) ;
    allIndices.push_back(f) ;
}

void initTriangles() {

    constexpr auto vertexShaderSource = "#version 330 core\n"
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

    constexpr auto fragmentShaderSource = "#version 330 core\n"
                                       "out vec4 vFragColor;\n"
                                       "in vec4 vColor;\n"
                                       "void main()\n"
                                       "{\n"
                                       "   vFragColor = vColor;\n"
                                       "}";

    // array of a 2 vertices at each corner of grid
    // 1 at floor level and 1 at ceiling
    vector<float> allVertices;
    const auto numGridPoints = (maze->M + 1) * (maze->N + 1);
    allVertices.reserve(numGridPoints * FloatsPerVertex * VerticesPerGridPoint + (FloatsPerVertex * 4));

    auto r = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
    auto g = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
    auto b = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);

    for (auto m = 0; m <= maze->M; m++) {
        for (auto n = 0; n <= maze->N; n++) {
            r += (static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) - 0.5f;
            g += (static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) - 0.5f;
            b += (static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) - 0.5f;
            if (r >= 1.f)
                r -= 1.f;
            if (g >= 1.f)
                g -= 1.f;
            if (b >= 1.f)
                b -= 1.f;

            makeVertex(allVertices, 10.f * n, 0.f, 10.f * m, r, g, b) ;

            r += (static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) - 0.5f;
            g += (static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) - 0.5f;
            b += (static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) - 0.5f;
            if (r >= 1.f)
                r -= 1.f;
            if (g >= 1.f)
                g -= 1.f;
            if (b >= 1.f)
                b -= 1.f;

            makeVertex(allVertices, 10.f * n, 10.f, 10.f * m, r, g, b) ;
        }
    }

    int startOfFloor = numGridPoints * 2;
    // define floor == full plate over floor
    makeVertex(allVertices, 0.f, 0.f, 0.f, .2f, .2f, .2f) ;
    makeVertex(allVertices, 10.f * maze->N, 0.f, 0.f, .5f, .5f, .5f) ;
    makeVertex(allVertices, 10.f * maze->N, 0.f, 10.f * maze->M, .8f, .8f, .8f) ;
    makeVertex(allVertices, 0.f, 0.f, 10.f * maze->M, .5f, .5f, .5f) ;


    // define ceiling == full plate over floor
    makeVertex(allVertices, 0.f, 10.f, 0.f, .2f, .5f, .8f) ;
    makeVertex(allVertices, 10.f * maze->N, 10.f, 0.f, .2f, .6f, .8f) ;
    makeVertex(allVertices, 10.f * maze->N, 10.f, 10.f * maze->M, .9f, .9f, .8f) ;
    makeVertex(allVertices, 0.f, 10.f, 10.f * maze->M, .2f, .2f, .8f) ;

    //
    //   Defines points for East-West wall and North-South wall
    //
    //      x .. E
    //      .
    //      .
    //      S
    //
    vector<GLushort> allIndices;
    allIndices.reserve((maze->M) * (maze->N) * 12 + 6); // max number of walls

    makeIndex (allIndices, startOfFloor, startOfFloor + 3, startOfFloor + 1,
                           startOfFloor + 1, startOfFloor + 3, startOfFloor + 2);
    makeIndex (allIndices, startOfFloor + 4, startOfFloor + 7, startOfFloor + 5,
                           startOfFloor + 5, startOfFloor + 7, startOfFloor + 6);

    numberOfWalls = 2;
    auto row_length = 2 * (maze->N + 1);

    for (auto m = 0; m < maze->M; m++) {
        for (auto n = 0; n < maze->N; n++) {

            auto x0 = m * row_length + n * 2;
            auto x1 = x0 + 2;
            auto x2 = x0 + row_length;

            if (maze->isWallInFront(North, m, n)) {
                makeIndex (allIndices, x0 + 1, x0, x1 + 1, x1, x0, x1 + 1);
                numberOfWalls++;
            }

            if (maze->isWallInFront(West, m, n)) {
                makeIndex (allIndices, x0, x0 + 1, x2, x2, x0 + 1, x2 + 1);
                numberOfWalls++;
            }
        }
    }

    for (auto m = 0; m < maze->M; m++) {
        horizontals.push_back(maze->horizontalSegments(m));
        auto n = maze->N;
        auto x0 = m * row_length + n * 2;
        auto x1 = x0 + 2;
        auto x2 = x0 + row_length;

        if (maze->isWallInFront(West, m, n)) {
            makeIndex (allIndices, x0, x0 + 1, x2, x2, x0 + 1, x2 + 1);
            numberOfWalls++;
        }
    }

    for (auto n = 0; n < maze->N; n++) {
        verticals.push_back(maze->verticalSegments(n));

        auto m = maze->M;
        auto x0 = m * row_length + n * 2;
        auto x1 = x0 + 2;
        auto x2 = x0 + row_length;

        if (maze->isWallInFront(North, m, n)) {
            makeIndex (allIndices, x0 + 1, x0, x1 + 1, x1, x0, x1 + 1);
            numberOfWalls++;
        }
    }

    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);

    int success;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
        std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
    }

    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
        std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
    }

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shaderProgram, 512, nullptr, infoLog);
        std::cerr << "ERROR::PROGRAM::LINKER_FAILED\n"
                  << infoLog << std::endl;
    }

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * allVertices.size(), allVertices.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned short) * allIndices.size(), allIndices.data(), GL_STATIC_DRAW);

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

    GLsizei stride = FloatsPerVertex * sizeof(float);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, nullptr);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void *)(3 * sizeof(float)));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glDrawElements(GL_TRIANGLES, 3 * TrianglesPerWall * numberOfWalls, GL_UNSIGNED_SHORT, 0);

    glUseProgram(0);

    // green box
    glColor4d(0x00, 0xff, 0x00, 0.1);
    glLineWidth(2.0);
    glBegin(GL_LINES);

    double xr = 1.0 / (2.0 * maze->N);
    double yr = 1.0 / (2.0 * maze->M);

    for (int x = 0; x < verticals.size(); x++) {
        for (auto vs : verticals[x]) {
            glVertex2d(0.51 + x * yr, 0.99 - vs.first * xr);
            glVertex2d(0.51 + x * yr, 0.99 - vs.second * xr);
        }
    }
    for (int y = 0; y < horizontals.size(); y++) {
        for (auto hs : horizontals[y]) {
            glVertex2d(0.51 + hs.first * yr, 0.99 - y * xr);
            glVertex2d(0.51 + hs.second * yr, 0.99 - y * xr);
        }
    }

    glEnd();

    glColor4d(0xff, 0xff, 0xff, 0.1);
    glPointSize(6.0);
    glBegin(GL_POINTS);
    glVertex2d(0.51 + player->getN() * yr, 0.99 - player->getM() * xr);
    glEnd();

    glutSwapBuffers();
}

void setCameraPosition() {

    // eye is dead-centre of the cube in which player is standing
    glm::vec3 eye(
        player->getN() * 10.f + 5.f,
        5.f,
        player->getM() * 10.f + 5.f);

    glm::vec3 facing(eye.x, 5.f, eye.z);
    glm::vec3 up(0, 1, 0);

    switch (player->getDirection()) {
    case North:
        facing.z -= 10.f;
        break;
    case South:
        facing.z += 10.f;
        break;
    case West:
        facing.x -= 10.f;
        break;
    case East:
        facing.x += 10.f;
        break;
    default:
        break;
    }

    glm::mat4 view = glm::lookAt(eye, facing, up);

    int location = glGetUniformLocation(shaderProgram, "uView");
    glProgramUniformMatrix4fv(shaderProgram, location, 1, GL_FALSE, &view[0][0]);

    // cout
    //     << *player << " | "
    //     << eye.x << ',' << eye.y << ',' << eye.z << " --> "
    //     << facing.x << ',' << facing.y << ',' << facing.z << "  "
    //     << ( player -> isWallInFront( *maze ) ? "Wall" : "Path" )
    //     << endl ;
}

void reshape(int w, int h) {
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);

    constexpr float zNear = .5f;
    constexpr float zFar = 150.f;
    const auto aspect = (GLfloat)w / (GLfloat)h;

    const auto fovRadians = glm::radians(105.f); // 85' in radians
    const auto projection = glm::perspective(fovRadians, aspect, zNear, zFar);

    const auto location = glGetUniformLocation(shaderProgram, "uProjection");
    glProgramUniformMatrix4fv(shaderProgram, location, 1, GL_FALSE, &projection[0][0]);
}

void mousebutton(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON) {
        if (state == GLUT_DOWN) {
            prev_x = x;
            prev_y = y;
        }
    }
}

void mousemove(int x, int y) {
    float dx = (x - prev_x) / 100.f;
    float dy = (y - prev_y) / 100.f;
}

void reset() {
}

void keyboard(unsigned char key, int x, int y) {
    //-------- reset -------
    if (key == 'r') {
        player->reset();
        reset();
    } else if (key == 'q') {
        exit(0);
    }

    setCameraPosition(); // set model view
    glutPostRedisplay();
}

void handleSpecialKeypress(int key, int x, int y) {
    switch (key) {
    case GLUT_KEY_LEFT:
        player->turn(Left);
        break;
    case GLUT_KEY_RIGHT:
        player->turn(Right);
        break;
    case GLUT_KEY_UP:
        player->step(*maze);
        break;
    }
    setCameraPosition(); // set model view
    glutPostRedisplay();
}

void handleSpecialKeyReleased(int key, int x, int y) {
}