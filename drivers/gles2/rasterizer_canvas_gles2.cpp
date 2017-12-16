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

void RasterizerCanvasGLES2::canvas_begin() {
	if (storage->frame.current_rt && storage->frame.clear_request) {

		glClearColor(storage->frame.clear_request_color.r,
				storage->frame.clear_request_color.g,
				storage->frame.clear_request_color.b,
				storage->frame.clear_request_color.a);
		glClear(GL_COLOR_BUFFER_BIT);
		storage->frame.clear_request = false;
	}

	reset_canvas();

	state.canvas_shader.set_custom_shader(0);
	state.canvas_shader.set_conditional(CanvasShaderGLES2::USE_GLES_OVER_GL, true);
	state.canvas_shader.set_conditional(CanvasShaderGLES2::USE_TEXTURE_RECT, true);
	state.canvas_shader.bind();
	state.canvas_shader.set_uniform(CanvasShaderGLES2::FINAL_MODULATE, Color(1, 1, 1, 1));
	state.canvas_shader.set_uniform(CanvasShaderGLES2::MODELVIEW_MATRIX, Transform2D());
	state.canvas_shader.set_uniform(CanvasShaderGLES2::EXTRA_MATRIX, Transform2D());
	state.canvas_shader.set_uniform(CanvasShaderGLES2::PROJECTION_MATRIX, state.uniforms.projection_matrix);

	glBindBuffer(GL_ARRAY_BUFFER, data.canvas_quad_vertices);
	glEnableVertexAttribArray(VS::ARRAY_VERTEX);
	glVertexAttribPointer(VS::ARRAY_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, NULL);
}

void RasterizerCanvasGLES2::canvas_end() {
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	for (int i = 0; i < VS::ARRAY_MAX; i++) {
		glDisableVertexAttribArray(i);
	}

	state.using_ninepatch = false;
	state.using_texture_rect = false;
}

RasterizerStorageGLES2::Texture *RasterizerCanvasGLES2::_bind_canvas_texture(const RID &p_texture, const RID &p_normal_map) {

	RasterizerStorageGLES2::Texture *tex_return = NULL;

	if (p_texture == state.current_tex) {
		tex_return = state.current_tex_ptr;
	} else if (p_texture.is_valid()) {

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

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, storage->resources.white_tex);
		state.current_tex = RID();
		state.current_tex_ptr = NULL;
	}

	if (p_normal_map == state.current_normal) {
		// state.canvas_shader.set_uniform(CanvasShaderGLES2::USE_DEFAULT_NORMAL, state.current_normal.is_valid());
	} else if (p_normal_map.is_valid()) {

		RasterizerStorageGLES2::Texture *normal_map = storage->texture_owner.getornull(p_normal_map);

		if (!normal_map) {
			state.current_normal = RID();
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, storage->resources.normal_tex);
			// state.canvas_shader.set_uniform(CanvasShaderGLES2::USE_DEFAULT_NORMAL, false);
		} else {
			normal_map = normal_map->get_ptr();
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, normal_map->tex_id);

			state.current_normal = p_normal_map;
			// state.canvas_shader.set_uniform(CanvasShaderGLES2::USE_DEFAULT_NORMAL, true);
		}

	} else {
		state.current_normal = RID();
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, storage->resources.normal_tex);
		// state.canvas_shader.set_uniform(CanvasShaderGLES2::USE_DEFAULT_NORMAL, false);
	}

	return tex_return;
}

void RasterizerCanvasGLES2::_set_texture_rect_mode(bool p_enable, bool p_ninepatch) {

	if (state.using_texture_rect == p_enable && state.using_ninepatch == p_ninepatch)
		return;

	if (p_enable) {
		glBindBuffer(GL_ARRAY_BUFFER, data.canvas_quad_vertices);
		glVertexAttribPointer(VS::ARRAY_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(VS::ARRAY_VERTEX);
	} else {
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glDisableVertexAttribArray(VS::ARRAY_VERTEX);
	}

	// state.canvas_shader.set_conditional(CanvasShaderGLES2::USE_NINEPATCH, p_ninepatch && p_enable);
	state.canvas_shader.set_conditional(CanvasShaderGLES2::USE_TEXTURE_RECT, p_enable);
	state.canvas_shader.bind();

	state.canvas_shader.set_uniform(CanvasShaderGLES2::FINAL_MODULATE, state.canvas_item_modulate);
	state.canvas_shader.set_uniform(CanvasShaderGLES2::MODELVIEW_MATRIX, state.final_transform);
	state.canvas_shader.set_uniform(CanvasShaderGLES2::EXTRA_MATRIX, state.extra_matrix);

	if (storage->frame.current_rt) {
		// state.canvas_shader.set_uniform(CanvasShaderGLES2::SCREEN_PIXEL_SIZE, Vector2(1.0 / storage->frame.current_rt->width, 1.0 / storage->frame.current_rt->height));
	} else {
		// state.canvas_shader.set_uniform(CanvasShaderGLES2::SCREEN_PIXEL_SIZE, Vector2(1.0, 1.0));
	}

	state.using_texture_rect = p_enable;
	state.using_ninepatch = p_ninepatch;
}

void RasterizerCanvasGLES2::_draw_polygon(const int *p_indices, int p_index_count, int p_vertex_count, const Vector2 *p_vertices, const Vector2 *p_uvs, const Color *p_colors, bool p_singlecolor) {
}

void RasterizerCanvasGLES2::_draw_generic(GLuint p_primitive, int p_vertex_count, const Vector2 *p_vertices, const Vector2 *p_uvs, const Color *p_colors, bool p_singlecolor) {
}

void RasterizerCanvasGLES2::_draw_gui_primitive(int p_points, const Vector2 *p_vertices, const Color *p_colors, const Vector2 *p_uvs) {
}

void RasterizerCanvasGLES2::_canvas_item_render_commands(Item *p_item, Item *current_clip, bool &reclip) {
	int cc = p_item->commands.size();
	Item::Command **commands = p_item->commands.ptrw();

	print_line(String("process ") + String::num(cc) + " commands");

	for (int i = 0; i < cc; i++) {

		Item::Command *c = commands[i];

		switch (c->type) {
			case Item::Command::TYPE_LINE: {
				print_line("line");
			} break;

			case Item::Command::TYPE_POLYLINE: {

			} break;

			case Item::Command::TYPE_RECT: {
				print_line("rect");

				Item::CommandRect *rect = static_cast<Item::CommandRect *>(c);

				_set_texture_rect_mode(true);

				//set color
				glVertexAttrib4f(VS::ARRAY_COLOR, rect->modulate.r, rect->modulate.g, rect->modulate.b, rect->modulate.a);

				// RasterizerStorageGLES2::Texture *texture = _bind_canvas_texture(rect->texture, rect->normal_map);

				if (false) {

				} else {
					Rect2 dst_rect = Rect2(rect->rect.position, rect->rect.size);

					if (dst_rect.size.width < 0) {
						dst_rect.position.x += dst_rect.size.width;
						dst_rect.size.width *= -1;
					}
					if (dst_rect.size.height < 0) {
						dst_rect.position.y += dst_rect.size.height;
						dst_rect.size.height *= -1;
					}

					state.canvas_shader.set_uniform(CanvasShaderGLES2::DST_RECT, Color(dst_rect.position.x, dst_rect.position.y, dst_rect.size.x, dst_rect.size.y));
					state.canvas_shader.set_uniform(CanvasShaderGLES2::SRC_RECT, Color(0, 0, 1, 1));
					// state.canvas_shader.set_uniform(CanvasShaderGLES2::CLIP_RECT_UV, false);
					glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
				}
			} break;
		}
	}
}

void RasterizerCanvasGLES2::_copy_texscreen(const Rect2 &p_rect) {
}

void RasterizerCanvasGLES2::canvas_render_items(Item *p_item_list, int p_z, const Color &p_modulate, Light *p_light) {

	Item *current_clip = NULL;
	RasterizerStorageGLES2::Shader *shader_cache = NULL;

	bool rebind_shader = true;

	Size2 rt_size = Size2(storage->frame.current_rt->width, storage->frame.current_rt->height);

	// state.canvas_shader.set_conditional(CanvasShaderGLES2::USE_DISTANCE_FIELD, false);

	// set uniforms
	state.canvas_shader.set_uniform(CanvasShaderGLES2::PROJECTION_MATRIX, state.uniforms.projection_matrix);
	// ^ set time as well.

	state.current_tex = RID();
	state.current_tex_ptr = NULL;
	state.current_normal = RID();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, storage->resources.white_tex);

	int last_blend_mode = -1;

	RID canvas_last_material;

	bool prev_distance_field = false;

	while (p_item_list) {

		Item *ci = p_item_list;

		if (prev_distance_field != ci->distance_field) {

			// state.canvas_shader.set_conditional(CanvasShaderGLES2::USE_DISTANCE_FIELD, ci->distance_field);
			prev_distance_field = ci->distance_field;
			rebind_shader = true;
		}

		if (current_clip != ci->final_clip_owner) {
			current_clip = ci->final_clip_owner;

			// setup clipping
			if (current_clip) {

				glEnable(GL_SCISSOR_TEST);
				glScissor(current_clip->final_clip_rect.position.x,
						(rt_size.height - (current_clip->final_clip_rect.position.y + current_clip->final_clip_rect.size.height)),
						current_clip->final_clip_rect.size.width,
						current_clip->final_clip_rect.size.height);
			} else {
				glDisable(GL_SCISSOR_TEST);
			}
		}

		if (ci->copy_back_buffer) {
			if (ci->copy_back_buffer->full) {
				_copy_texscreen(Rect2());
			} else {
				_copy_texscreen(ci->copy_back_buffer->rect);
			}
		}

		Item *material_owner = ci->material_owner ? ci->material_owner : ci;

		RID material = material_owner->material;

		if (material != canvas_last_material || rebind_shader) {

			RasterizerStorageGLES2::Material *material_ptr = storage->material_owner.getornull(material);
			RasterizerStorageGLES2::Shader *shader_ptr = NULL;

			if (material_ptr) {

				shader_ptr = material_ptr->shader;

				if (shader_ptr && shader_ptr->mode != VS::SHADER_CANVAS_ITEM) {
					shader_ptr = NULL;
				}
			}

			if (shader_ptr && shader_ptr != shader_cache) {

				if (shader_ptr->canvas_item.uses_screen_texture && !state.canvas_texscreen_used) {
					// copy if it wasn't copied before
					_copy_texscreen(Rect2());
				}

				if (shader_ptr->canvas_item.uses_time) {
					VisualServerRaster::redraw_request();
				}

				state.canvas_shader.set_custom_shader(shader_ptr->custom_code_id);
				state.canvas_shader.bind();

				// TODO bind uniforms

				int tc = material_ptr->textures.size();
				RID *textures = material_ptr->textures.ptrw();
				ShaderLanguage::ShaderNode::Uniform::Hint *texture_hints = shader_ptr->texture_hints.ptrw();

				for (int i = 0; i < tc; i++) {
					glActiveTexture(GL_TEXTURE2 + i);

					RasterizerStorageGLES2::Texture *t = storage->texture_owner.getornull(textures[i]);
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
			} else if (!shader_ptr) {
				state.canvas_shader.set_custom_shader(0);
				state.canvas_shader.bind();
			}

			shader_cache = shader_ptr;

			canvas_last_material = material;
			rebind_shader = false;
		}

		int blend_mode = shader_cache ? shader_cache->canvas_item.blend_mode : RasterizerStorageGLES2::Shader::CanvasItem::BLEND_MODE_MIX;
		bool unshaded = shader_cache && (shader_cache->canvas_item.light_mode == RasterizerStorageGLES2::Shader::CanvasItem::LIGHT_MODE_UNSHADED || blend_mode != RasterizerStorageGLES2::Shader::CanvasItem::BLEND_MODE_MIX);

		bool reclip = false;

		if (last_blend_mode != blend_mode) {

			switch (blend_mode) {
				case RasterizerStorageGLES2::Shader::CanvasItem::BLEND_MODE_MIX: {
					glBlendEquation(GL_FUNC_ADD);
					if (storage->frame.current_rt && storage->frame.current_rt->flags[RasterizerStorageGLES2::RENDER_TARGET_TRANSPARENT]) {
						glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
					} else {
						glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
					}
				} break;
				case RasterizerStorageGLES2::Shader::CanvasItem::BLEND_MODE_ADD: {
					glBlendEquation(GL_FUNC_ADD);
					glBlendFunc(GL_SRC_ALPHA, GL_ONE);
				} break;
				case RasterizerStorageGLES2::Shader::CanvasItem::BLEND_MODE_SUB: {
					glBlendEquation(GL_FUNC_REVERSE_SUBTRACT);
					glBlendFunc(GL_SRC_ALPHA, GL_ONE);
				} break;
				case RasterizerStorageGLES2::Shader::CanvasItem::BLEND_MODE_MUL: {
					glBlendEquation(GL_FUNC_ADD);
					glBlendFunc(GL_DST_COLOR, GL_ZERO);
				} break;
				case RasterizerStorageGLES2::Shader::CanvasItem::BLEND_MODE_PMALPHA: {
					glBlendEquation(GL_FUNC_ADD);
					glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
				} break;
			}

			last_blend_mode = blend_mode;
		}

		p_item_list = p_item_list->next;

		state.canvas_item_modulate = unshaded ? ci->final_modulate : Color(ci->final_modulate.r * p_modulate.r, ci->final_modulate.g * p_modulate.g, ci->final_modulate.b * p_modulate.b, ci->final_modulate.a * p_modulate.a);

		state.final_transform = ci->final_transform;
		state.extra_matrix = Transform2D();

		state.canvas_shader.set_uniform(CanvasShaderGLES2::FINAL_MODULATE, state.canvas_item_modulate);
		state.canvas_shader.set_uniform(CanvasShaderGLES2::MODELVIEW_MATRIX, state.final_transform);
		state.canvas_shader.set_uniform(CanvasShaderGLES2::EXTRA_MATRIX, state.extra_matrix);
		if (storage->frame.current_rt) {
			// state.canvas_shader.set_uniform(CanvasShaderGLES3::SCREEN_PIXEL_SIZE, Vector2(1.0 / storage->frame.current_rt->width, 1.0 / storage->frame.current_rt->height));
		} else {
			// state.canvas_shader.set_uniform(CanvasShaderGLES3::SCREEN_PIXEL_SIZE, Vector2(1.0, 1.0));
		}

		if (unshaded || (state.canvas_item_modulate.a > 0.001 && (!shader_cache || shader_cache->canvas_item.light_mode != RasterizerStorageGLES2::Shader::CanvasItem::LIGHT_MODE_LIGHT_ONLY) && !ci->light_masked))
			_canvas_item_render_commands(ci, current_clip, reclip);

		// TODO LIGHT

		if (reclip) {
			glEnable(GL_SCISSOR_TEST);
			glScissor(current_clip->final_clip_rect.position.x, (rt_size.height - (current_clip->final_clip_rect.position.y + current_clip->final_clip_rect.size.height)), current_clip->final_clip_rect.size.width, current_clip->final_clip_rect.size.height);
		}
	}

	if (current_clip) {
		glDisable(GL_SCISSOR_TEST);
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
	glBlendEquation(GL_FUNC_ADD);
	if (storage->frame.current_rt && storage->frame.current_rt->flags[RasterizerStorage::RENDER_TARGET_TRANSPARENT]) {
		glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	} else {
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

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
}

void RasterizerCanvasGLES2::draw_generic_textured_rect(const Rect2 &p_rect, const Rect2 &p_src) {
	state.canvas_shader.set_uniform(CanvasShaderGLES2::DST_RECT, Color(p_rect.position.x, p_rect.position.y, p_rect.size.x, p_rect.size.y));
	state.canvas_shader.set_uniform(CanvasShaderGLES2::SRC_RECT, Color(p_src.position.x, p_src.position.y, p_src.size.x, p_src.size.y));
	// state.canvas_shader.set_uniform(CanvasShaderGLES3::CLIP_RECT_UV, false);

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
}

void RasterizerCanvasGLES2::finalize() {
}

RasterizerCanvasGLES2::RasterizerCanvasGLES2() {
}
