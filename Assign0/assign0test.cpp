#include <GL/glew.h>
#include <GL/glut.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <sstream>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "geometry3.h"
#include <array>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#include "stb_image.h"
#include "shaders.h"
#include "texture.h"
#include "shaders.h"
#include "components.h"
struct Cloud {
    float x, z;  // �Ƶ�λ��
};

std::vector<Cloud> clouds;
float cloudSpeed = 0.05f;  // �Ƶ��ƶ��ٶ�
int windowWidth = 800;
int windowHeight = 600;

void initClouds() {
    srand(static_cast<unsigned int>(time(NULL))); // ��ʼ�������������
}


void drawCloud(float x, float z) {
    glPushMatrix();
    glTranslatef(x, 20.0f, z); // �����Ƶ�λ��
    glScalef(10.0f, 2.f, 10.0f); // �����ƵĴ�С����״
    glColor3f(0.8f, 0.8f, 0.8f); // �����Ƶ���ɫ
    glutSolidCube(1.0f); // ������
    glPopMatrix();
}
void onMouseMotion(int x, int y);
bool keyStates[256] = { false }; // ��¼����״̬������

void onKeyDown(unsigned char key, int x, int y) {
    keyStates[key] = true; // ���¼�ʱ��Ϊ true
}

void onKeyUp(unsigned char key, int x, int y) {
    keyStates[key] = false; // �ɿ���ʱ��Ϊ false
}
const GLfloat treeVertices[] = {
    // positions      // texture coords
    -1.0f, 0.0f, 0.0f,  0.0f, 0.0f,
     1.0f, 0.0f, 0.0f,  1.0f, 0.0f,
     1.0f, 2.0f, 0.0f,  1.0f, 1.0f,
    -1.0f, 2.0f, 0.0f,  0.0f, 1.0f
};

const GLubyte treeIndices[] = {
    0, 1, 2, 2, 3, 0
};
GLuint treeVAO, treeVBO, treeEBO;
// ������Ϣ
// ��״����
enum ShapeType { CUBE, SPHERE, CYLINDER, ROTATING_SPHERE, DOG, CAT };
int count1 = 0;
// ��״�ṹ������
struct Shape {
    ShapeType type;
    float posX, posY, posZ;
    bool isJumping = false;
    bool isSpinning = false;
    float jumpHeight = 0.0f;
    float jumpSpeed = 0.05f;
    float spinAngle = 0.0f; // ��ת�Ƕ�
    float angleYaw = 0.0f; // �����˵ĽǶ�
};
GLuint shaderProgram;
GLuint diffuseMap;
GLuint normalMap;
GLuint skybox;
GLuint treeTexture;
// ����ͷ��ٿ������λ�ú��ӽ�
float userPosX = 0.0f, userPosY = 5.0f, userPosZ = 10.0f; // �������ͷ�ĸ߶�
float cameraYaw = 0.0f, cameraPitch = -0.5f; // �ӽǿ���

// ������λ�úͽǶ�
Shape robot;

// �ӽ�ģʽ��0��ʾ�����˳ƣ�1��ʾ��һ�˳�
int viewMode = 0;

// ��·�������
bool isWalking = false;
float walkCycle = 0.0f;

// ��ɫ
GLfloat _cubecol[5][3] = {
    {1.0, 0.0, 0.0}, // ��ɫ
    {0.0, 1.0, 0.0}, // ��ɫ
    {0.0, 0.0, 1.0}, // ��ɫ
    {1.0, 1.0, 0.0}, // ��ɫ
    {1.0, 0.0, 1.0}  // ��ɫ
};

std::vector<std::string> messages = { "Hello", "This is a shape", "Press p to make it jump", "Press o to spin", "Press j to make the robot jump", "Press 1 for first-person view", "Press 2 for third-person view" };
int currentMessageIndex = 0;
bool showJumpMessage = false;
GLuint texNames[1];

std::vector<Shape> shapes;
const int numShapes = 0; // ��״����

// �������ɵĹ켣��
struct Point {
    float x, y, z;
};

std::vector<Point> trajectory;
const int maxPoints = 100; // ���켣����

// ����ģ������
struct Model {
    std::vector<float> vertices;
    std::vector<int> faces;
};

Model dogModel, catModel;
GLuint loadTexture(const char* path) {
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    int width, height, nrChannels;
    unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);
    if (data) {
        GLenum format;
        if (nrChannels == 1)
            format = GL_RED;
        else if (nrChannels == 3)
            format = GL_RGB;
        else if (nrChannels == 4)
            format = GL_RGBA;

        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(data);
    }
    else {
        std::cerr << "Failed to load texture: " << path << std::endl;
        glDeleteTextures(1, &textureID);
        return 0;
    }

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, 0);
    return textureID;
}

using namespace std;

GLuint loadTexture2(const char* filename) {
    int width, height, maxval;
    ifstream file(filename, ios::binary);
    if (!file.is_open()) {
        cerr << "Error opening file: " << filename << endl;
        return 0;
    }

    string header;
    file >> header; // Should be P6
    file >> width >> height >> maxval; // Read the size and max color value
    file.ignore(256, '\n'); // Skip newline character after header information

    unsigned char* data = new unsigned char[width * height * 3]; // Each pixel has red, green, and blue components
    file.read((char*)data, width * height * 3);
    file.close();

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

    delete[] data; // Clean up the data array
    return textureID;
}

GLuint floorTexture, WallTexture, wallTexture, doorTexture, roofTexture, windowTexture;
void initRenderingfloor() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D); // Enable texture mapping
    floorTexture = loadTexture("floor2.bmp"); // Load the wood texture
    WallTexture = loadTexture("house.bmp");
    wallTexture = loadTexture("wall.bmp");
    doorTexture = loadTexture("goldw.bmp");
    windowTexture = loadTexture("sky.bmp");
    roofTexture = loadTexture("goldw.bmp");
    glEnable(GL_DEPTH_TEST);

    glEnable(GL_LIGHT0);     // Enable light #0

    // Optional: Set global ambient light (if you want a globally ambient light)
    GLfloat globalAmbient[] = { 0.2f, 0.2f, 0.2f, 1.0f }; // Dim light
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmbient);

    // Bind the texture and set texture wrapping parameters
    glBindTexture(GL_TEXTURE_2D, floorTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Set texture filtering parameters (optional, but recommended)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

float _angle = 45.0f;

void drawScene() {

    // Position and set properties of the light
    GLfloat lightColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };   // White light
    GLfloat lightPos[] = { 1.0f, 1.0f, 0.5f, 1.0f };     // Positional light
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor);
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightColor);
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

    glPushMatrix();

    // Draw the floor
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, floorTexture);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0); glVertex3f(-7.0f, 0.2f, 5.1f);
    glTexCoord2f(1.0, 0.0); glVertex3f(1.8f, 0.2f, 12.0f);
    glTexCoord2f(1.0, 1.0); glVertex3f(36.2f, 0.2f, -23.0f);
    glTexCoord2f(0.0, 1.0); glVertex3f(28.0f, 0.2f, -30.0f);
    glEnd();
    // Set the new coordinates for the house
    float newHouseX = -10.0f; // New X position for the house
    float newHouseY = -1.0f; // New Y position for the house
    float newHouseZ = 8.0f; // Z position for the house, adjust if necessary

    // Apply translation to move the house to the new position
    glTranslatef(newHouseX, newHouseY, newHouseZ);

    // Transformations and drawing the house
    glTranslatef(1.0f, 1.0f, 0.0f);
    glRotatef(_angle, 0.0f, 1.0f, 0.0f);
    glScalef(30.0f, 10.0f, 10.0f);
    glColor3f(1.0, 0.25, 1.0);

    // Draw the floor



    // Main rectangle
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, wallTexture);
    glBegin(GL_POLYGON);
    glTexCoord2f(0.0, 0.0); glVertex3f(0.2, 0.1, 0.0);
    glTexCoord2f(1.0, 0.0); glVertex3f(1.8, 0.1, 0.0); // Double the width
    glTexCoord2f(1.0, 1.0); glVertex3f(1.8, 0.575, 0.0); // Double the width
    glTexCoord2f(0.0, 1.0); glVertex3f(0.2, 0.575, 0.0);
    glEnd();


    // Back rectangle
    glBindTexture(GL_TEXTURE_2D, WallTexture);
    glBegin(GL_POLYGON);
    glTexCoord2f(0.0, 0.0); glVertex3f(0.2, 0.1, 1.0); // Double the length
    glTexCoord2f(10.0, 0.0); glVertex3f(1.8, 0.1, 1.0); // Double the width and length
    glTexCoord2f(10.0, 10.0); glVertex3f(1.8, 0.575, 1.0); // Double the width and length
    glTexCoord2f(0.0, 10.0); glVertex3f(0.2, 0.575, 1.0); // Double the length
    glEnd();


    // Left rectangle
    glBindTexture(GL_TEXTURE_2D, WallTexture);
    glBegin(GL_POLYGON);
    glTexCoord2f(0.0, 0.0); glVertex3f(0.2, 0.1, 1.0); // Double the length
    glTexCoord2f(10.0, 0.0); glVertex3f(0.2, 0.1, 0.0);
    glTexCoord2f(10.0, 10.0); glVertex3f(0.2, 0.575, 0.0);
    glTexCoord2f(0.0, 10.0); glVertex3f(0.2, 0.575, 1.0); // Double the length
    glEnd();


    // Right rectangle
    glBindTexture(GL_TEXTURE_2D, WallTexture);
    glBegin(GL_POLYGON);
    glTexCoord2f(0.0, 0.0); glVertex3f(1.8, 0.1, 1.0); // Double the width and length
    glTexCoord2f(10.0, 0.0); glVertex3f(1.8, 0.1, 0.0); // Double the width
    glTexCoord2f(10.0, 10.0); glVertex3f(1.8, 0.575, 0.0); // Double the width
    glTexCoord2f(0.0, 10.0); glVertex3f(1.8, 0.575, 1.0); // Double the width and length
    glEnd();


    // Left triangle
    glBindTexture(GL_TEXTURE_2D, WallTexture);
    glBegin(GL_TRIANGLES);
    glTexCoord2f(0.0, 0.0); glVertex3f(1.8, 0.575, 0.0); // Double the width
    glTexCoord2f(1.0, 0.0); glVertex3f(1.8, 0.575, 1.0); // Double the width and length
    glTexCoord2f(0.5, 1.0); glVertex3f(1.8, 0.8, 0.5); // Double the width and length
    glEnd();


    // Right triangle
    glBindTexture(GL_TEXTURE_2D, WallTexture);
    glBegin(GL_TRIANGLES);
    glTexCoord2f(0.0, 0.0);glTexCoord2f(0.0, 0.0); glVertex3f(0.2, 0.575, 0.0);
    glTexCoord2f(1.0, 0.0); glVertex3f(0.2, 0.575, 1.0); // Double the length
    glTexCoord2f(0.5, 1.0); glVertex3f(0.2, 0.8, 0.5); // Double the length
    glEnd();



    /* roof */
    glBindTexture(GL_TEXTURE_2D, roofTexture);
    glColor3f(.55, 0.35, .2);
    glBegin(GL_POLYGON);
    glTexCoord2f(0.0, 0.0); glVertex3f(0.2, 0.575, 0.0);
    glTexCoord2f(10.0, 0.0); glVertex3f(1.8, 0.575, 0.0); // Double the width
    glTexCoord2f(10.0, 10.0); glVertex3f(1.8, 0.8, 0.5); // Double the width and length
    glTexCoord2f(0.0, 10.0); glVertex3f(0.2, 0.8, 0.5); // Double the length
    glEnd();

    /*back roof */
    glBindTexture(GL_TEXTURE_2D, roofTexture);
    glColor3f(.55, 0.35, .2);
    glBegin(GL_POLYGON);
    glTexCoord2f(0.0, 0.0); glVertex3f(0.2, 0.575, 1.0); // Double the length
    glTexCoord2f(10.0, 0.0); glVertex3f(1.8, 0.575, 1.0); // Double the width and length
    glTexCoord2f(10.0, 10.0); glVertex3f(1.8, 0.8, 0.5); // Double the width and length
    glTexCoord2f(0.0, 10.0); glVertex3f(0.2, 0.8, 0.5); // Double the length
    glEnd();

    /* door */
    glBindTexture(GL_TEXTURE_2D, doorTexture);
    glColor3f(.15, 0.2, .3);
    glBegin(GL_POLYGON);
    glTexCoord2f(0.0, 0.0); glVertex3f(0.47, 0.105, 0.0);
    glTexCoord2f(1.0, 0.0); glVertex3f(1.3, 0.105, 0.0); // Double the width
    glTexCoord2f(1.0, 1.0); glVertex3f(1.3, 0.46, 0.0); // Double the width
    glTexCoord2f(0.0, 1.0); glVertex3f(0.47, 0.46, 0.0);
    glEnd();

    /* window 1 */
    glBindTexture(GL_TEXTURE_2D, windowTexture);
    glColor3f(.3, 0.2, .1);
    glBegin(GL_POLYGON);
    glTexCoord2f(0.0, 0.0); glVertex3f(1.44, 0.25, 0.0); // Double the width
    glTexCoord2f(1.0, 0.0); glVertex3f(1.66, 0.25, 0.0); // Double the width
    glTexCoord2f(1.0, 1.0); glVertex3f(1.66, 0.4, 0.0); // Double the width
    glTexCoord2f(0.0, 1.0); glVertex3f(1.44, 0.4, 0.0); // Double the width
    glEnd();

    /* window 2 */
    glBindTexture(GL_TEXTURE_2D, windowTexture);
    glColor3f(.3, 0.2, .1);
    glBegin(GL_POLYGON);
    glTexCoord2f(0.0, 0.0); glVertex3f(0.54, 0.25, 0.0); // Double the width
    glTexCoord2f(1.0, 0.0); glVertex3f(0.76, 0.25, 0.0); // Double the width
    glTexCoord2f(1.0, 1.0); glVertex3f(0.76, 0.4, 0.0); // Double the width
    glTexCoord2f(0.0, 1.0); glVertex3f(0.54, 0.4, 0.0); // Double the width
    glEnd();
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
    //glutSwapBuffers();
}






GLuint loadCubemap(std::vector<std::string> faces) {
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++) {
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data) {
            GLenum format;
            if (nrChannels == 1)
                format = GL_RED;
            else if (nrChannels == 3)
                format = GL_RGB;
            else if (nrChannels == 4)
                format = GL_RGBA;

            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else {
            std::cerr << "Failed to load cubemap texture at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    return textureID;
}

void loadShadersAndTextures() {
    shaderProgram = initprogram(
        initshaders(GL_VERTEX_SHADER, "vertex_shader.glsl"),
        initshaders(GL_FRAGMENT_SHADER, "fragment_shader.glsl")
    );

    treeTexture = loadTexture("tree.png"); // ������ľ������
}
bool loadObj(const std::string& path, Model& model) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << path << std::endl;
        return false;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream ss(line);
        std::string prefix;
        ss >> prefix;
        if (prefix == "v") {
            float x, y, z;
            ss >> x >> y >> z;
            model.vertices.push_back(x);
            model.vertices.push_back(y);
            model.vertices.push_back(z);
        }
        else if (prefix == "f") {
            int v1, v2, v3;
            ss >> v1 >> v2 >> v3;
            model.faces.push_back(v1 - 1);
            model.faces.push_back(v2 - 1);
            model.faces.push_back(v3 - 1);
        }
    }
    return true;
}

void sphere() {
    float Radius = 0.5;
    int sectors = 10;
    int rings = 10;
    try {
        std::vector<std::vector<float>> v((rings + 1) * (sectors + 1), std::vector<float>(3));
        int np = 0;
        int index[4000];

        for (int i = 0; i < rings + 1; i++) {
            for (int j = 0; j < sectors + 1; j++) {
                float y = Radius * cos(i * M_PI / rings) / 2.5;
                float x = Radius * sin(i * M_PI / rings) * cos(2 * j * M_PI / sectors);
                float z = Radius * sin(i * M_PI / rings) * sin(2 * j * M_PI / sectors);
                v[i * (sectors + 1) + j][0] = x;
                v[i * (sectors + 1) + j][1] = y;
                v[i * (sectors + 1) + j][2] = z;
            }
        }

        for (int i = 0; i < rings / 2; i++) {
            for (int j = 0; j < sectors; j++) {
                index[4 * np + 0] = i * (sectors + 1) + j;
                index[4 * np + 1] = i * (sectors + 1) + (j + 1);
                index[4 * np + 2] = (i + 1) * (sectors + 1) + (j + 1);
                index[4 * np + 3] = (i + 1) * (sectors + 1) + j;
                np++;
            }
        }

        std::vector<int> index2(4 * rings / 2 * sectors);
        std::vector<std::vector<float>> ti(4 * rings / 2 * sectors, std::vector<float>(2));
        int t = 0;
        for (int r = 0; r < rings / 2 - 1; r++) {
            for (int s = 0; s < sectors; s++) {
                index2[4 * t + 0] = r * sectors + s;
                index2[4 * t + 1] = r * sectors + (s + 1);
                index2[4 * t + 2] = (r + 1) * sectors + (s + 1);
                index2[4 * t + 3] = (r + 1) * sectors + s;

                ti[4 * t + 0][0] = static_cast<float>(r);
                ti[4 * t + 0][1] = static_cast<float>(s);

                ti[4 * t + 1][0] = static_cast<float>(r);
                ti[4 * t + 1][1] = static_cast<float>(s + 1);

                ti[4 * t + 2][0] = static_cast<float>(r + 1);
                ti[4 * t + 2][1] = static_cast<float>(s + 1);

                ti[4 * t + 3][0] = static_cast<float>(r + 1);
                ti[4 * t + 3][1] = static_cast<float>(s);

                t++;
            }
        }

        for (int i = 0; i < np; i++) { // i: index for polygon
            glBegin(GL_QUADS);

            glNormal3fv(v[index[4 * i + 0]].data());
            glTexCoord2f(ti[4 * i + 0][1] / sectors, ti[4 * i + 0][0] / (rings / 2.0f));
            glVertex3fv(v[index[4 * i + 0]].data());

            glNormal3fv(v[index[4 * i + 1]].data());
            glTexCoord2f(ti[4 * i + 1][1] / sectors, ti[4 * i + 1][0] / (rings / 2.0f));
            glVertex3fv(v[index[4 * i + 1]].data());

            glNormal3fv(v[index[4 * i + 2]].data());
            glTexCoord2f(ti[4 * i + 2][1] / sectors, ti[4 * i + 2][0] / (rings / 2.0f));
            glVertex3fv(v[index[4 * i + 2]].data());

            glNormal3fv(v[index[4 * i + 3]].data());
            glTexCoord2f(ti[4 * i + 3][1] / sectors, ti[4 * i + 3][0] / (rings / 2.0f));
            glVertex3fv(v[index[4 * i + 3]].data());
            glEnd();
        }
    }
    catch (const std::bad_alloc& e) {
        std::cerr << "Memory allocation failed in sphere function. Exception: " << e.what() << std::endl;
    }
}
GLuint cola_textures[2];
void initCola() {
    init_texture(cola_textures, 2);

    char filename1[] = "cola_texture.bmp";
    set_texture(cola_textures, 0, filename1);

    char filename2[] = "straw_texture.bmp";
    set_texture(cola_textures, 1, filename2);
}


void cola() {
    glPushMatrix();
    init_texture(cola_textures, 2);
    char filename1[] = "cola_texture.bmp";
    set_texture(cola_textures, 0, filename1);

    // Draw bottle body
    glBegin(GL_QUAD_STRIP);
    int nslice = 20;
    double topR = 0.2;
    double bottomR = 0.15;
    double height = 0.7;
    double t = 0.0; double dt = 2 * M_PI / nslice;
    for (int j = 0; j <= nslice; ++j) {
        glNormal3f(bottomR * cos(t), 0.0, -1 * bottomR * sin(t));
        glTexCoord2f(t / (2 * M_PI), 1.0);
        glVertex3f(bottomR * cos(t), 0.0, -1 * bottomR * sin(t));

        glNormal3f(topR * cos(t), height, -1 * topR * sin(t));
        glTexCoord2f(t / (2 * M_PI), 0.0);
        glVertex3f(topR * cos(t), height, -1 * topR * sin(t));

        t = t + dt;
    }
    glEnd();
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();

    // Draw the door on the bottle
    glPushMatrix();
    glTranslatef(0.0, 0.3, -0.15); // Adjust the position of the door
    glBegin(GL_QUADS);
    glColor3f(0.5, 0.5, 0.5); // Gray door
    glVertex3f(-0.05, 0.0, 0.0);
    glVertex3f(0.05, 0.0, 0.0);
    glVertex3f(0.05, 0.2, 0.0);
    glVertex3f(-0.05, 0.2, 0.0);
    glEnd();
    glPopMatrix();

    // Straw
    glPushMatrix();
    char filename2[] = "straw_texture.bmp";
    set_texture(cola_textures, 1, filename2);

    glBegin(GL_QUAD_STRIP);
    nslice = 20;
    topR = 0.02;
    bottomR = 0.015;
    height = 1;
    t = 0.0; dt = 2 * M_PI / nslice;
    for (int j = 0; j <= nslice; ++j) {
        glNormal3f(topR * cos(t), height, -1 * topR * sin(t));
        glTexCoord2f(t / (2 * M_PI), 1.0);
        glVertex3f(topR * cos(t), height, -1 * topR * sin(t));
        glNormal3f(bottomR * cos(t), 0.0, -1 * bottomR * sin(t));
        glTexCoord2f(t / (2 * M_PI), 0.0);
        glVertex3f(bottomR * cos(t), 0.0, -1 * bottomR * sin(t));

        t = t + dt;
    }
    glEnd();
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
}
void drawCola() {
    glPushMatrix();
    glTranslatef(2.0f, 2.0f, 1.0f); //����λ��
    glScalef(5.0f, 5.0f, 5.0f); // ��������
    cola();
    glPopMatrix();
}
GLuint burger_textures[5];
void burger() {
    GLUquadricObj* qobj;
    qobj = gluNewQuadric();
    gluQuadricDrawStyle(qobj, GLU_FILL);
    gluQuadricTexture(qobj, GL_TRUE);
    gluQuadricNormals(qobj, GLU_SMOOTH);
    init_texture(burger_textures, 5);

    char bun[] = "bun-t.jpg";
    set_texture(burger_textures, 3, bun);
    glPushMatrix();
    glTranslated(0, 0.15, 0);
    glColor3f(1.0, 0.686, 0.067);
    sphere();
    glPopMatrix();

    char bunb[] = "bun-b.jpg";
    set_texture(burger_textures, 4, bunb);
    glPushMatrix();
    glTranslated(0, -0.15, 0);
    glRotatef(180, 1.0, 0.0, 0.0);
    sphere();
    glPopMatrix();

    char filename[] = "cookedmeat.jpg";
    set_texture(burger_textures, 0, filename);
    glPushMatrix();
    glTranslated(0, 0.1 / 2, 0);
    glRotatef(90, 1.0, 0.0, 0.0);
    glColor3f(0.337, 0.18, 0.078);
    gluCylinder(qobj, 0.5, 0.5, 0.1, 30, 2);
    glPopMatrix();

    char filename1[] = "tomato.jpg";
    set_texture(burger_textures, 1, filename1);
    glPushMatrix();
    glTranslated(0, 0.1 / 2, 0);
    glTranslated(0, -0.1, 0);
    glRotatef(90, 1.0, 0.0, 0.0);
    glColor3f(1.0, 0.321, 0.247);
    gluCylinder(qobj, 0.5, 0.5, 0.1, 30, 2);
    glPopMatrix();

    char filename2[] = "green.jpg";
    set_texture(burger_textures, 2, filename2);
    glPushMatrix();
    glTranslated(0, 0.1 / 2, 0);
    glTranslated(0, 0.1, 0);
    glRotatef(90, 1.0, 0.0, 0.0);
    glColor3f(0.247, 1.0, 0.337);
    gluCylinder(qobj, 0.5, 0.5, 0.1, 30, 2);
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
}

GLuint burgerTextures[5];

void initBurger() {
    init_texture(burgerTextures, 5);

    char bun[] = "bun-t.jpg";
    set_texture(burgerTextures, 3, bun);

    char bunb[] = "bun-b.jpg";
    set_texture(burgerTextures, 4, bunb);

    char filename[] = "cookedmeat.jpg";
    set_texture(burgerTextures, 0, filename);

    char filename1[] = "tomato.jpg";
    set_texture(burgerTextures, 1, filename1);

    char filename2[] = "green.jpg";
    set_texture(burgerTextures, 2, filename2);
}
GLuint grassTexture;
void drawFloor() {
    grassTexture = loadTexture("tree.bmp");
    glPushMatrix();
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, grassTexture);  // �󶨲�ƺ����

    // �������������Լ��������꣬ʵ������ӳ��
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-50.0f, 0.0f, -50.0f);
    glTexCoord2f(50.0f, 0.0f); glVertex3f(50.0f, 0.0f, -50.0f);
    glTexCoord2f(50.0f, 50.0f); glVertex3f(50.0f, 0.0f, 50.0f);
    glTexCoord2f(0.0f, 50.0f); glVertex3f(-50.0f, 0.0f, 50.0f);
    glEnd();

    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
}

// ���������״λ�ú�����
void generateRandomShapes() {
    srand(static_cast<unsigned>(time(0)));
    for (int i = 0; i < numShapes; ++i) {
        float x = static_cast<float>(rand() % 50 - 20); // ���� -50 �� 50 ֮��������
        float y = 0.5f; // ȷ����״λ���µĵذ�߶�
        float z = static_cast<float>(rand() % 50 - 20); // ���� -50 �� 50 ֮��������
        ShapeType type = static_cast<ShapeType>(rand() % 6); // ���ѡ����״���ͣ������µ� DOG �� CAT
        shapes.push_back({ type, x, y, z });
    }
}

// �����ж�
bool isWithinRange(float shapePosX, float shapePosY, float shapePosZ) {
    float distance = std::sqrt(std::pow(robot.posX - shapePosX, 2) + std::pow(robot.posY - shapePosY, 2) + std::pow(robot.posZ - shapePosZ, 2));
    return distance < 4.1f; // �趨һ����Χ������3.0��λ
}

// ��ײ���
bool isColliding(float newX, float newY, float newZ) {
    for (const auto& shape : shapes) {
        float distance = std::sqrt(std::pow(newX - shape.posX, 2) + std::pow(newY - shape.posY, 2) + std::pow(newZ - shape.posZ, 2));
        if (distance < 4.0f) { // ��������İ뾶Ϊ1.0������һЩ����
            return true;
        }
    }
    return false;
}

void displayMessage(float shapePosX, float shapePosY, float shapePosZ) {
    // ��OpenGL�����л�������
    glRasterPos3f(shapePosX, shapePosY + 1.0f, shapePosZ); // ����λ�õ���
    for (char c : messages[currentMessageIndex]) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
    }
}

void onMouseClick(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        for (auto& shape : shapes) {
            if (isWithinRange(shape.posX, shape.posY, shape.posZ)) {
                if (currentMessageIndex < messages.size() - 1) {
                    currentMessageIndex++;
                }
                if (currentMessageIndex == messages.size() - 1) {
                    showJumpMessage = true;
                }
                break;
            }
        }
    }
}

void onSpecialKey(int key, int x, int y) {
    for (auto& shape : shapes) {
        if (isWithinRange(shape.posX, shape.posY, shape.posZ)) {
            if (key == GLUT_KEY_RIGHT) {
                if (currentMessageIndex < messages.size() - 1) {
                    currentMessageIndex++;
                }
                if (currentMessageIndex == messages.size() - 1) {
                    showJumpMessage = true;
                }
            }
            break; // �ҵ�һ���ڷ�Χ�ڵ���״��Ͳ��ټ������
        }
    }
}

void onKeyboard(unsigned char key, int x, int y) {
    float moveSpeed = 0.1f;
    float newX = robot.posX;
    float newY = robot.posY;
    float newZ = robot.posZ;
    bool moving = false;

    switch (key) {
    case 'w':
        newX += moveSpeed * sin(cameraYaw);
        newZ -= moveSpeed * cos(cameraYaw);
        moving = true;
        break;
    case 's':
        newX -= moveSpeed * sin(cameraYaw);
        newZ += moveSpeed * cos(cameraYaw);
        moving = true;
        break;
    case 'a':
        cameraYaw -= 0.05f;
        newX -= moveSpeed * cos(cameraYaw);
        newZ -= moveSpeed * sin(cameraYaw);
        moving = true;
        break;
    case 'd':
        cameraYaw += 0.05f;
        newX += moveSpeed * cos(cameraYaw);
        newZ += moveSpeed * sin(cameraYaw);
        moving = true;
        break;
    case 'p':
        if (showJumpMessage) {
            for (auto& shape : shapes) {
                if (isWithinRange(shape.posX, shape.posY, shape.posZ)) {
                    shape.isJumping = true;
                    break; // �÷�Χ�ڵĵ�һ����״��Ծ
                }
            }
        }
        break;
    case 'o':
        for (auto& shape : shapes) {
            if ((shape.type == SPHERE || shape.type == ROTATING_SPHERE) && isWithinRange(shape.posX, shape.posY, shape.posZ)) {
                shape.isSpinning = true;
                break; // �÷�Χ�ڵĵ�һ����ת�������Բ����ת
            }
        }
        break;
    case 'j':
        robot.isJumping = true;
        break;
    case '1':
        // �л�����һ�˳��ӽ�
        viewMode = 1;
        cameraYaw = robot.angleYaw * 3.1415926f / 180.0f;  // ȷ���Ƕȵ�λ��ȷ��������Ҫת��
        userPosX = robot.posX;
        userPosY = robot.posY + 1.0f;  // ȷ�������1.0f�Ǹ�����ĳ���������
        userPosZ = robot.posZ;
        break;
    case '2':
        // �л��������˳��ӽ�
        viewMode = 0;
        cameraYaw = robot.angleYaw * 3.1415926f / 180.0f;  // ȷ���Ƕȵ�λ��ȷ��������Ҫת��
        userPosX = robot.posX - 2.0f * sin(cameraYaw);  // ���ڻ����˵�ǰ���򣬼���������󷽵�λ��
        userPosY = robot.posY + 1.5f;  // ȷ�������1.5f�Ǹ�����ĳ���������
        userPosZ = robot.posZ + 2.0f * cos(cameraYaw);
        break;
    }

    if (moving) {
        isWalking = true;
    }

    // �����λ���Ƿ���ײ
    if (!isColliding(newX, newY, newZ)) {
        robot.posX = newX;
        robot.posY = newY;
        robot.posZ = newZ;

        // Update camera position
        if (viewMode == 0) { // Update camera position in third-person view
            userPosX = robot.posX - 2.0f * sin(cameraYaw);
            userPosY = robot.posY + 1.5f;
            userPosZ = robot.posZ + 2.0f * cos(cameraYaw);
        }
        else if (viewMode == 1) { // Update camera position in first-person view
            userPosX = robot.posX;
            userPosY = robot.posY + 1.0f;
            userPosZ = robot.posZ;
        }
        robot.angleYaw = cameraYaw * 180.0f / 3.1415926f; // Convert radians to degrees

        // Print the robot's current position
        std::cout << "Robot Position: (" << robot.posX << ", " << robot.posY << ", " << robot.posZ << ")\n";
    }
}

void onMouseMotion(int x, int y) {
    x = x * 2, y = y * 2;

    static int lastX = -1, lastY = -1;
    if (lastX == -1) {
        lastX = x;
        lastY = y;
    }


    int deltaX = x - lastX;
    int deltaY = y - lastY;


    float sensitivity = 0.005f;
    cameraYaw += deltaX * sensitivity;
    cameraPitch -= deltaY * sensitivity;

    if (cameraPitch > 1.5f) cameraPitch = 1.5f;
    if (cameraPitch < -1.5f) cameraPitch = -1.5f;

    lastX = x;
    lastY = y;

    // ���»����˵ĽǶ�
    robot.angleYaw = cameraYaw * 180.0f / 3.1415926f; // ������ת��Ϊ�Ƕ�

    // �仯ʱ�ػ�
    if (std::abs(deltaX) > 2 || std::abs(deltaY) > 2) {
        glutPostRedisplay();
    }
}

void update(int value) {
    bool anyInRange = false;
    for (auto& shape : shapes) {
        if (isWithinRange(shape.posX, shape.posY, shape.posZ)) {
            anyInRange = true;
        }

        if (shape.isJumping) {
            shape.jumpHeight += shape.jumpSpeed;
            if (shape.jumpHeight > 1.0f) {
                shape.jumpSpeed = -shape.jumpSpeed;
            }
            if (shape.jumpHeight < 0.0f) {
                shape.jumpHeight = 0.0f;
                shape.jumpSpeed = 0.05f;
                shape.isJumping = false;
            }
        }

        if (shape.isSpinning) {
            shape.spinAngle += 5.0f; // ��ת�Ƕ�����
            if (shape.spinAngle >= 360.0f) {
                shape.spinAngle = 0.0f;
            }
        }
    }

    const float jumpVelocity = 0.1f; // ��Ծ���ٶ�
    const float gravity = -0.01f; // �������ٶ�

    if (robot.isJumping) {
        robot.jumpSpeed += gravity;
        robot.jumpHeight += robot.jumpSpeed;

        if (robot.jumpHeight < 0.0f) {
            robot.jumpHeight = 0.0f;
            robot.jumpSpeed = jumpVelocity;
            robot.isJumping = false;
        }
    }
    if (++count1 % 5 == 0) { // ������·����
        if (isWalking) {
            walkCycle += 0.5f;
            if (walkCycle > 2 * 3.1415926f) {
                walkCycle -= 2 * 3.1415926f;
            }
        }
        else {
            walkCycle = 0.0f;
        }

        isWalking = false;
    }
    if (count1 == 1000) {
        count1 = 0;
    }
   

    // �����µĹ켣��
    if (!trajectory.empty() && trajectory.size() >= maxPoints) {
        trajectory.erase(trajectory.begin()); // �Ƴ���ɵĵ�
    }
    float radius = 0.5f;
    float theta = glutGet(GLUT_ELAPSED_TIME) / 1000.0f * 2.0f * 3.1415926f;
    trajectory.push_back({ radius * cosf(theta), radius * sinf(theta), 0.0f });

    if (!anyInRange) {
        currentMessageIndex = 0;
        showJumpMessage = false;
    }

    glutPostRedisplay();
    glutTimerFunc(16, update, 0);
}

void drawcolor(GLuint cube, int idx) {
    glColor3fv(_cubecol[idx]);
    glutSolidCube(1.0);
}

void drawModel(const Model& model) {
    glBegin(GL_TRIANGLES);
    for (size_t i = 0; i < model.faces.size(); i += 3) {
        int idx1 = model.faces[i];
        int idx2 = model.faces[i + 1];
        int idx3 = model.faces[i + 2];
        glVertex3f(model.vertices[idx1 * 3], model.vertices[idx1 * 3 + 1], model.vertices[idx1 * 3 + 2]);
        glVertex3f(model.vertices[idx2 * 3], model.vertices[idx2 * 3 + 1], model.vertices[idx2 * 3 + 2]);
        glVertex3f(model.vertices[idx3 * 3], model.vertices[idx3 * 3 + 1], model.vertices[idx3 * 3 + 2]);
    }
    glEnd();
}

void drawShadow(float x, float y, float z, ShapeType type) {
    float lightPos[] = { 0.0f, 10.0f, 0.0f }; // ��Դλ�ã��ߴ���
    float shadowMat[16] = {
        lightPos[1], 0, 0, 0,
        -lightPos[0], 0, -lightPos[2], -1,
        0, 0, lightPos[1], 0,
        0, 0, 0, lightPos[1]
    };

    glDisable(GL_LIGHTING); // ���ù����Ի�����Ӱ
    glColor3f(0.2f, 0.2f, 0.2f); // ��ɫ��Ӱ
    glPushMatrix();
    glMultMatrixf(shadowMat);
    glTranslatef(x, y, z);
    switch (type) {
    case CUBE:
        glutSolidCube(1.0);
        break;
    case SPHERE:
        glutSolidSphere(0.5, 20, 20);
        break;
    case CYLINDER:
        GLUquadric* quadric;
        quadric = gluNewQuadric();
        gluCylinder(quadric, 0.5, 0.5, 1.0, 20, 20);
        gluDeleteQuadric(quadric);
        break;
    case DOG:
        glScalef(0.1f, 0.1f, 0.1f); // ����ģ��
        drawModel(dogModel);
        break;
    case CAT:
        glScalef(0.1f, 0.1f, 0.1f); // ����ģ��
        drawModel(catModel);
        break;
    case ROTATING_SPHERE:
        glutSolidSphere(0.5, 20, 20);
        break;
    }
    glPopMatrix();
    glEnable(GL_LIGHTING); // �������ù���
}

void drawRotatingSphere(const Shape& shape) {
    float radius = 0.5f;
    int segments = 20;
    int rings = 20;

    glPushMatrix();
    glTranslatef(shape.posX, shape.posY + shape.jumpHeight, shape.posZ);

    for (int i = 0; i < segments; i++) {
        float theta = 2.0f * 3.1415926f * float(i) / float(segments);
        float x = radius * cosf(theta);
        float y = radius * sinf(theta);

        glBegin(GL_LINE_LOOP);
        for (int j = 0; j < rings; j++) {
            float phi = 2.0f * 3.1415926f * float(j) / float(rings);
            float px = x * cosf(phi) - y * sinf(phi);
            float py = x * sinf(phi) + y * cosf(phi);
            float pz = radius * sinf(theta);
            glVertex3f(px, py, pz);
        }
        glEnd();
    }

    glPopMatrix();
}

void drawTrajectory() {
    glColor3f(1.0f, 0.0f, 0.0f); // ��ɫ�켣
    glBegin(GL_LINE_STRIP);
    for (const auto& point : trajectory) {
        glVertex3f(point.x, point.y, point.z);
    }
    glEnd();
}

void drawShape(const Shape& shape) {
    glPushMatrix();
    glTranslatef(shape.posX, shape.posY + shape.jumpHeight, shape.posZ);
    if (shape.isSpinning) {
        glRotatef(shape.spinAngle, 0.0f, 1.0f, 0.0f); // ��ת
    }
    switch (shape.type) {
    case CUBE:
        glutSolidCube(1.0);
        break;
    case SPHERE:
        glutSolidSphere(0.5, 20, 20);
        break;
    case CYLINDER:
        GLUquadric* quadric;
        quadric = gluNewQuadric();
        gluCylinder(quadric, 0.5, 0.5, 1.0, 20, 20);
        gluDeleteQuadric(quadric);
        break;
    case DOG:
        glScalef(0.003f, 0.003f, 0.003f); // ����ģ��
        drawModel(dogModel);
        break;
    case CAT:
        glScalef(0.1f, 0.1f, 0.1f); // ����ģ��
        drawModel(catModel);
        break;
    case ROTATING_SPHERE:
        drawRotatingSphere(shape);
        drawTrajectory();
        break;
    }
    glPopMatrix();
}
void initTree() {
    glGenVertexArrays(1, &treeVAO);
    glGenBuffers(1, &treeVBO);
    glGenBuffers(1, &treeEBO);

    glBindVertexArray(treeVAO);

    glBindBuffer(GL_ARRAY_BUFFER, treeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(treeVertices), treeVertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, treeEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(treeIndices), treeIndices, GL_STATIC_DRAW);

    // λ������
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);
    // ������������
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

void drawTree() {
    glBindVertexArray(treeVAO);
    // ʹ����ɫ������
    glUseProgram(shaderProgram);

    // ����ľ����
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, treeTexture);
    glUniform1i(glGetUniformLocation(shaderProgram, "diffuseMap"), 0);

    // ���ù�λ�ú��ӽ�λ��
    glUniform3f(glGetUniformLocation(shaderProgram, "lightPos"), 5.0f, 5.0f, 5.0f);
    glUniform3f(glGetUniformLocation(shaderProgram, "viewPos"), userPosX, userPosY, userPosZ);

    // ����ģ�;��󣬽���ľ�ƶ����������Ա�
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(3.0f, 0.0f, 0.0f)); // ������Ҫ����λ��
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

    // ������ľ
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, 0);

    // ���
    glBindVertexArray(0);
    glUseProgram(0);
}

enum { BUILDING = 2 }; // ��������������
void initBuilding() {
    glGenVertexArrays(1, &VAOs[BUILDING]);
    glBindVertexArray(VAOs[BUILDING]);

    glGenBuffers(numperobj + 1, &buffers[Vertices + BUILDING * numperobj]);

    glBindBuffer(GL_ARRAY_BUFFER, buffers[Vertices + BUILDING * numperobj]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(buildingVertices), buildingVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, buffers[Vertices + BUILDING * numperobj + 1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(buildingNormals), buildingNormals, GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[Elements + BUILDING * numperobj]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(buildingIndices), buildingIndices, GL_STATIC_DRAW);

    glBindVertexArray(0);
}

void drawBuilding() {
    glBindVertexArray(VAOs[BUILDING]);

    // ���þ��淴�����
    GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat mat_shininess[] = { 128.0 };
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

    // ���û�����������
    GLfloat mat_ambient[] = { 0.2, 0.2, 0.2, 1.0 };
    GLfloat mat_diffuse[] = { 0.8, 0.8, 0.8, 1.0 };
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);

    glDrawElements(GL_TRIANGLES, sizeof(buildingIndices) / sizeof(GLubyte), GL_UNSIGNED_BYTE, 0);

    glBindVertexArray(0);
}

void drawRobot(const Shape& robot) {
    glPushMatrix();
    glTranslatef(robot.posX, robot.posY + robot.jumpHeight, robot.posZ); // �����Ծ�߶�
    glRotatef(-cameraYaw * 180.0f / 3.1415926f, 0.0f, 1.0f, 0.0f); // ���������ƫ������ת������

    // ���ƻ���������
    glPushMatrix();
    glScalef(0.3f, 0.3f, 0.3f); // ��С������
    glColor3fv(_cubecol[1]); // ��ɫ
    glutSolidCube(1.0);
    glPopMatrix();

    // �������
    glPushMatrix();
    float leftArmAngle = sin(walkCycle) * 45.0f; // ��۰ڶ��Ƕ�
    glTranslatef(0.18f, 0.0f, 0.0f);
    glRotatef(leftArmAngle, 1.0f, 0.0f, 0.0f); // ������۰ڶ���
    glScalef(0.15f, 0.15f, 0.15f);
    glColor3fv(_cubecol[0]); // ��ɫ
    glutSolidCube(1.0f);
    glPopMatrix();

    // �����ұ�  
    glPushMatrix();
    float rightArmAngle = -sin(walkCycle) * 45.0f; // �ұ۰ڶ��Ƕ�
    glTranslatef(-0.18f, 0.0f, 0.0f);
    glRotatef(rightArmAngle, 1.0f, 0.0f, 0.0f); // �����ұ۰ڶ���
    glScalef(0.15f, 0.15f, 0.15f);
    glColor3fv(_cubecol[0]); // ��ɫ
    glutSolidCube(1.0f);
    glPopMatrix();

    // ��������
    glPushMatrix();
    float leftLegAngle = sin(walkCycle) * 30.0f;
    glTranslatef(0.06f, -0.4f, 0.0f);
    glRotatef(leftLegAngle, 1.0f, 0.0f, 0.0f); // ������Ȱڶ�
    glScalef(0.15f, 0.15f, 0.15f);
    glColor3fv(_cubecol[0]); // ��ɫ
    glutSolidCube(1.0f);
    glPopMatrix();

    // ��������
    glPushMatrix();
    float rightLegAngle = -sin(walkCycle) * 30.0f;
    glTranslatef(-0.06f, -0.4f, 0.0f);
    glRotatef(rightLegAngle, 1.0f, 0.0f, 0.0f); // ������Ȱڶ�
    glScalef(0.15f, 0.15f, 0.15f);
    glColor3fv(_cubecol[0]); // ��ɫ
    glutSolidCube(1.0f);
    glPopMatrix();

    // ����ͷ��
    glPushMatrix();
    glTranslatef(0.0f, 0.24f, 0.0f);
    glScalef(0.15f, 0.15f, 0.15f);
    glColor3fv(_cubecol[3]); // ��ɫ
    glutSolidCube(1.0f);
    glPopMatrix();

    // ��������
    glPushMatrix();
    glTranslatef(0.03f, 0.3f, 0.06f);
    glScalef(0.03f, 0.03f, 0.03f);
    glColor3fv(_cubecol[4]); // ��ɫ
    glutSolidCube(1.0f);
    glPopMatrix();

    // ��������
    glPushMatrix();
    glTranslatef(-0.03f, 0.3f, 0.06f);
    glScalef(0.03f, 0.03f, 0.03f);
    glColor3fv(_cubecol[4]); // ��ɫ
    glutSolidCube(1.0f);
    glPopMatrix();

    glPopMatrix();
}
void init() {
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glClearColor(0.53, 0.81, 0.92, 1.0);
    // ������ɫ����Ϊ��ɫ
    glMatrixMode(GL_PROJECTION);
    gluPerspective(45.0, 1.0, 1.0, 200.0);
    glMatrixMode(GL_MODELVIEW);

    // ���ò�������
    GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat mat_shininess[] = { 50.0 };
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

    // ���ù���ģ��
    // ���ù���ģ��
    GLfloat light_ambient[] = { 0.2, 0.2, 0.2, 1.0 };
    GLfloat light_diffuse[] = { 0.8, 0.8, 0.8, 1.0 };
    GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat light_position[] = { 0.0, 10.0, 0.0, 1.0 }; // �ߴ���Դ

    // ���һ��������̫���Ĵ��Դ
    GLfloat sun_light_ambient[] = { 0.5, 0.5, 0.5, 1.0 };
    GLfloat sun_light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat sun_light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat sun_light_position[] = { 5.0, 5.0, 5.0, 1.0 }; // ������̫����λ��

    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);

    glEnable(GL_LIGHT0);

    glLightfv(GL_LIGHT1, GL_AMBIENT, sun_light_ambient);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, sun_light_diffuse);
    glLightfv(GL_LIGHT1, GL_SPECULAR, sun_light_specular);
    glLightfv(GL_LIGHT1, GL_POSITION, sun_light_position);

    glEnable(GL_LIGHT1);

    glEnable(GL_LIGHTING);
    loadShadersAndTextures();

    // ���� VAOs
    glGenVertexArrays(numobjects, VAOs);

    // �������λ�ú����͵���״
    generateRandomShapes();
    // �����ת����
    // shapes.push_back({ ROTATING_SPHERE, 0.0f, 0.0f, 0.0f });

     //����С����Сèģ��
    /* loadObj("dog.Obj", dogModel);
     loadObj("cat.obj", catModel);*/

     // ��ʼ��������
    initBuilding();
    // ��ʼ����ľ
    initTree();

    initRenderingfloor();
    //initRenderingwall();

    // �������λ�ú����͵���״
    generateRandomShapes();
    // �����ת����
    shapes.push_back({ ROTATING_SPHERE, 0.0f, -1.0f, 0.0f });

    // ��ʼ��������λ�úͽǶ�
    robot.type = CUBE; // ���������������ʾ
    robot.posX = userPosX + 2.0f * sin(cameraYaw);
    robot.posY = userPosY - 2.5f; // ����������λ�ã������ڵ�����
    robot.posZ = userPosZ - 2.0f * cos(cameraYaw);
    robot.angleYaw = cameraYaw * 180.0f / 3.1415926f;

    // ����С����Сèģ��
    //loadObj("D:\\Resource\\CG\\ass\\0\\Dog\\files\\dog.Obj", dogModel);
    //loadObj("teapot.obj", catModel);
}
void drawBurger() {
    glPushMatrix();
    glTranslatef(1.5f, 3.0f, -10.0f); // ����λ��
    glScalef(7.0f, 7.0f, 7.0f); // �������ű���
    burger();
    glPopMatrix();
}
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    // ��������ͷλ�úͷ���
    float lookAtX = userPosX + cos(cameraPitch) * sin(cameraYaw);
    float lookAtY = userPosY + sin(cameraPitch);
    float lookAtZ = userPosZ - cos(cameraPitch) * cos(cameraYaw);
    gluLookAt(userPosX, userPosY, userPosZ, lookAtX, lookAtY, lookAtZ, 0.0, 1.0, 0.0);

    // ���ù���
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    // ���ù�Դ
    GLfloat light_position[] = { 0.0, 10.0, 0.0, 1.0 }; // �ߴ���Դ
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);

    // ���Ƶذ�
    drawFloor();

    // ������״����Ӱ
    for (const auto& shape : shapes) {
        drawShadow(shape.posX, 0.1f, shape.posZ, shape.type); // ������Ӱ
        drawShape(shape); // ������״

        // ��ʾ�ı���Ϣ
        if (isWithinRange(shape.posX, shape.posY, shape.posZ)) {
            displayMessage(shape.posX, shape.posY, shape.posZ);
        }
    }
    for (auto& cloud : clouds) {
        drawCloud(cloud.x, cloud.z);
        cloud.z += cloudSpeed; // �����Ƶ�λ��
    }

    // �Ƴ������߽����
    clouds.erase(std::remove_if(clouds.begin(), clouds.end(), [](const Cloud& c) { return c.z > 50; }), clouds.end());

    // ���ƻ�����
    drawRobot(robot);
//    drawBuilding();
 //   drawBurger();
//    drawCola();
    drawTree();
    drawScene();
    glutSwapBuffers();
}
void addCloud(int value) {
    // ������Ƶ����λ��
    Cloud newCloud;
    newCloud.x = -50 + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / 100.0f));
    newCloud.z = -50;
    clouds.push_back(newCloud);

    int nextCloudIn = 1000 + rand() % 1000; // 1��3����������
    glutTimerFunc(nextCloudIn, addCloud, 0);
}
void timer(int value) {
    glutPostRedisplay(); // �����ػ洰��
    glutTimerFunc(16, timer, 0); // ���ü�ʱ��ԼΪ60Hz
}
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("3D Interaction");
    // ��ʼ�� GLEW
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return -1;
    }
    init();
    glutDisplayFunc(display);
    glutTimerFunc(0, timer, 0);
    glutTimerFunc(100, addCloud, 0); // ��500�����ʼ������
    glutSpecialFunc(onSpecialKey);
    glutKeyboardFunc(onKeyboard);
    glutKeyboardUpFunc(onKeyUp);
    glutPassiveMotionFunc(onMouseMotion);
    glutMouseFunc(onMouseClick);
    glutTimerFunc(16, update, 0);

    glutMainLoop();
    return 0;
}
