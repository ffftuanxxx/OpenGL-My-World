#version 330 core
// Do not use any version older than 330!

// Inputs to the fragment shader are outputs of the same name of the vertex shader
in vec4 myvertex;
in vec3 mynormal;
in vec2 texcoord;

// Output the frag color
out vec4 fragColor;

uniform sampler2D tex ; 
uniform int istex ; 
uniform int islight ; // are we lighting. 
uniform vec3 color;

// The commented code below hardcodes directions/colors
// Instead we use uniform variables set from opengl
// uniform vec3 lightdirn = vec3 (0.0,0.0,1.0) ; 
// uniform vec4 lightcolor = vec4(1.0,1.0,1.0,1.0) ; 

// Assume light 0 is directional, light 1 is a point light.  
// The actual light values are passed from the main OpenGL program. 
// This could of course be fancier.  My goal is to illustrate a simple idea. 

uniform vec3 light0dirn ; 
uniform vec4 light0color ; 
uniform vec4 light1posn ; 
uniform vec4 light1color ; 

// Now, set the material parameters.  These could be bound to 
// a buffer.  But for now, I'll just make them uniform.  
// I use ambient, diffuse, specular, shininess.
// But, the ambient is just additive and doesn't multiply the lights.  

uniform vec4 ambient ; 
uniform vec4 diffuse ; 
uniform vec4 specular ; 
uniform float shininess ; 

vec4 ComputeLight (const in vec3 direction, const in vec4 lightcolor, const in vec3 normal, const in vec3 halfvec, const in vec4 mydiffuse, const in vec4 myspecular, const in float myshininess) {

        float nDotL = dot(normal, direction)  ;         
        vec4 lambert = mydiffuse * lightcolor * max (nDotL, 0.0) ;  

        float nDotH = dot(normal, halfvec) ; 
        vec4 phong = myspecular * lightcolor * pow (max(nDotH, 0.0), myshininess) ; 

        vec4 retval = lambert + phong ; 
        return retval ;            
}       

void main (void) 
{       
    if (istex > 0) fragColor = texture(tex, texcoord); 
    else if (islight == 0) fragColor = vec4(color, 1.0f) ; 
    else { 
        // They eye is always at (0,0,0) looking down -z axis 
        // Also compute current fragment position and direction to eye 

        const vec3 eyepos = vec3(0,0,0) ; 
        vec3 mypos = myvertex.xyz / myvertex.w ; // Dehomogenize current location 
        vec3 eyedirn = normalize(eyepos - mypos) ; 

        // Compute normal, needed for shading. 
        vec3 normal = normalize(mynormal) ; 

        // Light 0, directional
        vec3 direction0 = normalize (light0dirn) ; // L
        vec3 half0 = normalize (direction0 + eyedirn) ; // L+v
        vec4 col0 = ComputeLight(direction0, light0color, normal, half0, diffuse, specular, shininess) ;

        // Light 1, point 
        vec3 position = light1posn.xyz / light1posn.w ; 
        vec3 direction1 = normalize (position - mypos) ; // no attenuation 
        vec3 half1 = normalize (direction1 + eyedirn) ;  // L+v
        vec4 col1 = ComputeLight(direction1, light1color, normal, half1, diffuse, specular, shininess) ;
        
        fragColor = ambient + col0 + col1 ; 
        }
}
