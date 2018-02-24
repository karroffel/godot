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

void RasterizerSceneGLES2::render_scene(const Transform &p_cam_transform, const CameraMatrix &p_cam_projection, bool p_cam_ortogonal, InstanceBase **p_cull_result, int p_cull_count, RID *p_light_cull_result, int p_light_cull_count, RID *p_reflection_probe_cull_result, int p_reflection_probe_cull_count, RID p_environment, RID p_shadow_atlas, RID p_reflection_atlas, RID p_reflection_probe, int p_reflection_probe_pass) {
	GLuint current_fb = storage->frame.current_rt->fbo;

	glBindFramebuffer(GL_FRAMEBUFFER, current_fb);

	glClearColor(1.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);

	storage->canvas->_bind_quad_buffer();

	state.scene_shader.bind();

	state.scene_shader.set_uniform(SceneShaderGLES2::COLOR, Color(1, 1, 1));

	glDrawArrays(GL_TRIANGLES, 0, 3);

	storage->frame.clear_request = false;

	for (int i = 0; i < p_cull_count; i++) {
		InstanceBase *ib = p_cull_result[i];

		print_line("WHEEEEY");
	}

	/*
	//fill up ubo

	storage->info.render.object_count += p_cull_count;

	Environment *env = environment_owner.getornull(p_environment);
	ShadowAtlas *shadow_atlas = shadow_atlas_owner.getornull(p_shadow_atlas);
	ReflectionAtlas *reflection_atlas = reflection_atlas_owner.getornull(p_reflection_atlas);

	if (shadow_atlas && shadow_atlas->size) {
		glActiveTexture(GL_TEXTURE0 + storage->config.max_texture_image_units - 5);
		glBindTexture(GL_TEXTURE_2D, shadow_atlas->depth);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LESS);
		state.ubo_data.shadow_atlas_pixel_size[0] = 1.0 / shadow_atlas->size;
		state.ubo_data.shadow_atlas_pixel_size[1] = 1.0 / shadow_atlas->size;
	}

	if (reflection_atlas && reflection_atlas->size) {
		glActiveTexture(GL_TEXTURE0 + storage->config.max_texture_image_units - 3);
		glBindTexture(GL_TEXTURE_2D, reflection_atlas->color);
	}

	if (p_reflection_probe.is_valid()) {
		state.ubo_data.reflection_multiplier = 0.0;
	} else {
		state.ubo_data.reflection_multiplier = 1.0;
	}

	state.ubo_data.subsurface_scatter_width = subsurface_scatter_size;

	state.ubo_data.z_offset = 0;
	state.ubo_data.z_slope_scale = 0;
	state.ubo_data.shadow_dual_paraboloid_render_side = 0;
	state.ubo_data.shadow_dual_paraboloid_render_zfar = 0;

	p_cam_projection.get_viewport_size(state.ubo_data.viewport_size[0], state.ubo_data.viewport_size[1]);

	if (storage->frame.current_rt) {
		state.ubo_data.screen_pixel_size[0] = 1.0 / storage->frame.current_rt->width;
		state.ubo_data.screen_pixel_size[1] = 1.0 / storage->frame.current_rt->height;
	}

	_setup_environment(env, p_cam_projection, p_cam_transform);

	bool fb_cleared = false;

	glDepthFunc(GL_LEQUAL);

	state.used_contact_shadows = true;

	if (!storage->config.no_depth_prepass && storage->frame.current_rt && state.debug_draw != VS::VIEWPORT_DEBUG_DRAW_OVERDRAW) { //detect with state.used_contact_shadows too
		//pre z pass

		glDisable(GL_BLEND);
		glDepthMask(GL_TRUE);
		glEnable(GL_DEPTH_TEST);
		glDisable(GL_SCISSOR_TEST);
		glBindFramebuffer(GL_FRAMEBUFFER, storage->frame.current_rt->buffers.fbo);
		glDrawBuffers(0, NULL);

		glViewport(0, 0, storage->frame.current_rt->width, storage->frame.current_rt->height);

		glColorMask(0, 0, 0, 0);
		glClearDepth(1.0f);
		glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		render_list.clear();
		_fill_render_list(p_cull_result, p_cull_count, true, false);
		render_list.sort_by_key(false);
		state.scene_shader.set_conditional(SceneShaderGLES3::RENDER_DEPTH, true);
		_render_list(render_list.elements, render_list.element_count, p_cam_transform, p_cam_projection, 0, false, false, true, false, false);
		state.scene_shader.set_conditional(SceneShaderGLES3::RENDER_DEPTH, false);

		glColorMask(1, 1, 1, 1);

		if (state.used_contact_shadows) {

			glBindFramebuffer(GL_READ_FRAMEBUFFER, storage->frame.current_rt->buffers.fbo);
			glReadBuffer(GL_COLOR_ATTACHMENT0);
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, storage->frame.current_rt->fbo);
			glBlitFramebuffer(0, 0, storage->frame.current_rt->width, storage->frame.current_rt->height, 0, 0, storage->frame.current_rt->width, storage->frame.current_rt->height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
			glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
			//bind depth for read
			glActiveTexture(GL_TEXTURE0 + storage->config.max_texture_image_units - 8);
			glBindTexture(GL_TEXTURE_2D, storage->frame.current_rt->depth);
		}

		fb_cleared = true;
		render_pass++;
		state.using_contact_shadows = true;
	} else {
		state.using_contact_shadows = false;
	}

	_setup_lights(p_light_cull_result, p_light_cull_count, p_cam_transform.affine_inverse(), p_cam_projection, p_shadow_atlas);
	_setup_reflections(p_reflection_probe_cull_result, p_reflection_probe_cull_count, p_cam_transform.affine_inverse(), p_cam_projection, p_reflection_atlas, env);

	bool use_mrt = false;

	render_list.clear();
	_fill_render_list(p_cull_result, p_cull_count, false, false);
	//

	glEnable(GL_BLEND);
	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_SCISSOR_TEST);

	//rendering to a probe cubemap side
	ReflectionProbeInstance *probe = reflection_probe_instance_owner.getornull(p_reflection_probe);
	GLuint current_fbo;

	if (probe) {

		ReflectionAtlas *ref_atlas = reflection_atlas_owner.getptr(probe->atlas);
		ERR_FAIL_COND(!ref_atlas);

		int target_size = ref_atlas->size / ref_atlas->subdiv;

		int cubemap_index = reflection_cubemaps.size() - 1;

		for (int i = reflection_cubemaps.size() - 1; i >= 0; i--) {
			//find appropriate cubemap to render to
			if (reflection_cubemaps[i].size > target_size * 2)
				break;

			cubemap_index = i;
		}

		current_fbo = reflection_cubemaps[cubemap_index].fbo_id[p_reflection_probe_pass];
		use_mrt = false;
		state.scene_shader.set_conditional(SceneShaderGLES3::USE_MULTIPLE_RENDER_TARGETS, false);

		glViewport(0, 0, reflection_cubemaps[cubemap_index].size, reflection_cubemaps[cubemap_index].size);
		glBindFramebuffer(GL_FRAMEBUFFER, current_fbo);

	} else {

		use_mrt = env && (state.used_sss || env->ssao_enabled || env->ssr_enabled); //only enable MRT rendering if any of these is enabled
		//effects disabled and transparency also prevent using MRTs
		use_mrt = use_mrt && !storage->frame.current_rt->flags[RasterizerStorage::RENDER_TARGET_TRANSPARENT];
		use_mrt = use_mrt && !storage->frame.current_rt->flags[RasterizerStorage::RENDER_TARGET_NO_3D_EFFECTS];
		use_mrt = use_mrt && state.debug_draw != VS::VIEWPORT_DEBUG_DRAW_OVERDRAW;
		use_mrt = use_mrt && env && (env->bg_mode != VS::ENV_BG_KEEP && env->bg_mode != VS::ENV_BG_CANVAS);

		glViewport(0, 0, storage->frame.current_rt->width, storage->frame.current_rt->height);

		if (use_mrt) {

			current_fbo = storage->frame.current_rt->buffers.fbo;

			glBindFramebuffer(GL_FRAMEBUFFER, storage->frame.current_rt->buffers.fbo);
			state.scene_shader.set_conditional(SceneShaderGLES3::USE_MULTIPLE_RENDER_TARGETS, true);

			Vector<GLenum> draw_buffers;
			draw_buffers.push_back(GL_COLOR_ATTACHMENT0);
			draw_buffers.push_back(GL_COLOR_ATTACHMENT1);
			draw_buffers.push_back(GL_COLOR_ATTACHMENT2);
			if (state.used_sss) {
				draw_buffers.push_back(GL_COLOR_ATTACHMENT3);
			}
			glDrawBuffers(draw_buffers.size(), draw_buffers.ptr());

			Color black(0, 0, 0, 0);
			glClearBufferfv(GL_COLOR, 1, black.components); // specular
			glClearBufferfv(GL_COLOR, 2, black.components); // normal metal rough
			if (state.used_sss) {
				glClearBufferfv(GL_COLOR, 3, black.components); // normal metal rough
			}

		} else {

			if (storage->frame.current_rt->buffers.active) {
				current_fbo = storage->frame.current_rt->buffers.fbo;
			} else {
				current_fbo = storage->frame.current_rt->effects.mip_maps[0].sizes[0].fbo;
			}

			glBindFramebuffer(GL_FRAMEBUFFER, current_fbo);
			state.scene_shader.set_conditional(SceneShaderGLES3::USE_MULTIPLE_RENDER_TARGETS, false);

			Vector<GLenum> draw_buffers;
			draw_buffers.push_back(GL_COLOR_ATTACHMENT0);
			glDrawBuffers(draw_buffers.size(), draw_buffers.ptr());
		}
	}

	if (!fb_cleared) {
		glClearBufferfi(GL_DEPTH_STENCIL, 0, 1.0, 0);
	}

	Color clear_color(0, 0, 0, 0);

	RasterizerStorageGLES3::Sky *sky = NULL;
	GLuint env_radiance_tex = 0;

	if (state.debug_draw == VS::VIEWPORT_DEBUG_DRAW_OVERDRAW) {
		clear_color = Color(0, 0, 0, 0);
		storage->frame.clear_request = false;
	} else if (!probe && storage->frame.current_rt->flags[RasterizerStorage::RENDER_TARGET_TRANSPARENT]) {
		clear_color = Color(0, 0, 0, 0);
		storage->frame.clear_request = false;

	} else if (!env || env->bg_mode == VS::ENV_BG_CLEAR_COLOR) {

		if (storage->frame.clear_request) {

			clear_color = storage->frame.clear_request_color.to_linear();
			storage->frame.clear_request = false;
		}

	} else if (env->bg_mode == VS::ENV_BG_CANVAS) {

		clear_color = env->bg_color.to_linear();
		storage->frame.clear_request = false;
	} else if (env->bg_mode == VS::ENV_BG_COLOR) {

		clear_color = env->bg_color.to_linear();
		storage->frame.clear_request = false;
	} else if (env->bg_mode == VS::ENV_BG_SKY || env->bg_mode == VS::ENV_BG_COLOR_SKY) {

		sky = storage->sky_owner.getornull(env->sky);

		if (sky) {
			env_radiance_tex = sky->radiance;
		}
		storage->frame.clear_request = false;
		if (env->bg_mode == VS::ENV_BG_COLOR_SKY) {
			clear_color = env->bg_color.to_linear();
		}

	} else {
		storage->frame.clear_request = false;
	}

	if (!env || env->bg_mode != VS::ENV_BG_KEEP) {
		glClearBufferfv(GL_COLOR, 0, clear_color.components); // specular
	}

	if (env && env->bg_mode == VS::ENV_BG_CANVAS) {
		//copy canvas to 3d buffer and convert it to linear

		glDisable(GL_BLEND);
		glDepthMask(GL_FALSE);
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);

		glActiveTexture(GL_TEXTURE0);

		storage->shaders.copy.set_conditional(CopyShaderGLES3::DISABLE_ALPHA, true);

		storage->shaders.copy.set_conditional(CopyShaderGLES3::SRGB_TO_LINEAR, true);

		storage->shaders.copy.bind();

		_copy_screen(true, true);

		//turn off everything used
		storage->shaders.copy.set_conditional(CopyShaderGLES3::SRGB_TO_LINEAR, false);
		storage->shaders.copy.set_conditional(CopyShaderGLES3::DISABLE_ALPHA, false);

		//restore
		glEnable(GL_BLEND);
		glDepthMask(GL_TRUE);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
	}

	state.texscreen_copied = false;

	glBlendEquation(GL_FUNC_ADD);

	if (storage->frame.current_rt && storage->frame.current_rt->flags[RasterizerStorage::RENDER_TARGET_TRANSPARENT]) {
		glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_BLEND);
	} else {
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDisable(GL_BLEND);
	}

	render_list.sort_by_key(false);

	if (state.directional_light_count == 0) {
		directional_light = NULL;
		_render_list(render_list.elements, render_list.element_count, p_cam_transform, p_cam_projection, env_radiance_tex, false, false, false, false, shadow_atlas != NULL);
	} else {
		for (int i = 0; i < state.directional_light_count; i++) {
			directional_light = directional_lights[i];
			if (i > 0) {
				glEnable(GL_BLEND);
			}
			_setup_directional_light(i, p_cam_transform.affine_inverse(), shadow_atlas != NULL && shadow_atlas->size > 0);
			_render_list(render_list.elements, render_list.element_count, p_cam_transform, p_cam_projection, env_radiance_tex, false, false, false, i > 0, shadow_atlas != NULL);
		}
	}

	state.scene_shader.set_conditional(SceneShaderGLES3::USE_MULTIPLE_RENDER_TARGETS, false);

	if (use_mrt) {
		GLenum gldb = GL_COLOR_ATTACHMENT0;
		glDrawBuffers(1, &gldb);
	}

	if (env && env->bg_mode == VS::ENV_BG_SKY && (!storage->frame.current_rt || (!storage->frame.current_rt->flags[RasterizerStorage::RENDER_TARGET_TRANSPARENT] && state.debug_draw != VS::VIEWPORT_DEBUG_DRAW_OVERDRAW))) {

		//
		//if (use_mrt) {
		//	glBindFramebuffer(GL_FRAMEBUFFER,storage->frame.current_rt->buffers.fbo); //switch to alpha fbo for sky, only diffuse/ambient matters
		//

		_draw_sky(sky, p_cam_projection, p_cam_transform, false, env->sky_custom_fov, env->bg_energy);
	}

	//_render_list_forward(&alpha_render_list,camera_transform,camera_transform_inverse,camera_projection,false,fragment_lighting,true);
	//glColorMask(1,1,1,1);

	//state.scene_shader.set_conditional( SceneShaderGLES3::USE_FOG,false);

	if (use_mrt) {
		_render_mrts(env, p_cam_projection);
	} else {
		//FIXME: check that this is possible to use
		if (storage->frame.current_rt && storage->frame.current_rt->buffers.active && state.used_screen_texture) {
			glBindFramebuffer(GL_READ_FRAMEBUFFER, storage->frame.current_rt->buffers.fbo);
			glReadBuffer(GL_COLOR_ATTACHMENT0);
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, storage->frame.current_rt->effects.mip_maps[0].sizes[0].fbo);
			glBlitFramebuffer(0, 0, storage->frame.current_rt->width, storage->frame.current_rt->height, 0, 0, storage->frame.current_rt->width, storage->frame.current_rt->height, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, GL_NEAREST);
			glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
			_blur_effect_buffer();
			//restored framebuffer
			glBindFramebuffer(GL_FRAMEBUFFER, storage->frame.current_rt->buffers.fbo);
			glViewport(0, 0, storage->frame.current_rt->width, storage->frame.current_rt->height);
		}
	}

	if (storage->frame.current_rt && state.used_screen_texture && storage->frame.current_rt->buffers.active) {
		glActiveTexture(GL_TEXTURE0 + storage->config.max_texture_image_units - 7);
		glBindTexture(GL_TEXTURE_2D, storage->frame.current_rt->effects.mip_maps[0].color);
	}

	glEnable(GL_BLEND);
	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_SCISSOR_TEST);

	render_list.sort_by_reverse_depth_and_priority(true);

	if (state.directional_light_count == 0) {
		directional_light = NULL;
		_render_list(&render_list.elements[render_list.max_elements - render_list.alpha_element_count], render_list.alpha_element_count, p_cam_transform, p_cam_projection, env_radiance_tex, false, true, false, false, shadow_atlas != NULL);
	} else {
		for (int i = 0; i < state.directional_light_count; i++) {
			directional_light = directional_lights[i];
			_setup_directional_light(i, p_cam_transform.affine_inverse(), shadow_atlas != NULL && shadow_atlas->size > 0);
			_render_list(&render_list.elements[render_list.max_elements - render_list.alpha_element_count], render_list.alpha_element_count, p_cam_transform, p_cam_projection, env_radiance_tex, false, true, false, i > 0, shadow_atlas != NULL);
		}
	}

	if (probe) {
		//rendering a probe, do no more!
		return;
	}

	_post_process(env, p_cam_projection);

	if (false && shadow_atlas) {

		//_copy_texture_to_front_buffer(shadow_atlas->depth);
		storage->canvas->canvas_begin();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, shadow_atlas->depth);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);
		storage->canvas->draw_generic_textured_rect(Rect2(0, 0, storage->frame.current_rt->width / 2, storage->frame.current_rt->height / 2), Rect2(0, 0, 1, 1));
	}

	if (false && storage->frame.current_rt) {

		//_copy_texture_to_front_buffer(shadow_atlas->depth);
		storage->canvas->canvas_begin();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, exposure_shrink[4].color);
		//glBindTexture(GL_TEXTURE_2D,storage->frame.current_rt->exposure.color);
		storage->canvas->draw_generic_textured_rect(Rect2(0, 0, storage->frame.current_rt->width / 16, storage->frame.current_rt->height / 16), Rect2(0, 0, 1, 1));
	}

	if (false && reflection_atlas && storage->frame.current_rt) {

		//_copy_texture_to_front_buffer(shadow_atlas->depth);
		storage->canvas->canvas_begin();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, reflection_atlas->color);
		storage->canvas->draw_generic_textured_rect(Rect2(0, 0, storage->frame.current_rt->width / 2, storage->frame.current_rt->height / 2), Rect2(0, 0, 1, 1));
	}

	if (false && directional_shadow.fbo) {

		//_copy_texture_to_front_buffer(shadow_atlas->depth);
		storage->canvas->canvas_begin();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, directional_shadow.depth);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);
		storage->canvas->draw_generic_textured_rect(Rect2(0, 0, storage->frame.current_rt->width / 2, storage->frame.current_rt->height / 2), Rect2(0, 0, 1, 1));
	}

	if (false && env_radiance_tex) {

		//_copy_texture_to_front_buffer(shadow_atlas->depth);
		storage->canvas->canvas_begin();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, env_radiance_tex);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		storage->canvas->draw_generic_textured_rect(Rect2(0, 0, storage->frame.current_rt->width / 2, storage->frame.current_rt->height / 2), Rect2(0, 0, 1, 1));
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}

	//disable all stuff
	*/
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
}

void RasterizerSceneGLES2::iteration() {
}

void RasterizerSceneGLES2::finalize() {
}

RasterizerSceneGLES2::RasterizerSceneGLES2() {
}
