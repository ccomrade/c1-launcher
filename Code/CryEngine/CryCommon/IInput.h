// -------------------------------------------------------------------------
// Crytek Source File.
// Copyright (C) Crytek GmbH, 2001-2008.
// -------------------------------------------------------------------------
#ifndef __IINPUT_H__
#define __IINPUT_H__

#ifdef CRYINPUT_EXPORTS
	#define CRYINPUT_API DLL_EXPORT
#else
	#define CRYINPUT_API DLL_IMPORT
#endif

#include <platform.h>
#include <CryName.h>
#include <vector>
#include <map>

enum EModifierMask
{
	eMM_None				= 0,
	eMM_LCtrl				= (1 << 0),
	eMM_LShift			= (1 << 1),
	eMM_LAlt				= (1 << 2),
	eMM_LWin				= (1 << 3),
	eMM_RCtrl				= (1 << 4),
	eMM_RShift			= (1 << 5),
	eMM_RAlt				= (1 << 6),
	eMM_RWin				= (1 << 7),
	eMM_NumLock			= (1 << 8),
	eMM_CapsLock		= (1 << 9),
	eMM_ScrollLock	= (1 << 10),

	eMM_Ctrl				= (eMM_LCtrl | eMM_RCtrl),
	eMM_Shift				= (eMM_LShift | eMM_RShift),
	eMM_Alt					= (eMM_LAlt | eMM_RAlt),
	eMM_Win					= (eMM_LWin | eMM_RWin),
	eMM_Modifiers		= (eMM_Ctrl | eMM_Shift | eMM_Alt | eMM_Win),
	eMM_LockKeys		= (eMM_CapsLock | eMM_NumLock | eMM_ScrollLock)
};


enum EDeviceId
{
	eDI_Keyboard	= 0,
	eDI_Mouse			= 1,
	eDI_XI				= 2,
	eDI_Unknown		= 0xff,
};

enum EFFEffectId
{
	eFF_Rumble_Basic = 0
};

//! Input Event types.
enum EInputState
{
	eIS_Unknown		= 0,
	eIS_Pressed		= (1 << 0),
	eIS_Released	= (1 << 1),
	eIS_Down			= (1 << 2),
	eIS_Changed		= (1 << 3),
	eIS_UI				= (1 << 4),
};

enum EInputDeviceType
{
	eIDT_Keyboard,
	eIDT_Mouse,
	eIDT_Joystick,
	eIDT_Gamepad,
};

struct TKeyName
{
	const char *key;

	TKeyName() { key = ""; }
	TKeyName( const char *_key ) { key = _key; };
	operator const char*() const { return key; };
	bool operator<( const TKeyName &n ) const { return stricmp(key,n.key) < 0; }
	bool operator>( const TKeyName &n ) const { return stricmp(key,n.key) > 0; }
	bool operator==( const TKeyName &n ) const { return stricmp(key,n.key) == 0; }
	bool operator!=( const TKeyName &n ) const { return stricmp(key,n.key) != 0; }
	bool operator<( const char *str ) const { return stricmp(key,str) < 0; }
	bool operator>( const char *str ) const { return stricmp(key,str) > 0; }
	bool operator==( const char *str ) const { return stricmp(key,str) == 0; }
	bool operator!=( const char *str ) const { return stricmp(key,str) != 0; }
	const char *c_str() const { return key; }
};
inline bool operator==( const char *str,TKeyName &n ) { return n == str; }
inline bool operator!=( const char *str,TKeyName &n ) { return n != str; }
inline bool operator<( const char *str,TKeyName &n ) { return n < str; }
inline bool operator>( const char *str,TKeyName &n ) { return n > str; }

#define KI_KEYBOARD_BASE	0
#define KI_MOUSE_BASE			256
#define KI_XINPUT_BASE		512
#define KI_PS3_BASE				1024
#define KI_SYS_BASE				2048

enum EKeyId
{
	eKI_Escape = KI_KEYBOARD_BASE,
	eKI_1,
	eKI_2,
	eKI_3,
	eKI_4,
	eKI_5,
	eKI_6,
	eKI_7,
	eKI_8,
	eKI_9,
	eKI_0,
	eKI_Minus,
	eKI_Equals,
	eKI_Backspace,
	eKI_Tab,
	eKI_Q,
	eKI_W,
	eKI_E,
	eKI_R,
	eKI_T,
	eKI_Y,
	eKI_U,
	eKI_I,
	eKI_O,
	eKI_P,
	eKI_LBracket,
	eKI_RBracket,
	eKI_Enter,
	eKI_LCtrl,
	eKI_A,
	eKI_S,
	eKI_D,
	eKI_F,
	eKI_G,
	eKI_H,
	eKI_J,
	eKI_K,
	eKI_L,
	eKI_Semicolon,
	eKI_Apostrophe,
	eKI_Tilde,
	eKI_LShift,
	eKI_Backslash,
	eKI_Z,
	eKI_X,
	eKI_C,
	eKI_V,
	eKI_B,
	eKI_N,
	eKI_M,
	eKI_Comma,
	eKI_Period,
	eKI_Slash,
	eKI_RShift,
	eKI_NP_Multiply,
	eKI_LAlt,
	eKI_Space,
	eKI_CapsLock,
	eKI_F1,
	eKI_F2,
	eKI_F3,
	eKI_F4,
	eKI_F5,
	eKI_F6,
	eKI_F7,
	eKI_F8,
	eKI_F9,
	eKI_F10,
	eKI_NumLock,
	eKI_ScrollLock,
	eKI_NP_7,
	eKI_NP_8,
	eKI_NP_9,
	eKI_NP_Substract,
	eKI_NP_4,
	eKI_NP_5,
	eKI_NP_6,
	eKI_NP_Add,
	eKI_NP_1,
	eKI_NP_2,
	eKI_NP_3,
	eKI_NP_0,
	eKI_F11,
	eKI_F12,
	eKI_F13,
	eKI_F14,
	eKI_F15,
	eKI_Colon,
	eKI_Underline,
	eKI_NP_Enter,
	eKI_RCtrl,
	eKI_NP_Period,
	eKI_NP_Divide,
	eKI_Print,
	eKI_RAlt,
	eKI_Pause,
	eKI_Home,
	eKI_Up,
	eKI_PgUp,
	eKI_Left,
	eKI_Right,
	eKI_End,
	eKI_Down,
	eKI_PgDn,
	eKI_Insert,
	eKI_Delete,
	eKI_LWin,
	eKI_RWin,
	eKI_Apps,
	eKI_OEM_102,

	// mouse
	eKI_Mouse1 = KI_MOUSE_BASE,
	eKI_Mouse2,
	eKI_Mouse3,
	eKI_Mouse4,
	eKI_Mouse5,
	eKI_Mouse6,
	eKI_Mouse7,
	eKI_Mouse8,
	eKI_MouseWheelUp, 
	eKI_MouseWheelDown,
	eKI_MouseX,
	eKI_MouseY,
	eKI_MouseZ,
	eKI_MouseLast,
	
	// X360 controller
	eKI_XI_DPadUp = KI_XINPUT_BASE,
	eKI_XI_DPadDown,
	eKI_XI_DPadLeft,
	eKI_XI_DPadRight,
	eKI_XI_Start,
	eKI_XI_Back,
	eKI_XI_ThumbL,
	eKI_XI_ThumbR,
	eKI_XI_ShoulderL,
	eKI_XI_ShoulderR,
	eKI_XI_A,
	eKI_XI_B,
	eKI_XI_X,
	eKI_XI_Y,
	eKI_XI_TriggerL,
	eKI_XI_TriggerR,
	eKI_XI_ThumbLX,
	eKI_XI_ThumbLY,
	eKI_XI_ThumbLUp,
	eKI_XI_ThumbLDown,
	eKI_XI_ThumbLLeft,
	eKI_XI_ThumbLRight,
	eKI_XI_ThumbRX,
	eKI_XI_ThumbRY,
	eKI_XI_ThumbRUp,
	eKI_XI_ThumbRDown,
	eKI_XI_ThumbRLeft,
	eKI_XI_ThumbRRight,
	eKI_XI_TriggerLBtn,
	eKI_XI_TriggerRBtn,
	eKI_XI_Connect,
	eKI_XI_Disconnect,

	// PS3 controller
	eKI_PS3_Select = KI_PS3_BASE,
	eKI_PS3_L3,
	eKI_PS3_R3,
	eKI_PS3_Start,
	eKI_PS3_Up,
	eKI_PS3_Right,
	eKI_PS3_Down,
	eKI_PS3_Left,
	eKI_PS3_L2,
	eKI_PS3_R2,
	eKI_PS3_L1,
	eKI_PS3_R1,
	eKI_PS3_Triangle,
	eKI_PS3_Circle,
	eKI_PS3_Cross,
	eKI_PS3_Square,
	eKI_PS3_StickLX,
	eKI_PS3_StickLY,
	eKI_PS3_StickRX,
	eKI_PS3_StickRY,
	eKI_PS3_RotX,
	eKI_PS3_RotY,
	eKI_PS3_RotZ,
	eKI_PS3_ROTX_KeyL,
	eKI_PS3_ROTX_KeyR,
	eKI_PS3_ROTZ_KeyD,
	eKI_PS3_ROTZ_KeyU,

	eKI_SYS_Commit = KI_SYS_BASE,
	// terminator
	eKI_Unknown		= 0xffffffff,
};

/*! InputEvents are generated by input system and dispatched to all event listeners.
*/

struct SInputSymbol;

struct SInputEvent
{
	EDeviceId		deviceId;		// which device did the event originate from
	EInputState	state;			// type of input event
	TKeyName		keyName;		// human readable name of the event
	EKeyId			keyId;			// device-specific id corresponding to the event
	uint32			timestamp;	// timestamp of the event, (GetTickCount compatible)
	int					modifiers;	// key modifiers enabled at the time of this event
	float				value;			// value associated with the event
	SInputSymbol*	pSymbol;	// input symbol the event originated from

	SInputEvent()
	{
		deviceId	= eDI_Unknown;
		state			= eIS_Unknown;
		keyId			= eKI_Unknown;
		modifiers	= eMM_None;
		timestamp	= 0;
		value			= 0;
		keyName   = "";
		pSymbol		= 0;
	}
};

/*! SFFOutputEvents are force feedback signals send to an input controller
*/
struct SFFOutputEvent
{
	EDeviceId		deviceId;	// which device will receive the event
	EFFEffectId	eventId;
	float				amplifierS, amplifierA;
	float				timeInSeconds;
	SFFOutputEvent()
	{
		deviceId	= eDI_Unknown;
		eventId	= eFF_Rumble_Basic;
		amplifierS = 1.0f;
		amplifierA = 1.0f;
		timeInSeconds = 0.3f;
	}
	SFFOutputEvent(EDeviceId id, EFFEffectId event, float time = 1.0f, float ampA = 1.0f, float ampB = 1.0f) :
			deviceId(id), eventId(event), timeInSeconds(time), amplifierS(ampA), amplifierA(ampB) 
	{}
};

struct SInputSymbol
{
	//! input symbol type
	enum EType
	{
		Button,		// state == press/hold/release -- value = 0, 1
		Toggle,		// transition state with a press
		RawAxis,	// state == change -- value = movement of axis
		Axis,			// state == change -- value = -1.0 to 1.0
		Trigger,	// state == change -- value = 0.0 to 1.0
	};

	SInputSymbol(uint32 devSpecId_, EKeyId keyId_, const TKeyName& name_, EType type_, uint32 user_ = 0)
		: devSpecId(devSpecId_)
		,	keyId(keyId_), name(name_)
		, state(eIS_Unknown)
		, type(type_)
		, value(0.0f)
		, user(user_)
	{
	}

	void PressEvent(bool pressed)
	{
		if (pressed)
		{
			state = eIS_Pressed;
			value = 1.0f;
		}
		else
		{
			state = eIS_Released;
			value = 0.0f;
		}
	}
	void ChangeEvent(float v)
	{
		state = eIS_Changed;
		value = v;
	}
	void AssignTo(SInputEvent& event, int modifiers=0)
	{
		event.pSymbol = this;
		event.deviceId= deviceId;
		event.modifiers= modifiers;
		event.state		= state;
		event.value		= value;
		event.keyName = name;
		event.keyId		= keyId;
	}

	const EKeyId		keyId;	// external id for fast comparison
	const TKeyName	name;		// human readable name of the event
	const uint32		devSpecId;	// device internal id of this symbol (we will use it to look it up)
	EInputState			state;	// current state
	const EType			type;		// type of this symbol
	float						value;	// current value
	uint32					user;		// type dependent value (toggle-mask for toggles)
	EDeviceId				deviceId;		// which device does the symbol belong to
};

//////////////////////////////////////////////////////////////////////////
/* Input event listeners registered to input system and receive input events when they are generated.
*/
struct IInputEventListener
{
	//! Called every time input event is generated.
	//! @return if return True then broadcasting of this event should be aborted and the rest of input 
	//! listeners should not receive this event.
	virtual bool OnInputEvent( const SInputEvent &event ) = 0;
	virtual bool OnInputEventUI( const SInputEvent &event ) {	return false;	}
};

/*! Interface to the Input system.
The input system give access and initialize Keyboard,Mouse and Joystick SubSystems.
*/ 

////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
/*! Main Input system interface.
 */
struct IInput
{
	//////////////////////////////////////////////////////////////////////////
	//! Register new input events listener.
	virtual void AddEventListener( IInputEventListener *pListener ) = 0;
	virtual void RemoveEventListener( IInputEventListener *pListener ) = 0;

	//! Register new console input event listeners. console input listeners receive all events, no matter what.
	virtual void AddConsoleEventListener( IInputEventListener *pListener ) = 0;
	virtual void RemoveConsoleEventListener( IInputEventListener *pListener ) = 0;

	//! Register an exclusive listener which has the ability to filter out events before they arrive at the normal
	//! listeners
	virtual void SetExclusiveListener( IInputEventListener *pListener ) = 0;
	virtual IInputEventListener *GetExclusiveListener() = 0;

	//! Register an exclusive listener which has the ability to filter out events before they arrive at the normal
	//! listeners
	virtual void EnableEventPosting ( bool bEnable ) = 0;
	virtual void PostInputEvent( const SInputEvent &event ) = 0;

	//! post a force feedback / rumble output event
	virtual void ForceFeedbackEvent( const SFFOutputEvent &event ) = 0;

	//////////////////////////////////////////////////////////////////////////
	//! init input system (required params should be passed through constructor)
	virtual bool	Init() = 0;
	//! update Keyboard, Mouse and Joystick. Set bFocus to true if window has focus and input is enabled.
	virtual void	Update(bool bFocus) = 0;
	//! clear all subsystems
	virtual void	ShutDown() = 0;

	//! @see IInputDevice::SetExclusive
	virtual void	SetExclusiveMode(EDeviceId deviceId, bool exclusive,void *hwnd=0) = 0;

	//! @see IInputDevice::InputState
	virtual bool	InputState(const TKeyName& key, EInputState state) = 0;

	//! Convert an input event to the key name. Should internally dispatch to all managed input devices and
	//! return the first recognized event.
	//! @param event input event to translate into a name
	virtual const char *GetKeyName(const SInputEvent& event, bool bGUI = 0) = 0;

	// Lookup a symbol for a given symbol and key ids.
	virtual SInputSymbol* LookupSymbol( EDeviceId deviceId, EKeyId keyId ) = 0;

	//! Get OS Keyname 
	//! @param event input event to translate into a name
	virtual const wchar_t* GetOSKeyName(const SInputEvent& event) = 0;

	//! clear key states of all devices
	virtual void ClearKeyState() = 0;

	//! retriggers pressed keys (used for transitioning action maps)
	virtual void RetriggerKeyState() = 0;

	//! are we currently retriggering (needed to filter out actions)
	virtual bool Retriggering() = 0;

	// query to see if this machine has some kind of input device connected
	virtual bool HasInputDeviceOfType( EInputDeviceType type ) = 0;

	// get the currently pressed modifiers 
	virtual int GetModifiers() const = 0;

	//! tell devices whether to report input or not
	virtual void EnableDevice( EDeviceId deviceId, bool enable) = 0;
};



#ifdef __cplusplus
extern "C" {
#endif

typedef IInput  (*  CRY_PTRCREATEINPUTFNC(ISystem *pSystem, void* hwnd));

CRYINPUT_API IInput *CreateInput(ISystem *pSystem, void* hwnd);

#ifdef __cplusplus
};
#endif


#endif //__IINPUT_H__
