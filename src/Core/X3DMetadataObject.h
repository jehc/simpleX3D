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

#ifndef _X3D_X3DMETADATAOBJECT_H_
#define _X3D_X3DMETADATAOBJECT_H_

#include "types.h"

namespace X3D {
namespace Core {

/**
 * This abstract type is the parent for any node
 * which provides a metadata value. The node must
 * provide both a name and value.
 */
class X3DMetadataObject : public X3DNode {
private:
	SFString _name;
	SFString _reference;

public:
	const InOutField<X3DMetadataObject, SFString> name;
	const InOutField<X3DMetadataObject, SFString> reference;

	X3DMetadataObject(const SFString& name, const SFString& ref="") :
		_name(name),
		_reference(ref),
		name(this, &_name),
		reference(this, &_reference)
		{}
};

}}

#endif // #ifndef _X3D_X3DMETADATAOBJECT_H_
