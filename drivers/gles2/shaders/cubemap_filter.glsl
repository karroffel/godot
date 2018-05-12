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

#extension GL_ARB_shader_texture_lod : require

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

#ifdef USE_SOURCE_PANORAMA

vec4 texturePanorama(sampler2D pano, vec3 normal) {

	vec2 st = vec2(
	        atan(normal.x, normal.z),
	        acos(normal.y)
	);

	if(st.x < 0.0)
		st.x += M_PI*2.0;

	st/=vec2(M_PI*2.0,M_PI);

	return texture2DLod(pano,st,0.0);

}

#endif

vec3 texelCoordToVec(vec2 uv, int faceID) {
	mat3 faceUvVectors[6];

	// -x
	faceUvVectors[0][0] = vec3(0.0, 0.0, 1.0);  // u -> +z
	faceUvVectors[0][1] = vec3(0.0, -1.0, 0.0); // v -> -y
	faceUvVectors[0][2] = vec3(-1.0, 0.0, 0.0); // -x face

	// +x
	faceUvVectors[1][0] = vec3(0.0, 0.0, -1.0); // u -> -z
	faceUvVectors[1][1] = vec3(0.0, -1.0, 0.0); // v -> -y
	faceUvVectors[1][2] = vec3(1.0, 0.0, 0.0);  // +x face

	// -y
	faceUvVectors[2][0] = vec3(1.0, 0.0, 0.0);  // u -> +x
	faceUvVectors[2][1] = vec3(0.0, 0.0, -1.0); // v -> -z
	faceUvVectors[2][2] = vec3(0.0, -1.0, 0.0); // -y face

	// +y
	faceUvVectors[3][0] = vec3(1.0, 0.0, 0.0);  // u -> +x
	faceUvVectors[3][1] = vec3(0.0, 0.0, 1.0);  // v -> +z
	faceUvVectors[3][2] = vec3(0.0, 1.0, 0.0);  // +y face

	// -z
	faceUvVectors[4][0] = vec3(-1.0, 0.0, 0.0); // u -> -x
	faceUvVectors[4][1] = vec3(0.0, -1.0, 0.0); // v -> -y
	faceUvVectors[4][2] = vec3(0.0, 0.0, -1.0); // -z face

	// +z
	faceUvVectors[5][0] = vec3(1.0, 0.0, 0.0);  // u -> +x
	faceUvVectors[5][1] = vec3(0.0, -1.0, 0.0); // v -> -y
	faceUvVectors[5][2] = vec3(0.0, 0.0, 1.0);  // +z face

	// out = u * s_faceUv[0] + v * s_faceUv[1] + s_faceUv[2].
	vec3 result = (faceUvVectors[faceID][0] * uv.x) + (faceUvVectors[faceID][1] * uv.y) + faceUvVectors[faceID][2];
	return normalize(result);
}

vec3 ImportanceSampleGGX(vec2 Xi, float Roughness, vec3 N) {
	return vec3(0.0);
}

float radical_inverse_VdC(int i) {
	return 1.0;
}

vec2 Hammersley(int i, int N) {
	return vec2(float(i) / float(N), radical_inverse_VdC(i));
}

uniform bool z_flip;

void main() {

	vec3 color = vec3(0.0);

	vec2 uv = (uv_interp * 2.0) - 1.0;
	vec3 N = texelCoordToVec(uv, face_id);

#ifdef USE_SOURCE_PANORAMA
	color = texturePanorama(source_panorama, N).xyz;
#endif

	gl_FragColor = vec4(color, 1.0);

}

