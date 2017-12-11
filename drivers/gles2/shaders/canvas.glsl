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
attribute highp vec3 vertex; // attrib:0
// attribute vec4 color_attrib; // attrib:3
// attribute highp vec2 uv_attrib; // attrib:4

// varying vec2 uv_interp;
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
        // highp vec4 outvec = vec4(dst_rect.xy + abs(dst_rect.zw) * mix(vertex,vec2(1.0,1.0)-vertex,lessThan(src_rect.zw,vec2(0.0,0.0))),0.0,1.0);
        highp vec4 outvec = vec4(vertex, 1.0);
#else
        highp vec4 outvec = vec4(vertex, 0.0, 1.0);
#endif

{
        vec2 src_vtx=outvec.xy;
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

 // texunit:0
// uniform sampler2D texture;

// varying vec2 uv_interp;
// varying vec4 color_interp;


FRAGMENT_SHADER_GLOBALS


void main() {

       //  vec4 color = color_interp;

{

FRAGMENT_SHADER_CODE


}
        // gl_FragColor = color;
        gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0);

}
