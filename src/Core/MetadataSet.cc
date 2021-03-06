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

#include "internal/Browser.h"
#include "Core/MetadataString.h"
#include "Core/MetadataSet.h"

namespace X3D {
namespace Core {

void MetadataSet::assignFromMap(const map<string,string>& meta) {
	map<string,string>::const_iterator it = meta.begin();
	for (; it != meta.end(); it++) {
		X3DMetadataObject* entry = browser()->createNode<MetadataString>("MetadataString");
		addMetadata(entry);
		entry->name(it->first);
		entry->assignFromString(it->second);
	}
}

void MetadataSet::addMetadata(X3DMetadataObject* entry) {
	value().add(entry);
	value.changed();
}

}}
