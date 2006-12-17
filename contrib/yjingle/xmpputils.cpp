/**
 * utils.cpp
 * Yet Another Jabber Component Protocol Stack
 * This file is part of the YATE Project http://YATE.null.ro
 *
 * Yet Another Telephony Engine - a fully featured software PBX and IVR
 * Copyright (C) 2004-2006 Null Team
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include <xmpputils.h>
#include <string.h>

using namespace TelEngine;

static XMPPNamespace s_ns;
static XMPPError s_err;

/**
 * XMPPNamespace
 */
TokenDict XMPPNamespace::s_value[] = {
	{"http://etherx.jabber.org/streams",                   Stream},
	{"jabber:component:accept",                            ComponentAccept},
	{"jabber:component:connect",                           ComponentConnect},
	{"urn:ietf:params:xml:ns:xmpp-streams",                StreamError},
	{"urn:ietf:params:xml:ns:xmpp-stanzas",                StanzaError},
	{"urn:ietf:params:xml:ns:xmpp-bind",                   Bind},
	{"http://jabber.org/protocol/disco#info",              DiscoInfo},
	{"http://jabber.org/protocol/disco#items",             DiscoItems},
	{"http://www.google.com/session",                      Jingle},
	{"http://www.google.com/session/phone",                JingleAudio},
	{"http://www.google.com/transport/p2p",                JingleTransport},
	{"http://jabber.org/protocol/jingle/info/dtmf",        Dtmf},
	{"http://jabber.org/protocol/jingle/info/dtmf#errors", DtmfError},
	{"http://jabber.org/protocol/commands",                Command},
	{0,0}
	};

bool XMPPNamespace::isText(Type index, const char* txt)
{
    const char* tmp = lookup(index,s_value,0);
    if (!(txt && tmp))
	return false;
    return 0 == strcmp(tmp,txt);
}

/**
 * XMPPError
 */
TokenDict XMPPError::s_value[] = {
	{"cancel",                   TypeCancel},
	{"continue",                 TypeContinue},
	{"modify",                   TypeModify},
	{"auth",                     TypeAuth},
	{"wait",                     TypeWait},
	{"bad-format",               BadFormat},
	{"bad-namespace-prefix",     BadNamespace},
	{"connection-timeout",       ConnTimeout},
	{"host-gone",                HostGone},
	{"host-unknown",             HostUnknown},
	{"improper-addressing",      BadAddressing},
	{"internal-server-error",    Internal},
	{"invalid-from",             InvalidFrom},
	{"invalid-id",               InvalidId},
	{"invalid-namespace",        InvalidNamespace},
	{"invalid-xml",              InvalidXml},
	{"not-authorized",           NotAuth},
	{"policy-violation",         Policy},
	{"remote-connection-failed", RemoteConn},
	{"resource-constraint",      ResConstraint},
	{"restricted-xml",           RestrictedXml},
	{"see-other-host",           SeeOther},
	{"system-shutdown",          Shutdown},
	{"undefined-condition",      UndefinedCondition},
	{"unsupported-encoding",     UnsupportedEnc},
	{"unsupported-stanza-type",  UnsupportedStanza},
	{"unsupported-version",      UnsupportedVersion},
	{"xml-not-well-formed",      Xml},
	{"bad-request",              SBadRequest},
	{"conflict",                 SConflict},
	{"feature-not-implemented",  SFeatureNotImpl},
	{"forbidden",                SForbidden},
	{"gone",                     SGone},
	{"internal-server-error",    SInternal},
	{"item-not-found",           SItemNotFound},
	{"jid-malformed",            SBadJid},
	{"not-acceptable",           SNotAcceptable},
	{"not-allowed",              SNotAllowed},
	{"not-authorized",           SNotAuth},
	{"payment-required",         SPayment},
	{"recipient-unavailable",    SUnavailable},
	{"redirect",                 SRedirect},
	{"registration-required",    SReg},
	{"remote-server-not-found",  SNoRemote},
	{"remote-server-timeout",    SRemoteTimeout},
	{"resource-constraint",      SResource},
	{"service-unavailable",      SServiceUnavailable},
	{"subscription-required",    SSubscription},
	{"undefined-condition",      SUndefinedCondition},
	{"unexpected-request",       SRequest},
	{"unsupported-dtmf-method",  DtmfNoMethod},
	{0,0}
	};

bool XMPPError::isText(int index, const char* txt)
{
    const char* tmp = lookup(index,s_value,0);
    if (!(txt && tmp))
	return false;
    return 0 == strcmp(tmp,txt);
}

/**
 * JabberID
 */
void JabberID::set(const char* jid)
{
    this->assign(jid);
    parse();
}

void JabberID::set(const char* node, const char* domain, const char* resource)
{
    if (node != m_node.c_str())
	m_node = node;
    if (domain != m_domain.c_str())
	m_domain = domain;
    if (resource != m_resource.c_str())
	m_resource = resource;
    clear();
    if (m_node.length())
	*this << m_node << "@";
    *this << m_domain;
    m_bare = *this;
    if (m_node.length() && m_resource.length())
	*this << "/" << m_resource;
}

bool JabberID::valid(const String& value)
{
    if (value.null())
	return true;
    return s_regExpValid.matches(value);
}

Regexp JabberID::s_regExpValid("^\\([[:alnum:]]*\\)");

#if 0
~`!#$%^*_-+=()[]{}|\;?.
#endif

void JabberID::parse()
{
    String tmp = *this;
    int i = tmp.find('@');
    if (i == -1)
	m_node = "";
    else {
	m_node = tmp.substr(0,i);
	tmp = tmp.substr(i+1,tmp.length()-i-1);
    }
    i = tmp.find('/');
    if (i == -1) {
	m_domain = tmp;
	m_resource = "";
    }
    else {
	m_domain = tmp.substr(0,i);
	m_resource = tmp.substr(i+1,tmp.length()-i-1);
    }
    // Set bare JID
    m_bare = "";
    if (m_node.length())
	m_bare << m_node << "@";
    m_bare << m_domain;
}

/**
 * JIDIdentity
 */
TokenDict JIDIdentity::s_category[] = {
	{"account",   Account},
	{"client",    Client},
	{"component", Component},
	{"gateway",   Gateway},
	{0,0},
	};

TokenDict JIDIdentity::s_type[] = {
	{"registered", AccountRegistered},
	{"phone",      ClientPhone},
	{"generic",    ComponentGeneric},
	{"presence",   ComponentPresence},
	{"generic",    GatewayGeneric},
	{0,0},
	};

XMLElement* JIDIdentity::toXML()
{
    return XMPPUtils::createIdentity(categoryText(m_category),typeText(m_type),*this);
}

bool JIDIdentity::fromXML(const XMLElement* element)
{
    if (!element)
	return false;
    XMLElement* id = ((XMLElement*)element)->findFirstChild("identity");
    if (!id)
	return false;
    m_category = categoryValue(id->getAttribute("category"));
    m_type = typeValue(id->getAttribute("type"));
    id->getAttribute("name",*this);
    return true;
}

/**
 * JIDFeatureList
 */
JIDFeature* JIDFeatureList::get(XMPPNamespace::Type feature)
{
    ObjList* obj = m_features.skipNull();
    for (; obj; obj = obj->skipNext()) {
	JIDFeature* f = static_cast<JIDFeature*>(obj->get());
	if (*f == feature)
	    return f;
    }
    return 0;
}

XMLElement* JIDFeatureList::addTo(XMLElement* element)
{
    if (!element)
	return 0;
    ObjList* obj = m_features.skipNull();
    for (; obj; obj = obj->skipNext()) {
	JIDFeature* f = static_cast<JIDFeature*>(obj->get());
	XMLElement* feature = new XMLElement(XMLElement::Feature);
	feature->setAttribute("var",s_ns[*f]);
	element->addChild(feature);
    }
    return element;
}

/**
 * XMPPUtils
 */
TokenDict XMPPUtils::s_iq[] = {
	{"set",     IqSet},
	{"get",     IqGet},
	{"result",  IqResult},
	{"error",   IqError},
	{0,0}
	};

TokenDict XMPPUtils::s_msg[] = {
	{"chat",    MsgChat},
	{0,0}
	};

TokenDict XMPPUtils::s_commandAction[] = {
	{"execute",  CommExecute},
	{"cancel",   CommCancel},
	{"prev",     CommPrev},
	{"next",     CommNext},
	{"complete", CommComplete},
	{0,0}
	};

TokenDict XMPPUtils::s_commandStatus[] = {
	{"executing", CommExecuting},
	{"completed", CommCompleted},
	{"cancelled", CommCancelled},
	{0,0}
	};

XMLElement* XMPPUtils::createElement(const char* name, XMPPNamespace::Type ns,
	const char* text)
{
    XMLElement* element = new XMLElement(name,0,text);
    element->setAttribute("xmlns",s_ns[ns]);
    return element;
}

XMLElement* XMPPUtils::createElement(XMLElement::Type type, XMPPNamespace::Type ns,
	const char* text)
{
    XMLElement* element = new XMLElement(type,0,text);
    element->setAttribute("xmlns",s_ns[ns]);
    return element;
}

XMLElement* XMPPUtils::createMessage(MsgType type, const char* from,
	const char* to, const char* id, const char* message)
{
    XMLElement* msg = new XMLElement(XMLElement::Message);
    msg->setAttribute("type",lookup(type,s_msg,""));
    msg->setAttribute("from",from);
    msg->setAttribute("to",to);
    if (id)
	msg->setAttributeValid("id",id);
    msg->addChild(new XMLElement(XMLElement::Body,0,message));
    return msg;
}

XMLElement* XMPPUtils::createIq(IqType type, const char* from,
	const char* to, const char* id)
{
    XMLElement* iq = new XMLElement(XMLElement::Iq);
    iq->setAttribute("type",lookup(type,s_iq,""));
    iq->setAttribute("from",from);
    iq->setAttribute("to",to);
    iq->setAttribute("id",id);
    return iq;
}

XMLElement* XMPPUtils::createIqBind( const char* from,
	const char* to, const char* id, const ObjList& resources)
{
    XMLElement* iq = createIq(IqSet,from,to,id);
    XMLElement* bind = createElement(XMLElement::Bind,XMPPNamespace::Bind);
    ObjList* obj = resources.skipNull();
    for (; obj; obj = resources.skipNext()) {
	String* s = static_cast<String*>(obj->get());
	if (!(s && s->length()))
	    continue;
	XMLElement* res = new XMLElement(XMLElement::Resource,0,*s);
	bind->addChild(res);
    }
    iq->addChild(bind);
    return iq;
}

XMLElement* XMPPUtils::createCommand(CommandAction action, const char* node,
	const char* sessionId)
{
    XMLElement* command = createElement(XMLElement::Command,XMPPNamespace::Command);
    if (sessionId)
	command->setAttribute("sessionid",sessionId);
    command->setAttribute("node",node);
    command->setAttribute("action",lookup(action,s_commandAction));
    return command;
}

XMLElement* XMPPUtils::createIdentity(const char* category, const char* type,
	const char* name)
{
    XMLElement* id = new XMLElement("identity");
    id->setAttribute("category",category);
    id->setAttribute("type",type);
    id->setAttribute("name",name);
    return id;
}

XMLElement* XMPPUtils::createIqDisco(const char* from, const char* to,
	const char* id, bool info)
{
    XMLElement* xml = createIq(IqGet,from,to,id);
    xml->addChild(createElement(XMLElement::Query,
	info ? XMPPNamespace::DiscoInfo : XMPPNamespace::DiscoItems));
    return xml;
}

XMLElement* XMPPUtils::createError(XMPPError::ErrorType type,
	XMPPError::Type condition, const char* text)
{
    XMLElement* err = new XMLElement("error");
    err->setAttribute("type",s_err[type]);
    XMLElement* tmp = createElement(s_err[condition],XMPPNamespace::StanzaError);
    err->addChild(tmp);
    if (text) {
	tmp = createElement("text",XMPPNamespace::StanzaError,text);
	err->addChild(tmp);
    }
    return err;
}

XMLElement* XMPPUtils::createStreamError(XMPPError::Type error, const char* text)
{
    XMLElement* element = new XMLElement(XMLElement::StreamError);
    XMLElement* err = createElement(s_err[error],XMPPNamespace::StreamError);
    element->addChild(err);
    if (text) {
	XMLElement* txt = createElement("text",XMPPNamespace::StreamError,text);
	element->addChild(txt);
    }
    return element;
}

void XMPPUtils::print(String& xmlStr, XMLElement* element, const char* indent)
{
#define STARTLINE(indent) "\r\n" << indent
    const char* enclose = "-----";
    if (!element)
	return;
    bool hasAttr = (0 != element->firstAttribute());
    bool hasChild = (0 != element->findFirstChild());
    const char* txt = element->getText();
    bool root = false;
    if (!(indent && *indent)) {
	indent = "";
	root = true;
    }
    if (root)
	xmlStr << STARTLINE(indent) << enclose;
    // Name
    if (!(hasAttr || hasChild || txt)) {
	// indent<element->name()/>
	xmlStr << STARTLINE(indent) << '<' << element->name();
	if ((element->name())[0] != '/')
	    xmlStr << '/';
	xmlStr << '>';
	if (root)
	    xmlStr << STARTLINE(indent) << enclose;
	return;
    }
    // <element->name()> or <element->name()
    xmlStr << STARTLINE(indent) << '<' << element->name();
    if (hasChild)
	xmlStr << '>';
    String sindent = indent;
    sindent << "  ";
    // Attributes
    const TiXmlAttribute* attr = element->firstAttribute();
    for (; attr; attr = attr->Next())
	// attr->name()="attr->value()"
	xmlStr << STARTLINE(sindent) << attr->Name() << "=\"" << attr->Value() << '"';
    // Text
    if (txt)
	xmlStr << STARTLINE(sindent) << txt;
    // Children
    XMLElement* child = element->findFirstChild();
    String si = sindent;
    for (; child; child = element->findNextChild(child))
	print(xmlStr,child,si);
    // End tag
    if (hasChild)
	xmlStr << STARTLINE(indent) << "</" << element->name() << '>';
    else
	xmlStr << STARTLINE(indent) << "/>";
    if (root)
	xmlStr << STARTLINE(indent) << enclose;
#undef STARTLINE
}

bool XMPPUtils::split(NamedList& dest, const char* src, const char sep,
	bool nameFirst)
{
    if (!src)
	return false;
    u_int32_t index = 1;
    for (u_int32_t i = 0; src[i];) {
	// Skip separator(s)
	for (; src[i] && src[i] == sep; i++) ;
	// Find first separator
	u_int32_t start = i;
	for (; src[i] && src[i] != sep; i++) ;
	// Get part
	if (start != i) {
	    String tmp(src + start,i - start);
	    if (nameFirst)
		dest.addParam(tmp,String((int)index++));
	    else
		dest.addParam(String((int)index++),tmp);
	}
    }
    return true;
}

/* vi: set ts=8 sw=4 sts=4 noet: */
