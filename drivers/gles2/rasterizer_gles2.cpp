/*************************************************************************/
/*  rasterizer_gles2.cpp                                                 */
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
#include "rasterizer_gles2.h"

#include "gl_context/context_gl.h"
#include "os/os.h"
#include "project_settings.h"
#include <string.h>
RasterizerStorage *RasterizerGLES2::get_storage() {

	return storage;
}

RasterizerCanvas *RasterizerGLES2::get_canvas() {

	return canvas;
}

RasterizerScene *RasterizerGLES2::get_scene() {

	return scene;
}

void RasterizerGLES2::initialize() {
}

void RasterizerGLES2::begin_frame() {
}

void RasterizerGLES2::set_current_render_target(RID p_render_target) {
}

void RasterizerGLES2::restore_render_target() {
}

void RasterizerGLES2::clear_render_target(const Color &p_color) {
}

void RasterizerGLES2::set_boot_image(const Ref<Image> &p_image, const Color &p_color, bool p_scale) {

	if (p_image.is_null() || p_image->empty())
		return;

	int window_w = OS::get_singleton()->get_video_mode(0).width;
	int window_h = OS::get_singleton()->get_video_mode(0).height;

	glBindFramebuffer(GL_FRAMEBUFFER, RasterizerStorageGLES2::system_fbo);
	glViewport(0, 0, window_w, window_h);
	glDisable(GL_BLEND);
	glDepthMask(GL_FALSE);
	glClearColor(p_color.r, p_color.g, p_color.b, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);

	canvas->canvas_begin();

	RID texture = storage->texture_create();
	storage->texture_allocate(texture, p_image->get_width(), p_image->get_height(), p_image->get_format(), VS::TEXTURE_FLAG_FILTER);
	storage->texture_set_data(texture, p_image);
}

void RasterizerGLES2::blit_render_target_to_screen(RID p_render_target, const Rect2 &p_screen_rect, int p_screen) {
}

void RasterizerGLES2::end_frame(bool p_swap_buffers) {
}

void RasterizerGLES2::finalize() {
}

Rasterizer *RasterizerGLES2::_create_current() {

	return memnew(RasterizerGLES2);
}

void RasterizerGLES2::make_current() {
	_create_func = _create_current;
}

void RasterizerGLES2::register_config() {
}

RasterizerGLES2::RasterizerGLES2() {

	storage = memnew(RasterizerStorageGLES2);
	canvas = memnew(RasterizerCanvasGLES2);
	scene = memnew(RasterizerSceneGLES2);
	canvas->storage = storage;
	canvas->scene_render = scene;
	storage->canvas = canvas;
	scene->storage = storage;
	storage->scene = scene;

	prev_ticks = 0;
	time_total = 0;
}

RasterizerGLES2::~RasterizerGLES2() {

	memdelete(storage);
	memdelete(canvas);
}
