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
#include "internal/FieldIterator.h"

namespace X3D {

Browser* Browser::_inst;

Browser::Browser() : profile(new Profile()) {
	if (_inst == NULL)
		_inst = this;
	else
		throw X3DError("multiple browser instances!");
	Builtin::init(profile);
}

Browser::~Browser() {
	list<Node*>::iterator it = nodes.begin();
	for (; it != nodes.end(); it++)
		delete *it;
	delete profile;
}

Node* Browser::createNode(const std::string& name) {
	NodeDef* def = profile->getNode(name);
	if (def == NULL)
		return NULL;
	Node* node = def->create();
	if (node == NULL)
		return NULL;
	nodes.push_back(node);
	return node;
}

void Browser::route() {
    list<Node*>::iterator it = sources.begin();
    for (; it != sources.end(); it++) {
        FieldIterator fields = (*it)->fields(FieldIterator::DIRTY);
        while (fields.hasNext())
            routeFrom(fields.nextField());
    }
}

void Browser::routeFrom(SAIField* field) {
    // TODO
}

void Browser::persist(Node* node) {
	persistent.push_back(node);
}

}
