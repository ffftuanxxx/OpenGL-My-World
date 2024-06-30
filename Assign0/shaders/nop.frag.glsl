# version 330 core
// Do not modify the version directive to anything older than 330.

// Fragment shader inputs are outputs of same name from vertex shader
in vec3 Color;

// Uniform variables (none)

// Output
out vec4 fragColor;

void main (void) 
{        
	fragColor = vec4(Color, 1.0f);
}
