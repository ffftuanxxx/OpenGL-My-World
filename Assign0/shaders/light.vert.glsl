#version 330 core
// Do not use any version older than 330!

// Inputs
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal; // For this project, we will actually use normals now
layout (location = 2) in vec2 texCoords;

// Extra outputs, if any
out vec4 myvertex;
out vec3 mynormal;
out vec2 texcoord;

// Uniform variables
uniform mat4 projection;
uniform mat4 modelview;
uniform int istex ; 

void main() {
    gl_Position = projection * modelview * vec4(position, 1.0f);
    mynormal = mat3(transpose(inverse(modelview))) * normal ; 
    myvertex = modelview * vec4(position, 1.0f) ; 
	texcoord = vec2 (0.0, 0.0); // Default value just to prevent errors
	if (istex != 0){
		texcoord = texCoords;
	}
}

