[vertex]

#ifdef USE_GLES_OVER_GL
#define mediump
#define highp
#else
precision mediump float;
precision mediump int;
#endif

attribute highp vec2 vertex; // attrib:0
attribute highp vec2 uv; // attrib:4

varying highp vec2 uv_interp;

void main() {

	uv_interp=uv;
	gl_Position=vec4(vertex,0,1);
}

[fragment]

#ifdef USE_GLES_OVER_GL
#define mediump
#define highp
#else
precision mediump float;
precision mediump int;
#endif

#ifdef USE_SOURCE_PANORAMA
uniform sampler2D source_panorama; //texunit:0
#else
uniform samplerCube source_cube; //texunit:0
#endif

uniform int face_id;
uniform float roughness;
varying highp vec2 uv_interp;


#define M_PI 3.14159265359

#ifdef LOW_QUALITY

#define SAMPLE_COUNT 64

#else

#define SAMPLE_COUNT 512

#endif

uniform bool z_flip;

void main() {

	gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0);

}

