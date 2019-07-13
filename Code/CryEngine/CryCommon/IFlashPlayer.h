// -------------------------------------------------------------------------
// Crytek Source File.
// Copyright (C) Crytek GmbH, 2001-2008.
// -------------------------------------------------------------------------
#ifndef _I_FLASH_PLAYER_H_
#define _I_FLASH_PLAYER_H_

#pragma once


struct IFSCommandHandler;
struct IExternalInterfaceHandler;
struct SFlashCursorEvent;
struct SFlashKeyEvent;


struct SFlashVarValue
{
	union Data
	{
		bool b;
		int i;
		unsigned int ui;
		double d;
		float f;
		const char* pStr;
		const wchar_t* pWstr;
	};

	enum Type
	{
		eUndefined,
		eBool,
		eInt,
		eUInt,
		eDouble,
		eFloat,
		eConstStrPtr,
		eConstWstrPtr,
	};

	SFlashVarValue(bool val)
		: type(eBool)
	{
		data.b = val;
	}
	SFlashVarValue(int val)
		: type(eInt)
	{
		data.i = val;
	}
	SFlashVarValue(unsigned int val)
		: type(eUInt)
	{
		data.ui = val;
	}
	SFlashVarValue(double val)
		: type(eDouble)
	{
		data.d = val;
	}
	SFlashVarValue(float val)
		: type(eFloat)
	{
		data.f = val;
	}
	SFlashVarValue(const char* val)
		: type(eConstStrPtr)
	{
		data.pStr = val;
	}
	SFlashVarValue(const wchar_t* val)
		: type(eConstWstrPtr)
	{
		data.pWstr = val;
	}
	static SFlashVarValue CreateUndefined()
	{
		return SFlashVarValue();
	}

	bool GetBool() const
	{
		assert(type == eBool);
		return data.b;
	}
	int GetInt() const
	{
		assert(type == eInt);
		return data.i;
	}
	int GetUInt() const
	{
		assert(type == eUInt);
		return data.ui;
	}
	double GetDouble() const
	{
		assert(type == eDouble);
		return data.d;
	}
	float GetFloat() const
	{
		assert(type == eFloat);
		return data.f;
	}
	const char* GetConstStrPtr() const
	{
		assert(type == eConstStrPtr);
		return data.pStr;
	}
	const wchar_t* GetConstWstrPtr() const
	{
		assert(type == eConstWstrPtr);
		return data.pWstr;
	}

	Type GetType() const
	{
		return type;
	}

protected:
	Type type;
	Data data;

private:
	// !!! don't publicly define default constructor to enforce efficient default initialization of argument lists !!!
	SFlashVarValue()
		: type(eUndefined)
	{
	}
};


enum EFlashVariableArrayType
{
	FVAT_Int,
	FVAT_Double,
	FVAT_Float,
	FVAT_ConstStrPtr,
	FVAT_ConstWstrPtr
	// !!! currently arrays of SFlashVarValue are not supported as it would require runtime conversion from / to Scaleform internal variant type 
	// GFxValue unless we enforce binary compatibility !!!
};


struct IFlashPlayer
{
	enum EOptions
	{
		//LOG_FLASH_LOADING			= 0x01, // logs loading of flash file
		//LOG_ACTION_SCRIPT			= 0x02, // logs action script
		RENDER_EDGE_AA				= 0x04, // enables edge anti-aliased flash rendering
		INIT_FIRST_FRAME			= 0x08, // init objects of frame #1 when creating instance of flash file
		ENABLE_MOUSE_SUPPORT	= 0x10, // enable mouse input support

		DEFAULT								= RENDER_EDGE_AA | INIT_FIRST_FRAME | ENABLE_MOUSE_SUPPORT,
		DEFAULT_NO_MOUSE			= RENDER_EDGE_AA | INIT_FIRST_FRAME
	};

	// lifetime
	virtual void Release() = 0;

	// initialization
	virtual bool Load(const char* pFilePath, unsigned int options = DEFAULT) = 0;
	
	// rendering
	virtual void SetBackgroundColor(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha) = 0;
	virtual void SetBackgroundAlpha(float alpha) = 0;
	virtual void SetViewport(int x0, int y0, int width, int height, float aspectRatio = 1.0f) = 0;
	virtual void GetViewport(int& x0, int& y0, int& width, int& height, float& aspectRatio) = 0;
	virtual void SetScissorRect(int x0, int y0, int width, int height) = 0;
	virtual void GetScissorRect(int& x0, int& y0, int& width, int& height) = 0;
	virtual void Advance(float deltaTime) = 0;
	virtual void Render() = 0;
	virtual void SetCompositingDepth(float depth) = 0;

	// callbacks & events
	virtual void SetFSCommandHandler(IFSCommandHandler* pHandler) = 0;
	virtual void SetExternalInterfaceHandler(IExternalInterfaceHandler* pHandler) = 0;
	virtual void SendCursorEvent(const SFlashCursorEvent& cursorEvent) = 0;
	virtual void SendKeyEvent(const SFlashKeyEvent& keyEvent) = 0;

	// visibility
	virtual void SetVisible(bool visible) = 0;
	virtual bool GetVisible() const = 0;

	// action script
	virtual bool SetVariable(const char* pPathToVar, const SFlashVarValue& value) = 0;
	virtual bool GetVariable(const char* pPathToVar, SFlashVarValue* pValue) const = 0;
	virtual bool IsAvailable(const char* pPathToVar) const = 0;
	virtual bool SetVariableArray(EFlashVariableArrayType type, const char* pPathToVar, unsigned int index, const void* pData, unsigned int count) const = 0;
	virtual unsigned int GetVariableArraySize(const char* pPathToVar) const = 0;
	virtual bool GetVariableArray(EFlashVariableArrayType type, const char* pPathToVar, unsigned int index, void* pData, unsigned int count) = 0;
	virtual bool Invoke(const char* pMethodName, const SFlashVarValue* pArgs, unsigned int numArgs, SFlashVarValue* pResult = 0) = 0;

	bool Invoke0(const char* pMethodName, SFlashVarValue* pResult = 0)
	{
		return Invoke(pMethodName, 0, 0, pResult);
	}
	bool Invoke1(const char* pMethodName, const SFlashVarValue& arg, SFlashVarValue* pResult = 0)
	{
		return Invoke(pMethodName, &arg, 1, pResult);
	}

	// general property queries
	virtual int GetWidth() const = 0;
	virtual int GetHeight() const = 0;
	virtual size_t GetMetadata(char* pBuff, unsigned int buffSize) const = 0;
	virtual const char* GetFilePath() const = 0;

	// coordinate translation
	virtual void ScreenToClient(int& x, int& y) const = 0;
	virtual void ClientToScreen(int& x, int& y) const = 0;

	// more to come...

protected:
	IFlashPlayer() {}
	virtual ~IFlashPlayer() {}
};


// clients of IFlashPlayer should implement this interface to receive action script events
struct IFSCommandHandler
{
	virtual void HandleFSCommand(const char* pCommand, const char* pArgs) = 0;
};


// clients of IFlashPlayer should implement this interface to expose external interface calls
struct IExternalInterfaceHandler
{
	virtual void HandleExternalInterfaceCall(const char* pMethodName, const SFlashVarValue* pArgs, int numArgs, SFlashVarValue* pResult) = 0;
};


// clients of IFlashPlayer should implement this interface to handle custom loadMovie API calls
struct IFlashLoadMovieImage
{
	virtual void Release() = 0;

	virtual int GetWidth() const = 0;
	virtual int GetHeight() const = 0;
	virtual int GetPitch() const = 0;
	virtual void* GetPtr() const = 0;

	enum EFmt
	{
		eFmt_None,
		eFmt_RGB_888,
		eFmt_ARGB_8888,
	};

	virtual EFmt GetFormat() const = 0;

	bool IsValid() const
	{
		return GetPtr() && GetPitch() > 0 && GetWidth() > 0 && GetHeight() > 0;
	}
};

struct IFlashLoadMovieHandler
{
	virtual IFlashLoadMovieImage* LoadMovie(const char* pFilePath) = 0;
};


// cursor input event sent to flash
struct SFlashCursorEvent
{
public:
	enum ECursorState
	{
		eCursorMoved,
		eCursorPressed,
		eCursorReleased
	};

	SFlashCursorEvent(ECursorState state, int32 cursorX, int32 cursorY)
	: m_state(state)
	, m_cursorX(cursorX)
	, m_cursorY(cursorY)
	{
	}

	ECursorState m_state;
	int32 m_cursorX;
	int32 m_cursorY;
};


// key input event sent to flash
struct SFlashKeyEvent
{
public:
	enum EKeyState
	{
		eKeyDown,
		eKeyUp
	};

	enum EKeyCode
	{
		VoidSymbol      = 0,

		// A through Z and numbers 0 through 9.
		A               = 65,
		B,
		C,
		D,
		E,
		F,
		G,
		H,
		I,
		J,
		K,
		L,
		M,
		N,
		O,
		P,
		Q,
		R,	
		S,
		T,
		U,
		V,
		W,
		X,
		Y,
		Z,
		Num0            = 48,
		Num1,
		Num2,
		Num3,
		Num4,
		Num5,
		Num6,
		Num7,
		Num8,
		Num9,

		// Numeric keypad.
		KP_0            = 96,
		KP_1,
		KP_2,
		KP_3,
		KP_4,
		KP_5,
		KP_6,
		KP_7,
		KP_8,
		KP_9,
		KP_Multiply,
		KP_Add,
		KP_Enter,
		KP_Subtract,
		KP_Decimal,
		KP_Divide,

		// Function keys.
		F1              = 112,
		F2,
		F3,
		F4,
		F5,
		F6,
		F7,
		F8,
		F9,
		F10,
		F11,
		F12,
		F13,
		F14,
		F15,

		// Other keys.
		Backspace       = 8,
		Tab,
		Clear           = 12,
		Return,
		Shift           = 16,
		Control,
		Alt,
		CapsLock        = 20, // Toggle
		Escape          = 27,
		Space           = 32,
		PageUp,
		PageDown,
		End             = 35,
		Home,
		Left,
		Up,
		Right,
		Down,
		Insert          = 45,
		Delete,
		Help,
		NumLock         = 144, // Toggle
		ScrollLock      = 145, // Toggle

		Semicolon       = 186,
		Equal           = 187,
		Comma           = 188, // Platform specific?
		Minus           = 189,
		Period          = 190, // Platform specific?
		Slash           = 191,
		Bar             = 192,
		BracketLeft     = 219,
		Backslash       = 220,
		BracketRight    = 221,
		Quote           = 222,

		// Total number of keys.
		KeyCount
	};

	enum ESpecialKeyState
	{
		eShiftPressed		= 0x01,
		eCtrlPressed		= 0x02,
		eAltPressed			= 0x04,
		eCapsToggled		= 0x08,
		eNumToggled			= 0x10,
		eScrollToggled	= 0x20
	};

	SFlashKeyEvent(EKeyState state, EKeyCode keyCode, unsigned char specialKeyState, unsigned char asciiCode, unsigned int wcharCode)
	: m_state(state)
	, m_keyCode(keyCode)
	, m_specialKeyState(specialKeyState)
	, m_asciiCode(asciiCode)
	, m_wcharCode(wcharCode)
	{
	}

	EKeyState m_state;
	EKeyCode m_keyCode;
	unsigned char m_specialKeyState;
	unsigned char m_asciiCode;
	unsigned int m_wcharCode;	
};


#endif //#ifndef _I_FLASH_PLAYER_H_
