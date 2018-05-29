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

	vec4 outvec = vec4(vertex_interp, 1.0);

	mat4 modelview = camera_matrix * model_matrix_copy;

	vec4 model_vec = model_matrix_copy * outvec;
	vec4 camera_vec = camera_matrix * model_vec;
	vec4 projected_vec = projection_matrix * camera_vec;

	vertex_interp = camera_vec.xyz;

	normal_interp = normalize((modelview * vec4(normal_interp, 0.0)).xyz);

	gl_Position = projected_vec;

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

#include "stdlib.glsl"

#define M_PI 3.14159265359

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

#ifdef USE_RADIANCE_MAP

#define RADIANCE_MAX_LOD 6.0

uniform samplerCube radiance_map; // texunit:0

uniform mat4 radiance_inverse_xform;

#endif

uniform float bg_energy;

#ifdef LIGHT_PASS

#define LIGHT_TYPE_DIRECTIONAL 0
#define LIGHT_TYPE_OMNI 1
#define LIGHT_TYPE_SPOT 2

// general for all lights
uniform int light_type;

uniform float light_energy;
uniform vec4 light_color;
uniform float light_specular;

// omni
uniform vec3 light_position;
uniform vec3 light_position_camera_space;

uniform float light_range;
uniform vec4 light_attenuation;

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

varying vec3 view_interp;

vec3 metallic_to_specular_color(float metallic, float specular, vec3 albedo) {
	float dielectric = (0.034 * 2.0) * specular;
	// energy conservation
	return mix(vec3(dielectric), albedo, metallic); // TODO: reference?
}

FRAGMENT_SHADER_GLOBALS


#ifdef LIGHT_PASS
void light_compute(vec3 N,
                   vec3 L,
                   vec3 V,
                   vec3 B,
                   vec3 T,
                   vec3 light_color,
                   vec3 attenuation,
                   vec3 diffuse_color,
                   vec3 transmission,
                   float specular_blob_intensity,
                   float roughness,
                   float metallic,
                   float rim,
                   float rim_tint,
                   float clearcoat,
                   float clearcoat_gloss,
                   float anisotropy,
                   inout vec3 diffuse_light,
                   inout vec3 specular_light) {

	float NdotL = dot(N, L);
	float cNdotL = max(NdotL, 0.0);
	float NdotV = dot(N, V);
	float cNdotV = max(NdotV, 0.0);

	{
		// calculate diffuse reflection

		// TODO hardcode Oren Nayar for now
		float diffuse_brdf_NL;

		diffuse_brdf_NL = max(0.0,(NdotL + roughness) / ((1.0 + roughness) * (1.0 + roughness)));

		diffuse_light += light_color * diffuse_color * diffuse_brdf_NL * attenuation * roughness;
	}

	{
		// calculate specular reflection

		 vec3 R = normalize(-reflect(L,N));
		 float cRdotV = max(dot(R, V), 0.0);
		 float blob_intensity = pow(cRdotV, (1.0 - roughness) * 256.0);
		 specular_light += light_color * attenuation * blob_intensity * specular_blob_intensity;


	}
}

void light_process_omni(vec3 vertex,
                        vec3 eye_vec,
                        vec3 normal,
                        vec3 binormal,
                        vec3 tangent,
                        vec3 albedo,
                        vec3 transmission,
                        float roughness,
                        float metallic,
                        float rim,
                        float rim_tint,
                        float clearcoat,
                        float clearcoat_gloss,
                        float anisotropy,
                        float specular,
                        inout vec3 diffuse_light,
                        inout vec3 specular_light) {
}

void light_process_directional() {

}
#endif

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

	vec3 normal = normalize(normal_interp);
	vec3 binormal = vec3(0.0);
	vec3 tangent = vec3(0.0);

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

	vec3 N = normal;
	
	vec3 specular_light = vec3(0.0, 0.0, 0.0);
	vec3 diffuse_light = vec3(0.0, 0.0, 0.0);

	vec3 ambient_light = vec3(0.0, 0.0, 0.0);

	vec3 env_reflection_light = vec3(0.0, 0.0, 0.0);

	vec3 eye_position = -normalize(vertex_interp);

#ifdef ALPHA_SCISSOR_USED
	if (alpha < alpha_scissor) {
		discard;
	}
#endif
	
//
// Lighting
//
#ifdef LIGHT_PASS

	if (light_type == LIGHT_TYPE_OMNI) {
		vec3 light_vec = light_position_camera_space - vertex;
		float light_length = length(light_vec);

		float normalized_distance = light_length / light_range;

		float omni_attenuation = pow(1.0 - normalized_distance, light_attenuation.w);

		vec3 attenuation = vec3(omni_attenuation);

		light_compute(normal,
		              normalize(light_vec),
		              eye_position,
		              binormal,
		              tangent,
		              light_color.xyz * light_energy,
		              attenuation,
		              albedo,
		              transmission,
		              specular * light_specular,
		              roughness,
		              metallic,
		              rim,
		              rim_tint,
		              clearcoat,
		              clearcoat_gloss,
		              anisotropy,
		              diffuse_light,
		              specular_light);

	} else if (light_type == LIGHT_TYPE_DIRECTIONAL) {

		specular_light = vec3(1.0);

		/*
		vec3 light_vec = vec3(0.5, -0.5, 0.0);
		vec3 attenuation = vec3(1.0, 1.0, 1.0);

		light_compute(normal,
			      normalize(light_vec),
			      eye_position,
			      binormal,
			      tangent,
			      light_color.xyz * light_energy,
			      attenuation,
			      albedo,
			      transmission,
			      specular, // * light_specular,
			      roughness,
			      metallic,
			      rim,
			      rim_tint,
			      clearcoat,
			      clearcoat_gloss,
			      anisotropy,
			      diffuse_light,
			      specular_light);
		*/
	} else if (light_type == LIGHT_TYPE_SPOT) {

	}

//	vec3 light_income_vector = (camera_matrix * vec4(light_position, 1.0)).xyz - vertex_interp;

//	float intensity = length(light_income_vector);

//	intensity = intensity / light_range;

//	intensity = clamp(intensity, 0.0, 1.0);

//	intensity = 1.0 - intensity;

	// normalize the light vector

//	light_income_vector = normalize(light_income_vector);

	// is pixel directly lit?

//	float NdotL = dot(normal_interp, light_income_vector);

//	NdotL = clamp(NdotL, 0.0, 1.0);

//	color = vec4(intensity) * NdotL;

	gl_FragColor = vec4(ambient_light + diffuse_light + specular_light, alpha);
#else


#ifdef USE_RADIANCE_MAP


	vec3 ref_vec = reflect(-eye_position, N);
	ref_vec = normalize((radiance_inverse_xform * vec4(ref_vec, 0.0)).xyz);

	ref_vec.xz *= -1.0;

	env_reflection_light = textureCubeLod(radiance_map, ref_vec, roughness * RADIANCE_MAX_LOD).xyz * bg_energy;
	ambient_light = textureCubeLod(radiance_map, ref_vec, RADIANCE_MAX_LOD).xyz * bg_energy;
	
	specular_light += env_reflection_light;
	
	ambient_light *= albedo;
	
	diffuse_light *= 1.0 - metallic;
	ambient_light *= 1.0 - metallic;
	
	// environment BRDF approximation
	
	// TODO shadeless
	{
		const vec4 c0 = vec4(-1.0, -0.0275, -0.572, 0.022);
		const vec4 c1 = vec4( 1.0, 0.0425, 1.04, -0.04);
		vec4 r = roughness * c0 + c1;
		float ndotv = clamp(dot(normal,eye_position),0.0,1.0);
		float a004 = min( r.x * r.x, exp2( -9.28 * ndotv ) ) * r.x + r.y;
		vec2 AB = vec2( -1.04, 1.04 ) * a004 + r.zw;

		vec3 specular_color = metallic_to_specular_color(metallic, specular, albedo);
		specular_light *= AB.x * specular_color + AB.y;
	}
	
	gl_FragColor = vec4(ambient_light + diffuse_light + specular_light, alpha);
#else
	gl_FragColor = vec4(albedo, alpha);
#endif


#endif // lighting


}
