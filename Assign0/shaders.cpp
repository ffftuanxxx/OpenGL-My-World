#include <iostream>
#include <fstream>
#include <cstring>
#include <string>
#include <GL/glew.h>
#include <GL/glut.h>

using namespace std;

string textFileRead(const char* filename) {
    string str, ret = "";
    ifstream in;
    in.open(filename);
    if (in.is_open()) {
        getline(in, str);
        while (in) {
            ret += str + "\n";
            getline(in, str);
        }
        return ret;
    }
    else {
        cerr << "Unable to Open File " << filename << "\n";
        throw 2;
    }
}

void programerrors(const GLint program) {
    GLint length;
    GLchar* log;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
    log = new GLchar[length + 1];
    glGetProgramInfoLog(program, length, &length, log);
    cout << "Compile Error, Log Below\n" << log << "\n";
    delete[] log;
}
void shadererrors(const GLint shader) {
    GLint length;
    GLchar* log;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
    log = new GLchar[length + 1];
    glGetShaderInfoLog(shader, length, &length, log);
    cout << "Compile Error, Log Below\n" << log << "\n";
    delete[] log;
}


GLuint initshaders(GLenum type, const char* filename) {
    // Using GLSL shaders, OpenGL book, page 679 of 7th edition
    GLuint shader = glCreateShader(type);
    GLint compiled;
    string str = textFileRead(filename);
    const GLchar* cstr = str.c_str();
    glShaderSource(shader, 1, &cstr, NULL);
    glCompileShader(shader);
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled); //test 
    // shader if successfully compiled
    if (!compiled) {
        shadererrors(shader);
        throw 3;
    }
    cout << "Shader file " << filename << " successfully compiled." << endl;
    return shader;
}


GLuint initprogram(GLuint vertexshader, GLuint fragmentshader)
{
    GLuint program = glCreateProgram();
    GLint linked;
    glAttachShader(program, vertexshader);
    glAttachShader(program, fragmentshader);
    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &linked);
    if (linked) glUseProgram(program);
    else {
        programerrors(program);
        throw 4;
    }
    cout << "Shader program successfully attached and linked." << endl;
    return program;
}

