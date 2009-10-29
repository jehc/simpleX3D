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

#ifndef _X3D_INITFIELD_H_
#define _X3D_INITFIELD_H_

#include "internal/SAIField.h"

namespace X3D {

template <class N, class TT>
class InitField : public BaseField<N,TT> {
private:
    typedef typename TT::TYPE T;
protected:
    inline N* node() const { return NodeField<N>::node; }
public:
    TT value;
    InitField() {}
    InitField(T init) : BaseField<N,TT>(), value(init) {}
    inline const TT& get() const {
        if (node()->realized())
            throw X3DError("wrong stage");
        return value;
    }
    inline void set(const X3DField& value) {
        if (node()->realized())
            throw X3DError("wrong stage");
        this->value = value;
    }
    inline T operator()() const {
        if (node()->realized())
            throw X3DError("wrong stage");
        return value();
    }
    inline void operator()(T x) {
        if (node()->realized())
            throw X3DError("wrong stage");
        value = x;
    }
    void route() {}
};

}

#endif // #ifndef _X3D_INITFIELD_H_