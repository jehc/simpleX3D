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

#ifndef _X3D_MFFIELDS_H_
#define _X3D_MFFIELDS_H_

#include "internal/X3DField.h"
#include "internal/NodeIterator.h"
#include <list>
using std::list;

#define MAKE_MF(CONST,MF,SF,SFOBJ,KIND) \
class MF : public KIND<SF> { \
public:\
    typedef MF& TYPE; \
    typedef const MF& CONST_TYPE; \
    MF() : KIND<SF>() {} \
    INLINE X3DField::Type getType() const { return X3DField::CONST; } \
    static INLINE const MF& unwrap(const X3DField& value) { \
        if (value.getType() != X3DField::CONST) \
            throw X3DError(\
                string("base type mismatch; expected ") + \
                #MF + ", but was " + value.getTypeName()); \
        return static_cast<const MF&>(value); \
    } \
    static INLINE MF& unwrap(X3DField& value) { \
        if (value.getType() != X3DField::CONST) \
            throw X3DError(\
                string("base type mismatch; expected ") + \
                #MF + ", but was " + value.getTypeName()); \
        return static_cast<MF&>(value); \
    } \
    INLINE MF& operator()() { \
        return *this; \
    } \
    INLINE bool operator==(const X3DField& value) const { \
        return this->MFBase<SF>::operator==(unwrap(value)); \
    } \
    INLINE bool operator!=(const X3DField& value) const { \
        return this->MFBase<SF>::operator!=(unwrap(value)); \
    } \
    bool parse(istream& ss) { \
        SFOBJ x; \
        ss >> std::ws; \
        if (ss.peek() == ',') \
            return false; \
        while (true) { \
            ss >> std::ws; \
            if (ss.eof()) \
                break; \
            if (!x.parse(ss)) \
                return false; \
            add(x()); \
            ss >> std::ws; \
            if (ss.peek() == ',') \
                ss.get(); \
        } \
        return true; \
    } \
    void print(ostream& os) const { \
        list<SF>::const_iterator it; \
        for (it = elements.begin(); it != elements.end(); it++) \
            os << *it << ", "; \
    } \
};

namespace X3D {

/**
 * Base class for implementations of MF types.
 * 
 * Contains a list of elements. The two subclasses,
 * MFNative and MFReference, differ only in the parameters of
 * some of their accessor functions; MFReference uses const
 * reference types, while MFNative uses undecorated raw types.
 */
template <typename T>
class MFBase : public X3DField {
protected:
    /** internal list of elements */
    list<T> elements;

public:
    /** Empty constructor. */
    MFBase() : X3DField() {}

    /** Clear the elements list. */
    void clear() {
        elements.clear();
    }

    /**
     * Native comparison operator.
     * 
     * @param mf list to compare to
     * @returns whether lists are equal
     */
    bool operator==(const MFBase<T>& mf) const {
        return elements == mf.elements;
    }

    /**
     * Native comparison operator.
     * 
     * @param mf list to compare to
     * @returns whether lists are not equal
     */
    bool operator!=(const MFBase<T>& mf) const {
        return elements != mf.elements;
    }

    /**
     * Return the inner list of elements.
     */
    const list<T>& getElements() const {
        return elements;
    }

    /**
     * Return the inner list of elements.
     */
    list<T>& getElements() {
        return elements;
    }
};

/**
 * Base type for lists of complex data structures.
 * Uses a const reference type for accessors with parameters.
 */
template <class T>
class MFReference : public MFBase<T> {
public:
    /** Empty constructor. */
    MFReference() : MFBase<T>() {}

    /**
     * Add an element to the list.
     * 
     * @param elem element to add
     */
	void add(const T& elem) {
        MFBase<T>::elements.push_back(elem);
    }
};

/**
 * Baes type for lists of simple data types (including pointers).
 * Uses an undecorated basic type for accessors with parameters.
 */
template <typename T>
class MFNative : public MFBase<T> {
public:
    /** Empty constructor. */
    MFNative() : MFBase<T>() {}

    /**
     * Add an element to the list.
     * 
     * @param elem element to add.
     */
	void add(T elem) {
        MFBase<T>::elements.push_back(elem);
    }
};

/**
 * This abstract class exists so that you can access arbitrary
 * fields which you know to be MFNode<?>.
 */
class MFAbstractNode {
public:

    virtual void add(Node* node) = 0;
    virtual void clear() = 0;
    virtual NodeIterator nodes() = 0;
    virtual void begin(NodeIterator& iter) = 0;
    virtual void next(NodeIterator& iter) = 0;

    static const MFAbstractNode& unwrap(const X3DField& f) {
		if (f.getType() != X3DField::MFNODE)
			throw X3DError(
                string("base type mismatch; expected MFNode") + \
                ", but was " + f.getTypeName()); \
        const MFAbstractNode* mf = dynamic_cast<const MFAbstractNode*>(&f);
        if (mf == NULL)
            throw X3DError(
                string("list type mismatch; not a node list"));
        return *mf;
    }
    static MFAbstractNode& unwrap(X3DField& f) {
		if (f.getType() != X3DField::MFNODE)
			throw X3DError(
                string("base type mismatch; expected MFNode") + \
                ", but was " + f.getTypeName()); \
        MFAbstractNode* mf = dynamic_cast<MFAbstractNode*>(&f);
        if (mf == NULL)
            throw X3DError(
                string("list type mismatch; not a node list"));
        return *mf;
    }
};

/**
 * Templatized list of nodes. The template type identifies the root node
 * type contained in the list. This type may be abstract. When unwrapping
 * a generic value, the list type contained in the generic must match the
 * root node type exactly (no dynamic casting is performed).
 */
template <class N>
class MFNode : public MFNative<N*>, public MFAbstractNode {
public:
    typedef typename std::list<N*>::iterator ITER;
	typedef MFNode<N>& TYPE;
	typedef const MFNode<N>& CONST_TYPE;

    /** Empty constructor. */
    MFNode() : MFNative<N*>() {}

    /** @returns MFNODE */
	INLINE X3DField::Type getType() const { return X3DField::MFNODE; }

    /**
     * Unwrap a generic value into a list type. This method will
     * only succeed if the generic value is an MFNode list of the
     * exact same root node type.
     * 
     * @param f generic value
     * @returns native list type
     */
	static INLINE const MFNode<N>& unwrap(const X3DField& f) {
		if (f.getType() != X3DField::MFNODE)
			throw X3DError(
                string("base type mismatch; expected MFNode") + \
                ", but was " + f.getTypeName()); \
		const MFNode* mf = dynamic_cast<const MFNode<N>*>(&f);
        if (mf == NULL)
            throw X3DError("node type mismatch");
        return *mf;
	}

    /**
     * Unwrap a generic value into a list type. This method will
     * only succeed if the generic value is an MFNode list of the
     * exact same root node type.
     * 
     * @param f generic value
     * @returns native list type
     */
	static INLINE MFNode<N>& unwrap(X3DField& f) {
		if (f.getType() != X3DField::MFNODE)
			throw X3DError(
                string("base type mismatch; expected MFNode") + \
                ", but was " + f.getTypeName()); \
		const MFNode* mf = dynamic_cast<MFNode<N>*>(&f);
        if (mf == NULL)
            throw X3DError("node type mismatch");
        return *mf;
	}

    /** @returns native list type */
    INLINE MFNode<N>& operator()() {
        return *this;
    }

    void add(Node* node) {
        if (node == NULL) {
            MFBase<N*>::elements.push_back(NULL);
        } else {
            N* n = dynamic_cast<N*>(node);
            if (n == NULL)
                throw X3DError("node type mismatch");
            n->realize();
            MFBase<N*>::elements.push_back(n);
        }
    }

    void clear() {
        this->MFBase<N*>::elements.clear();
    }

    NodeIterator nodes() {
        return NodeIterator(*this);
    }

    void begin(NodeIterator& iter) {
        void* container = (void*) &(iter.iter);
        ITER* it = (ITER*) container;
        *it = this->MFBase<N*>::elements.begin();
        if (*it == this->MFBase<N*>::elements.end())
            iter.current = NULL;
        else
            iter.current = **it;
    }

    void next(NodeIterator& iter) {
        void* container = (void*) &(iter.iter);
        ITER* it = (ITER*) container;
        if (++(*it) == this->MFBase<N*>::elements.end())
            iter.current = NULL;
        else
            iter.current = **it;
    }

    bool parse(istream& ss) {
        SFNode<N> x;
        ss >> std::ws;
        if (ss.peek() == ',')
            return false;
        while (true) {
            ss >> std::ws;
            if (ss.eof())
                break;
            if (!x.parse(ss))
                return false;
            add(x());
            ss >> std::ws;
            if (ss.peek() == ',')
                ss.get();
        }
        return true;
    }

    void print(ostream& os) const {
        SFNode<N> sf;
        typename list<N*>::const_iterator it;
        for (   it = MFBase<N*>::elements.begin();
                it != MFBase<N*>::elements.end();
                it++) {
            sf = *it;
            os << sf << ", ";
        }
    }

    bool operator==(const X3DField& f) const {
        return this->MFBase<N*>::operator==(unwrap(f));
    }

    bool operator!=(const X3DField& f) const {
        return this->MFBase<N*>::operator!=(unwrap(f));
    }
};

MAKE_MF(MFBOOL,		MFBool,		bool,		SFBool,     MFNative)
MAKE_MF(MFCOLOR,	MFColor,	SFColor,	SFColor,    MFReference)
MAKE_MF(MFCOLORRGBA,MFColorRGBA,SFColorRGBA,SFColorRGBA,MFReference)
MAKE_MF(MFDOUBLE,	MFDouble,	double,		SFDouble,   MFNative)
MAKE_MF(MFFLOAT,	MFFloat,	float,		SFFloat,    MFNative)
MAKE_MF(MFIMAGE,	MFImage,	SFImage,	SFImage,    MFReference)
MAKE_MF(MFINT32,	MFInt32,	int,		SFInt32,    MFNative)
MAKE_MF(MFMATRIX3D,	MFMatrix3d,	SFMatrix3d,	SFMatrix3d, MFReference)
MAKE_MF(MFMATRIX3F,	MFMatrix3f,	SFMatrix3f,	SFMatrix3f, MFReference)
MAKE_MF(MFMATRIX4D,	MFMatrix4d,	SFMatrix4d,	SFMatrix4d, MFReference)
MAKE_MF(MFMATRIX4F,	MFMatrix4f,	SFMatrix4f,	SFMatrix4f, MFReference)
MAKE_MF(MFROTATION,	MFRotation,	SFRotation,	SFRotation, MFReference)
MAKE_MF(MFSTRING,	MFString,	std::string,SFString,   MFReference)
MAKE_MF(MFTIME,		MFTime,		double,		SFDouble,   MFNative)
MAKE_MF(MFVEC2D,	MFVec2d,	SFVec2d,	SFVec2d,    MFReference)
MAKE_MF(MFVEC2F,	MFVec2f,	SFVec2f,	SFVec2f,    MFReference)
MAKE_MF(MFVEC3D,	MFVec3d,	SFVec3d,	SFVec3d,    MFReference)
MAKE_MF(MFVEC3F,	MFVec3f,	SFVec3f,	SFVec3f,    MFReference)
MAKE_MF(MFVEC4D,	MFVec4d,	SFVec4d,	SFVec4d,    MFReference)
MAKE_MF(MFVEC4F,	MFVec4f,	SFVec4f,	SFVec4f,    MFReference)

}

#endif // #ifndef _X3D_MFFIELDS_H_
