#pragma once

namespace sdk
{
	struct bf_read;
	struct bf_write;

	class game_event
	{
	public:
		virtual ~game_event( ) {};
		virtual const char* GetName( ) const = 0; // get event name

		virtual bool IsReliable( ) const = 0; // if event handled reliable
		virtual bool IsLocal( ) const = 0; // if event is never networked
		virtual bool IsEmpty( const char* keyName = 0 ) = 0; // check if data field exists

		// Data access
		virtual bool GetBool( const char* keyName = 0, bool defaultValue = false ) = 0;
		virtual int GetInt( const char* keyName = 0, int defaultValue = 0 ) = 0;
		virtual unsigned long long GetUint64( char const* keyName = 0, unsigned long long defaultValue = 0 ) = 0;
		virtual float GetFloat( const char* keyName = 0, float defaultValue = 0.0f ) = 0;

		virtual const char* GetString( const char* keyName = 0, const char* defaultValue = "" ) = 0;
		virtual const wchar_t* GetWString( char const* keyName = 0, const wchar_t* defaultValue = L"" ) = 0;

		virtual void SetBool( const char* keyName, bool value ) = 0;
		virtual void SetInt( const char* keyName, int value ) = 0;
		virtual void SetUInt64( const char* keyName, unsigned long long value ) = 0;
		virtual void SetFloat( const char* keyName, float value ) = 0;
		virtual void SetString( const char* keyName, const char* value ) = 0;
		virtual void SetWString( const char* keyName, const wchar_t* value ) = 0;
	};

	class game_event_listener
	{
	public:
		virtual ~game_event_listener( ) {}
		virtual void FireGameEvent( game_event* Event ) = 0;

		virtual int GetEventDebugID( )
		{
			return 42;
		}
	};

	class game_event_manager
	{
	public:
		virtual int __Unknown_1( int* dwUnknown ) = 0;

		// load game event descriptions from a file eg "resource\gameevents.res"
		virtual int LoadEventsFromFile( const char* filename ) = 0;

		// removes all and anything
		virtual void Reset( ) = 0;

		// adds a listener for a particular event
		virtual bool AddListener( game_event_listener* listener, const char* name, bool bServerSide ) = 0;

		// returns true if this listener is listens to given event
		virtual bool FindListener( game_event_listener* listener, const char* name ) = 0;

		// removes a listener 
		virtual int RemoveListener( game_event_listener* listener ) = 0;

		// create an event by name, but doesn't fire it. returns NULL is event is not
		// known or no listener is registered for it. bForce forces the creation even if no listener is active
		virtual game_event*CreateEvent( const char* name, bool bForce, unsigned int dwUnknown ) = 0;

		// fires a server event created earlier, if bDontBroadcast is set, event is not send to clients
		virtual bool FireEvent( game_event* event, bool bDontBroadcast = false ) = 0;

		// fires an event for the local client only, should be used only by client code
		virtual bool FireEventClientSide( game_event* event ) = 0;

		// create a new copy of this event, must be free later
		virtual game_event* DuplicateEvent( game_event* event ) = 0;

		// if an event was created but not fired for some reason, it has to bee freed, same UnserializeEvent
		virtual void FreeEvent( game_event* event ) = 0;

		// write/read event to/from bitbuffer
		virtual bool SerializeEvent( game_event* event, bf_write* buf ) = 0;

		// create new KeyValues, must be deleted
		virtual game_event* UnserializeEvent( bf_read* buf ) = 0;
	};
}
