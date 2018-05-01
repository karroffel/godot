#ifndef TRANSFORM_COMPS_H
#define TRANSFORM_COMPS_H

#include "core/math/math_2d.h"
#include "core/math/transform.h"

namespace components {

struct TransformComp {
	Transform transform;
};

struct Transform2dComp {
	Transform2D transform;
};

} // namespace components

#endif // TRANSFORM_COMPS_H
