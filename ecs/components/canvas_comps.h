#ifndef CANVAS_COMPS_H
#define CANVAS_COMPS_H

#include "servers/visual_server.h"

namespace components {

struct CanvasItemTag {};

struct CanvasLine {
	Point2 from;
	Point2 to;
	Color color;
	float width;
	bool antialiased;
};

struct CanvasPolyLine {
	Vector<Point2> points;
	Vector<Color> colors;
	float width;
	bool antialiased;
};

struct CanvasMultiLine {
	Vector<Point2> points;
	Vector<Color> colors;
	float width;
	bool antialiased;
};

struct CanvasRect {
	Rect2 rect;
	Color color;
};

struct CanvasCircle {
	Point2 position;
	float radius;
	Color color;
};

struct CanvasTextureRect {
	Rect2 rect;
	RID texture;
	bool tile;
	Color modulate;
	bool transpose;
	RID normal_map;
};

struct CanvasTextureRectRegion {
	Rect2 rect;
	RID texture;
	Rect2 src_rect;
	Color modulate;
	bool transpose;
	RID normal_map;
	bool clip_uv;
};

struct CanvasNinePath {};

struct CanvasPrimitive {};

struct CanvasPolygon {};

struct CanvasTriangleArray {};

struct CanvasMesh {};

struct CanvasMultimesh {};

struct CanvasParticles {};

} // namespace components

#endif // CANVAS_COMPS_H
