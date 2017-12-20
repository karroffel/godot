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
attribute highp vec2 uv_attrib; // attrib:4

varying vec2 uv_interp;
varying vec4 color_interp;

#ifdef USE_TEXTURE_RECT

uniform vec4 dst_rect;
uniform vec4 src_rect;

#endif

#ifdef USE_NINEPATCH

varying highp vec2 pixel_size_interp;

#endif

uniform highp vec2 color_texpixel_size;

VERTEX_SHADER_GLOBALS

void main() {

	vec4 color = color_attrib;

	mat4 extra_matrix2 = extra_matrix;

#ifdef USE_TEXTURE_RECT

	if (dst_rect.z < 0.0) {
		uv_interp = src_rect.xy + abs(src_rect.zw) * vertex.yx;
	} else {
		uv_interp = src_rect.xy + abs(src_rect.zw) * vertex;
	}

	highp vec4 outvec = vec4(
	        dst_rect.xy + (vertex.xy * dst_rect.zw),
	        0.0,
	        1.0);
#else
	uv_interp = uv_attrib;
        highp vec4 outvec = vec4(vertex, 0.0, 1.0);
#endif

{
        vec2 src_vtx=outvec.xy;
VERTEX_SHADER_CODE

}

#ifdef USE_NINEPATCH

	pixel_size_interp = abs(dst_rect.zw) * vertex;
#endif

	color_interp = color;


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

uniform sampler2D color_texture; // texunit:0
uniform highp vec2 color_texpixel_size;
uniform mediump sampler2D normal_texture; // texunit:1

varying mediump vec2 uv_interp;
varying mediump vec4 color_interp;

uniform vec4 final_modulate;

uniform highp vec2 screen_pixel_size;

FRAGMENT_SHADER_GLOBALS

uniform bool use_default_normal;

#ifdef USE_TEXTURE_RECT


uniform vec4 dst_rect;
uniform vec4 src_rect;
uniform bool clip_rect_uv;


#ifdef USE_NINEPATCH

#ifdef USE_DISTANCE_FIELD
#endif

varying highp vec2 pixel_size_interp;

uniform int np_repeat_v;
uniform int np_repeat_h;
uniform bool np_draw_center;
//left top right bottom in pixel coordinates
uniform vec4 np_margins;

/*
float map_ninepatch_axis(float pixel, float draw_size,float tex_pixel_size,float margin_begin,float margin_end,int np_repeat,inout int draw_center) {


	float tex_size = 1.0/tex_pixel_size;

	if (pixel < margin_begin) {
		return pixel * tex_pixel_size;
	} else if (pixel >= draw_size-margin_end) {
		return (tex_size-(draw_size-pixel)) * tex_pixel_size;
	} else {
		if (!np_draw_center){
			draw_center--;
		}

		if (np_repeat==0) { //stretch
			//convert to ratio
			float ratio = (pixel - margin_begin) / (draw_size - margin_begin - margin_end);
			//scale to source texture
			return (margin_begin + ratio * (tex_size - margin_begin - margin_end)) * tex_pixel_size;
		} else if (np_repeat==1) { //tile
			//convert to ratio
			float ofs = mod((pixel - margin_begin), tex_size - margin_begin - margin_end);
			//scale to source texture
			return (margin_begin + ofs) * tex_pixel_size;
		} else if (np_repeat==2) { //tile fit
			//convert to ratio
			float src_area = draw_size - margin_begin - margin_end;
			float dst_area = tex_size - margin_begin - margin_end;
			float scale = max(1.0,floor(src_area / max(dst_area,0.0000001) + 0.5));

			//convert to ratio
			float ratio = (pixel - margin_begin) / src_area;
			ratio = mod(ratio * scale,1.0);
			return (margin_begin + ratio * dst_area) * tex_pixel_size;
		}
	}

}
*/


#endif
#endif


void main() {

	vec4 color = color_interp;
	vec2 uv = uv_interp;

#ifdef USE_TEXTURE_RECT


	//if (clip_rect_uv) {
	//	uv = clamp(uv, src_rect.xy, src_rect.xy + abs(src_rect.zw));
	//}


	// color.rgb = vec3(uv, 0);
	color = texture2D(color_texture, uv_interp);
	//color = vec4(0.0, 0.0, 1.0, 1.0);

#endif

{

FRAGMENT_SHADER_CODE


}
	color = color * final_modulate;
	gl_FragColor = color;

}
