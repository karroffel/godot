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

		float csy = -1.0;
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

	glBindBuffer(GL_ARRAY_BUFFER, data.polygon_buffer);

	uint32_t buffer_ofs = 0;

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vector2) * p_vertex_count, p_vertices);
	glEnableVertexAttribArray(VS::ARRAY_VERTEX);
	glVertexAttribPointer(VS::ARRAY_VERTEX, 2, GL_FLOAT, GL_FALSE, sizeof(Vector2), NULL);
	buffer_ofs += sizeof(Vector2) * p_vertex_count;

	if (p_singlecolor) {
		glDisableVertexAttribArray(VS::ARRAY_COLOR);
		Color m = *p_colors;
		glVertexAttrib4f(VS::ARRAY_COLOR, m.r, m.g, m.b, m.a);
	} else if (!p_colors) {
		glDisableVertexAttribArray(VS::ARRAY_COLOR);
		glVertexAttrib4f(VS::ARRAY_COLOR, 1, 1, 1, 1);
	} else {
		glBufferSubData(GL_ARRAY_BUFFER, buffer_ofs, sizeof(Color) * p_vertex_count, p_colors);
		glEnableVertexAttribArray(VS::ARRAY_COLOR);
		glVertexAttribPointer(VS::ARRAY_COLOR, 4, GL_FLOAT, GL_FALSE, sizeof(Color), ((uint8_t *)0) + buffer_ofs);
		buffer_ofs += sizeof(Color) * p_vertex_count;
	}

	if (p_uvs) {
		glBufferSubData(GL_ARRAY_BUFFER, buffer_ofs, sizeof(Vector2) * p_vertex_count, p_uvs);
		glEnableVertexAttribArray(VS::ARRAY_TEX_UV);
		glVertexAttribPointer(VS::ARRAY_TEX_UV, 2, GL_FLOAT, GL_FALSE, sizeof(Vector2), ((uint8_t *)0) + buffer_ofs);
		buffer_ofs += sizeof(Vector2) * p_vertex_count;
	} else {
		glDisableVertexAttribArray(VS::ARRAY_TEX_UV);
	}

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, data.polygon_index_buffer);
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(int) * p_index_count, p_indices);

	glDrawElements(GL_TRIANGLES, p_index_count, GL_UNSIGNED_INT, 0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void RasterizerCanvasGLES2::_draw_generic(GLuint p_primitive, int p_vertex_count, const Vector2 *p_vertices, const Vector2 *p_uvs, const Color *p_colors, bool p_singlecolor) {
}

void RasterizerCanvasGLES2::_draw_gui_primitive(int p_points, const Vector2 *p_vertices, const Color *p_colors, const Vector2 *p_uvs) {

	static const GLenum prim[5] = { GL_POINTS, GL_POINTS, GL_LINES, GL_TRIANGLES, GL_TRIANGLE_FAN };

	int color_offset = 0;
	int uv_offset = 0;
	int stride = 2;

	if (p_colors) {
		color_offset = stride;
		stride += 4;
	}

	if (p_uvs) {
		uv_offset = stride;
		stride += 2;
	}

	float buffer_data[(2 + 2 + 4) * 4];

	for (int i = 0; i < p_points; i++) {
		buffer_data[stride * i + 0] = p_vertices[i].x;
		buffer_data[stride * i + 1] = p_vertices[i].y;
	}

	if (p_colors) {
		for (int i = 0; i < p_points; i++) {
			buffer_data[stride * i + color_offset + 0] = p_colors[i].r;
			buffer_data[stride * i + color_offset + 1] = p_colors[i].g;
			buffer_data[stride * i + color_offset + 2] = p_colors[i].b;
			buffer_data[stride * i + color_offset + 3] = p_colors[i].a;
		}
	}

	if (p_uvs) {
		for (int i = 0; i < p_points; i++) {
			buffer_data[stride * i + uv_offset + 0] = p_uvs[i].x;
			buffer_data[stride * i + uv_offset + 1] = p_uvs[i].y;
		}
	}

	glBindBuffer(GL_ARRAY_BUFFER, data.polygon_buffer);
	glBufferSubData(GL_ARRAY_BUFFER, 0, p_points * stride * 4 * sizeof(float), buffer_data);

	glVertexAttribPointer(VS::ARRAY_VERTEX, 2, GL_FLOAT, GL_FALSE, stride * sizeof(float), NULL);

	if (p_colors)
		glVertexAttribPointer(VS::ARRAY_COLOR, 4, GL_FLOAT, GL_FALSE, stride * sizeof(float), (uint8_t *)0 + color_offset * sizeof(float));

	if (p_uvs)
		glVertexAttribPointer(VS::ARRAY_TEX_UV, 2, GL_FLOAT, GL_FALSE, stride * sizeof(float), (uint8_t *)0 + uv_offset * sizeof(float));

	glDrawArrays(prim[p_points], 0, p_points);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void RasterizerCanvasGLES2::_canvas_item_render_commands(Item *p_item, Item *current_clip, bool &reclip) {

	state.uniforms.modelview_matrix = p_item->final_transform;

	int command_count = p_item->commands.size();
	Item::Command **commands = p_item->commands.ptrw();

	for (int i = 0; i < command_count; i++) {

		Item::Command *command = commands[i];

		switch (command->type) {

			case Item::Command::TYPE_LINE: {

				Item::CommandLine *line = static_cast<Item::CommandLine *>(command);

				state.canvas_shader.set_conditional(CanvasShaderGLES2::USE_TEXTURE_RECT, false);
				state.canvas_shader.set_conditional(CanvasShaderGLES2::USE_UV_ATTRIBUTE, false);
				state.canvas_shader.bind();

				_set_uniforms();

				_bind_canvas_texture(RID(), RID());

				glVertexAttrib4fv(VS::ARRAY_COLOR, line->color.components);

				state.canvas_shader.set_uniform(CanvasShaderGLES2::MODELVIEW_MATRIX, state.uniforms.modelview_matrix);

				if (line->width <= 1) {
					Vector2 verts[2] = {
						Vector2(line->from.x, line->from.y),
						Vector2(line->to.x, line->to.y)
					};

					_draw_gui_primitive(2, verts, NULL, NULL);
				} else {
					Vector2 t = (line->from - line->to).normalized().tangent() * line->width * 0.5;

					Vector2 verts[4] = {
						line->from - t,
						line->from + t,
						line->to + t,
						line->to - t
					};

					_draw_gui_primitive(4, verts, NULL, NULL);
				}

			} break;

			case Item::Command::TYPE_RECT: {
				Item::CommandRect *r = static_cast<Item::CommandRect *>(command);

				glVertexAttrib4fv(VS::ARRAY_COLOR, r->modulate.components);

				_bind_quad_buffer();

				state.canvas_shader.set_conditional(CanvasShaderGLES2::USE_TEXTURE_RECT, true);
				state.canvas_shader.set_conditional(CanvasShaderGLES2::USE_UV_ATTRIBUTE, false);
				if (state.canvas_shader.bind())
					_set_uniforms();

				RasterizerStorageGLES2::Texture *tex = _bind_canvas_texture(r->texture, r->normal_map);

				if (!tex) {
					state.canvas_shader.set_uniform(CanvasShaderGLES2::SRC_RECT, Color(0, 0, 1, 1));
				} else {
					if (r->source != Rect2())
						state.canvas_shader.set_uniform(CanvasShaderGLES2::SRC_RECT, Color(r->source.position.x / tex->width, r->source.position.y / tex->height, r->source.size.x / tex->width, r->source.size.y / tex->height));
					else
						state.canvas_shader.set_uniform(CanvasShaderGLES2::SRC_RECT, Color(0, 0, 1, 1));
				}

				state.canvas_shader.set_uniform(CanvasShaderGLES2::DST_RECT, Color(r->rect.position.x, r->rect.position.y, r->rect.size.x, r->rect.size.y));

				state.canvas_shader.set_uniform(CanvasShaderGLES2::MODELVIEW_MATRIX, state.uniforms.modelview_matrix);

				glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
			} break;

			case Item::Command::TYPE_NINEPATCH: {

				Item::CommandNinePatch *np = static_cast<Item::CommandNinePatch *>(command);

				state.canvas_shader.set_conditional(CanvasShaderGLES2::USE_TEXTURE_RECT, false);
				state.canvas_shader.set_conditional(CanvasShaderGLES2::USE_UV_ATTRIBUTE, true);
				if (state.canvas_shader.bind())
					_set_uniforms();

				glVertexAttrib4fv(VS::ARRAY_COLOR, np->color.components);

				RasterizerStorageGLES2::Texture *tex = _bind_canvas_texture(np->texture, np->normal_map);

				if (!tex) {
					print_line("TODO: ninepatch without texture");
					continue;
				}

				Size2 texpixel_size(1.0 / tex->width, 1.0 / tex->height);

				state.canvas_shader.set_uniform(CanvasShaderGLES2::MODELVIEW_MATRIX, state.uniforms.modelview_matrix);

				// prepare vertex buffer

				float buffer[16 * 2 + 16 * 2];

				{

					// first row

					buffer[(0 * 4 * 4) + 0] = np->rect.position.x;
					buffer[(0 * 4 * 4) + 1] = np->rect.position.y;

					buffer[(0 * 4 * 4) + 2] = np->source.position.x * texpixel_size.x;
					buffer[(0 * 4 * 4) + 3] = np->source.position.y * texpixel_size.y;

					buffer[(0 * 4 * 4) + 4] = np->rect.position.x + np->margin[MARGIN_LEFT];
					buffer[(0 * 4 * 4) + 5] = np->rect.position.y;

					buffer[(0 * 4 * 4) + 6] = (np->source.position.x + np->margin[MARGIN_LEFT]) * texpixel_size.x;
					buffer[(0 * 4 * 4) + 7] = np->source.position.y * texpixel_size.y;

					buffer[(0 * 4 * 4) + 8] = np->rect.position.x + np->rect.size.x - np->margin[MARGIN_RIGHT];
					buffer[(0 * 4 * 4) + 9] = np->rect.position.y;

					buffer[(0 * 4 * 4) + 10] = (np->source.position.x + np->source.size.x - np->margin[MARGIN_RIGHT]) * texpixel_size.x;
					buffer[(0 * 4 * 4) + 11] = np->source.position.y * texpixel_size.y;

					buffer[(0 * 4 * 4) + 12] = np->rect.position.x + np->rect.size.x;
					buffer[(0 * 4 * 4) + 13] = np->rect.position.y;

					buffer[(0 * 4 * 4) + 14] = (np->source.position.x + np->source.size.x) * texpixel_size.x;
					buffer[(0 * 4 * 4) + 15] = np->source.position.y * texpixel_size.y;

					// second row

					buffer[(1 * 4 * 4) + 0] = np->rect.position.x;
					buffer[(1 * 4 * 4) + 1] = np->rect.position.y + np->margin[MARGIN_TOP];

					buffer[(1 * 4 * 4) + 2] = np->source.position.x * texpixel_size.x;
					buffer[(1 * 4 * 4) + 3] = (np->source.position.y + np->margin[MARGIN_TOP]) * texpixel_size.y;

					buffer[(1 * 4 * 4) + 4] = np->rect.position.x + np->margin[MARGIN_LEFT];
					buffer[(1 * 4 * 4) + 5] = np->rect.position.y + np->margin[MARGIN_TOP];

					buffer[(1 * 4 * 4) + 6] = (np->source.position.x + np->margin[MARGIN_LEFT]) * texpixel_size.x;
					buffer[(1 * 4 * 4) + 7] = (np->source.position.y + np->margin[MARGIN_TOP]) * texpixel_size.y;

					buffer[(1 * 4 * 4) + 8] = np->rect.position.x + np->rect.size.x - np->margin[MARGIN_RIGHT];
					buffer[(1 * 4 * 4) + 9] = np->rect.position.y + np->margin[MARGIN_TOP];

					buffer[(1 * 4 * 4) + 10] = (np->source.position.x + np->source.size.x - np->margin[MARGIN_RIGHT]) * texpixel_size.x;
					buffer[(1 * 4 * 4) + 11] = (np->source.position.y + np->margin[MARGIN_TOP]) * texpixel_size.y;

					buffer[(1 * 4 * 4) + 12] = np->rect.position.x + np->rect.size.x;
					buffer[(1 * 4 * 4) + 13] = np->rect.position.y + np->margin[MARGIN_TOP];

					buffer[(1 * 4 * 4) + 14] = (np->source.position.x + np->source.size.x) * texpixel_size.x;
					buffer[(1 * 4 * 4) + 15] = (np->source.position.y + np->margin[MARGIN_TOP]) * texpixel_size.y;

					// thrid row

					buffer[(2 * 4 * 4) + 0] = np->rect.position.x;
					buffer[(2 * 4 * 4) + 1] = np->rect.position.y + np->rect.size.y - np->margin[MARGIN_BOTTOM];

					buffer[(2 * 4 * 4) + 2] = np->source.position.x * texpixel_size.x;
					buffer[(2 * 4 * 4) + 3] = (np->source.position.y + np->source.size.y - np->margin[MARGIN_BOTTOM]) * texpixel_size.y;

					buffer[(2 * 4 * 4) + 4] = np->rect.position.x + np->margin[MARGIN_LEFT];
					buffer[(2 * 4 * 4) + 5] = np->rect.position.y + np->rect.size.y - np->margin[MARGIN_BOTTOM];

					buffer[(2 * 4 * 4) + 6] = (np->source.position.x + np->margin[MARGIN_LEFT]) * texpixel_size.x;
					buffer[(2 * 4 * 4) + 7] = (np->source.position.y + np->source.size.y - np->margin[MARGIN_BOTTOM]) * texpixel_size.y;

					buffer[(2 * 4 * 4) + 8] = np->rect.position.x + np->rect.size.x - np->margin[MARGIN_RIGHT];
					buffer[(2 * 4 * 4) + 9] = np->rect.position.y + np->rect.size.y - np->margin[MARGIN_BOTTOM];

					buffer[(2 * 4 * 4) + 10] = (np->source.position.x + np->source.size.x - np->margin[MARGIN_RIGHT]) * texpixel_size.x;
					buffer[(2 * 4 * 4) + 11] = (np->source.position.y + np->source.size.y - np->margin[MARGIN_BOTTOM]) * texpixel_size.y;

					buffer[(2 * 4 * 4) + 12] = np->rect.position.x + np->rect.size.x;
					buffer[(2 * 4 * 4) + 13] = np->rect.position.y + np->rect.size.y - np->margin[MARGIN_BOTTOM];

					buffer[(2 * 4 * 4) + 14] = (np->source.position.x + np->source.size.x) * texpixel_size.x;
					buffer[(2 * 4 * 4) + 15] = (np->source.position.y + np->source.size.y - np->margin[MARGIN_BOTTOM]) * texpixel_size.y;

					// fourth row

					buffer[(3 * 4 * 4) + 0] = np->rect.position.x;
					buffer[(3 * 4 * 4) + 1] = np->rect.position.y + np->rect.size.y;

					buffer[(3 * 4 * 4) + 2] = np->source.position.x * texpixel_size.x;
					buffer[(3 * 4 * 4) + 3] = (np->source.position.y + np->source.size.y) * texpixel_size.y;

					buffer[(3 * 4 * 4) + 4] = np->rect.position.x + np->margin[MARGIN_LEFT];
					buffer[(3 * 4 * 4) + 5] = np->rect.position.y + np->rect.size.y;

					buffer[(3 * 4 * 4) + 6] = (np->source.position.x + np->margin[MARGIN_LEFT]) * texpixel_size.x;
					buffer[(3 * 4 * 4) + 7] = (np->source.position.y + np->source.size.y) * texpixel_size.y;

					buffer[(3 * 4 * 4) + 8] = np->rect.position.x + np->rect.size.x - np->margin[MARGIN_RIGHT];
					buffer[(3 * 4 * 4) + 9] = np->rect.position.y + np->rect.size.y;

					buffer[(3 * 4 * 4) + 10] = (np->source.position.x + np->source.size.x - np->margin[MARGIN_RIGHT]) * texpixel_size.x;
					buffer[(3 * 4 * 4) + 11] = (np->source.position.y + np->source.size.y) * texpixel_size.y;

					buffer[(3 * 4 * 4) + 12] = np->rect.position.x + np->rect.size.x;
					buffer[(3 * 4 * 4) + 13] = np->rect.position.y + np->rect.size.y;

					buffer[(3 * 4 * 4) + 14] = (np->source.position.x + np->source.size.x) * texpixel_size.x;
					buffer[(3 * 4 * 4) + 15] = (np->source.position.y + np->source.size.y) * texpixel_size.y;

					// print_line(String::num((np->source.position.y + np->source.size.y) * texpixel_size.y));
				}

				glBindBuffer(GL_ARRAY_BUFFER, data.ninepatch_vertices);
				glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * (16 + 16) * 2, buffer);

				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, data.ninepatch_elements);

				glEnableVertexAttribArray(VS::ARRAY_VERTEX);
				glEnableVertexAttribArray(VS::ARRAY_TEX_UV);

				glVertexAttribPointer(VS::ARRAY_VERTEX, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), NULL);
				glVertexAttribPointer(VS::ARRAY_TEX_UV, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (uint8_t *)0 + (sizeof(float) * 2));

				glDrawElements(GL_TRIANGLES, 18 * 3 - (np->draw_center ? 0 : 6), GL_UNSIGNED_BYTE, NULL);

				glBindBuffer(GL_ARRAY_BUFFER, 0);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

			} break;

			case Item::Command::TYPE_CIRCLE: {
				print_line("circle");
			} break;

			case Item::Command::TYPE_POLYGON: {

				Item::CommandPolygon *polygon = static_cast<Item::CommandPolygon *>(command);

				state.canvas_shader.set_conditional(CanvasShaderGLES2::USE_TEXTURE_RECT, false);
				state.canvas_shader.set_conditional(CanvasShaderGLES2::USE_UV_ATTRIBUTE, false);

				if (state.canvas_shader.bind())
					_set_uniforms();

				RasterizerStorageGLES2::Texture *texture = _bind_canvas_texture(polygon->texture, polygon->normal_map);

				if (texture) {
				}

				_draw_polygon(polygon->indices.ptr(), polygon->count, polygon->points.size(), polygon->points.ptr(), polygon->uvs.ptr(), polygon->colors.ptr(), polygon->colors.size() == 1);
			} break;

			case Item::Command::TYPE_POLYLINE: {
				print_line("polyline");
			} break;

			case Item::Command::TYPE_PRIMITIVE: {
				print_line("primitive");
			} break;

			case Item::Command::TYPE_TRANSFORM: {
				Item::CommandTransform *transform = static_cast<Item::CommandTransform *>(command);
				state.extra_matrix = transform->xform;
				state.uniforms.extra_matrix = state.extra_matrix;
				state.canvas_shader.set_uniform(CanvasShaderGLES2::EXTRA_MATRIX, state.uniforms.extra_matrix);
			} break;

			case Item::Command::TYPE_PARTICLES: {

			} break;

			case Item::Command::TYPE_CLIP_IGNORE: {

			} break;

			default: {
				print_line("other");
			} break;
		}
	}
}

void RasterizerCanvasGLES2::_copy_texscreen(const Rect2 &p_rect) {
}

void RasterizerCanvasGLES2::canvas_render_items(Item *p_item_list, int p_z, const Color &p_modulate, Light *p_light) {

	while (p_item_list) {

		bool reclip = true;

		_canvas_item_render_commands(p_item_list, NULL, reclip);

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

	// quad buffer
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

	// polygon buffer
	{
		uint32_t poly_size = GLOBAL_DEF("rendering/limits/buffers/canvas_polygon_buffer_size_kb", 128);
		poly_size *= 1024;
		poly_size = MAX(poly_size, (2 + 2 + 4) * 4 * sizeof(float));
		glGenBuffers(1, &data.polygon_buffer);
		glBindBuffer(GL_ARRAY_BUFFER, data.polygon_buffer);
		glBufferData(GL_ARRAY_BUFFER, poly_size, NULL, GL_DYNAMIC_DRAW);

		// data.polygon_buffer_size = poly_size;

		glBindBuffer(GL_ARRAY_BUFFER, 0);

		uint32_t index_size = GLOBAL_DEF("rendering/limits/buffers/canvas_polygon_index_size_kb", 128);
		index_size *= 1024; // kb
		glGenBuffers(1, &data.polygon_index_buffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, data.polygon_index_buffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_size, NULL, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	// ninepatch buffers
	{
		// array buffer
		glGenBuffers(1, &data.ninepatch_vertices);
		glBindBuffer(GL_ARRAY_BUFFER, data.ninepatch_vertices);

		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * (16 + 16) * 2, NULL, GL_DYNAMIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// element buffer
		glGenBuffers(1, &data.ninepatch_elements);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, data.ninepatch_elements);

#define _EIDX(y, x) (y * 4 + x)
		uint8_t elems[3 * 2 * 9] = {

			// first row

			_EIDX(0, 0), _EIDX(0, 1), _EIDX(1, 1),
			_EIDX(1, 1), _EIDX(1, 0), _EIDX(0, 0),

			_EIDX(0, 1), _EIDX(0, 2), _EIDX(1, 2),
			_EIDX(1, 2), _EIDX(1, 1), _EIDX(0, 1),

			_EIDX(0, 2), _EIDX(0, 3), _EIDX(1, 3),
			_EIDX(1, 3), _EIDX(1, 2), _EIDX(0, 2),

			// second row

			_EIDX(1, 0), _EIDX(1, 1), _EIDX(2, 1),
			_EIDX(2, 1), _EIDX(2, 0), _EIDX(1, 0),

			// the center one would be here, but we'll put it at the end
			// so it's easier to disable the center and be able to use
			// one draw call for both

			_EIDX(1, 2), _EIDX(1, 3), _EIDX(2, 3),
			_EIDX(2, 3), _EIDX(2, 2), _EIDX(1, 2),

			// third row

			_EIDX(2, 0), _EIDX(2, 1), _EIDX(3, 1),
			_EIDX(3, 1), _EIDX(3, 0), _EIDX(2, 0),

			_EIDX(2, 1), _EIDX(2, 2), _EIDX(3, 2),
			_EIDX(3, 2), _EIDX(3, 1), _EIDX(2, 1),

			_EIDX(2, 2), _EIDX(2, 3), _EIDX(3, 3),
			_EIDX(3, 3), _EIDX(3, 2), _EIDX(2, 2),

			// center field

			_EIDX(1, 1), _EIDX(1, 2), _EIDX(2, 2),
			_EIDX(2, 2), _EIDX(2, 1), _EIDX(1, 1)
		};
		;
#undef _EIDX

		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elems), elems, GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	state.canvas_shader.init();

	state.canvas_shader.set_conditional(CanvasShaderGLES2::USE_TEXTURE_RECT, true);

	state.canvas_shader.bind();
}

void RasterizerCanvasGLES2::finalize() {
}

RasterizerCanvasGLES2::RasterizerCanvasGLES2() {
}
