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
#include "internal/Route.h"

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
    // route until cascade is done
    while (!dirtyFields.empty()) {
        SAIField* field = dirtyFields.front();
        dirtyFields.pop_front();
        fromFrom(field);
    }
    // finally, clear all dirty flags
    list<SAIField*>::iterator it;
    for (it = firedFields.begin(); it != firedFields.end(); it++)
        (*it)->clearDirty();
    firedFields.clear();
}

void Browser::addDirtyField(SAIField* field) {
    dirtyFields.push_back(field);
}

void Browser::routeFrom(SAIField* field) {
    const list<Route*>& routes = field->getOutgoingRoutes();
    list<Route*>::const_iterator it;
    for (it = routes.begin(); it != routes.end(); it++)
        (*it)->activate();
    firedFields.push_back(field);
}

void Browser::persist(Node* node) {
	persistent.push_back(node);
}

Route* Browser::createRoute(Node* fromNode, const string& fromFieldName,
                            Node* toNode, const string& toFieldName) const {
    SAIField* fromField = fromNode->getField(fromFieldName);
    if (fromField == NULL)
        throw X3DError("couldn't find source field");
    SAIField* toField = toNode->getField(toFieldName);
    if (toField == NULL)
        throw X3DError("couldn't find target field");
    return createRoute(fromField, toField);
}

Route* Browser::createRoute(SAIField* fromField, SAIField* toField) const {
    const list<Route*>& routes = fromField->getOutgoingRoutes();
    list<Route*>::const_iterator it;
    for (it = routes.begin(); it != routes.end(); it++)
        if ((*it)->toField == toField)
            return *it;
    Route* route = new Route(fromField, toField);
    fromField->addOutgoingRoute(route);
    toField->addIncomingRoute(route);
    return route;
}

void Browser::addNamedNode(const string& name, Node* node) {
    defs[name] = node;
}

Node* Browser::getNodeByName(const string& name) {
    if (defs.count(name))
        return defs[name];
    return NULL;
}

}
