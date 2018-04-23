/*************************************************************************/
/*  rasterizer_scene_gles2.cpp                                           */
/*************************************************************************/
/*                       This file is part of:                           */
/*                           GODOT ENGINE                                */
/*                      https://godotengine.org                          */
/*************************************************************************/
/* Copyright (c) 2007-2018 Juan Linietsky, Ariel Manzur.                 */
/* Copyright (c) 2014-2018 Godot Engine contributors (cf. AUTHORS.md)    */
/*                                                                       */
/* Permission is hereby granted, free of charge, to any person obtaining */
/* a copy of this software and associated documentation files (the       */
/* "Software"), to deal in the Software without restriction, including   */
/* without limitation the rights to use, copy, modify, merge, publish,   */
/* distribute, sublicense, and/or sell copies of the Software, and to    */
/* permit persons to whom the Software is furnished to do so, subject to */
/* the following conditions:                                             */
/*                                                                       */
/* The above copyright notice and this permission notice shall be        */
/* included in all copies or substantial portions of the Software.       */
/*                                                                       */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,       */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF    */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.*/
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY  */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,  */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE     */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                */
/*************************************************************************/
#include "rasterizer_scene_gles2.h"
#include "math/transform.h"
#include "math_funcs.h"
#include "os/os.h"
#include "project_settings.h"
#include "rasterizer_canvas_gles2.h"
#include "servers/visual/visual_server_raster.h"

#ifndef GLES_OVER_GL
#define glClearDepth glClearDepthf
#endif

/* SHADOW ATLAS API */

RID RasterizerSceneGLES2::shadow_atlas_create() {

	return RID();
}

void RasterizerSceneGLES2::shadow_atlas_set_size(RID p_atlas, int p_size) {
}

void RasterizerSceneGLES2::shadow_atlas_set_quadrant_subdivision(RID p_atlas, int p_quadrant, int p_subdivision) {
}

bool RasterizerSceneGLES2::shadow_atlas_update_light(RID p_atlas, RID p_light_intance, float p_coverage, uint64_t p_light_version) {
	return false;
}

void RasterizerSceneGLES2::set_directional_shadow_count(int p_count) {
}

int RasterizerSceneGLES2::get_directional_light_shadow_size(RID p_light_intance) {
	return 0;
}
//////////////////////////////////////////////////////

RID RasterizerSceneGLES2::reflection_atlas_create() {
	return RID();
}

void RasterizerSceneGLES2::reflection_atlas_set_size(RID p_ref_atlas, int p_size) {
}

void RasterizerSceneGLES2::reflection_atlas_set_subdivision(RID p_ref_atlas, int p_subdiv) {
}

////////////////////////////////////////////////////

RID RasterizerSceneGLES2::reflection_probe_instance_create(RID p_probe) {
	return RID();
}

void RasterizerSceneGLES2::reflection_probe_instance_set_transform(RID p_instance, const Transform &p_transform) {
}

void RasterizerSceneGLES2::reflection_probe_release_atlas_index(RID p_instance) {
}

bool RasterizerSceneGLES2::reflection_probe_instance_needs_redraw(RID p_instance) {
	return false;
}

bool RasterizerSceneGLES2::reflection_probe_instance_has_reflection(RID p_instance) {
	return false;
}

bool RasterizerSceneGLES2::reflection_probe_instance_begin_render(RID p_instance, RID p_reflection_atlas) {
	return false;
}

bool RasterizerSceneGLES2::reflection_probe_instance_postprocess_step(RID p_instance) {
	return false;
}

/* ENVIRONMENT API */

RID RasterizerSceneGLES2::environment_create() {

	return RID();
}

void RasterizerSceneGLES2::environment_set_background(RID p_env, VS::EnvironmentBG p_bg) {
}

void RasterizerSceneGLES2::environment_set_sky(RID p_env, RID p_sky) {
}

void RasterizerSceneGLES2::environment_set_sky_custom_fov(RID p_env, float p_scale) {
}

void RasterizerSceneGLES2::environment_set_bg_color(RID p_env, const Color &p_color) {
}

void RasterizerSceneGLES2::environment_set_bg_energy(RID p_env, float p_energy) {
}

void RasterizerSceneGLES2::environment_set_canvas_max_layer(RID p_env, int p_max_layer) {
}

void RasterizerSceneGLES2::environment_set_ambient_light(RID p_env, const Color &p_color, float p_energy, float p_sky_contribution) {
}

void RasterizerSceneGLES2::environment_set_dof_blur_far(RID p_env, bool p_enable, float p_distance, float p_transition, float p_amount, VS::EnvironmentDOFBlurQuality p_quality) {
}

void RasterizerSceneGLES2::environment_set_dof_blur_near(RID p_env, bool p_enable, float p_distance, float p_transition, float p_amount, VS::EnvironmentDOFBlurQuality p_quality) {
}

void RasterizerSceneGLES2::environment_set_glow(RID p_env, bool p_enable, int p_level_flags, float p_intensity, float p_strength, float p_bloom_threshold, VS::EnvironmentGlowBlendMode p_blend_mode, float p_hdr_bleed_threshold, float p_hdr_bleed_scale, bool p_bicubic_upscale) {
}

void RasterizerSceneGLES2::environment_set_fog(RID p_env, bool p_enable, float p_begin, float p_end, RID p_gradient_texture) {
}

void RasterizerSceneGLES2::environment_set_ssr(RID p_env, bool p_enable, int p_max_steps, float p_fade_in, float p_fade_out, float p_depth_tolerance, bool p_roughness) {
}

void RasterizerSceneGLES2::environment_set_ssao(RID p_env, bool p_enable, float p_radius, float p_intensity, float p_radius2, float p_intensity2, float p_bias, float p_light_affect, float p_ao_channel_affect, const Color &p_color, VS::EnvironmentSSAOQuality p_quality, VisualServer::EnvironmentSSAOBlur p_blur, float p_bilateral_sharpness) {
}

void RasterizerSceneGLES2::environment_set_tonemap(RID p_env, VS::EnvironmentToneMapper p_tone_mapper, float p_exposure, float p_white, bool p_auto_exposure, float p_min_luminance, float p_max_luminance, float p_auto_exp_speed, float p_auto_exp_scale) {
}

void RasterizerSceneGLES2::environment_set_adjustment(RID p_env, bool p_enable, float p_brightness, float p_contrast, float p_saturation, RID p_ramp) {
}

void RasterizerSceneGLES2::environment_set_fog(RID p_env, bool p_enable, const Color &p_color, const Color &p_sun_color, float p_sun_amount) {
}

void RasterizerSceneGLES2::environment_set_fog_depth(RID p_env, bool p_enable, float p_depth_begin, float p_depth_curve, bool p_transmit, float p_transmit_curve) {
}

void RasterizerSceneGLES2::environment_set_fog_height(RID p_env, bool p_enable, float p_min_height, float p_max_height, float p_height_curve) {
}

bool RasterizerSceneGLES2::is_environment(RID p_env) {
	return false;
}

VS::EnvironmentBG RasterizerSceneGLES2::environment_get_background(RID p_env) {
	return VS::ENV_BG_CLEAR_COLOR;
}

int RasterizerSceneGLES2::environment_get_canvas_max_layer(RID p_env) {
	return 0;
}

RID RasterizerSceneGLES2::light_instance_create(RID p_light) {
	return RID();
}

void RasterizerSceneGLES2::light_instance_set_transform(RID p_light_instance, const Transform &p_transform) {
}

void RasterizerSceneGLES2::light_instance_set_shadow_transform(RID p_light_instance, const CameraMatrix &p_projection, const Transform &p_transform, float p_far, float p_split, int p_pass, float p_bias_scale) {
}

void RasterizerSceneGLES2::light_instance_mark_visible(RID p_light_instance) {
}

//////////////////////

RID RasterizerSceneGLES2::gi_probe_instance_create() {

	return RID();
}

void RasterizerSceneGLES2::gi_probe_instance_set_light_data(RID p_probe, RID p_base, RID p_data) {
}
void RasterizerSceneGLES2::gi_probe_instance_set_transform_to_data(RID p_probe, const Transform &p_xform) {
}

void RasterizerSceneGLES2::gi_probe_instance_set_bounds(RID p_probe, const Vector3 &p_bounds) {
}

////////////////////////////
////////////////////////////
////////////////////////////

void RasterizerSceneGLES2::_add_geometry(RasterizerStorageGLES2::Geometry *p_geometry, InstanceBase *p_instance, RasterizerStorageGLES2::GeometryOwner *p_owner, int p_material, bool p_depth_pass, bool p_shadow_pass) {

	RasterizerStorageGLES2::Material *material = NULL;
	RID material_src;

	if (p_instance->material_override.is_valid()) {
		material_src = p_instance->material_override;
	} else if (p_material >= 0) {
		material_src = p_instance->materials[p_material];
	} else {
		material_src = p_geometry->material;
	}

	if (material_src.is_valid()) {
		material = storage->material_owner.getornull(material_src);

		if (!material->shader || !material->shader->valid) {
			material = NULL;
		}
	}

	if (!material) {
		material = storage->material_owner.getptr(default_material);
	}

	ERR_FAIL_COND(!material);

	_add_geometry_with_material(p_geometry, p_instance, p_owner, material, p_depth_pass, p_shadow_pass);

	while (material->next_pass.is_valid()) {
		material = storage->material_owner.getornull(material->next_pass);

		if (!material || !material->shader || !material->shader->valid) {
			break;
		}

		_add_geometry_with_material(p_geometry, p_instance, p_owner, material, p_depth_pass, p_shadow_pass);
	}
}
void RasterizerSceneGLES2::_add_geometry_with_material(RasterizerStorageGLES2::Geometry *p_geometry, InstanceBase *p_instance, RasterizerStorageGLES2::GeometryOwner *p_owner, RasterizerStorageGLES2::Material *p_material, bool p_depth_pass, bool p_shadow_pass) {

	bool has_base_alpha = (p_material->shader->spatial.uses_alpha && !p_material->shader->spatial.uses_alpha_scissor) || p_material->shader->spatial.uses_screen_texture || p_material->shader->spatial.uses_depth_texture;
	bool has_blend_alpha = p_material->shader->spatial.blend_mode != RasterizerStorageGLES2::Shader::Spatial::BLEND_MODE_MIX;
	bool has_alpha = has_base_alpha || has_blend_alpha;

	// TODO add this stuff
	// bool mirror = p_instance->mirror;
	// bool no_cull = false;

	RenderList::Element *e = has_alpha ? render_list.add_alpha_element() : render_list.add_element();

	if (!e) {
		return;
	}

	e->geometry = p_geometry;
	e->material = p_material;
	e->instance = p_instance;
	e->owner = p_owner;
	e->sort_key = 0;

	// TODO check render pass of geometry

	// TODO check directional light flag

	e->sort_key |= uint64_t(e->geometry->index) << RenderList::SORT_KEY_GEOMETRY_INDEX_SHIFT;
	e->sort_key |= uint64_t(e->instance->base_type) << RenderList::SORT_KEY_GEOMETRY_TYPE_SHIFT;

	if (!p_depth_pass) {
		e->sort_key |= uint64_t(e->material->index) << RenderList::SORT_KEY_MATERIAL_INDEX_SHIFT;

		e->sort_key |= uint64_t(p_material->render_priority + 128) << RenderList::SORT_KEY_PRIORITY_SHIFT;
	} else {
		// TODO
	}

	if (p_material->shader->spatial.uses_time) {
		VisualServerRaster::redraw_request();
	}
}

void RasterizerSceneGLES2::_fill_render_list(InstanceBase **p_cull_result, int p_cull_count, bool p_depth_pass, bool p_shadow_pass) {

	for (int i = 0; i < p_cull_count; i++) {

		InstanceBase *instance = p_cull_result[i];

		switch (instance->base_type) {

			case VS::INSTANCE_MESH: {

				RasterizerStorageGLES2::Mesh *mesh = storage->mesh_owner.getornull(instance->base);
				ERR_CONTINUE(!mesh);

				int num_surfaces = mesh->surfaces.size();

				for (int i = 0; i < num_surfaces; i++) {
					int material_index = instance->materials[i].is_valid() ? i : -1;

					RasterizerStorageGLES2::Surface *surface = mesh->surfaces[i];

					_add_geometry(surface, instance, NULL, material_index, p_depth_pass, p_shadow_pass);
				}

			} break;

			default: {

			} break;
		}
	}
}

static const GLenum gl_primitive[] = {
	GL_POINTS,
	GL_LINES,
	GL_LINE_STRIP,
	GL_LINE_LOOP,
	GL_TRIANGLES,
	GL_TRIANGLE_STRIP,
	GL_TRIANGLE_FAN
};

void RasterizerSceneGLES2::_setup_material(RasterizerStorageGLES2::Material *p_material) {

	// material parameters

	state.scene_shader.set_custom_shader(p_material->shader->custom_code_id);

	state.scene_shader.bind();

	int tc = p_material->textures.size();
	Pair<StringName, RID> *textures = p_material->textures.ptrw();

	ShaderLanguage::ShaderNode::Uniform::Hint *texture_hints = p_material->shader->texture_hints.ptrw();

	for (int i = 0; i < tc; i++) {

		glActiveTexture(GL_TEXTURE0 + i);

		RasterizerStorageGLES2::Texture *t = storage->texture_owner.getornull(textures[i].second);

		if (!t) {

			switch (texture_hints[i]) {
				case ShaderLanguage::ShaderNode::Uniform::HINT_BLACK_ALBEDO:
				case ShaderLanguage::ShaderNode::Uniform::HINT_BLACK: {
					glBindTexture(GL_TEXTURE_2D, storage->resources.black_tex);
				} break;
				case ShaderLanguage::ShaderNode::Uniform::HINT_ANISO: {
					glBindTexture(GL_TEXTURE_2D, storage->resources.aniso_tex);
				} break;
				case ShaderLanguage::ShaderNode::Uniform::HINT_NORMAL: {
					glBindTexture(GL_TEXTURE_2D, storage->resources.normal_tex);
				} break;
				default: {
					glBindTexture(GL_TEXTURE_2D, storage->resources.white_tex);
				} break;
			}

			continue;
		}

		t = t->get_ptr();

		glBindTexture(t->target, t->tex_id);
	}
	state.scene_shader.use_material((void *)p_material, 0);
}

void RasterizerSceneGLES2::_setup_geometry(RenderList::Element *p_element, RasterizerStorageGLES2::Skeleton *p_skeleton) {

	state.scene_shader.set_conditional(SceneShaderGLES2::USE_SKELETON, p_skeleton != NULL);

	if (p_skeleton) {
		ERR_FAIL_COND(p_skeleton->use_2d);

		PoolVector<float> &transform_buffer = storage->resources.skeleton_transform_cpu_buffer;

		switch (p_element->instance->base_type) {
			case VS::INSTANCE_MESH: {
				RasterizerStorageGLES2::Surface *s = static_cast<RasterizerStorageGLES2::Surface *>(p_element->geometry);

				if (!s->attribs[VS::ARRAY_BONES].enabled || !s->attribs[VS::ARRAY_WEIGHTS].enabled) {
					break; // the whole instance has a skeleton, but this surface is not affected by it.
				}

				// 3 * vec4 per vertex
				if (transform_buffer.size() < s->array_len * 12) {
					transform_buffer.resize(s->array_len * 12);
				}

				const size_t bones_offset = s->attribs[VS::ARRAY_BONES].offset;
				const size_t bones_stride = s->attribs[VS::ARRAY_BONES].stride;
				const size_t bone_weight_offset = s->attribs[VS::ARRAY_WEIGHTS].offset;
				const size_t bone_weight_stride = s->attribs[VS::ARRAY_WEIGHTS].stride;

				{
					PoolVector<float>::Write write = transform_buffer.write();
					float *buffer = write.ptr();

					PoolVector<uint8_t>::Read vertex_array_read = s->data.read();
					const uint8_t *vertex_data = vertex_array_read.ptr();

					for (int i = 0; i < s->array_len; i++) {

						// do magic

						size_t bones[4];
						float bone_weight[4];

						if (s->attribs[VS::ARRAY_BONES].type == GL_UNSIGNED_BYTE) {
							// read as byte
							const uint8_t *bones_ptr = vertex_data + bones_offset + (i * bones_stride);
							bones[0] = bones_ptr[0];
							bones[1] = bones_ptr[1];
							bones[2] = bones_ptr[2];
							bones[3] = bones_ptr[3];
						} else {
							// read as short
							const uint16_t *bones_ptr = (const uint16_t *)(vertex_data + bones_offset + (i * bones_stride));
							bones[0] = bones_ptr[0];
							bones[1] = bones_ptr[1];
							bones[2] = bones_ptr[2];
							bones[3] = bones_ptr[3];
						}

						if (s->attribs[VS::ARRAY_WEIGHTS].type == GL_FLOAT) {
							// read as float
							const float *weight_ptr = (const float *)(vertex_data + bone_weight_offset + (i * bone_weight_stride));
							bone_weight[0] = weight_ptr[0];
							bone_weight[1] = weight_ptr[1];
							bone_weight[2] = weight_ptr[2];
							bone_weight[3] = weight_ptr[3];
						} else {
							// read as half
							const uint16_t *weight_ptr = (const uint16_t *)(vertex_data + bone_weight_offset + (i * bone_weight_stride));
							bone_weight[0] = (weight_ptr[0] / (float)UINT16_MAX);
							bone_weight[1] = (weight_ptr[1] / (float)UINT16_MAX);
							bone_weight[2] = (weight_ptr[2] / (float)UINT16_MAX);
							bone_weight[3] = (weight_ptr[3] / (float)UINT16_MAX);
						}

						size_t offset = i * 12;

						Transform transform;

						Transform bone_transforms[4] = {
							storage->skeleton_bone_get_transform(p_element->instance->skeleton, bones[0]),
							storage->skeleton_bone_get_transform(p_element->instance->skeleton, bones[1]),
							storage->skeleton_bone_get_transform(p_element->instance->skeleton, bones[2]),
							storage->skeleton_bone_get_transform(p_element->instance->skeleton, bones[3]),
						};

						transform.origin =
								bone_weight[0] * bone_transforms[0].origin +
								bone_weight[1] * bone_transforms[1].origin +
								bone_weight[2] * bone_transforms[2].origin +
								bone_weight[3] * bone_transforms[3].origin;

						transform.basis =
								bone_transforms[0].basis * bone_weight[0] +
								bone_transforms[1].basis * bone_weight[1] +
								bone_transforms[2].basis * bone_weight[2] +
								bone_transforms[3].basis * bone_weight[3];

						float row[3][4] = {
							{ transform.basis[0][0], transform.basis[0][1], transform.basis[0][2], transform.origin[0] },
							{ transform.basis[1][0], transform.basis[1][1], transform.basis[1][2], transform.origin[1] },
							{ transform.basis[2][0], transform.basis[2][1], transform.basis[2][2], transform.origin[2] },
						};

						size_t transform_buffer_offset = i * 12;

						copymem(&buffer[transform_buffer_offset], row, sizeof(row));
					}
				}

				storage->_update_skeleton_transform_buffer(transform_buffer, s->array_len * 12);
			} break;

			default: {

			} break;
		}
	}

	switch (p_element->instance->base_type) {

		case VS::INSTANCE_MESH: {
			RasterizerStorageGLES2::Surface *s = static_cast<RasterizerStorageGLES2::Surface *>(p_element->geometry);

			state.scene_shader.set_conditional(SceneShaderGLES2::ENABLE_COLOR_INTERP, s->attribs[VS::ARRAY_COLOR].enabled);
			state.scene_shader.set_conditional(SceneShaderGLES2::ENABLE_UV_INTERP, s->attribs[VS::ARRAY_TEX_UV].enabled);
			state.scene_shader.set_conditional(SceneShaderGLES2::ENABLE_UV2_INTERP, s->attribs[VS::ARRAY_TEX_UV2].enabled);

		} break;

		default: {

		} break;
	}
}

void RasterizerSceneGLES2::_render_geometry(RenderList::Element *p_element) {

	switch (p_element->instance->base_type) {

		case VS::INSTANCE_MESH: {

			RasterizerStorageGLES2::Surface *s = static_cast<RasterizerStorageGLES2::Surface *>(p_element->geometry);

			// set up

			if (p_element->instance->skeleton.is_valid() && s->attribs[VS::ARRAY_BONES].enabled && s->attribs[VS::ARRAY_WEIGHTS].enabled) {
				glBindBuffer(GL_ARRAY_BUFFER, storage->resources.skeleton_transform_buffer);

				glEnableVertexAttribArray(VS::ARRAY_MAX + 0);
				glEnableVertexAttribArray(VS::ARRAY_MAX + 1);
				glEnableVertexAttribArray(VS::ARRAY_MAX + 2);

				glVertexAttribPointer(VS::ARRAY_MAX + 0, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 12, (const void *)(sizeof(float) * 4 * 0));
				glVertexAttribPointer(VS::ARRAY_MAX + 1, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 12, (const void *)(sizeof(float) * 4 * 1));
				glVertexAttribPointer(VS::ARRAY_MAX + 2, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 12, (const void *)(sizeof(float) * 4 * 2));
			} else {
				// just to make sure
				glDisableVertexAttribArray(VS::ARRAY_MAX + 0);
				glDisableVertexAttribArray(VS::ARRAY_MAX + 1);
				glDisableVertexAttribArray(VS::ARRAY_MAX + 2);

				glVertexAttrib4f(VS::ARRAY_MAX + 0, 1, 0, 0, 0);
				glVertexAttrib4f(VS::ARRAY_MAX + 1, 0, 1, 0, 0);
				glVertexAttrib4f(VS::ARRAY_MAX + 2, 0, 0, 1, 0);
			}

			glBindBuffer(GL_ARRAY_BUFFER, s->vertex_id);

			if (s->index_array_len > 0) {
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s->index_id);
			}

			for (int i = 0; i < VS::ARRAY_MAX - 1; i++) {
				if (s->attribs[i].enabled) {
					glEnableVertexAttribArray(i);
					glVertexAttribPointer(s->attribs[i].index, s->attribs[i].size, s->attribs[i].type, s->attribs[i].normalized, s->attribs[i].stride, (uint8_t *)0 + s->attribs[i].offset);
				} else {
					glDisableVertexAttribArray(i);
				}
			}

			// drawing

			if (s->index_array_len > 0) {
				glDrawElements(gl_primitive[s->primitive], s->index_array_len, (s->array_len >= (1 << 16)) ? GL_UNSIGNED_INT : GL_UNSIGNED_SHORT, 0);
			} else {
				glDrawArrays(gl_primitive[s->primitive], 0, s->array_len);
			}

			// tear down

			for (int i = 0; i < VS::ARRAY_MAX - 1; i++) {
				glDisableVertexAttribArray(i);
			}

			if (s->index_array_len > 0) {
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
			}

			if (p_element->instance->skeleton.is_valid() && s->attribs[VS::ARRAY_BONES].enabled && s->attribs[VS::ARRAY_WEIGHTS].enabled) {
				glBindBuffer(GL_ARRAY_BUFFER, storage->resources.skeleton_transform_buffer);

				glDisableVertexAttribArray(VS::ARRAY_MAX + 0);
				glDisableVertexAttribArray(VS::ARRAY_MAX + 1);
				glDisableVertexAttribArray(VS::ARRAY_MAX + 2);
			}

			glBindBuffer(GL_ARRAY_BUFFER, 0);

		} break;
	}
}

void RasterizerSceneGLES2::_render_render_list(RasterizerSceneGLES2::RenderList::Element **p_elements, int p_element_count, const Transform &p_view_transform, const CameraMatrix &p_projection, GLuint p_base_env, bool p_reverse_cull, bool p_alpha_pass, bool p_shadow, bool p_directional_add, bool p_directional_shadows) {

	Vector2 screen_pixel_size;
	screen_pixel_size.x = 1.0 / storage->frame.current_rt->width;
	screen_pixel_size.y = 1.0 / storage->frame.current_rt->height;

	for (int i = 0; i < p_element_count; i++) {
		RenderList::Element *e = p_elements[i];

		RasterizerStorageGLES2::Material *material = e->material;

		RasterizerStorageGLES2::Skeleton *skeleton = storage->skeleton_owner.getornull(e->instance->skeleton);

		_setup_geometry(e, skeleton);

		_setup_material(material);

		if (p_alpha_pass || p_directional_add) {
			int desired_blend_mode;
			if (p_directional_add) {
				desired_blend_mode = RasterizerStorageGLES2::Shader::Spatial::BLEND_MODE_ADD;
			} else {
				desired_blend_mode = material->shader->spatial.blend_mode;
			}

			switch (desired_blend_mode) {

				case RasterizerStorageGLES2::Shader::Spatial::BLEND_MODE_MIX: {
					glBlendEquation(GL_FUNC_ADD);
					if (storage->frame.current_rt && storage->frame.current_rt->flags[RasterizerStorage::RENDER_TARGET_TRANSPARENT]) {
						glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
					} else {
						glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
					}

				} break;
				case RasterizerStorageGLES2::Shader::Spatial::BLEND_MODE_ADD: {

					glBlendEquation(GL_FUNC_ADD);
					glBlendFunc(p_alpha_pass ? GL_SRC_ALPHA : GL_ONE, GL_ONE);

				} break;
				case RasterizerStorageGLES2::Shader::Spatial::BLEND_MODE_SUB: {

					glBlendEquation(GL_FUNC_REVERSE_SUBTRACT);
					glBlendFunc(GL_SRC_ALPHA, GL_ONE);
				} break;
				case RasterizerStorageGLES2::Shader::Spatial::BLEND_MODE_MUL: {
					glBlendEquation(GL_FUNC_ADD);
					if (storage->frame.current_rt && storage->frame.current_rt->flags[RasterizerStorage::RENDER_TARGET_TRANSPARENT]) {
						glBlendFuncSeparate(GL_DST_COLOR, GL_ZERO, GL_DST_ALPHA, GL_ZERO);
					} else {
						glBlendFuncSeparate(GL_DST_COLOR, GL_ZERO, GL_ZERO, GL_ONE);
					}

				} break;
			}
		}

		state.scene_shader.set_uniform(SceneShaderGLES2::CAMERA_MATRIX, p_view_transform.inverse());
		state.scene_shader.set_uniform(SceneShaderGLES2::CAMERA_INVERSE_MATRIX, p_view_transform);
		state.scene_shader.set_uniform(SceneShaderGLES2::PROJECTION_MATRIX, p_projection);
		state.scene_shader.set_uniform(SceneShaderGLES2::PROJECTION_INVERSE_MATRIX, p_projection.inverse());

		state.scene_shader.set_uniform(SceneShaderGLES2::MODEL_MATRIX, e->instance->transform);

		state.scene_shader.set_uniform(SceneShaderGLES2::TIME, storage->frame.time[0]);

		state.scene_shader.set_uniform(SceneShaderGLES2::SCREEN_PIXEL_SIZE, screen_pixel_size);

		_render_geometry(e);
	}
}

void RasterizerSceneGLES2::render_scene(const Transform &p_cam_transform, const CameraMatrix &p_cam_projection, bool p_cam_ortogonal, InstanceBase **p_cull_result, int p_cull_count, RID *p_light_cull_result, int p_light_cull_count, RID *p_reflection_probe_cull_result, int p_reflection_probe_cull_count, RID p_environment, RID p_shadow_atlas, RID p_reflection_atlas, RID p_reflection_probe, int p_reflection_probe_pass) {

	glEnable(GL_BLEND);

	GLuint current_fb = storage->frame.current_rt->fbo;

	// render list stuff

	render_list.clear();
	_fill_render_list(p_cull_result, p_cull_count, false, false);

	// other stuff

	glBindFramebuffer(GL_FRAMEBUFFER, current_fb);

	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_TRUE);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	storage->frame.clear_request = false;

	glVertexAttrib4f(VS::ARRAY_COLOR, 1, 1, 1, 1);

	// render opaque things first

	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	render_list.sort_by_key(false);
	_render_render_list(render_list.elements, render_list.element_count, p_cam_transform, p_cam_projection, 0, false, false, false, false, false);

	// alpha pass

	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

	render_list.sort_by_key(true);
	_render_render_list(&render_list.elements[render_list.max_elements - render_list.alpha_element_count], render_list.alpha_element_count, p_cam_transform, p_cam_projection, 0, false, true, false, false, false);

	glDepthMask(GL_FALSE);
	glDisable(GL_DEPTH_TEST);
}

void RasterizerSceneGLES2::render_shadow(RID p_light, RID p_shadow_atlas, int p_pass, InstanceBase **p_cull_result, int p_cull_count) {
}

void RasterizerSceneGLES2::set_scene_pass(uint64_t p_pass) {
}

bool RasterizerSceneGLES2::free(RID p_rid) {
	return true;
}

void RasterizerSceneGLES2::set_debug_draw_mode(VS::ViewportDebugDraw p_debug_draw) {
}

void RasterizerSceneGLES2::initialize() {
	state.scene_shader.init();

	render_list.init();

	{
		//default material and shader

		default_shader = storage->shader_create();
		storage->shader_set_code(default_shader, "shader_type spatial;\n");
		default_material = storage->material_create();
		storage->material_set_shader(default_material, default_shader);

		default_shader_twosided = storage->shader_create();
		default_material_twosided = storage->material_create();
		storage->shader_set_code(default_shader_twosided, "shader_type spatial; render_mode cull_disabled;\n");
		storage->material_set_shader(default_material_twosided, default_shader_twosided);
	}
}

void RasterizerSceneGLES2::iteration() {
}

void RasterizerSceneGLES2::finalize() {
}

RasterizerSceneGLES2::RasterizerSceneGLES2() {
}
