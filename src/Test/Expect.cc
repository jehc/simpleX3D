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

#include "Test/Expect.h"
#include <sstream>

namespace X3D {
namespace Test {

Expect::Expect(
        TestNode* node, const string& type, const string& name, const string& value)
            : NodeField<TestNode>(node), name(name), actual(NULL) {
    expected = X3DField::create(type);
    std::stringstream ss(value);
    if (!expected->parse(ss))
        throw X3DError(string("invalid field value: ") + value);
}

bool Expect::test(string* reason) {
    std::stringstream ss;
    if (actual == NULL) {
        ss << name << " expected " << *expected << ", but was never activated";
    } else if (*expected != *actual) {
        ss << name << " expected " << *expected << ", but was actually " << *actual;
    } else {
        return true;
    }
    *reason = ss.str();
    return false;
}

Expect::~Expect() {
    delete expected;
}

X3DField::Type Expect::getType() const {
    return expected->getType();
}

string Expect::getTypeName() const {
    return expected->getTypeName();
}

SAIField::Access Expect::getAccess() const {
    return SAIField::INPUT_ONLY;
}

// unlike in-only, we have a value from the start
const X3DField& Expect::get() const {
    return *expected;
}

// unlike in-only, we have a value from the start
X3DField& Expect::get() {
    return *expected;
}

void Expect::set(const X3DField& value) {
    actual = &value;
}

bool Expect::isDirty() const {
    return false;
}

void Expect::clearDirty() {
}

void Expect::addIncomingRoute(Route* route) {
    routes.push_back(route);
}
void Expect::removeIncomingRoute(Route* route) {
    routes.remove(route);
}
const list<Route*>& Expect::getIncomingRoutes() const {
    return routes;
}

}}