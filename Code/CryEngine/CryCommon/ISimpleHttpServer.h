// -------------------------------------------------------------------------
// Crytek Source File.
// Copyright (C) Crytek GmbH, 2001-2008.
// -------------------------------------------------------------------------
#ifndef __ISIMPLEHTTPSERVER_H__
#define __ISIMPLEHTTPSERVER_H__

#pragma once

struct IHttpServerListener
{
	enum EResultDesc {eRD_Okay, eRD_Failed, eRD_AlreadyStarted};
	virtual void OnStartResult(bool started, EResultDesc desc) = 0;

	virtual void OnClientConnected(string client) = 0;
	virtual void OnClientDisconnected() = 0;

	virtual void OnGetRequest(string url) = 0;
	virtual void OnRpcRequest(string xml) = 0;

	// required by work queueing
	virtual void AddRef() const {}
	virtual void Release() const {}
	virtual bool IsDead() const { return false; }
};

struct ISimpleHttpServer
{
	// starts an HTTP server with a password using Digest Access Authentication method
	virtual void Start(uint16 port, const string& password, IHttpServerListener* pListener) = 0;

	// stops the HTTP server
	virtual void Stop() = 0;

	enum EStatusCode {eSC_Okay, eSC_BadRequest, eSC_NotFound, eSC_RequestTimeout, eSC_NotImplemented, eSC_ServiceUnavailable, eSC_UnsupportedVersion, eSC_InvalidStatus};
	enum EContentType {eCT_HTML, eCT_XML, eCT_MAX};
	virtual void SendResponse(EStatusCode statusCode, EContentType contentType, const string& content, bool closeConnection = false) = 0;

	virtual void SendWebpage(const string& webpage) = 0;
};

#endif

