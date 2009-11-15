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

#include "internal/Node.h"
#include "internal/NodeDef.h"
#include "internal/Profile.h"
#include "internal/builtin.h"
#include <list>

using std::list;

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

	/// all nodes managed by the browser
	list<Node*> nodes;

	/// nodes we shouldn't garbage-collect
	list<Node*> persistent;

    /// nodes which generate events asynchronously
    list<Node*> sources;

	/// root scene nodes
	list<Node*> roots;

    /// nodes which need to be routed
    list<Node*> dirtyNodes;

    /// nodes which should be cleared
    list<SAIField*> firedFields;

    /// named nodes
    map<string, Node*> defs;

	/// singleton instance
	static Browser* _inst;

public:

	/// profile supported by the browser
	Profile* const profile;

	/// Default constructor. Initializes profile.
	Browser();

	/// Default destructor. Cleans up memory.
	virtual ~Browser();

	/**
	 * Create a new node instance of the given qualified
	 * name. The actual type of the node returned will be
	 * determined by the profile, so that plugins can register
	 * their own X3D types.
	 * 
	 * The node returned is managed by the browser automatically.
	 * If you want the node to persist, you must either add the
	 * node to the scene graph as a root node or as a field of
	 * a live node, or you can explicitly tell the browser to
	 * persist the node by calling #persist.
	 * 
	 * @param name qualified X3D node type name
	 * @returns an instance of the registered node type
	 */
	Node* createNode(const std::string& name);

    /**
     * Process routes in a cascade. This function will block until
     * the cascade is complete.
     */
    void route();

	/**
	 * Make the given node persistent, so that it will not be
	 * garbage collecected (until the browser shuts down).
	 * 
	 * @param node pointer to node to persist
	 */
	void persist(Node* node);

    /**
     * Make the given node a "source", meaning that we expect it to
     * generate events asynchronously.
     * 
     * @param node node which will generate events
     */
    void addSource(Node* node);

    /**
     * Add a named node mapping.
     * 
     * Field, event, PROTO, EXTERNPROTO, and node names shall not contain control
     * characters (0x0-0x1f, 0x7f), space (0x20), double or single quotes
     * (0x22: ", 0x27: '), sharp (0x23: #), comma (0x2c: ,), period (0x2e: .),
     * brackets (0x5b, 0x5d: []), backslash (0x5c: \) or braces (0x7b, 0x7d: {}).
     * Further, their first character shall not be a digit (0x30-0x39), plus
     * (0x2b: +), or minus (0x2d: -) character. Otherwise, names may contain any
     * ISO 10646 character encoded using UTF-8. X3D is case-sensitive; "Sphere"
     * is different from "sphere" and "BEGIN" is different from "begin."
     * 
     * @param name name of node
     * @param node node to map to
     */
    void addNamedNode(const string& name, Node* node);

    /**
     * Look up a node by its name, returning NULL if it isn't found.
     * 
     * @param name stored node name
     * @returns stored node
     */
    Node* getNodeByName(const string& name);

	/**
	 * Templatized node creation. This version of createNode
	 * will use the constructor of the actual template type,
	 * so that even if a subclass has been registered for the
	 * named X3D type, the base class can still be instantiated.
	 * 
	 * @param name qualified X3D node type name
	 * @returns an instance of the given (template) node type
	 */
	template <class N>
	N* createNode(const std::string& name) {
		NodeDef* def = profile->getNode(name);
		if (def == NULL)
            throw X3DError("no such node " + name);
		N* node = def->create<N>();
		if (node == NULL)
            throw X3DError("node creation failed");
		nodes.push_back(node);
        if (node->eventSource())
            addSource(node);
		return node;
	}

	/**
	 * Singleton access.
	 * 
	 * @returns singleton instance
	 */
	static Browser* getSingleton() {
		return _inst;
	}

    /**
     * Create a route between nodes based on field names. If such a route already
     * exists, this function will return the existing route. Otherwise, it will
     * create a new route and hook it up to the fields, then return it.
     * 
     * @param fromNode node to route from
     * @param fromField name of field to route from
     * @param toNode node to route to
     * @param toField name of field to route to
     * @returns new or existing route between fields
     */
    Route* createRoute(Node* fromNode, const string& fromField,
                       Node* toNode, const string& toField) const;

    /**
     * Create a route between nodes based on their field pointers. If such a
     * route already exists, this function will return the existing route. Otherwise,
     * it will create a new route and hook it up to the fields, then return it.
     * 
     * @param fromField field object to route from
     * @param toField field object to route to
     * @returns new or existing route between fields
     */
    Route* createRoute(SAIField* fromField, SAIField* toField) const;

private:

    /**
     * Route events from the given field, which should be marked dirty.
     * 
     * @param field field to route events from
     */
    void routeFrom(SAIField* field);

    /**
     * Add a (potentially) dirty node to the list of
     * nodes to route from.
     * 
     * @param node node to route from
     */
    void addDirtyNode(Node* node);

};

}

#endif // #ifndef _X3D_BROWSER_H_
