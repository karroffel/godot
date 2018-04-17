[vertex]

#ifdef USE_GLES_OVER_GL
#define mediump
#define highp
#else
precision mediump float;
precision mediump int;
#endif

#include "stdlib.glsl"



//
// attributes
//

attribute highp vec3 vertex_attrib; // attrib:0
attribute vec3 normal_attrib; // attrib:1

#ifdef ENABLE_COLOR_INTERP
attribute vec4 color_attrib; // attrib:3
#endif

#ifdef ENABLE_UV_INTERP
attribute vec2 uv_attrib; // attrib:4
#endif

#ifdef ENABLE_UV2_INTERP
attribute vec2 uv2_attrib; // attrib:5
#endif

#ifdef USE_SKELETON
attribute highp vec4 bone_transform_row_0; // attrib:9
attribute highp vec4 bone_transform_row_1; // attrib:10
attribute highp vec4 bone_transform_row_2; // attrib:11
#endif



//
// uniforms
//

uniform mat4 model_matrix;
uniform mat4 camera_matrix;
uniform mat4 camera_inverse_matrix;
uniform mat4 projection_matrix;
uniform mat4 projection_inverse_matrix;

uniform highp float time;




//
// varyings
//

varying highp vec3 vertex_interp;
varying vec3 normal_interp;

#ifdef ENABLE_COLOR_INTERP
varying vec4 color_interp;
#endif

#ifdef ENABLE_UV_INTERP
varying vec2 uv_interp;
#endif

#ifdef ENABLE_UV2_INTERP
varying vec2 uv2_interp;
#endif



VERTEX_SHADER_GLOBALS

void main() {

	vertex_interp = vertex_attrib;
	normal_interp = normal_attrib;

#ifdef ENABLE_COLOR_INTERP
	color_interp = color_attrib;
#endif

#ifdef ENABLE_UV_INTERP
	uv_interp = uv_attrib;
#endif

#ifdef ENABLE_UV2_INTERP
	uv2_interp = uv2_attrib;
#endif

	vec4 outvec = vec4(vertex_attrib, 1.0);

	mat4 model_matrix_copy = model_matrix;

#ifdef USE_SKELETON
	highp mat4 bone_transform = mat4(1.0);
	bone_transform[0] = vec4(bone_transform_row_0.x, bone_transform_row_1.x, bone_transform_row_2.x, 0.0);
	bone_transform[1] = vec4(bone_transform_row_0.y, bone_transform_row_1.y, bone_transform_row_2.y, 0.0);
	bone_transform[2] = vec4(bone_transform_row_0.z, bone_transform_row_1.z, bone_transform_row_2.z, 0.0);
	bone_transform[3] = vec4(bone_transform_row_0.w, bone_transform_row_1.w, bone_transform_row_2.w, 1.0);

	model_matrix_copy = bone_transform * model_matrix;
#endif


	// FIXME: what to do about instancing?
	vec4 instance_custom = vec4(0.0);


	mat4 world_transform = mat4(1.0);

{

VERTEX_SHADER_CODE

}

	gl_Position = (projection_matrix * (camera_matrix * (model_matrix_copy * outvec)));

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

//
// uniforms
//

uniform mat4 model_matrix;
uniform mat4 camera_matrix;
uniform mat4 camera_inverse_matrix;
uniform mat4 projection_matrix;
uniform mat4 projection_inverse_matrix;

uniform highp float time;


#ifdef SCREEN_UV_USED
uniform vec2 screen_pixel_size;
#endif

//
// varyings
//

varying highp vec3 vertex_interp;
varying vec3 normal_interp;

#ifdef ENABLE_COLOR_INTERP
varying vec4 color_interp;
#endif

#ifdef ENABLE_UV_INTERP
varying vec2 uv_interp;
#endif

#ifdef ENABLE_UV2_INTERP
varying vec2 uv2_interp;
#endif

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

#ifdef ALPHA_SCISSOR_USED
	float alpha_scissor = 0.5;
#endif

#ifdef SCREEN_UV_USED
	vec2 screen_uv = gl_FragCoord.xy * screen_pixel_size;
#endif

{

FRAGMENT_SHADER_CODE


}

#ifdef ALPHA_SCISSOR_USED
	if (alpha < alpha_scissor) {
		discard;
	}
#endif

	gl_FragColor = vec4(albedo, alpha);

}
