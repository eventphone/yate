/**
 * NamedList.cpp
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
#include "yatexml.h"

using namespace TelEngine;

static const NamedList s_empty("");

const NamedList& NamedList::empty()
{
    return s_empty;
}

NamedList::NamedList(const char* name)
    : String(name)
{
}

NamedList::NamedList(const NamedList& original)
    : String(original)
{
    copyParams(false,original);
}

NamedList::NamedList(const char* name, const NamedList& original, const String& prefix)
    : String(name)
{
    copySubParams(original,prefix);
}

NamedList& NamedList::operator=(const NamedList& value)
{
    String::operator=(value);
    clearParams();
    return copyParams(value);
}

void* NamedList::getObject(const String& name) const
{
    if (name == YATOM("NamedList"))
	return const_cast<NamedList*>(this);
    return String::getObject(name);
}

NamedList& NamedList::addParam(NamedString* param)
{
    XDebug(DebugInfo,"NamedList::addParam(%p) [\"%s\",\"%s\"]",
        param,(param ? param->name().c_str() : ""),TelEngine::c_safe(param));
    if (param)
	m_params.append(param);
    return *this;
}

NamedList& NamedList::addParam(const char* name, const char* value, bool emptyOK)
{
    XDebug(DebugInfo,"NamedList::addParam(\"%s\",\"%s\",%s)",name,value,String::boolText(emptyOK));
    if (emptyOK || !TelEngine::null(value))
	m_params.append(new NamedString(name, value));
    return *this;
}

NamedList& NamedList::setParam(NamedString* param)
{
    XDebug(DebugAll,"NamedList::setParam(%p) [%p]",param,this);
    if (!param)
	return *this;
    ObjList* o = m_params.skipNull();
    while (o) {
        NamedString* s = static_cast<NamedString*>(o->get());
        if (s->name() == param->name()) {
	    o->set(param);
	    return *this;
	}
	ObjList* next = o->skipNext();
	if (next)
	    o = next;
	else
	    break;
    }
    if (o)
	o->append(param);
    else
	m_params.append(param);
    return *this;   
}

static inline NamedString* nlSetParamCreate(NamedList& list, const String& name, ObjList*& append)
{
    append = list.paramList()->skipNull();
    while (append) {
        NamedString* ns = static_cast<NamedString*>(append->get());
        if (ns->name() == name) {
	    append = 0;
	    return ns;
	}
	ObjList* next = append->skipNext();
	if (!next)
	    return new NamedString(name);
	append = next;
    }
    append = list.paramList();
    return new NamedString(name);
}

NamedList& NamedList::setParam(const String& name, unsigned int flags, const TokenDict* tokens,
    bool unknownflag)
{
    XDebug(DebugAll,"NamedList::setParam(%s) flags=%u tokens=%p unkFlag=%u [%p]",
	name.safe(),flags,tokens,unknownflag,this);
    ObjList* append = 0;
    NamedString* ns = nlSetParamCreate(*this,name,append);
    *static_cast<String*>(ns) = "";
    ns->decodeFlags(flags,tokens,unknownflag);
    if (append)
	append->append(ns);
    return *this;   
}

NamedList& NamedList::setParam(const String& name, uint64_t flags, const TokenDict64* tokens,
    bool unknownflag)
{
    XDebug(DebugAll,"NamedList::setParam(%s) flags64=" FMT64U " tokens=%p unkFlag=%u [%p]",
	name.safe(),flags,tokens,unknownflag,this);
    ObjList* append = 0;
    NamedString* ns = nlSetParamCreate(*this,name,append);
    *static_cast<String*>(ns) = "";
    ns->decodeFlags(flags,tokens,unknownflag);
    if (append)
	append->append(ns);
    return *this;   
}

NamedList& NamedList::setParamHex(const String& name, const void* buf, unsigned int len, char sep)
{
    XDebug(DebugAll,"NamedList::setParamHex(%s,%p,%u,%c) [%p]",name.safe(),buf,len,sep,this);
    ObjList* append = 0;
    NamedString* ns = nlSetParamCreate(*this,name,append);
    ns->hexify((void*)buf,len,sep);
    if (append)
	append->append(ns);
    return *this;   
}

template <class Obj> NamedList& nlSetParamValue(NamedList& list, const String& name, Obj& value)
{
    ObjList* append = 0;
    NamedString* ns = nlSetParamCreate(list,name,append);
    *static_cast<String*>(ns) = value;
    if (append)
	append->append(ns);
    return list;
}

NamedList& NamedList::setParam(const String& name, const char* value)
{
    XDebug(DebugAll,"NamedList::setParam('%s','%s') [%p]",name.c_str(),value,this);
    return nlSetParamValue(*this,name,value);
}

NamedList& NamedList::setParam(const String& name, int64_t value)
{
    XDebug(DebugAll,"NamedList::setParam(%s) INT64=" FMT64 " [%p]",name.c_str(),value,this);
    return nlSetParamValue(*this,name,value);
}

NamedList& NamedList::setParam(const String& name, uint64_t value)
{
    XDebug(DebugAll,"NamedList::setParam(%s) UINT64=" FMT64U " [%p]",name.c_str(),value,this);
    return nlSetParamValue(*this,name,value);
}

NamedList& NamedList::setParam(const String& name, int32_t value)
{
    XDebug(DebugAll,"NamedList::setParam(%s) INT32=%d [%p]",name.c_str(),value,this);
    return nlSetParamValue(*this,name,value);
}

NamedList& NamedList::setParam(const String& name, uint32_t value)
{
    XDebug(DebugAll,"NamedList::setParam(%s) UINT32=%u [%p]",name.c_str(),value,this);
    return nlSetParamValue(*this,name,value);
}

NamedList& NamedList::setParam(const String& name, double value)
{
    XDebug(DebugAll,"NamedList::setParam(%s) DOUBLE=%f [%p]",name.c_str(),value,this);
    return nlSetParamValue(*this,name,value);
}

NamedList& NamedList::clearParam(const String& name, char childSep, const String* value)
{
    XDebug(DebugInfo,"NamedList::clearParam(\"%s\",'%.1s')",
	name.c_str(),&childSep);
    String tmp;
    if (childSep)
	tmp << name << childSep;
    ObjList *p = &m_params;
    while (p) {
        NamedString *s = static_cast<NamedString *>(p->get());
        if (s && ((s->name() == name) || s->name().startsWith(tmp))
	    && (!value || value->matches(*s)))
            p->remove();
	else
	    p = p->next();
    }
    return *this;
}

// Remove a specific parameter
NamedList& NamedList::clearParam(NamedString* param, bool delParam)
{
    if (!param)
	return *this;
    ObjList* o = m_params.find(param);
    if (o)
	o->remove(delParam);
    XDebug(DebugInfo,"NamedList::clearParam(%p) found=%p",param,o);
    return *this;
}

NamedList& NamedList::copyParam(const NamedList& original, const String& name, char childSep)
{
    XDebug(DebugInfo,"NamedList::copyParam(%p,\"%s\",'%.1s')",
	&original,name.c_str(),&childSep);
    if (!childSep) {
	// faster and simpler - used in most cases
	const NamedString* s = original.getParam(name);
	return s ? setParam(name,*s) : clearParam(name);
    }
    clearParam(name,childSep);
    String tmp;
    tmp << name << childSep;
    ObjList* dest = &m_params;
    for (const ObjList* l = original.m_params.skipNull(); l; l = l->skipNext()) {
	const NamedString* s = static_cast<const NamedString*>(l->get());
        if ((s->name() == name) || s->name().startsWith(tmp))
	    dest = dest->append(new NamedString(s->name(),*s));
    }
    return *this;
}

static inline NamedString* nlCopyParam(const NamedString& param)
{
    NamedPointer* np = YOBJECT(NamedPointer,&param);
    if (!(np && np->userData()))
	return 0;
    GenObject* ud = 0;
#define NP_COPY_PARAM_INTERNAL(Type) \
    ud = YOBJECT(Type,np->userData()); \
    if (ud) \
	return new NamedPointer(np->name(),new Type(*(Type*)ud),*np)
    NP_COPY_PARAM_INTERNAL(DataBlock);
    NP_COPY_PARAM_INTERNAL(XmlElement);
#undef NP_COPY_PARAM_INTERNAL
    return 0;
}
    
NamedList& NamedList::copyParams(bool replace, const NamedList& original, bool copyUserData)
{
    XDebug(DebugInfo,"NamedList::copyParams(%p,%u) [%p]",&original,replace,this);
    ObjList* append = replace ? 0 : &m_params;
    for (const ObjList* l = original.m_params.skipNull(); l; l = l->skipNext()) {
	const NamedString* p = static_cast<const NamedString*>(l->get());
	NamedString* ns = 0;
	if (copyUserData)
	    ns = nlCopyParam(*p);
	if (!ns)
	    ns = new NamedString(p->name(),*p);
	if (append)
	    append = append->append(ns);
	else
	    setParam(ns);
    }
    return *this;
}

NamedList& NamedList::copyParams(const NamedList& original, ObjList* list, char childSep)
{
    XDebug(DebugInfo,"NamedList::copyParams(%p,%p,'%.1s') [%p]",
	&original,list,&childSep,this);
    for (; list; list = list->next()) {
	GenObject* obj = list->get();
	if (!obj)
	    continue;
	String name = obj->toString();
	name.trimBlanks();
	if (name)
	    copyParam(original,name,childSep);
    }
    return *this;
}

NamedList& NamedList::copyParams(const NamedList& original, const String& list, char childSep)
{
    XDebug(DebugInfo,"NamedList::copyParams(%p,\"%s\",'%.1s') [%p]",
	&original,list.c_str(),&childSep,this);
    ObjList* l = list.split(',',false);
    if (l) {
	copyParams(original,l,childSep);
	l->destruct();
    }
    return *this;
}

NamedList& NamedList::copySubParams(const NamedList& original, const String& prefix,
    bool skipPrefix, bool replace)
{
    XDebug(DebugInfo,"NamedList::copySubParams(%p,\"%s\",%s,%s) [%p]",
	&original,prefix.c_str(),String::boolText(skipPrefix),
	String::boolText(replace),this);
    if (prefix) {
	unsigned int offs = skipPrefix ? prefix.length() : 0;
	ObjList* dest = &m_params;
	for (const ObjList* l = original.m_params.skipNull(); l; l = l->skipNext()) {
	    const NamedString* s = static_cast<const NamedString*>(l->get());
	    if (s->name().startsWith(prefix)) {
		const char* name = s->name().c_str() + offs;
		if (!*name)
		    continue;
		if (!replace)
		    dest = dest->append(new NamedString(name,*s));
		else if (offs)
		    setParam(name,*s);
		else
		    setParam(s->name(),*s);
	    }
	}
    }
    return *this;
}

bool NamedList::hasSubParams(const char* prefix) const
{
    XDebug(DebugInfo,"NamedList::hasSubParams(\"%s\") [%p]",prefix,this);
    if (!TelEngine::null(prefix)) {
	for (const ObjList* l = m_params.skipNull(); l; l = l->skipNext()) {
	    const NamedString* s = static_cast<const NamedString*>(l->get());
	    if (s->name().startsWith(prefix))
		return true;
	}
    }
    return false;
}

void NamedList::dump(String& str, const char* separator, char quote, bool force) const
{
    if (force && str.null())
	str << separator;
    str << quote << *this << quote;
    const ObjList *p = m_params.skipNull();
    for (; p; p = p->skipNext()) {
        const NamedString* s = static_cast<const NamedString *>(p->get());
	String tmp;
	tmp << quote << s->name() << quote << "=" << quote << *s << quote;
	str.append(tmp,separator);
    }
}

bool NamedList::dump(String& str, unsigned int flags, const char* separator,
    const char* nameSep, const char* prefix, char quote) const
{
    ObjList data;
    if ((0 != (flags & DumpName)) && (*this || (0 != (flags & DumpEmptyName)))) {
	String* tmp = 0;
	if (quote && 0 != (flags & DumpQuoteName)) {
	    tmp = new String;
	    *tmp << quote << *this << quote;
	}
	else if (*this)
	    tmp = new String(*this);
	if (tmp)
	    data.append(tmp);
    }
    if (!nameSep)
	nameSep = "=";
    bool quotePname = quote && 0 != (flags & DumpQuoteParamName);
    bool quotePvalue = quote && 0 == (flags & DumpDontQuoteParamValue);
    for (const ObjList* o = m_params.skipNull(); o; o = o->skipNext()) {
        const NamedString* s = static_cast<const NamedString *>(o->get());
	String* tmp = 0;
	if (quotePname) {
	    tmp = new String;
	    *tmp << quote << s->name() << quote;
	}
	else
	    tmp = new String(s->name());
	*tmp << nameSep;
	if (quotePvalue)
	    *tmp << quote << *s << quote;
	else
	    *tmp << *s;
	if (*tmp)
	    data.append(tmp);
	else
	    TelEngine::destruct(tmp);
    }
    unsigned int len = str.length();
    bool haveData = data.skipNull();
    if (haveData || 0 != (flags & DumpForcePrefix))
	str << prefix;
    if (haveData) {
	if (0 != (flags & DumpAddSeparator))
	    str << separator;
	str.append(data,separator,true);
    }
    return len != str.length();
}

int NamedList::getIndex(const NamedString* param) const
{
    if (!param)
	return -1;
    const ObjList *p = &m_params;
    for (int i=0; p; p=p->next(),i++) {
        if (static_cast<const NamedString *>(p->get()) == param)
            return i;
    }
    return -1;
}

int NamedList::getIndex(const String& name) const
{
    const ObjList *p = &m_params;
    for (int i=0; p; p=p->next(),i++) {
        NamedString *s = static_cast<NamedString *>(p->get());
        if (s && (s->name() == name))
            return i;
    }
    return -1;
}

NamedString* NamedList::getParam(const String& name) const
{
    XDebug(DebugInfo,"NamedList::getParam(\"%s\")",name.c_str());
    const ObjList *p = m_params.skipNull();
    for (; p; p=p->skipNext()) {
        NamedString *s = static_cast<NamedString *>(p->get());
        if (s->name() == name)
            return s;
    }
    return 0;
}

NamedString* NamedList::getParam(unsigned int index) const
{
    XDebug(DebugInfo,"NamedList::getParam(%u)",index);
    return static_cast<NamedString *>(m_params[index]);
}

const String& NamedList::operator[](const String& name) const
{
    const String* s = getParam(name);
    return s ? *s : String::empty();
}

const char* NamedList::getValue(const String& name, const char* defvalue) const
{
    XDebug(DebugInfo,"NamedList::getValue(\"%s\",\"%s\")",name.c_str(),defvalue);
    const NamedString *s = getParam(name);
    return s ? s->c_str() : defvalue;
}

int NamedList::getIntValue(const String& name, int defvalue, int minvalue, int maxvalue,
    bool clamp) const
{
    const NamedString *s = getParam(name);
    return s ? s->toInteger(defvalue,0,minvalue,maxvalue,clamp) : defvalue;
}

int NamedList::getIntValue(const String& name, const TokenDict* tokens, int defvalue) const
{
    const NamedString *s = getParam(name);
    return s ? s->toInteger(tokens,defvalue) : defvalue;
}

int64_t NamedList::getInt64Value(const String& name, int64_t defvalue, int64_t minvalue,
    int64_t maxvalue, bool clamp) const
{
    const NamedString *s = getParam(name);
    return s ? s->toInt64(defvalue,0,minvalue,maxvalue,clamp) : defvalue;
}

int64_t NamedList::getInt64ValueDict(const String& name, const TokenDict64* tokens,
    int64_t defvalue) const
{
    const NamedString* s = getParam(name);
    return s ? s->toInt64Dict(tokens,defvalue) : defvalue;
}

uint64_t NamedList::getUInt64Value(const String& name, uint64_t defvalue, uint64_t minvalue,
    uint64_t maxvalue, bool clamp) const
{
    const NamedString *s = getParam(name);
    return s ? s->toUInt64(defvalue,0,minvalue,maxvalue,clamp) : defvalue;
}

double NamedList::getDoubleValue(const String& name, double defvalue) const
{
    const NamedString *s = getParam(name);
    return s ? s->toDouble(defvalue) : defvalue;
}

bool NamedList::getBoolValue(const String& name, bool defvalue) const
{
    const NamedString *s = getParam(name);
    return s ? s->toBoolean(defvalue) : defvalue;
}

int NamedList::replaceParams(String& str, bool sqlEsc, char extraEsc) const
{
    int p1 = 0;
    int cnt = 0;
    while ((p1 = str.find("${",p1)) >= 0) {
	int p2 = str.find('}',p1+2);
	if (p2 > 0) {
	    String def;
	    String tmp = str.substr(p1+2,p2-p1-2);
	    tmp.trimBlanks();
	    int pq = tmp.find('$');
	    if (pq >= 0) {
		// param is in ${<name>$<default>} format
		def = tmp.substr(pq+1).trimBlanks();
		tmp = tmp.substr(0,pq).trimBlanks();
	    }
	    DDebug(DebugAll,"NamedList replacing parameter '%s' [%p]",tmp.c_str(),this);
	    const String* ns = getParam(tmp);
	    if (ns) {
		if (sqlEsc) {
		    const DataBlock* data = 0;
		    if (ns->null()) {
			NamedPointer* np = YOBJECT(NamedPointer,ns);
			if (np)
			    data = YOBJECT(DataBlock,np->userData());
		    }
		    if (data)
			tmp = data->sqlEscape(extraEsc);
		    else
			tmp = ns->sqlEscape(extraEsc);
		}
		else
		    tmp = *ns;
	    }
	    else
		tmp = def;
	    str = str.substr(0,p1) + tmp + str.substr(p2+1);
	    // advance search offset past the string we just replaced
	    p1 += tmp.length();
	    cnt++;
	}
	else
	    return -1;
    }
    return cnt;
}

/* vi: set ts=8 sw=4 sts=4 noet: */
