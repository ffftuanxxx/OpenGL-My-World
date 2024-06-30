/**** Basic setup for defining and drawing objects ****/
// Moved to a header for the second OpenGL program

#ifndef __INCLUDEGEOMETRY
#define __INCLUDEGEOMETRY


#include <vector>
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <vector>

// May need to replace with absolute path on some systems
#define PATH_TO_TEAPOT_OBJ "teapot.obj"


const int numobjects = 3;  // number of objects (2 planes) for buffer 
const int numperobj = 3;  // Vertices, colors, indices
GLuint VAOs[numobjects];    // A Vertex Array Object per object
GLuint buffers[numperobj * numobjects]; // List of buffers geometric data 
GLuint objects[numobjects]; // For each object
GLenum PrimType[numobjects];// Primitive type (triangles, strips) of the object
GLsizei NumElems[numobjects]; //Number of triangles/geometric elements for each object


// For the geometry of the teapot
std::vector <glm::vec3> teapotVertices;
std::vector <glm::vec3> teapotNormals;
std::vector <unsigned int> teapotIndices;

// For the geometry of the sphere
std::vector <glm::vec3> sphereVertices;
std::vector <glm::vec3> sphereNormals;
std::vector <unsigned int> sphereIndices;

// To be used as a matrix stack for the modelview.
std::vector <glm::mat4> modelviewStack;

// ** NEW ** Floor Geometry is specified with a vertex array
// ** NEW ** Same for other Geometry 

//enum {Vertices, Colors, Elements} ; // For arrays for object 
//enum {FLOOR, CUBE} ; // For objects, for the floor
enum { Vertices, Colors, Elements }; // 0, 1, 2, for buffers of an object 
enum { FLOOR, FLOOR2 }; // For objects, for the floor 0, 1


const GLfloat floorverts[4][3] = { // vertex for floor plane
  {0.5, -0.0, -0.5} ,{0.5, 0.0, 0.5}, {-0.5, 0.0, 0.5}, {-0.5, -0.0, -0.5}  // (x, y, z)
};
const GLfloat floorcol[4][3] = { // color (r, g, b)
  {1.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, {0.0, 0.0, 1.0}, {1.0, 1.0, 1.0}
};  // red, green, blue, white
const GLubyte floorinds[1][6] = { {0, 1, 2, 0, 2, 3} }; //specification for index of triangles, right hand          
const GLfloat floorverts2[4][3] = {
  {0.5, 0.5, 1.0}, {-0.5, 0.5, 1.0}, {-0.5, -0.5, 1.0}, {0.5, -0.5, 1.0}
};
const GLfloat floorcol2[4][3] = {
  {1.0, 0.0, 0.0}, {1.0, 0.0, 0.0}, {1.0, 0.0, 0.0}, {1.0, 0.0, 0.0}
}; // all red on top
const GLubyte floorinds2[1][6] = { {0, 1, 2, 0, 2, 3} }; //triangles 

const GLfloat wd = 0.1;
const GLfloat ht = 0.5;

const GLfloat cubeverts[8][3] = {
	{ -wd, -wd, 0.0 },{ -wd, wd, 0.0 },{ wd, wd, 0.0 },{ wd, -wd, 0.0 },
	{ -wd, -wd, ht },{ wd, -wd, ht },{ wd, wd, ht },{ -wd, wd, ht }
};
GLfloat cubecol[12][3];
const GLubyte cubeinds[12][3] = {
	{ 0, 1, 2 },{ 0, 2, 3 }, // BOTTOM 
	{ 4, 5, 6 },{ 4, 6, 7 }, // TOP 
	{ 0, 4, 7 },{ 0, 7, 1 }, // LEFT 
	{ 0, 3, 5 },{ 0, 5, 4 }, // FRONT
	{ 3, 2, 6 },{ 3, 6, 5 }, // RIGHT 
	{ 1, 7, 6 },{ 1, 6, 2 } // BACK
};
//法线向量
const GLfloat buildingNormals[8][3] = {
	{ 0.0, 0.0, -1.0 }, { 0.0, 0.0, -1.0 }, { 0.0, 0.0, -1.0 }, { 0.0, 0.0, -1.0 },
	{ 0.0, 0.0, 1.0 }, { 0.0, 0.0, 1.0 }, { 0.0, 0.0, 1.0 }, { 0.0, 0.0, 1.0 }
};
// 新增建筑物的顶点和索引
const GLfloat buildingVertices[8][3] = {
	{ -1.0, 0.0, -1.0 }, { 1.0, 0.0, -1.0 }, { 1.0, 2.0, -1.0 }, { -1.0, 2.0, -1.0 },
	{ -1.0, 0.0, 1.0 }, { 1.0, 0.0, 1.0 }, { 1.0, 2.0, 1.0 }, { -1.0, 2.0, 1.0 }
};

const GLubyte buildingIndices[12][3] = {
	{ 0, 1, 2 }, { 2, 3, 0 }, // Front
	{ 4, 5, 6 }, { 6, 7, 4 }, // Back
	{ 0, 1, 5 }, { 5, 4, 0 }, // Bottom
	{ 2, 3, 7 }, { 7, 6, 2 }, // Top
	{ 3, 0, 4 }, { 4, 7, 3 }, // Left
	{ 1, 2, 6 }, { 6, 5, 1 }  // Right
};

const GLfloat radius = 0.1;
const int sectors = 36;
const int stacks = 18;
bool smooth;

void initobject(GLuint object, GLfloat* vert, GLint sizevert, GLfloat* col, GLint sizecol, GLubyte* inds, GLint sizeind, GLenum type);
void drawobject(GLuint object);
void initcubes(GLuint object, GLfloat* vert, GLint sizevert, GLubyte* inds, GLint sizeind, GLenum type);
void drawcolor(GLuint object, GLuint color);
void inittexture(const char* filename, GLuint program);
void drawtexture(GLuint object, GLuint texture);
void loadteapot();
void drawteapot();

void initsphere();
void drawsphere();

void pushMatrix(glm::mat4);
void popMatrix(glm::mat4&);

void initobject(GLuint object, GLfloat* vert, GLint sizevert, GLfloat* col, GLint sizecol, GLubyte* inds, GLint sizeind, GLenum type) {
	int offset = object * numperobj;
	glBindVertexArray(VAOs[object]);
	glBindBuffer(GL_ARRAY_BUFFER, buffers[Vertices + offset]);
	glBufferData(GL_ARRAY_BUFFER, sizevert, vert, GL_STATIC_DRAW);
	// Use layout location 0 for the vertices
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

	glBindBuffer(GL_ARRAY_BUFFER, buffers[Colors + offset]);
	glBufferData(GL_ARRAY_BUFFER, sizecol, col, GL_STATIC_DRAW);
	// Use layout location 1 for the colors
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[Elements + offset]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeind, inds, GL_STATIC_DRAW);
	PrimType[object] = type;
	NumElems[object] = sizeind;
	// Prevent further modification of this VAO by unbinding it
	glBindVertexArray(0);
}


void drawcolor(GLuint object, GLuint color) {
	glBindVertexArray(VAOs[object + color]);
	glDrawElements(PrimType[object], NumElems[object], GL_UNSIGNED_BYTE, 0);
	glBindVertexArray(0);
}

// And a function to draw with textures, similar to drawobject
void drawtexture(GLuint object, GLuint texture) {
	glBindTexture(GL_TEXTURE_2D, texture);
	glBindVertexArray(VAOs[object]);
	glDrawElements(PrimType[object], NumElems[object], GL_UNSIGNED_BYTE, 0);
	glBindVertexArray(0);
}

void drawobject(GLuint object) {
	glBindVertexArray(VAOs[object]);
	glDrawElements(PrimType[object],
		NumElems[object], GL_UNSIGNED_BYTE, 0);
	glBindVertexArray(0); //unbind
}

void pushMatrix(glm::mat4 mat) {
	modelviewStack.push_back(glm::mat4(mat));
}

// This function pops a matrix from the modelview stack and assigns that to the matrix passed in
void popMatrix(glm::mat4& mat) {
	if (modelviewStack.size()) {
		mat = glm::mat4(modelviewStack.back());
		modelviewStack.pop_back();
	}
	else { // Just to prevent an unwanted error
		mat = glm::mat4(1.0f);
	}
}

#endif
