/*************************************************************************/
/*  rasterizer_canvas_gles2.cpp                                          */
/*************************************************************************/
/*                       This file is part of:                           */
/*                           GODOT ENGINE                                */
/*                      https://godotengine.org                          */
/*************************************************************************/
/* Copyright (c) 2007-2017 Juan Linietsky, Ariel Manzur.                 */
/* Copyright (c) 2014-2017 Godot Engine contributors (cf. AUTHORS.md)    */
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
#include "rasterizer_canvas_gles2.h"
#include "os/os.h"
#include "project_settings.h"
#include "rasterizer_scene_gles2.h"
#include "servers/visual/visual_server_raster.h"
#ifndef GLES_OVER_GL
#define glClearDepth glClearDepthf
#endif

static _FORCE_INLINE_ void store_transform2d(const Transform2D &p_mtx, float *p_array) {

	p_array[0] = p_mtx.elements[0][0];
	p_array[1] = p_mtx.elements[0][1];
	p_array[2] = 0;
	p_array[3] = 0;
	p_array[4] = p_mtx.elements[1][0];
	p_array[5] = p_mtx.elements[1][1];
	p_array[6] = 0;
	p_array[7] = 0;
	p_array[8] = 0;
	p_array[9] = 0;
	p_array[10] = 1;
	p_array[11] = 0;
	p_array[12] = p_mtx.elements[2][0];
	p_array[13] = p_mtx.elements[2][1];
	p_array[14] = 0;
	p_array[15] = 1;
}

static _FORCE_INLINE_ void store_transform(const Transform &p_mtx, float *p_array) {
	p_array[0] = p_mtx.basis.elements[0][0];
	p_array[1] = p_mtx.basis.elements[1][0];
	p_array[2] = p_mtx.basis.elements[2][0];
	p_array[3] = 0;
	p_array[4] = p_mtx.basis.elements[0][1];
	p_array[5] = p_mtx.basis.elements[1][1];
	p_array[6] = p_mtx.basis.elements[2][1];
	p_array[7] = 0;
	p_array[8] = p_mtx.basis.elements[0][2];
	p_array[9] = p_mtx.basis.elements[1][2];
	p_array[10] = p_mtx.basis.elements[2][2];
	p_array[11] = 0;
	p_array[12] = p_mtx.origin.x;
	p_array[13] = p_mtx.origin.y;
	p_array[14] = p_mtx.origin.z;
	p_array[15] = 1;
}

RID RasterizerCanvasGLES2::light_internal_create() {

	return RID();
}

void RasterizerCanvasGLES2::light_internal_update(RID p_rid, Light *p_light) {
}

void RasterizerCanvasGLES2::light_internal_free(RID p_rid) {
}

void RasterizerCanvasGLES2::_set_uniforms() {

	state.canvas_shader.set_uniform(CanvasShaderGLES2::PROJECTION_MATRIX, state.uniforms.projection_matrix);
	state.canvas_shader.set_uniform(CanvasShaderGLES2::MODELVIEW_MATRIX, state.uniforms.modelview_matrix);
	state.canvas_shader.set_uniform(CanvasShaderGLES2::EXTRA_MATRIX, state.uniforms.extra_matrix);

	state.canvas_shader.set_uniform(CanvasShaderGLES2::FINAL_MODULATE, state.uniforms.final_modulate);
}

void RasterizerCanvasGLES2::canvas_begin() {

	if (storage->frame.clear_request) {
		glClearColor(storage->frame.clear_request_color.r,
				storage->frame.clear_request_color.g,
				storage->frame.clear_request_color.b,
				storage->frame.clear_request_color.a);
		glClear(GL_COLOR_BUFFER_BIT);
		storage->frame.clear_request = false;
	}

	if (storage->frame.current_rt) {
		glBindFramebuffer(GL_FRAMEBUFFER, storage->frame.current_rt->fbo);
		glColorMask(1, 1, 1, 1);
	}

	reset_canvas();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, storage->resources.white_tex);

	glVertexAttrib4f(VS::ARRAY_COLOR, 1, 1, 1, 1);

	// set up default uniforms

	Transform canvas_transform;

	if (storage->frame.current_rt) {

		float csy = 1.0;
		if (storage->frame.current_rt && storage->frame.current_rt->flags[RasterizerStorage::RENDER_TARGET_VFLIP]) {
			csy = -1.0;
		}
		canvas_transform.translate(-(storage->frame.current_rt->width / 2.0f), -(storage->frame.current_rt->height / 2.0f), 0.0f);
		canvas_transform.scale(Vector3(2.0f / storage->frame.current_rt->width, csy * -2.0f / storage->frame.current_rt->height, 1.0f));
	} else {
		Vector2 ssize = OS::get_singleton()->get_window_size();
		canvas_transform.translate(-(ssize.width / 2.0f), -(ssize.height / 2.0f), 0.0f);
		canvas_transform.scale(Vector3(2.0f / ssize.width, -2.0f / ssize.height, 1.0f));
	}

	state.uniforms.projection_matrix = canvas_transform;

	state.uniforms.final_modulate = Color(1, 1, 1, 1);

	state.uniforms.modelview_matrix = Transform2D();
	state.uniforms.extra_matrix = Transform2D();

	_set_uniforms();
	_bind_quad_buffer();
}

void RasterizerCanvasGLES2::canvas_end() {

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	for (int i = 0; i < VS::ARRAY_MAX; i++) {
		glDisableVertexAttribArray(i);
	}

	state.using_texture_rect = false;
	state.using_ninepatch = false;
}

RasterizerStorageGLES2::Texture *RasterizerCanvasGLES2::_bind_canvas_texture(const RID &p_texture, const RID &p_normal_map) {

	RasterizerStorageGLES2::Texture *tex_return = NULL;

	if (p_texture.is_valid()) {

		RasterizerStorageGLES2::Texture *texture = storage->texture_owner.getornull(p_texture);

		if (!texture) {
			state.current_tex = RID();
			state.current_tex_ptr = NULL;

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, storage->resources.white_tex);

		} else {

			texture = texture->get_ptr();

			if (texture->render_target) {
				texture->render_target->used_in_frame = true;
			}

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, texture->tex_id);

			state.current_tex = p_texture;
			state.current_tex_ptr = texture;

			tex_return = texture;
		}
	} else {
		state.current_tex = RID();
		state.current_tex_ptr = NULL;

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, storage->resources.white_tex);
	}

	return tex_return;
}

void RasterizerCanvasGLES2::_set_texture_rect_mode(bool p_enable, bool p_ninepatch) {
}

void RasterizerCanvasGLES2::_draw_polygon(const int *p_indices, int p_index_count, int p_vertex_count, const Vector2 *p_vertices, const Vector2 *p_uvs, const Color *p_colors, bool p_singlecolor) {
}

void RasterizerCanvasGLES2::_draw_generic(GLuint p_primitive, int p_vertex_count, const Vector2 *p_vertices, const Vector2 *p_uvs, const Color *p_colors, bool p_singlecolor) {
}

void RasterizerCanvasGLES2::_draw_gui_primitive(int p_points, const Vector2 *p_vertices, const Color *p_colors, const Vector2 *p_uvs) {
}

void RasterizerCanvasGLES2::_canvas_item_render_commands(Item *p_item, Item *current_clip, bool &reclip) {
}

void RasterizerCanvasGLES2::_copy_texscreen(const Rect2 &p_rect) {
}

void RasterizerCanvasGLES2::canvas_render_items(Item *p_item_list, int p_z, const Color &p_modulate, Light *p_light) {

	while (p_item_list) {

		for (int i = 0; i < p_item_list->commands.size(); i++) {

			Item::Command *command = p_item_list->commands[i];

			switch (command->type) {
				case Item::Command::TYPE_NINEPATCH: {

					Item::CommandNinePatch *np = static_cast<Item::CommandNinePatch *>(command);

					glVertexAttrib4f(VS::ARRAY_COLOR,
							np->color.r,
							np->color.g,
							np->color.b,
							np->color.a);

					// set texture rect mode

					RasterizerStorageGLES2::Texture *tex = _bind_canvas_texture(np->texture, np->normal_map);

					if (!tex) {
						state.canvas_shader.set_uniform(CanvasShaderGLES2::SRC_RECT, Color(0, 0, 1, 1));
					} else {
						if (np->source != Rect2())
							state.canvas_shader.set_uniform(CanvasShaderGLES2::SRC_RECT, Color(np->source.position.x / tex->width, np->source.position.y / tex->height, np->source.size.x / tex->width, np->source.size.y / tex->height));
						else
							state.canvas_shader.set_uniform(CanvasShaderGLES2::SRC_RECT, Color(0, 0, 1, 1));
					}

					state.canvas_shader.set_uniform(CanvasShaderGLES2::DST_RECT, Color(np->rect.position.x, np->rect.position.y, np->rect.size.x, np->rect.size.y));

					glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

				} break;

				default: {
				} break;
			}
		}

		p_item_list = p_item_list->next;
	}
}

void RasterizerCanvasGLES2::canvas_debug_viewport_shadows(Light *p_lights_with_shadow) {
}

void RasterizerCanvasGLES2::canvas_light_shadow_buffer_update(RID p_buffer, const Transform2D &p_light_xform, int p_light_mask, float p_near, float p_far, LightOccluderInstance *p_occluders, CameraMatrix *p_xform_cache) {
}

void RasterizerCanvasGLES2::reset_canvas() {

	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_SCISSOR_TEST);
	glDisable(GL_DITHER);
	glEnable(GL_BLEND);

	if (storage->frame.current_rt && storage->frame.current_rt->flags[RasterizerStorage::RENDER_TARGET_TRANSPARENT]) {
		glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	} else {
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void RasterizerCanvasGLES2::_bind_quad_buffer() {
	glBindBuffer(GL_ARRAY_BUFFER, data.canvas_quad_vertices);
	glEnableVertexAttribArray(VS::ARRAY_VERTEX);
	glVertexAttribPointer(VS::ARRAY_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, NULL);
}
void RasterizerCanvasGLES2::draw_generic_textured_rect(const Rect2 &p_rect, const Rect2 &p_src) {

	state.canvas_shader.set_uniform(CanvasShaderGLES2::DST_RECT, Color(p_rect.position.x, p_rect.position.y, p_rect.size.x, p_rect.size.y));
	state.canvas_shader.set_uniform(CanvasShaderGLES2::SRC_RECT, Color(p_src.position.x, p_src.position.y, p_src.size.x, p_src.size.y));

	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

void RasterizerCanvasGLES2::draw_window_margins(int *black_margin, RID *black_image) {
}

void RasterizerCanvasGLES2::initialize() {

	{
		glGenBuffers(1, &data.canvas_quad_vertices);
		glBindBuffer(GL_ARRAY_BUFFER, data.canvas_quad_vertices);

		const float qv[8] = {
			0, 0,
			0, 1,
			1, 1,
			1, 0
		};

		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 8, qv, GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	state.canvas_shader.init();

	state.canvas_shader.set_conditional(CanvasShaderGLES2::USE_TEXTURE_RECT, true);

	state.canvas_shader.bind();
}

void RasterizerCanvasGLES2::finalize() {
}

RasterizerCanvasGLES2::RasterizerCanvasGLES2() {
}
