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

#ifndef _X3D_BROWSER_H_
#define _X3D_BROWSER_H_

#include "types.h"
#include "profile.h"
#include "builtin.h"
#include <list>

using std::list;
using Core::X3DNode;

namespace X3D {

/**
 * This is the main X3D instance class. The browser controls
 * type registration, scene memory management (TODO), and
 * the execution model. It implements the X3D SAI (TODO).
 * 
 * Feel free to subclass the browser if you really need to
 * change how routing or memory management works, but keep
 * in mind that you should still attempt to support third-
 * party plugins.
 */
class Browser {
private:

	list<X3DNode*> nodes;

public:

	Profile* const profile;

	Browser() : profile(new Profile()) {
		Builtin::init(profile);
	}

	template <class N>
	N* createNode(const std::string& name) {
		NodeDefinition* def = profile->getNode(name);
		if (def == NULL)
			return NULL;
		X3DNode* node = def->create();
		if (node == NULL)
			return NULL;
		nodes.push_back(node);
		return node->cast<N>();
	}

	virtual ~Browser() {
		list<X3DNode*>::iterator it = nodes.begin();
		for (; it != nodes.end(); it++)
			delete *it;
		delete profile;
	}
};

}

#endif // #ifndef _X3D_BROWSER_H_
