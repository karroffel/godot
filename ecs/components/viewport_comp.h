#ifndef VIEWPORT_H
#define VIEWPORT_H

#include "core/color.h"
#include "core/math/math_2d.h"
#include "core/rid.h"

struct ViewportComponent {
	RID viewport;
};

struct CircleComponent {

	float radius;
	Color color;
};

struct Position2dComponent {
	Point2 position;
};

#endif // VIEWPORT_H
