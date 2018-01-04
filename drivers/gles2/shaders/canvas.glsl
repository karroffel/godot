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
attribute vec4 color_attrib; // attrib:3
attribute vec2 uv_attrib; // attrib:4

varying vec2 uv_interp;
varying vec4 color_interp;

#ifdef USE_TEXTURE_RECT

uniform vec4 dst_rect;
uniform vec4 src_rect;

#endif

uniform bool blit_pass;

VERTEX_SHADER_GLOBALS

void main() {

	vec4 color = color_attrib;

#ifdef USE_TEXTURE_RECT

	if (dst_rect.z < 0.0) { // Transpose is encoded as negative dst_rect.z
		uv_interp = src_rect.xy + abs(src_rect.zw) * vertex.yx;
	} else {
		uv_interp = src_rect.xy + abs(src_rect.zw) * vertex;
	}

	vec4 outvec = vec4(0.0, 0.0, 0.0, 1.0);

	outvec.xy = dst_rect.xy + (dst_rect.zw * vertex.xy);
#else
	vec4 outvec = vec4(vertex.xy, 0.0, 1.0);

#ifdef USE_UV_ATTRIBUTE
	uv_interp = uv_attrib;
#else
	uv_interp = vertex.xy;
#endif

#endif

{
        vec2 src_vtx=outvec.xy;
VERTEX_SHADER_CODE

}

	color_interp = color;

	gl_Position = projection_matrix * modelview_matrix * outvec;

}

[fragment]

#ifdef USE_GLES_OVER_GL
#define mediump
#define highp
#else
precision mediump float;
precision mediump int;
#endif

uniform sampler2D color_texture; // texunit:0
uniform mediump sampler2D normal_texture; // texunit:1

varying mediump vec2 uv_interp;
varying mediump vec4 color_interp;

uniform bool blit_pass;

uniform vec4 final_modulate;

FRAGMENT_SHADER_GLOBALS


void main() {

	vec4 color = color_interp;

	color *= texture2D(color_texture, uv_interp);
{

FRAGMENT_SHADER_CODE


}

	color *= final_modulate;

	gl_FragColor = color;

}
