[vertex]

#ifdef USE_GLES_OVER_GL
#define mediump
#define highp
#else
precision mediump float;
precision mediump int;
#endif

uniform highp mat4 projection_matrix;
uniform highp mat4 modelview_matrix;
uniform highp mat4 extra_matrix;
attribute highp vec2 vertex; // attrib:0
// attribute vec4 color_attrib; // attrib:3
// attribute highp vec2 uv_attrib; // attrib:4

varying vec2 uv_interp;
// varying vec4 color_interp;

#ifdef USE_TEXTURE_RECT

uniform vec4 dst_rect;
uniform vec4 src_rect;

#endif

VERTEX_SHADER_GLOBALS

void main() {

        // color_interp = color_attrib;
        // uv_interp = uv_attrib;

#ifdef USE_TEXTURE_RECT
	highp vec4 outvec = vec4(
	        dst_rect.xy + (vertex.xy * dst_rect.zw),
	        0.0,
	        1.0);

	uv_interp = vec2(vertex.x, vertex.y);
	//highp vec4 outvec = vec4(vertex, 0.0, 1.0);


#else
        highp vec4 outvec = vec4(vertex, 0.0, 1.0);
#endif

{
        vec2 src_vtx=outvec.xy;
VERTEX_SHADER_CODE

}


	gl_Position = projection_matrix * outvec;

}

[fragment]

#ifdef USE_GLES_OVER_GL
#define mediump
#define highp
#else
precision mediump float;
precision mediump int;
#endif

 // texunit:0
uniform sampler2D texture;

varying vec2 uv_interp;
// varying vec4 color_interp;


uniform vec4 final_modulate;


FRAGMENT_SHADER_GLOBALS


void main() {

	// vec4 color = color_interp;
	vec2 uv = uv_interp;
	vec4 color = texture2D(texture, uv);

{

FRAGMENT_SHADER_CODE


}
	color = color * final_modulate;
	gl_FragColor = color;

}
