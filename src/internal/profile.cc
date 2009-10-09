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

#include "internal/types.h"
#include "internal/profile.h"
#include <iostream>

using std::cout;
using std::endl;
using X3D::Core::X3DNode;

namespace X3D {

NodeDefinition::~NodeDefinition() {
	vector<Field*>::iterator it = fields.begin();
	for (; it != fields.end(); it++)
		delete *it;
}

void NodeDefinition::inherits(const string& name) {
	parent = component->getNode(name);
	if (parent == NULL)
		throw X3DError("can't set nonexistent parent: " + name);
}

void NodeDefinition::print(bool full) {
	cout << "\t" << name;
	if (parent != NULL)
		cout << " : " << parent->name;
	cout << " {" << endl;
	print_fields(full);
	cout << "\t}" << endl;
}

void NodeDefinition::print_fields(bool full) {
	if (full && (parent != NULL))
		parent->print_fields(full);
	vector<Field*>::iterator it = fields.begin();
	for (; it != fields.end(); it++)
		(*it)->print();
}

SafePointer NodeDefinition::get(const X3DNode* node, const string& field_name) const {
	map<string,OutField*>* fields = const_cast<map<string,OutField*>*>(&out_fields);
	const OutField* field = (*fields)[field_name];
	if (field == NULL)
		throw X3DError("invalid field");
	return field->get(node);
}

void NodeDefinition::set(X3DNode* node, const string& field_name, const SafePointer& value, bool quiet) const {
	map<string,InField*>* fields = const_cast<map<string,InField*>*>(&in_fields);
	const InField* field = (*fields)[field_name];
	if (field == NULL)
		throw X3DError("invalid field");
	field->set(node, value);
	if (!quiet)
		changed(node, field_name);
}

void NodeDefinition::changed(const X3DNode* node, const string& field_name) const {
	// TODO: routing system goes here
}

void NodeDefinition::addInitField(InitField* field) {
	init_fields[field->name] = field;
	fields.push_back(field);
}

void NodeDefinition::addInField(InField* field) {
	in_fields[field->name] = field;
	fields.push_back(field);
}

void NodeDefinition::addOutField(OutField* field) {
	out_fields[field->name] = field;
	fields.push_back(field);
}

void NodeDefinition::addInOutField(InOutField* field) {
	inout_fields[field->name] = field;
	fields.push_back(field);
}

Component::~Component() {
	vector<NodeDefinition*>::iterator it = node_list.begin();
	for (; it != node_list.end(); it++)
		delete *it;
}

NodeDefinition* Component::getNode(const string& name) {
	return node_map[name];
}

void Component::print() {
	cout << "COMPONENT " << name << " {" << endl;
	vector<NodeDefinition*>::iterator it = node_list.begin();
	for (; it != node_list.end(); it++)
		(*it)->print();
	cout << "}" << endl;
}

Profile::~Profile() {
	vector<Component*>::iterator it = comp_list.begin();
	for (; it != comp_list.end(); it++)
		delete *it;
}

NodeDefinition* Profile::getNode(const string& name) {
	vector<Component*>::iterator it = comp_list.begin();
	for (; it != comp_list.end(); it++) {
		NodeDefinition* def = (*it)->getNode(name);
		if (def != NULL)
			return def;
	}
	return NULL;
}

Component* Profile::createComponent(const string& name) {
	Component* comp = new Component(name);
	comp_map[name] = comp;
	comp_list.push_back(comp);
	return comp;
}

void Profile::print() {
	cout << "PROFILE" << endl;
	vector<Component*>::iterator it = comp_list.begin();
	for (; it != comp_list.end(); it++)
		(*it)->print();
}

}