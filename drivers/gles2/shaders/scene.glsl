[vertex]

#ifdef USE_GLES_OVER_GL
#define mediump
#define highp
#else
precision mediump float;
precision mediump int;
#endif

#include "stdlib.glsl"

attribute highp vec3 vertex; // attrib:0
attribute vec4 color_attrib; // attrib:3
attribute vec2 uv_attrib; // attrib:4

uniform mat4 model_matrix;
uniform mat4 camera_matrix;
uniform mat4 camera_inverse_matrix;
uniform mat4 projection_matrix;
uniform mat4 projection_inverse_matrix;


varying highp vec3 vertex_interp;
varying vec4 color_interp;
varying vec2 uv_interp;

VERTEX_SHADER_GLOBALS

void main() {

	vertex_interp = vertex;
	color_interp = color_attrib;
	uv_interp = uv_attrib;
	vec4 outvec = vec4(vertex, 1.0);

	mat4 model_matrix_copy = model_matrix;

	mat4 world_transform = mat4(1.0);

{

VERTEX_SHADER_CODE

}

	gl_Position = projection_matrix * camera_matrix * model_matrix_copy * outvec;

}

[fragment]

#ifdef USE_GLES_OVER_GL
#define mediump
#define highp
#else
precision mediump float;
precision mediump int;
#endif

#include "stdlib.glsl"

uniform vec4 color;

varying highp vec3 vertex_interp;
varying vec4 color_interp;
varying vec2 uv_interp;

FRAGMENT_SHADER_GLOBALS

void main() {

	highp vec3 vertex = vertex_interp;
	vec3 albedo = vec3(0.8, 0.8, 0.8);
	vec3 transmission = vec3(0.0);
	float metallic = 0.0;
	float specular = 0.5;
	vec3 emission = vec3(0.0, 0.0, 0.0);
	float roughness = 1.0;
	float rim = 0.0;
	float rim_tint = 0.0;
	float clearcoat = 0.0;
	float clearcoat_gloss = 0.0;
	float anisotropy = 1.0;
	vec2 anisotropy_flow = vec2(1.0,0.0);

	float alpha = 1.0;

{

FRAGMENT_SHADER_CODE


}

	gl_FragColor = vec4(albedo, alpha);

}
