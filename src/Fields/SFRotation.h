/*
 * Copyright 2009 Nathan Matthews <lowentropy@gmail.com>
 *
 * This file is part of SimpleX3D.
 *
 * SimpleX3D is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * SimpleX3D is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with SimpleX3D.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _X3D_ROTATION_H_
#define _X3D_ROTATION_H_

#include "types.h"
#include <math.h>

/**
 * Rotation around an arbitrary axis.
 * 
 * The #x, #y, and #z members define an axis and should always be
 * normalized. The #a member defines a right-handed rotation about this
 * axis. The default rotation value is (0,0,1,0).
 */
class SFRotation {
public:
	float x; ///< rotation axis X value
	float y; ///< rotation axis Y value
	float z; ///< rotation axis Z value
	float a; ///< angle of rotation

	/**
	 * Default constructor.
	 * 
	 * Default rotation is (0,0,1,0).
	 */
	SFRotation() : x(0), y(0), z(1), a(0) {}

	/**
	 * Copy constructor.
	 * 
	 * @param r rotation to copy from.
	 */
	SFRotation(const SFRotation& r) : x(r.x), y(r.y), z(r.z), a(r.a) {}

	/**
	 * Full constructor.
	 * 
	 * @param x rotation axis X value
	 * @param y rotation axis Y value
	 * @param z rotation axis Z value
	 * @param a angle of rotation
	 */
	SFRotation(float x, float y, float z, float a) : x(x), y(y), z(z), a(a) {}

	/**
	 * Convert into rotation matrix.
	 * 
	 * The matrix will be of the form
	 * \f$ \left[
	 *   \begin{array}{ccc}
	 *     t x^2 + c   & t x y + s z & t x z - s y \\
	 *     t x y - s z & t y^2 + c   & t y z + s x \\
	 *     t x z + s y & t y z - s x & t z^2 + c
	 *   \end{array}
	 * \right] \f$
	 * 
	 * where \f$ s = sin(\alpha) \f$,
	 * \f$ c = cos(\alpha) \f$, and \f$ t = 1 - c \f$.
	 */
	SFMatrix3<float> matrix() const {
		float c = cos(a), s = sin(a), t = 1-c;
		float tx = t * x, ty = t * y, tz = t * z;
		float txy = tx * y, tyz = ty * z, txz = tx * z;
		float sx = s * x, sy = s * y, sz = s * z;
		float m[9] = {
			tx * x + c, txy + sz, txz - sy,
			txy - sz, ty * y + c, tyz + sx,
			txz + sy, tyz - sx, tz * z + c
		};
		return SFMatrix3<float>(m);
	}

	/**
	 * Array accessor (mutable version).
	 * 
	 * @returns mutable pointer to rotation component array
	 */
	float* array() { return &x; }

	/**
	 * Array accessor (const version).
	 * 
	 * @returns const pointer to rotation component array
	 */
	const float* array() const { return &x; }
};

#endif // #ifndef _X3D_ROTATION_H_