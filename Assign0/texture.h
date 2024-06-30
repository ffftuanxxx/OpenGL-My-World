#ifndef UTILS_H
#define UTILS_H

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

void init_texture(GLuint* textures, int type_num);

void set_texture(GLuint* textures, int index, char* filename);
#endif