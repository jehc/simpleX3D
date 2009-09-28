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

#ifndef _X3D_METADATAFLOAT_H_
#define _X3D_METADATAFLOAT_H_

#include "types.h"
#include "Core/X3DMetadataObject.h"

namespace X3D {
namespace Core {

/** Metadata value containing a list of floats (MFFloat). */
class MetadataFloat : public X3DNode, public X3DMetadataObject {
private:
	MFFloat _value;

public:
	const InOutField<MetadataFloat, MFFloat> value;

	MetadataFloat(const SFString& name, const MFFloat& value) :
		X3DNode(),
		X3DMetadataObject(name),
		_value(value),
		value(this, &_value)
		{}

	MetadataFloat(const SFString& name, const SFString& ref, const MFFloat& value) :
		X3DNode(),
		X3DMetadataObject(name, ref),
		_value(value),
		value(this, &_value)
		{}
};

}} // namespace X3D::Core

#endif // #ifndef _X3D_METADATAFLOAT_H_
