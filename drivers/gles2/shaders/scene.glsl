[vertex]

#ifdef USE_GLES_OVER_GL
#define mediump
#define highp
#else
precision mediump float;
precision mediump int;
#endif

attribute highp vec3 vertex; // attrib:0


uniform mat4 camera_matrix;

VERTEX_SHADER_GLOBALS

void main() {


	vec4 outvec = vec4(vertex, 1.0);

{

VERTEX_SHADER_CODE

}

	gl_Position = outvec;

}

[fragment]

#ifdef USE_GLES_OVER_GL
#define mediump
#define highp
#else
precision mediump float;
precision mediump int;
#endif

uniform vec4 color;


FRAGMENT_SHADER_GLOBALS


void main() {

{

FRAGMENT_SHADER_CODE


}

	gl_FragColor = color;

}