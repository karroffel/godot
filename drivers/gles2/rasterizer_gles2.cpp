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

	if (OS::get_singleton()->is_stdout_verbose()) {
		print_line("Using GLES2 video driver");
	}

#ifdef GLAD_ENABLED
	if (!gladLoadGL()) {
		ERR_PRINT("Error initializing GLAD");
	}

// GLVersion seems to be used for both GL and GL ES, so we need different version checks for them
#ifdef OPENGL_ENABLED // OpenGL 3.3 Core Profile required
	if (GLVersion.major < 3) {
#else // OpenGL ES 3.0
	if (GLVersion.major < 2) {
#endif
		ERR_PRINT("Your system's graphic drivers seem not to support OpenGL 3.0 / OpenGL ES 2.0, sorry :(\n"
				  "Try a drivers update, buy a new GPU or try software rendering on Linux; Godot will now crash with a segmentation fault.");
		OS::get_singleton()->alert("Your system's graphic drivers seem not to support OpenGL 3.0 / OpenGL ES 2.0, sorry :(\n"
								   "Godot Engine will self-destruct as soon as you acknowledge this error message.",
				"Fatal error: Insufficient OpenGL / GLES driver support");
	}

#ifdef __APPLE__
// FIXME glDebugMessageCallbackARB does not seem to work on Mac OS X and opengl 3, this may be an issue with our opengl canvas..
#else
		// if (OS::get_singleton()->is_stdout_verbose()) {
		// 	glEnable(_EXT_DEBUG_OUTPUT_SYNCHRONOUS_ARB);
		//	glDebugMessageCallbackARB(_gl_debug_print, NULL);
		//	glEnable(_EXT_DEBUG_OUTPUT);
		// }
#endif

#endif // GLAD_ENABLED

	/* // For debugging
	glDebugMessageControlARB(GL_DEBUG_SOURCE_API_ARB,GL_DEBUG_TYPE_ERROR_ARB,GL_DEBUG_SEVERITY_HIGH_ARB,0,NULL,GL_TRUE);
	glDebugMessageControlARB(GL_DEBUG_SOURCE_API_ARB,GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_ARB,GL_DEBUG_SEVERITY_HIGH_ARB,0,NULL,GL_TRUE);
	glDebugMessageControlARB(GL_DEBUG_SOURCE_API_ARB,GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_ARB,GL_DEBUG_SEVERITY_HIGH_ARB,0,NULL,GL_TRUE);
	glDebugMessageControlARB(GL_DEBUG_SOURCE_API_ARB,GL_DEBUG_TYPE_PORTABILITY_ARB,GL_DEBUG_SEVERITY_HIGH_ARB,0,NULL,GL_TRUE);
	glDebugMessageControlARB(GL_DEBUG_SOURCE_API_ARB,GL_DEBUG_TYPE_PERFORMANCE_ARB,GL_DEBUG_SEVERITY_HIGH_ARB,0,NULL,GL_TRUE);
	glDebugMessageControlARB(GL_DEBUG_SOURCE_API_ARB,GL_DEBUG_TYPE_OTHER_ARB,GL_DEBUG_SEVERITY_HIGH_ARB,0,NULL,GL_TRUE);
	glDebugMessageInsertARB(
			GL_DEBUG_SOURCE_API_ARB,
			GL_DEBUG_TYPE_OTHER_ARB, 1,
			GL_DEBUG_SEVERITY_HIGH_ARB,5, "hello");
	*/

	const GLubyte *renderer = glGetString(GL_RENDERER);
	print_line("OpenGL ES 2.0 Renderer: " + String((const char *)renderer));
	storage->initialize();
	canvas->initialize();
	scene->initialize();
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

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
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
