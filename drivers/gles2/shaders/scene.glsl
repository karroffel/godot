[vertex]

#ifdef USE_GLES_OVER_GL
#define mediump
#define highp
#else
precision mediump float;
precision mediump int;
#endif

attribute highp vec3 vertex; // attrib:0
attribute vec4 color_attrib; // attrib:3
attribute vec2 uv_attrib; // attrib:4

uniform mat4 model_matrix;
uniform mat4 camera_transform;
uniform mat4 projection_matrix;


varying highp vec3 vertex_interp;
varying vec4 color_interp;
varying vec2 uv_interp;

VERTEX_SHADER_GLOBALS

void main() {

	vertex_interp = vertex;
	color_interp = color_attrib;
	uv_interp = uv_attrib;
	vec4 outvec = vec4(vertex, 1.0);

{

VERTEX_SHADER_CODE

}

	gl_Position = projection_matrix * camera_transform * model_matrix * outvec;

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

varying highp vec3 vertex_interp;
varying vec4 color_interp;
varying vec2 uv_interp;

FRAGMENT_SHADER_GLOBALS

vec3 select(vec3 a, vec3 b, bvec3 c) {
	vec3 ret;

	ret.x = c.x ? b.x : a.x;
	ret.y = c.y ? b.y : a.y;
	ret.z = c.z ? b.z : a.z;

	return ret;
}

void main() {

	highp vec3 vertex = vertex_interp;
	vec3 albedo = vec3(0.8, 0.8, 0.8);


{

FRAGMENT_SHADER_CODE


}

	gl_FragColor = color * vec4(albedo, 1.0);

}
