/**
 * yatejingle.h
 * Yet Another Jingle Stack
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

#ifndef __YATEJINGLE_H
#define __YATEJINGLE_H

#include <yateclass.h>
#include <yatejabber.h>

/**
 * Holds all Telephony Engine related classes.
 */
namespace TelEngine {

class JGAudio;                           // A Jingle data payload
class JGAudioList;                       // A List of Jingle data payloads
class JGTransport;                       // A Jingle transport description
class JGSession;                         // A Jingle session
class JGEvent;                           // An event generated by a Jingle session
class JGEngine;                          // The Jingle engine
class JGSentStanza;                      // Sent stanza timeout info


/**
 * This class holds a Jingle data payload description
 * @short A Jingle data payload
 */
class YJINGLE_API JGAudio : public GenObject
{
public:
    /**
     * Constructor. Fill this object from the given attributes
     * @param _id The 'id' attribute
     * @param _name The 'name' attribute
     * @param _clockrate The 'clockrate' attribute
     * @param _bitrate The 'bitrate' attribute
     * @param _synonym The 'synonym' attribute
     */
    inline JGAudio(const char* _id, const char* _name, const char* _clockrate,
	const char* _bitrate, const char* _synonym)
	{ set(_id,_name,_clockrate,_bitrate,_synonym); }

    /**
     * Constructor. Fill this object from an XML element
     * @param xml The element to fill from
     */
    inline JGAudio(XMLElement* xml)
	{ fromXML(xml); }

    /**
     * Copy constructor
     */
    inline JGAudio(const JGAudio& src)
	{ set(src.id,src.name,src.clockrate,src.bitrate,src.synonym); }

    /**
     * Set the data
     * @param _id The 'id' attribute
     * @param _name The 'name' attribute
     * @param _clockrate The 'clockrate' attribute
     * @param _bitrate The 'bitrate' attribute
     * @param _synonym The 'synonym' attribute
     */
    inline void set(const char* _id, const char* _name, const char* _clockrate,
	const char* _bitrate, const char* _synonym) {
	    id = _id;
	    name = _name;
	    clockrate = _clockrate;
	    bitrate = _bitrate;
	    synonym = _synonym;
	}

    /**
     * Get the string repreasentation (id) of this payload
     * @return The string repreasentation (id) of this payload
     */
    virtual const String& toString() const
	{ return id; }

    /**
     * Create a 'payload-type' element from this object
     * @return Valid XMLElement pointer
     */
    XMLElement* toXML();

    /**
     * Fill this object from a given element
     * @param xml The element
     */
    void fromXML(XMLElement* xml);

    /**
     * The numeric id of this payload
     */
    String id;

    /**
     * The Jingle name of this payload
     */
    String name;

    /**
     * The clockrate of this payload
     */
    String clockrate;

    /**
     * The bitrate of this payload
     */
    String bitrate;

    /**
     * A synonym of this payload's name
     */
    String synonym;
};


/**
 * Hold a list of data payloads
 * @short A List of Jingle data payloads
 */
class JGAudioList : public ObjList
{
public:
    /**
     * Append a new data payload
     * @param id The payload's id
     * @param name The payload's name
     * @param clockrate The payload's clockrate
     * @param bitrate The payload's bitrate
     * @param synonym The payload's synonym
     */
    inline void add(const char* id, const char* name, const char* clockrate,
	const char* bitrate, const char* synonym)
	{ append(new JGAudio(id,name,clockrate,bitrate,synonym)); }

    /**
     * Find a data payload by its synonym
     * @param value The value to compare with
     * @return JGAudio pointer or 0 if not found
     */
    JGAudio* findSynonym(const String& value);

    /**
     * Create a 'description' element and add payload children to it
     * @param telEvent True to append a telephone event data payload
     * @return Valid XMLElement pointer
     */
    XMLElement* toXML(bool telEvent = true);

    /**
     * Fill this list from an XML element's children. Clear before attempting to fill
     * @param xml The source XML element
     */
    void fromXML(XMLElement* xml);

    /**
     * Create a list from data payloads
     * @param dest Destination string
     * @param synonym True to create from synonyms, false to create from names
     * @param sep List item separator
     * @return False if the list is empty
     */
    bool createList(String& dest, bool synonym, const char* sep = ",");
};


/**
 * This class holds a Jingle transport description (protocol, address, port ...)
 * @short A Jingle transport description
 */
class YJINGLE_API JGTransport : public RefObject
{
public:
    /**
     * Constructor
     */
    inline JGTransport()
	{}

    /**
     * Copy constructor
     */
    JGTransport(const JGTransport& src);

    /**
     * Constructor. Fill this object from an XML element
     * @param element The element to fill from
     */
    inline JGTransport(XMLElement* element)
	{ fromXML(element); }

    /**
     * Destructor
     */
    virtual ~JGTransport()
	{}

    /**
     * Create a 'transport' element
     * @return Valid XMLElement pointer
     */
    static XMLElement* createTransport();

    /**
     * Create a 'candidate' element from this object
     * @return Valid XMLElement pointer
     */
    XMLElement* toXML();

    /**
     * Fill this object from a given element
     * @param element The element
     */
    void fromXML(XMLElement* element);

    /**
     * Create and add a 'candidate' child to the given element
     * @param transport The element
     */
    inline void addTo(XMLElement* transport)
	{ if (transport) transport->addChild(toXML()); }

    // Attributes
    String name;
    String address;
    String port;
    String preference;
    String username;
    String protocol;
    String generation;
    String password;
    String type;
    String network;
};


/**
 * This class does the management of a Jingle session
 * @short A Jingle session
 */
class YJINGLE_API JGSession : public RefObject, public Mutex
{
    friend class JGEvent;
    friend class JGEngine;
public:
    /**
     * Session state enumeration
     */
    enum State {
	Idle    = 0,                     // Outgoing stream is waiting for 
	Pending = 1,                     // Session is pending, session-initiate sent/received
	Active  = 2,                     // Session is active, session-accept sent/received
	Ending  = 3,                     // Session terminated: Wait for write result
	Destroy = 4,                     // The session will be destroyed
    };

    /**
     * Jingle action enumeration
     */
    enum Action {
	ActAccept,                       // accept
	ActInitiate,                     // initiate
	ActReject,                       // reject
	ActTerminate,                    // terminate
	ActInfo,                         // session-info
	ActTransport,                    // Used to set/get transport info
	ActTransportInfo,                // transport-info
	ActTransportCandidates,          // candidates
	ActTransportAccept,              // transport-accept
	ActContentInfo,                  // content-info
	ActTransfer,                     // session-transfer
	ActRinging,                      // session-info: Ringing
	ActHold,                         // session-info: Hold
	ActActive,                       // session-info: Active
	ActMute,                         // session-info: Mute
	ActDtmf,                         // Used to set/get dtmf content-info element
	ActDtmfMethod,                   // Used to set/get dtmf method content-info element
	ActCount,
    };

    /**
     * Jingle transport type enumeration
     */
    enum TransportType {
	TransportUnknown,               // Detect transport type on incoming, sent both on outgoing
	TransportInfo,                  // transport-info
	TransportCandidates,            // candidates
    };

    /**
     * Jingle defined termination reasons
     */
    enum Reason {
	ReasonBusy,                      // <busy/>
	ReasonDecline,                   // <decline/>
	ReasonConn,                      // <connectivity-error/>
	ReasonMedia,                     // <media-error/>
	ReasonTransport,                 // <unsupported-transports/>
	ReasonNoError,                   // <no-error/>
	ReasonOk,                        // <success/>
	ReasonNoApp,                     // <unsupported-applications/>
	ReasonAltSess,                   // <alternative-session/>
	ReasonUnknown,                   // <general-error/>
	ReasonTransfer,                  // <transferred>
	ReasonNone                       // None of the above
    };

    /**
     * Destructor. Hangup if Pending or Active
     */
    virtual ~JGSession();

    /**
     * Get the session direction
     * @return True if it is an outgoing session
     */
    inline bool outgoing() const
	{ return m_outgoing; }

    /**
     * Get the session id
     * @return The session id
     */
    const String& sid() const
	{ return m_sid; }

    /**
     * Get the local peer's JID
     * @return The local peer's JID
     */
    const JabberID& local() const
	{ return m_localJID; }

    /**
     * Get the remote peer's JID
     * @return The remote peer's JID
     */
    const JabberID& remote() const
	{ return m_remoteJID; }

    /**
     * Get the session state.
     * @return The session state as enumeration.
     */
    inline State state() const
	{ return m_state; }

    /**
     * Get the stream this session is bound to
     * @return The stream this session is bound to
     */
    inline const JBStream* stream() const
	{ return m_stream; }

    /**
     * Get the arbitrary user data of this session
     * @return The arbitrary user data of this session
     */
    inline void* userData()
	{ return m_private; }

    /**
     * Ask this session to accept an event
     * @param event The event to accept
     * @param sid The session id if this is a request
     * @return True if accepted (the event was enqueued), false if not
     */
    bool acceptEvent(JBEvent* event, const String& sid = String::empty());

    /**
     * Set the arbitrary user data of this session
     * @param userdata The new arbitrary user data's value
     */
    inline void userData(void* userdata)
	{ m_private = userdata; }

    /**
     * Accept a Pending incoming session.
     * This method is thread safe
     * @param description The media description element to send
     * @param stanzaId Optional string to be filled with sent stanza id (used to track the response)
     * @return False if send failed
     */
    bool accept(XMLElement* description, String* stanzaId = 0);

    /**
     * Close a Pending or Active session
     * This method is thread safe
     * @param reason Termination reason
     * @param msg Optional termination message
     * @return False if send failed
     */
    bool hangup(int reason, const char* msg = 0);

    /**
     * Confirm a received element. If the error is NoError a result stanza will be sent.
     *  Otherwise, an error stanza will be created and sent and the received element is
     *  consumed (attached to the sent error stanza)
     * @param xml The element to confirm
     * @param error The error condition
     * @param text Optional text to add to the error element
     * @param type Error type
     * @return False if send failed or element is 0
     */
    bool confirm(XMLElement* xml, XMPPError::Type error = XMPPError::NoError,
	const char* text = 0, XMPPError::ErrorType type = XMPPError::TypeModify);

    /**
     * Send a dtmf string to remote peer. If the string's lenght is greater then 1, each
     *  character is added as a 'dtmf' child of the jingle element
     * @param dtmf The dtmf string
     * @param buttonUp True to send button-up action. False to send button-down
     * @param stanzaId Optional string to be filled with sent stanza id (used to track the response)
     * @return False if send failed
     */
    bool sendDtmf(const char* dtmf, bool buttonUp = true, String* stanzaId = 0);

    /**
     * Send a dtmf method to remote peer
     * @param method The method to send
     * @param stanzaId Optional string to be filled with sent stanza id (used to track the response)
     * @return False if send failed
     */
    bool sendDtmfMethod(const char* method, String* stanzaId = 0);

    /**
     * Deny a dtmf method request from remote peer
     * @param element The received 'iq' element with the method
     * @return False if send failed
     */
    bool denyDtmfMethod(XMLElement* element);

    /**
     * Send a 'transport-accept' element to the remote peer.
     * This method is thread safe
     * @param transport Optional transport data to send
     * @param stanzaId Optional string to be filled with sent stanza id (used to track the response)
     * @return False if send failed
     */
    inline bool acceptTransport(JGTransport* transport = 0, String* stanzaId = 0)
	{ return sendTransport(transport,ActTransportAccept,stanzaId); }

    /**
     * Send a 'transport-info' element to the remote peer.
     * This method is thread safe
     * @param transport The transport data
     * @param stanzaId Optional string to be filled with sent stanza id (used to track the response)
     * @return False if send failed
     */
    inline bool sendTransport(JGTransport* transport, String* stanzaId = 0)
	{ return sendTransport(transport,ActTransport,stanzaId); }

    /**
     * Send a message to the remote peer.
     * This method is thread safe
     * @param msg The message to send
     * @return False on socket error
     */
    inline bool sendMessage(const char* msg) {
	    return sendStanza(JBMessage::createMessage(JBMessage::Chat,
		m_localJID,m_remoteJID,0,msg),0,false);
	}

    /**
     * Send a session info element to the remote peer.
     * This method is thread safe
     * @param data The XMLElement carried by the session info element
     * @param stanzaId Optional string to be filled with sent stanza id (used to track the response)
     * @return False on failure
     */
    bool sendInfo(XMLElement* xml, String* stanzaId = 0);

    /**
     * Check if the remote party supports a given feature
     * @param feature The requested feature
     * @return True if the remote party supports the given feature
     */
    bool hasFeature(XMPPNamespace::Type feature);

    /**
     * Build a transfer element
     * @param transferTo The JID to transfer to
     * @param transferFrom The transferror's JID
     * @param sid Optional session id used for attended transfer (empty for unattended transfer)
     * @return Valid XMLElement pointer
     */
    static XMLElement* buildTransfer(const String& transferTo, const String& transferFrom,
	const String& sid = String::empty());

    /**
     * Get the termination code associated with a text
     * @param value The termination text
     * @param def Default value to return if not found
     * @return Termination code
     */
    static inline int lookupReason(const char* value, int def = ReasonOk)
	{ return lookup(value,s_reasons,def); }

    /**
     * Get the termination code associated with a text
     * @param value The termination code
     * @param def Default value to return if not found
     * @return Termination text
     */
    static inline const char* lookupReason(int value, const char* def = 0)
	{ return lookup(value,s_reasons,def); }

    /**
     * Get the name of an action
     * @return The name of an action
     */
    static inline const char* lookupAction(int act)
	{ return lookup(act,s_actions); }

    /**
     * Termination reasons
     */
    static TokenDict s_reasons[];

protected:
    /**
     * Constructor. Create an outgoing session
     * @param engine The engine that owns this session
     * @param stream The stream this session is bound to
     * @param callerJID The caller's full JID
     * @param calledJID The called party's full JID
     * @param media The session media description
     * @param transport The session transport method(s)
     * @param sid True to used 'sid' instead of 'id' as session id attribute
     * @param extra Optional extra child to be added to the session initiate element
     * @param msg Optional message to be sent before session initiate
     */
    JGSession(JGEngine* engine, JBStream* stream,
	const String& callerJID, const String& calledJID,
	XMLElement* media, XMLElement* transport,
	bool sid, XMLElement* extra = 0, const char* msg = 0);

    /**
     * Constructor. Create an incoming session.
     * @param engine The engine that owns this session
     * @param event A valid Jabber Jingle event with action session initiate
     * @param id Session id
     * @param sid True to used 'sid' instead of 'id' as session id attribute
     */
    JGSession(JGEngine* engine, JBEvent* event, const String& id, bool sid);

    /**
     * Release this session and its memory
     */
    virtual void destroyed();

    /**
     * Enqueue a Jabber engine event.
     * This method is thread safe
     * @param event The event event to process
     */
    void enqueue(JBEvent* event);

    /**
     * Get a Jingle event from the queue.
     * This method is thread safe
     * @param time Current time in miliseconds
     * @return JGEvent pointer or 0
     */
    JGEvent* getEvent(u_int64_t time);

    /**
     * Send a stanza to the remote peer
     * @param stanza The stanza to send
     * @param stanzaId Optional string to be filled with sent stanza id (used to track the response)
     * @param confirmation True if the stanza needs confirmation (add 'id' attribute)
     * @return True on success
     */
    bool sendStanza(XMLElement* stanza, String* stanzaId = 0, bool confirmation = true);

    /**
     * Decode a valid jingle set event. Set the event's data on success
     * @param jbev The event to decode
     * @return JGEvent pointer or 0
     */
    JGEvent* decodeJingle(JBEvent* jbev);

    /**
     * Create an 'iq' of type 'set' with a 'jingle' child
     * @param action The action of the Jingle stanza
     * @param element1 Optional child element
     * @param element2 Optional child element
     * @param element3 Optional child element
     * @return Valid XMLElement pointer
     */
    XMLElement* createJingle(Action action, XMLElement* element1 = 0,
	XMLElement* element2 = 0, XMLElement* element3 = 0);

    /**
     * Send a transport related element to the remote peer
     * @param transport Transport data to send
     * @param act The element's type (info, accept, etc)
     * @param stanzaId Optional string to be filled with sent stanza id (used to track the response)
     * @return True on success
     */
    bool sendTransport(JGTransport* transport, Action act, String* stanzaId = 0);

    /**
     * Get the name of a session state
     * @return The name of a session state
     */
    static const char* lookupState(int state)
	{ return lookup(state,s_states); }

private:
    static TokenDict s_states[];         // Session state names
    static TokenDict s_actions[];        // Action names

    JGSession() {}                       // Don't use it
    // Terminate notification from an event. Reset the last generated event.
    void eventTerminated(JGEvent* event);
    // Change session state
    void changeState(State newState);

    // State info
    State m_state;                       // Session state
    TransportType m_transportType;       // Remote client transport type
    // Links
    JGEngine* m_engine;                  // The engine that owns this session
    JBStream* m_stream;                  // The stream this session is bound to
    // Session info
    bool m_outgoing;                     // Session direction
    String m_sid;                        // Session id
    JabberID m_localJID;                 // Local peer's JID
    JabberID m_remoteJID;                // Remote peer's JID
    String m_sidAttr;                    // Session id attribute name
    // Session data
    ObjList m_events;                    // Incoming events from Jabber engine
    JGEvent* m_lastEvent;                // Last generated event
    void* m_private;                     // Arbitrary user data
    // Sent stanzas id generation
    String m_localSid;                   // Local session id (used to generate element's id)
    u_int32_t m_stanzaId;                // Sent stanza id counter
    ObjList m_sentStanza;                // Sent stanzas' id
};

/**
 * This class holds an event generated by a Jingle session
 * @short A Jingle event
 */
class YJINGLE_API JGEvent
{
    friend class JGSession;
public:
    /**
     * Jingle events enumeration
     */
    enum Type {
	Jingle,                          // Actions:
	                                 //  ActAccept
	                                 //  ActInitiate
	                                 //  ActTransport            Transport candidade(s)
	                                 //  ActTransportAccept
	                                 //  ActDtmf                 m_reason is button-up/button-down. m_text is the dtmf
	                                 //  ActDtmfMethod           m_text is the dtmf method: rtp/xmpp
	ResultOk,                        // Response for a sent stanza (iq with type=result)
	ResultError,                     // Response for a sent stanza (iq with type=error)
	ResultWriteFail,                 // Response for a sent stanza (failed to send stanza)
	ResultTimeout,                   // Response for a sent stanza (stanza timeout)
	// Final
	Terminated,                      // m_element is the element that caused the termination
	                                 //  m_reason contains the reason
	Destroy,                         // The engine sould delete the event (causing session destruction)
    };

    /**
     * Destructor. Deref the session. Delete the XML element
     */
    virtual ~JGEvent();

    /**
     * Get the type of this event
     * @return The type of this event as enumeration
     */
    inline Type type() const
	{ return m_type; }

    /**
     * Get the name of this
     * @return The name of this event
     */
    inline const char* name()
	{ return lookupType(m_type); }

    /**
     * Get the session that generated this event
     * @return The session that generated this event
     */
    inline JGSession* session() const
	{ return m_session; }

    /**
     * Get the XML element that generated this event
     * @return The XML element that generated this event
     */
    inline XMLElement* element() const
	{ return m_element; }

    /**
     * Get the Jingle child of the XML element carried by the event
     * Don't delete it after use: it is owned by the event
     * @return The Jingle child of the XML element carried by the event
     */
    inline XMLElement* jingle() const
	{ return m_jingle; }

    /**
     * Get the jingle action as enumeration
     * @return The jingle action as enumeration
     */
    inline JGSession::Action action() const
	{ return m_action; }

    /**
     * Get the name of an action
     * @return The name of an action
     */
    inline const char* actionName()
	{ return JGSession::lookupAction(m_action); }

    /**
     * Get the audio payloads list
     * @return The audio payloads list
     */
    inline JGAudioList& audio()
	{ return m_audio; }

    /**
     * Get the transports list
     * @return The transports list
     */
    inline ObjList& transport()
	{ return m_transport; }

    /**
     * Get the id
     * @return The id
     */
    inline const String& id() const
	{ return m_id; }

    /**
     * Get the reason
     * @return The reason
     */
    inline const String& reason() const
	{ return m_reason; }

    /**
     * Get the text
     * @return The text
     */
    inline const String& text() const
	{ return m_text; }

    /**
     * Get the XML element that generated this event and set it to 0
     * @return The XML element that generated this event
     */
    inline XMLElement* releaseXML() {
	    TelEngine::destruct(m_jingle);
	    XMLElement* tmp = m_element;
	    m_element = 0;
	    return tmp;
	 }

    /**
     * Check if this event is a final one (Terminated or Destroy)
     * @return True if it is
     */
    inline bool final() const
	{ return m_type == Terminated || m_type == Destroy; }

    /**
     * Get the name of an event type
     * @return The name of an event type
     */
    static inline const char* lookupType(int type)
	{ return lookup(type,s_typeName); }

    /**
     * Dictionary with event type names
     */
    static TokenDict s_typeName[];

protected:
    /**
     * Constructor. Set the id parameter if the element is valid
     * @param type Event type
     * @param session The session that generated this event
     * @param element Optional XML element that generated this event
     * @param reason Optional reason data
     * @param text Optional text data
     */
    inline JGEvent(Type type, JGSession* session, XMLElement* element = 0,
	const char* reason = 0, const char* text = 0)
	: m_type(type), m_session(0), m_element(element), m_jingle(0),
	m_action(JGSession::ActCount), m_reason(reason), m_text(text)
	{ init(session); }

    /**
     * Constructor. Create a Jingle event. Set the id parameter if the element is valid
     * @param act The jingle action
     * @param session The session that generated this event
     * @param element XML element that generated this event
     * @param reason Optional reason data
     * @param text Optional text data
     */
    inline JGEvent(JGSession::Action act, JGSession* session, XMLElement* element,
	const char* reason = 0, const char* text = 0)
	: m_type(Jingle), m_session(0), m_element(element), m_jingle(0),
	m_action(act), m_reason(reason), m_text(text)
	{ init(session); }

private:
    JGEvent() {}                         // Don't use it
    void init(JGSession* session);

    Type m_type;                         // The type of this event
    JGSession* m_session;                // Jingle session that generated this event
    XMLElement* m_element;               // XML element that generated this event
    XMLElement* m_jingle;                // The session child, if present
    // Event specific
    JGSession::Action m_action;          // The action if type is Jingle
    JGAudioList m_audio;                 // The received audio payloads
    ObjList m_transport;                 // The received transport data
    String m_id;                         // The element's id attribute
    String m_reason;                     // The reason if type is Error or Terminated 
    String m_text;                       // Dtmf text
};

/**
 * This class holds a Jingle service for the Jabber engine. Handle jingle stanzas,
 *  stanza write fail events and stream termination events
 * @short A Jingle engine
 */
class YJINGLE_API JGEngine : public JBService, public JBThreadList
{
    friend class JGSession;
public:
    /**
     * Constructor. Constructs a Jingle service
     * @param engine The Jabber engine
     * @param params Service's parameters
     * @param prio The priority of this service
     */
    JGEngine(JBEngine* engine, const NamedList* params, int prio = 0);

    /**
     * Destructor. Terminates all active sessions
     */
    virtual ~JGEngine();

    /**
     * Get the timeout interval of a sent stanza
     * @return The timeout interval of a sent stanza
     */
    inline u_int64_t stanzaTimeout() const
	{ return m_stanzaTimeout; }

    /**
     * Initialize this service
     * @param params Service's parameters
     */
    virtual void initialize(const NamedList& params);

    /**
     * Call getEvent() for each session list until an event is generated or the end is reached
     * This method is thread safe
     * @param time Current time in miliseconds
     * @return The first generated event
     */
    JGEvent* getEvent(u_int64_t time);

    /**
     * Make an outgoing call.
     * 'media' and 'transport' will be invalid on exit. Don't delete them
     * @param callerName The local peer's username
     * @param remoteJID The remote peer's JID
     * @param media A valid 'description' XML element
     * @param transport A valid 'transport' XML element
     * @param extra Optional extra child for session initiate element
     * @param msg Optional message to send before call
     * @return Valid JGSession pointer (referenced) on success
     */
    JGSession* call(const String& callerName, const String& remoteJID,
	XMLElement* media, XMLElement* transport, XMLElement* extra = 0,
	const char* msg = 0);

    /**
     * Default event processor. Delete event.
     * @param event The event to process
     */
    void defProcessEvent(JGEvent* event);

    /**
     * Process events from the sessions. Default action: Delete event.
     * Descendants must override this method to process generated events
     * @param event The event to process
     */
    virtual void processEvent(JGEvent* event);

protected:
    /**
     * Accept an event from the Jabber engine
     * @param event The event to accept
     * @param processed Set to true on exit to signal that the event was already processed
     * @param insert Set to true if accepted to insert on top of the event queue
     * @return False if not accepted, let the engine try another service
     */
    virtual bool accept(JBEvent* event, bool& processed, bool& insert);

private:
    // Create a local session id
    void createSessionId(String& id);

    ObjList m_sessions;                  // List of sessions
    Mutex m_sessionIdMutex;              // Session id counter lock
    u_int32_t m_sessionId;               // Session id counter
    u_int64_t m_stanzaTimeout;           // The timeout of a sent stanza
    bool m_useSidAttr;                   // Use 'sid' for session id attribute name for outgoing calls
};


/**
 * This class holds sent stanzas info used for timeout checking
 * @short Send stanza timeout info
 */
class YJINGLE_API JGSentStanza : public String
{
public:
    /**
     * Constructor
     * @param id The sent stanza's id
     * @param time The sent time
     * @param notif True to notify stanza timeout or response
     */
    JGSentStanza(const char* id, u_int64_t time, bool notif = false)
	: String(id), m_time(time), m_notify(notif)
	{}

    /**
     * Check if this element timed out
     * @return True if timeout
     */
    inline bool timeout(u_int64_t time) const
	{ return time > m_time; }

    /**
     * Check if timeout should be notified to sender
     * @return True to notify
     */
    inline bool notify() const
	{ return m_notify; }

private:
    u_int64_t m_time;                    // Timeout
    bool m_notify;                       // Notify timeout to sender
};

}

#endif /* __YATEJINGLE_H */

/* vi: set ts=8 sw=4 sts=4 noet: */
