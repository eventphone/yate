/**
 * ObjList.cpp
 * This file is part of the YATE Project http://YATE.null.ro
 *
 * Yet Another Telephony Engine - a fully featured software PBX and IVR
 * Copyright (C) 2004-2023 Null Team
 *
 * This software is distributed under multiple licenses;
 * see the COPYING file in the main directory for licensing
 * information for this specific distribution.
 *
 * This use of this software may be subject to additional restrictions.
 * See the LEGAL file in the main directory for details.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */

#include "yateclass.h"
#include <string.h>

using namespace TelEngine;

static const ObjList s_empty;

const ObjList& ObjList::empty()
{
    return s_empty;
}

ObjList::ObjList()
    : m_next(0), m_obj(0), m_delete(true)
{
    XDebug(DebugAll,"ObjList::ObjList() [%p]",this);
}

ObjList::~ObjList()
{
    XDebug(DebugAll,"ObjList::~ObjList() [%p]",this);
    clear();
}

void* ObjList::getObject(const String& name) const
{
    if (name == YATOM("ObjList"))
	return const_cast<ObjList*>(this);
    return GenObject::getObject(name);
}

unsigned int ObjList::length() const
{
    unsigned int c = 0;
    const ObjList *n = this;
    while (n) {
	c++;
	n = n->next();
    }
    return c;
}

unsigned int ObjList::count() const
{
    unsigned int c = 0;
    const ObjList *n = this;
    while (n) {
	if (n->get())
	    c++;
	n = n->next();
    }
    return c;
}

ObjList* ObjList::last() const
{
    const ObjList *n = this;
    while (n->next())
	n = n->next();
    return const_cast<ObjList*>(n);
}

ObjList* ObjList::skipNull() const
{
    const ObjList *n = this;
    while (n && !n->get())
	n = n->next();
    return const_cast<ObjList*>(n);
}

ObjList* ObjList::skipNext() const
{
    const ObjList *n = this;
    while (n) {
	n = n->next();
	if (n && n->get())
	    break;
    }
    return const_cast<ObjList*>(n);
}

ObjList* ObjList::operator+(int index) const
{
    if (index < 0)
	return 0;
    ObjList *obj = const_cast<ObjList*>(this);
    for (;obj;obj=obj->next(),index--)
	if (!index) break;
    return obj;
}

GenObject* ObjList::at(int index) const
{
    ObjList *obj = operator+(index);
    return obj ? obj->get() : 0;
}

ObjList* ObjList::find(const GenObject* obj) const
{
    XDebug(DebugAll,"ObjList::find(%p) [%p]",obj,this);
    const ObjList *n = this;
    while (n && (n->get() != obj))
	n = n->next();
    XDebug(DebugInfo,"ObjList::find returning %p",n);
    return const_cast<ObjList*>(n);
}

ObjList* ObjList::find(const String& str) const
{
    XDebug(DebugAll,"ObjList::find(\"%s\") [%p]",str.c_str(),this);
    const ObjList *n = skipNull();
    while (n) {
	if (str.matches(n->get()->toString()))
	    break;
	n = n->skipNext();
    }
    XDebug(DebugInfo,"ObjList::find returning %p",n);
    return const_cast<ObjList*>(n);
}

int ObjList::index(const GenObject* obj) const
{
    if (!obj)
	return -1;
    int idx = 0;
    for (const ObjList* n = this; n; n = n->next(), idx++)
	if (n->get() == obj)
	    return idx;
    return -1;
}

int ObjList::index(const String& str) const
{
    int idx = 0;
    for (const ObjList* n = this; n; n = n->next(), idx++)
	if (n->get() && str.matches(n->get()->toString()))
	    return idx;
    return -1;
}

GenObject* ObjList::set(const GenObject* obj, bool delold)
{
    if (m_obj == obj)
	return 0;
    GenObject *tmp = m_obj;
    m_obj = const_cast<GenObject*>(obj);
    if (delold && tmp) {
	tmp->destruct();
	return 0;
    }
    return tmp;
}

ObjList* ObjList::insert(const GenObject* obj, bool compact)
{
#ifdef XDEBUG
    Debugger debug("ObjList::insert","(%p,%d) [%p]",obj,compact,this);
#endif
    if (m_obj || !compact) {
	ObjList *n = new ObjList();
	n->set(m_obj);
	set(obj,false);
	n->m_delete = m_delete;
	n->m_next = m_next;
	m_delete = true;
	m_next = n;
    }
    else
	m_obj = const_cast<GenObject*>(obj);
    return this;
}

ObjList* ObjList::append(const GenObject* obj, bool compact)
{
#ifdef XDEBUG
    Debugger debug("ObjList::append","(%p,%d) [%p]",obj,compact,this);
#endif
    ObjList *n = last();
    if (n->get() || !compact) {
	n->m_next = new ObjList();
	n = n->m_next;
    }
    else
	n->m_delete = true;
    n->set(obj);
    return n;
}

ObjList* ObjList::setUnique(const GenObject* obj, bool compact)
{
    XDebug(DebugAll,"ObjList::setUnique(\"%p\") [%p]",obj,this);
    if (!obj)
	return 0;
    const String& name = obj->toString();
    ObjList* o = skipNull();
    while (o) {
	if (name.matches(o->get()->toString())) {
	    o->set(obj);
	    return const_cast<ObjList*>(o);
	}
	ObjList* n = o->skipNext();
	if (n)
	    o = n;
	else
	    break;
    }
    if (o)
	o = o->append(obj,compact);
    else
	o = append(obj,compact);
    return const_cast<ObjList*>(o);
}

GenObject* ObjList::remove(bool delobj)
{
    GenObject *tmp = m_obj;

    if (m_next) {
	ObjList *n = m_next;
	m_next = n->next();
	m_obj = n->get();
	m_delete = n->m_delete;
	n->m_obj = 0;
	n->m_next = 0;
	n->destruct();
    }
    else
	m_obj = 0;

    if (delobj && tmp) {
	XDebug(DebugInfo,"ObjList::remove() deleting %p",tmp);
	// Don't use TelEngine::destruct(): the compiler will call the non-template
	// function (which doesn't reset the pointer)
	tmp->destruct();
	tmp = 0;
    }
    return tmp;
}

GenObject* ObjList::remove(GenObject* obj, bool delobj)
{
    ObjList *n = find(obj);
    return n ? n->remove(delobj) : 0;
}

GenObject* ObjList::remove(const String& str, bool delobj)
{
    ObjList *n = find(str);
    return n ? n->remove(delobj) : 0;
}

void ObjList::clear()
{
#ifdef XDEBUG
    Debugger debug("ObjList::clear()"," [%p]",this);
#endif
    while (m_obj)
	remove(m_delete);
    ObjList *n = m_next;
    m_next = 0;
    TelEngine::destruct(n);
}

static inline ObjList* skipLastEmpty(ObjList* list)
{
    ObjList* l = 0;
    for (; list; list = list->next()) {
	if (list->get())
	    break;
	l = list;
    }
    return l;
}

// Remove all empty objects in the list
void ObjList::compact()
{
    if (!m_next)
	return;
    // Compact list head
    if (!get()) {
	ObjList* lastEmpty = skipLastEmpty(this);
	if (!lastEmpty->next()) {
	    clear();
	    return;
	}
	ObjList* nonEmpty = lastEmpty->next();
	ObjList* tmp = m_next;
	m_next = nonEmpty->next();
	m_obj = nonEmpty->get();
	m_delete = nonEmpty->m_delete;
	nonEmpty->m_obj = 0;
	nonEmpty->m_next = 0;
	tmp->destruct();
    }
    ObjList* last = this;
    while (last->next()) {
	// Find last non empty item
	for (ObjList* o = last->next(); o; o = o->next()) {
	    if (o->get())
		last = o;
	    else
		break;
	}
	if (!last->next())
	    break;
	// Next item after last is empty, find the last empty
	ObjList* lastEmpty = skipLastEmpty(last->next());
	if (!lastEmpty->next())
	    break;
	ObjList* firstNonEmpty = lastEmpty->next();
	lastEmpty->m_next = 0;
	ObjList* tmp = last->m_next;
	last->m_next = firstNonEmpty;
	last = firstNonEmpty;
	tmp->destruct();
    }
    if (last && last->next()) {
	ObjList* tmp = last->next();
	last->m_next = 0;
	tmp->destruct();
    }
}

GenObject* ObjList::find(Lockable& lock, const GenObject* obj, bool ref, long maxwait) const
{
    Lock lck(lock,maxwait,true);
    ObjList* o = find(obj);
    if (!o)
	return 0;
    if (ref) {
	RefObject* r = YOBJECT(RefObject,o->get());
	if (!(r && r->ref()))
	    return 0;
    }
    return o->get();
}

GenObject* ObjList::find(Lockable& lock, const String& str, bool ref, long maxwait) const
{
    Lock lck(lock,maxwait,true);
    ObjList* o = find(str);
    if (!o)
	return 0;
    if (ref) {
	RefObject* r = YOBJECT(RefObject,o->get());
	if (!(r && r->ref()))
	    return 0;
    }
    return o->get();
}

ObjList* ObjList::insert(Lockable& lock, const GenObject* obj, bool autoDelete, long maxwait,
    bool compact)
{
    Lock lck(lock,maxwait,false);
    ObjList* o = insert(obj,compact);
    if (o)
	o->setDelete(autoDelete);
    return o;
}

ObjList* ObjList::append(Lockable& lock, const GenObject* obj, bool autoDelete, long maxwait,
    bool compact)
{
    Lock lck(lock,maxwait,false);
    ObjList* o = append(obj,compact);
    if (o)
	o->setDelete(autoDelete);
    return o;
}

ObjList* ObjList::setUnique(Lockable& lock, const GenObject* obj, bool autoDelete, long maxwait,
    bool compact)
{
    Lock lck(lock,maxwait,false);
    ObjList* o = setUnique(obj,compact);
    if (o)
	o->setDelete(autoDelete);
    return o;
}

GenObject* ObjList::remove(Lockable& lock, bool delobj, long maxwait)
{
    Lock lck(lock,maxwait,false);
    return remove(delobj);
}

GenObject* ObjList::remove(Lockable& lock, GenObject* obj, bool delobj, long maxwait)
{
    Lock lck(lock,maxwait,false);
    return remove(obj,delobj);
}

GenObject* ObjList::remove(Lockable& lock, const String& str, bool delobj, long maxwait)
{
    Lock lck(lock,maxwait,false);
    return remove(str,delobj);
}

void ObjList::clear(Lockable& lock, long maxwait)
{
    Lock lck(lock,maxwait,false);
    clear();
}

void ObjList::compact(Lockable& lock, long maxwait)
{
    Lock lck(lock,maxwait,false);
    compact();
}

ObjList* ObjList::move(ObjList* dest, Lockable* lock, long maxwait)
{
    if (!dest)
	dest = new ObjList;
    ObjList* add = dest;
    Lock lck(lock,maxwait,false);
    for (ObjList* o = skipNull(); o; o = o->skipNull()) {
	bool del = o->autoDelete();
	add = add->append(o->remove(false));
	add->setDelete(del);
    }
    return dest;
}

ObjList* ObjList::copy(ObjList* dest, Lockable* lock, long maxwait) const
{
    if (!dest)
	dest = new ObjList;
    ObjList* add = dest;
    Lock lck(lock,maxwait,true);
    for (ObjList* o = skipNull(); o; o = o->skipNext()) {
	RefObject* r = YOBJECT(RefObject,o->get());
	if (r && r->ref())
	    add = add->append(r);
    }
    return dest;
}

static void merge(ObjList* first, ObjList* second,int (*compare)(GenObject* obj1, GenObject* obj2, void* data), void* dat)
{
    if (!(first && second))
	return;
    bool del = true;
    if (!first->skipNull()) {
	while (second->skipNull()) {
	    del = second->autoDelete();
	    first->append(second->remove(false))->setDelete(del);
	}
	return;
    }
    ObjList* head = first->skipNull();
    GenObject* current = head->get();
    while (second->skipNull()) {
	del = second->autoDelete();
	GenObject* next = second->remove(false);
	while (current && compare(current,next,dat) < 1) {
	    if (!head->skipNext()) {
		current = 0;
		break;
	    }
	    head = head->skipNext();
	    current = head->get();
	}
	if (!current) {
	    first->append(next)->setDelete(del);
	    continue;
	}
	head->insert(next)->setDelete(del);
	head = head->skipNext();
    }
}

static void splitList(ObjList& list, ObjList& splits, int (*compare)(GenObject* obj1, GenObject* obj2, void* data), void* data)
{
    if (!list.skipNull())
	return;
    ObjList* slice = new ObjList();
    splits.append(slice);
    bool autodel = list.autoDelete();
    GenObject* last = list.remove(false);
    slice->append(last)->setDelete(autodel);
    while (list.skipNull()) {
	autodel = list.autoDelete();
	GenObject* next = list.remove(false);
	if (compare(last,next,data) < 1) {
	    slice->append(next)->setDelete(autodel);
	    last = next;
	    continue;
	}
	slice = new ObjList();
	slice->append(next)->setDelete(autodel);
	splits.append(slice);
	last = next;
    }
}

void ObjList::sort(int (*callbackCompare)(GenObject* obj1, GenObject* obj2, void* data), void* data)
{
    if (!callbackCompare) {
	Debug(DebugNote,"ObjList::sort called without callback method!");
	return;
    }
    ObjList splits;
    splitList(*this,splits,callbackCompare,data);
    while (splits.skipNull()) {
	ObjList* first = this;
	for (ObjList* o = splits.skipNull();o;o = o->skipNext()) {
	    ObjList* second = static_cast<ObjList*>(o->get());
	    merge(first,second,callbackCompare,data);
	    o->remove();
	    o = o->skipNull();
	    if (!o)
		break;
	    first = static_cast<ObjList*>(o->get());
	}
    }
}

ObjVector::ObjVector(unsigned int maxLen, bool autodelete)
    : m_length(maxLen), m_objects(0), m_delete(autodelete)
{
    XDebug(DebugAll,"ObjVector::ObjVector(%u,%s) [%p]",
	maxLen,String::boolText(autodelete),this);
    if (maxLen) {
	m_objects = new GenObject*[maxLen];
	for (unsigned int i = 0; i < maxLen; i++)
	    m_objects[i] = 0;
    }
}

ObjVector::ObjVector(ObjList& list, bool move, unsigned int maxLen, bool autodelete)
    : m_length(0), m_objects(0), m_delete(autodelete)
{
    XDebug(DebugAll,"ObjVector::ObjVector(%p,%s,%u,%s) [%p]",
	&list,String::boolText(move),maxLen,String::boolText(autodelete),this);
    assign(list,move,maxLen);
}

ObjVector::~ObjVector()
{
    XDebug(DebugAll,"ObjVector::~ObjVector() [%p]",this);
    clear();
}

void* ObjVector::getObject(const String& name) const
{
    if (name == YATOM("ObjVector"))
	return const_cast<ObjVector*>(this);
    return GenObject::getObject(name);
}

unsigned int ObjVector::assign(ObjList& list, bool move, unsigned int maxLen)
{
    if (!maxLen)
	maxLen = list.count();
    clear();
    if (maxLen) {
	m_objects = new GenObject*[maxLen];
	ObjList* l = list.skipNull();
	for (unsigned int i = 0; i < maxLen; i++) {
	    if (l) {
		if (move) {
		    m_objects[i] = l->remove(false);
		    l = l->skipNull();
		}
		else {
		    m_objects[i] = l->get();
		    l = l->skipNext();
		}
	    }
	    else
		m_objects[i] = 0;
	}
	m_length = maxLen;
    }
    return maxLen;
}

static inline void clearObjVector(GenObject** objs, unsigned int len)
{
    while (len--) {
	TelEngine::destruct(*objs);
	objs++;
    }
}

unsigned int ObjVector::resize(unsigned int len, bool keepData)
{
    if (!len) {
	clear();
	return length();
    }
    if (len == length()) {
	if (!keepData) {
	    if (m_delete)
		clearObjVector(m_objects,length());
	    ::memset(m_objects,0,length() * sizeof(GenObject*));
	}
	return length();
    }
    GenObject** buf = new GenObject*[len];
    if (!(keepData && length()))
	::memset(buf,0,len * sizeof(GenObject*));
    else if (len < length()) {
	::memcpy(buf,m_objects,len * sizeof(GenObject*));
	::memset(m_objects,0,len * sizeof(GenObject*));
    }
    else {
	::memcpy(buf,m_objects,length() * sizeof(GenObject*));
	::memset(m_objects,0,length() * sizeof(GenObject*));
	if (len > length())
	    ::memset(buf + length(),0,(len - length()) * sizeof(GenObject*));
    }
    clear();
    m_objects = buf;
    m_length = len;
    return length();
}

unsigned int ObjVector::compact(bool resizeToCount)
{
    if (!m_objects)
	return 0;
    unsigned int n = 0;
    GenObject** buf = m_objects;
    for (unsigned int i = 0; i < m_length; ++i, ++buf) {
	if (*buf) {
	    ++n;
	    continue;
	}
	if (i == m_length - 1)
	    break;
	GenObject** b = 0;
	for (unsigned int j = i + 1; j < m_length; ++j) {
	    if (!buf[j])
		continue;
	    b = buf + j;
	    i = j;
	    break;
	}
	if (!b)
	    break;
	*buf = *b;
	*b = 0;
	++n;
    }
    if (resizeToCount)
	resize(n,true);
    return n;
}

unsigned int ObjVector::count() const
{
    if (!m_objects)
	return 0;
    unsigned int c = 0;
    for (unsigned int i = 0; i < m_length; i++)
	if (m_objects[i])
	    c++;
    return c;
}

bool ObjVector::null() const
{
    if (!m_objects)
	return true;
    for (unsigned int i = 0; i < m_length; i++)
	if (m_objects[i])
	    return false;
    return true;
}

int ObjVector::index(const GenObject* obj) const
{
    if (!m_objects)
	return -1;
    for (unsigned int i = 0; i < m_length; i++)
	if (m_objects[i] == obj)
	    return i;
    return -1;
}

int ObjVector::index(const String& str) const
{
    if (!m_objects)
	return -1;
    for (unsigned int i = 0; i < m_length; i++)
	if (m_objects[i] && str.matches(m_objects[i]->toString()))
	    return i;
    return -1;
}

GenObject* ObjVector::take(unsigned int index)
{
    if (index >= m_length || !m_objects)
	return 0;
    GenObject* ret = m_objects[index];
    m_objects[index] = 0;
    return ret;
}

bool ObjVector::set(GenObject* obj, unsigned int index)
{
    if (index >= m_length || !m_objects)
	return false;
    GenObject* old = m_objects[index];
    if (old == obj)
	return true;
    m_objects[index] = obj;
    if (m_delete)
	TelEngine::destruct(old);
    return true;
}

void ObjVector::clear()
{
#ifdef XDEBUG
    Debugger debug("ObjVector::clear()"," [%p]",this);
#endif
    GenObject** objs = m_objects;
    unsigned int len = m_length;
    m_length = 0;
    m_objects = 0;
    if (m_delete && objs)
	clearObjVector(objs,len);
    delete[] objs;
}

/* vi: set ts=8 sw=4 sts=4 noet: */
