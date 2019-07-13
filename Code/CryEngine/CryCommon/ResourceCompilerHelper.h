////////////////////////////////////////////////////////////////////////////
//
//  Crytek Engine Source File.
//  Copyright (C), Crytek Studios, 2002.
// -------------------------------------------------------------------------
//  File name:   ResourceCompilerHelper.h
//  Version:     v1.00
//  Created:     12/07/2004 by MartinM.
//  Compilers:   Visual Studio.NET
//  Description: 
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////

#ifndef __RESOURCECOMPILERIMAGEHELPER_H__
#define __RESOURCECOMPILERIMAGEHELPER_H__
#pragma once

#include <platform.h>														// string

#if defined(WIN32) || defined(WIN64)
#include <windows.h>														// HWND
#endif


class CResourceCompilerHelper
{
public:
#if (defined(WIN32) || defined(WIN64))				// run compiler only on developer platform

	// constructor
	CResourceCompilerHelper();

#if defined(_ICRY_PAK_HDR_)

	// checks file date and existence
	// Return:
	//   destination filename that should be loaded
	string ProcessIfNeeded( const char *szFilePath, const bool bReload )
	{
		string sFileToLoad=szFilePath;

		string ext = GetExt(sFileToLoad);

		string sDestFile = GetOutputFilename(sFileToLoad.c_str());

		for(uint32 dwIndex=0;;++dwIndex)		// check for all input files
		{
			string sSrcFile = GetInputFilename(sFileToLoad.c_str(),dwIndex);

			if(sSrcFile=="")
				break;					// last input file

			// compile if there is no destination
			// compare date of destination and source , recompile if needed
			// load dds header, check hash-value of the compile settings in the dds file, recompile if needed (not done yet)

			FILE* pDestFile = gEnv->pCryPak->FOpen(sDestFile.c_str(),"rb");
			FILE* pSrcFile = gEnv->pCryPak->FOpen(sSrcFile.c_str(),"rb");

			// files from the pak file do not count as date comparison do not seem to work there
			if(pDestFile)
			if(gEnv->pCryPak->IsInPak(pDestFile))
			{
				gEnv->pCryPak->FClose(pDestFile);pDestFile=0;
			}

			bool bInvokeResourceCompiler=false;

			// is there no destination file?
			if(pSrcFile && !pDestFile)
					bInvokeResourceCompiler=true;

			// if both files exist, is the source file newer?
			if(pDestFile && pSrcFile)
			{
				bInvokeResourceCompiler=true;

				ICryPak::FileTime timeSrc = gEnv->pCryPak->GetModificationTime(pSrcFile);
				ICryPak::FileTime timeDest = gEnv->pCryPak->GetModificationTime(pDestFile);

				if(timeDest>=timeSrc)
					bInvokeResourceCompiler=false;
			}

			if(pDestFile)
			{
				gEnv->pCryPak->FClose(pDestFile);pDestFile=0;
			}
			if(pSrcFile)
			{
				gEnv->pCryPak->FClose(pSrcFile);pSrcFile=0;
			}

			if(bInvokeResourceCompiler)
			{
				// call rc.exe
				if(!InvokeResourceCompiler(sSrcFile.c_str(),sDestFile.c_str(),DATA_FOLDER,false))		// false=no window
				{
					m_bErrorFlag=true;
					assert(!pSrcFile);assert(!pDestFile);		// internal error
					return szFilePath;	// rc failed
				}
			}

			assert(!pSrcFile);assert(!pDestFile);		// internal error
		}
		return sDestFile;	// load without using RC (e.g. TGA)
	}

#endif // _ICRY_PAK_HDR_

	//
	void ResourceCompilerUI( HWND hParent );

	// Arguments:
	//   szBin32Path - e.g. "c:/MasterCD/Bin32", must not be 0
	void SetBin32Path( const char *szBin32Path ) { assert(szBin32Path);m_sBin32Path=szBin32Path; }

	//
	bool SetDataToRegistry();

	//
	bool IsError() const;

	// you can test if the resource compiler is there with bool bOk=CallResourceCompilerForOneFile("")
	// Arguments:
	//   szFileName null terminated ABSOLUTE file path or 0 can be used to test for rc.exe existance, relative path needs to be relative to bin32/rc directory
	//   szAdditionalSettings - 0 or e.g. "/refresh" or "/refresh /xyz=56"
	// Returns:
	//   true=success, false=failed (you should open settings)
	bool CallResourceCompiler( const char *szFileName=0, const char *szAdditionalSettings=0 );

private: // ---------------------------------------------------------------------------

	bool							m_bErrorFlag;								// 
	string						m_sBin32Path;								//
	bool							m_bShowWindow;							//
	bool							m_bHideCustom;							//

	// ----------------------------------------------------------------------------------

	// Arguments:
	//   szExtension e.g. "tif"
	bool IsSourceFormat( const char *szExtension ) const;

	// Arguments:
	//   szExtension e.g. "tif"
	bool IsDestinationFormat( const char *szExtension ) const;

	// Arguments:
	//   szFilePath - could be source or destination filename
	//   dwIndex - used to iterator through all input filenames, start with 0 and increment by 1
	// Return:
	//   "" if that was the last input format, a valid filename otherwise
	string GetInputFilename( const char *szFilePath, const uint32 dwIndex ) const;	

	// Return:
	//   "" if failes, requested path otherwise
	void GetDataFromRegistry();

	// Arguments:
	//   szDataFolder usually DATA_FOLDER = "Game"
	bool InvokeResourceCompiler( const char *szSrcFile, const char *szDestFile, const char *szDataFolder, const bool bWindow ) const; 

#endif // defined(WIN32) && !defined(WIN64)

public: // -----------------------------------------------------------------------------------------

	// little helper function (to stay independent)
	static const char *GetExtension( const char *in )
	{
		const char *pRet=0;

		while(*in)
		{
			if(*in=='.')
				pRet=in;

			in++;
		}
		return pRet;
	}

	// little helper function (to stay independent)
	static string ReplaceExtension( const char *path, const char *new_ext )
	{
		const char *pDot = GetExtension(path);	// find last dot

		if(pDot)
			return string(path,pDot)+new_ext;
		else
			return string(path)+"."+new_ext;
	}

	// Arguments:
	//   szFilePath - could be source or destination filename
	static string GetOutputFilename( const char *szFilePath )
	{
		const char *ext = GetExtension(szFilePath);

		if(ext)
		{
			if(stricmp(ext,".tif")==0)
				return ReplaceExtension(szFilePath,".dds");
			if(stricmp(ext,".srf")==0)
				return ReplaceExtension(szFilePath,".dds");
		}

		return szFilePath;
	}

	// only for image formats supported by the resource compiler
	// Arguments:
	//   szExtension - e.g. ".tif", can be 0
	static bool IsImageFormat( const char *szExtension )
	{
		if(szExtension)
			if(	 stricmp(szExtension,"dds")==0				// DirectX surface format
				|| stricmp(szExtension,"tif")==0)		// Crytek resource compiler image input format	
				return true;

		return false;
	}

	//! Extract extension from full specified file path (copy to keep ResourceCompilerHelper independent from other code - needed as it is used in many projects)
	inline string GetExt( const string &filepath )
	{
		const char *str = filepath.c_str();
		for (const char* p = str + filepath.length()-1; p >= str; --p)
		{
			switch(*p)
			{
			case ':':
			case '/':
			case '\\':
				// we've reached a path separator - it means there's no extension in this name
				return string();
			case '.':
				// there's an extension in this file name
				return filepath.substr(p-str+1);
			}
		}
		return string();
	}
};


#endif // __RESOURCECOMPILERIMAGEHELPER_H
