/*************************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2001-2006.
-------------------------------------------------------------------------
$Id$
$DateTime$
Description: interface definition file for the Crysis remote control system
-------------------------------------------------------------------------
History:
- Created by Lin Luo, November 06, 2006
- Refactored by Lin Luo, November 20, 2006
*************************************************************************/

#ifndef __IREMOTECONTROL_H__
#define __IREMOTECONTROL_H__

#pragma once

struct IRemoteControlServer;
struct IRemoteControlClient;

struct IRemoteControlSystem
{
	virtual IRemoteControlServer* GetServerSingleton() = 0;
	virtual IRemoteControlClient* GetClientSingleton() = 0;
};

// the event handler on an RCON_server
struct IRemoteControlServerListener
{
	//
	enum EResultDesc {eRD_Okay, eRD_Failed, eRD_AlreadyStarted};
	virtual void OnStartResult(bool started, EResultDesc desc) = 0;

	// invoked when a client is authorized
	virtual void OnClientAuthorized(string clientAddr) = 0;

	// invoked when the authorized client disconnects
	virtual void OnAuthorizedClientLeft(string clientAddr) = 0;

	// invoked when a client command is received on the server
	virtual void OnClientCommand(uint32 commandId, string command) = 0;

	// required by work queueing
	virtual void AddRef() const {}
	virtual void Release() const {}
	virtual bool IsDead() const { return false; }
};

// the RCON_server interface
struct IRemoteControlServer
{
	// starts up an RCON_server
	virtual void Start(uint16 serverPort, const string& password, IRemoteControlServerListener* pListener) = 0;

	// stops the RCON_server
	virtual void Stop() = 0;

	// sends command result back to the command client
	virtual void SendResult(uint32 commandId, const string& result) = 0;
};

// the event handler on an RCON_client
struct IRemoteControlClientListener
{
	//
	enum EResultDesc {eRD_Okay, eRD_Failed, eRD_CouldNotResolveServerAddr, eRD_UnsupportedAddressType, eRD_ConnectAgain};
	virtual void OnConnectResult(bool okay, EResultDesc desc) = 0;

	// invoked when connection is authorized or not (raw TCP connection failure are considered unauthorized)
	enum EStatusDesc {eSD_Authorized, eSD_ConnectFailed, eSD_ServerSessioned, eSD_AuthFailed, eSD_AuthTimeout, eSD_ServerClosed, eSD_BogusMessage};
	virtual void OnSessionStatus(bool connected, EStatusDesc desc) = 0;

	// invoked when the RCON_client receives a command result
	virtual void OnCommandResult(uint32 commandId, string command, string result) = 0;

	// required by work queueing
	virtual void AddRef() const {}
	virtual void Release() const {}
	virtual bool IsDead() const { return false; }
};

// the RCON_client interface
struct IRemoteControlClient
{
	// connects to an RCON_server
	virtual void Connect(const string& serverAddr, uint16 serverPort, const string& password, IRemoteControlClientListener* pListener) = 0;

	// disconnects from an RCON_server
	virtual void Disconnect() = 0;

	// sends RCON commands to the connected RCON_server; returns a unique command ID
	virtual uint32 SendCommand(const string& command) = 0;
};

#endif

