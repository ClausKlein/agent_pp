/*_############################################################################
 * _##
 * _##  AGENT++ 4.5 - avl_map.h
 * _##
 * _##  Copyright (C) 2000-2021  Frank Fock and Jochen Katz (agentpp.com)
 * _##
 * _##  Licensed under the Apache License, Version 2.0 (the "License");
 * _##  you may not use this file except in compliance with the License.
 * _##  You may obtain a copy of the License at
 * _##
 * _##      http://www.apache.org/licenses/LICENSE-2.0
 * _##
 * _##  Unless required by applicable law or agreed to in writing, software
 * _##  distributed under the License is distributed on an "AS IS" BASIS,
 * _##  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * _##  See the License for the specific language governing permissions and
 * _##  limitations under the License.
 * _##
 * _##########################################################################*/
// This may look like C code, but it is really -*- C++ -*-

/*
 * Copyright (C) 1988 Free Software Foundation
 *  written by Doug Lea (dl@rocky.oswego.edu)
 *
 * This file is part of the GNU C++ Library.  This library is free
 * software; you can redistribute it and/or modify it under the terms of
 * the GNU Library General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your
 * option) any later version.  This library is distributed in the hope
 * that it will be useful, but WITHOUT ANY WARRANTY; without even the
 * implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU Library General Public License for more details.
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef _OidxPtrEntryPtrAVLMap_h
#define _OidxPtrEntryPtrAVLMap_h 1

#include <agent_pp/map.h>

#ifdef AGENTPP_NAMESPACE
namespace Agentpp
{
using namespace Snmp_pp;
#endif

// NOLINTBEGIN

struct OidxPtrEntryPtrAVLNode {
    OidxPtrEntryPtrAVLNode* lt;
    OidxPtrEntryPtrAVLNode* rt;
    OidxPtr                 item;
    EntryPtr                cont;
    char                    stat;
    OidxPtrEntryPtrAVLNode(OidxPtr h, EntryPtr c, OidxPtrEntryPtrAVLNode* l = nullptr,
        OidxPtrEntryPtrAVLNode* r = nullptr);
    ~OidxPtrEntryPtrAVLNode() { }
};

inline OidxPtrEntryPtrAVLNode::OidxPtrEntryPtrAVLNode(
    OidxPtr h, EntryPtr c, OidxPtrEntryPtrAVLNode* l, OidxPtrEntryPtrAVLNode* r)
    : lt(l), rt(r), item(h), cont(c), stat(0)
{ }

typedef OidxPtrEntryPtrAVLNode* OidxPtrEntryPtrAVLNodePtr;

class AGENTPP_DECL OidxPtrEntryPtrAVLMap : public OidxPtrEntryPtrMap {
protected:
    OidxPtrEntryPtrAVLNode* root;

    OidxPtrEntryPtrAVLNode* leftmost() const;
    OidxPtrEntryPtrAVLNode* rightmost() const;
    OidxPtrEntryPtrAVLNode* pred(OidxPtrEntryPtrAVLNode* t) const;
    OidxPtrEntryPtrAVLNode* succ(OidxPtrEntryPtrAVLNode* t) const;
    void                    _kill(OidxPtrEntryPtrAVLNode* t);
    void                    _add(OidxPtrEntryPtrAVLNode*& t);
    void                    _del(OidxPtrEntryPtrAVLNode* p, OidxPtrEntryPtrAVLNode*& t);

    // state information stored per instance to
    // allow independent use of separate instances in separate threads without
    // interference.
    bool                    _need_rebalancing {}; // to send back balance info from rec. calls
    OidxPtr*                _target_item {};      // add/del_item target
    OidxPtrEntryPtrAVLNode* _found_node {};       // returned added/deleted node
    bool                    _already_found {};    // for deletion subcases

public:
    OidxPtrEntryPtrAVLMap(EntryPtr deflt);
    OidxPtrEntryPtrAVLMap(OidxPtrEntryPtrAVLMap& a);
    inline ~OidxPtrEntryPtrAVLMap() override;

    EntryPtr& operator[](OidxPtr key) override;

    void del(OidxPtr key) override;

    inline Pix       first() const override;
    inline void      next(Pix& i) const override;
    inline OidxPtr&  key(Pix i) const override;
    inline EntryPtr& contents(Pix i) override;

    Pix         seek(OidxPtr key) const override;
    Pix         seek_inexact(OidxPtr key) const;
    inline bool contains(OidxPtr key_) const override;

    inline void clear() override;

    Pix  last() const;
    void prev(Pix& i) const;

    bool OK() override;
};

inline OidxPtrEntryPtrAVLMap::~OidxPtrEntryPtrAVLMap() { _kill(root); }

inline OidxPtrEntryPtrAVLMap::OidxPtrEntryPtrAVLMap(EntryPtr deflt) : OidxPtrEntryPtrMap(deflt)
{
    root = nullptr;
}

inline Pix OidxPtrEntryPtrAVLMap::first() const { return Pix(leftmost()); }

inline Pix OidxPtrEntryPtrAVLMap::last() const { return Pix(rightmost()); }

inline void OidxPtrEntryPtrAVLMap::next(Pix& i) const
{
    if (i != nullptr)
    {
        i = Pix(succ((OidxPtrEntryPtrAVLNode*)i));
    }
}

inline void OidxPtrEntryPtrAVLMap::prev(Pix& i) const
{
    if (i != nullptr)
    {
        i = Pix(pred((OidxPtrEntryPtrAVLNode*)i));
    }
}

inline OidxPtr& OidxPtrEntryPtrAVLMap::key(Pix i) const
{
    if (i == nullptr)
    {
        error("null Pix"); // FIXME: Warning C6011 Dereferencing NULL pointer
    }
    return ((OidxPtrEntryPtrAVLNode*)i)->item;
}

inline EntryPtr& OidxPtrEntryPtrAVLMap::contents(Pix i)
{
    if (i == nullptr)
    {
        error("null Pix"); // FIXME: Warning C6011 Dereferencing NULL pointer
    }
    return ((OidxPtrEntryPtrAVLNode*)i)->cont;
}

inline void OidxPtrEntryPtrAVLMap::clear()
{
    _kill(root);
    count = 0;
    root  = nullptr;
}

inline bool OidxPtrEntryPtrAVLMap::contains(OidxPtr key_) const { return seek(key_) != nullptr; }

// NOLINTEND

#ifdef AGENTPP_NAMESPACE
}
#endif
#endif
