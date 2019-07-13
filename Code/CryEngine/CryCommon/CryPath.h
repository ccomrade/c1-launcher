////////////////////////////////////////////////////////////////////////////
//
//  Crytek Engine Source File.
//  Copyright (C), Crytek Studios, 2002.
// -------------------------------------------------------------------------
//  File name:   CryPath.h
//  Version:     v1.00
//  Created:     19/10/2004 by Timur.
//  Compilers:   Visual Studio.NET
//  Description: Defines namespace PathUtil for operations on files paths.
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////

#ifndef __CryPath_h__
#define __CryPath_h__
#pragma once

#ifndef DATA_FOLDER
#define DATA_FOLDER "game"
#endif

#include <ISystem.h>
#include <ICryPak.h>

namespace PathUtil
{
	//! Split full file name to path and filename
	//! @param filepath [IN] Full file name inclusing path.
	//! @param path [OUT] Extracted file path.
	//! @param file [OUT] Extracted file (with extension).
	inline void Split( const string &filepath,string &path,string &file )
	{
		string fext;
		Split(filepath,path,file);
	}

	//! Convert a path to the uniform form.
	inline string ToUnixPath( const string& strPath )
	{
		if (strPath.find('\\') != string::npos)
		{
			string path = strPath;
			path.replace( '\\','/' );
			return path;
		}
		return strPath;
	}

  //! Convert a path to the DOS form.
  inline string ToDosPath( const string& strPath )
  {
    if (strPath.find('/') != string::npos)
    {
      string path = strPath;
			path.replace( '/','\\' );
      return path;
    }
    return strPath;
  }

	//! Split full file name to path and filename
	//! @param filepath [IN] Full file name including path.
	//! @param path [OUT] Extracted file path.
	//! @param filename [OUT] Extracted file (without extension).
	//! @param ext [OUT] Extracted files extension.
	inline void Split( const string &filepath,string &path,string &filename,string &fext )
	{
		path = filename = fext = string();
		if (filepath.empty())
			return;
		const char *str = filepath.c_str();
		const char* pext = str + filepath.length()-1;
		for (const char* p = str + filepath.length()-1; p >= str; --p)
		{
			switch(*p)
			{
			case ':':
			case '/':
			case '\\':
				path = filepath.substr(0,p-str+1);
				filename = filepath.substr(p-str+1,pext-p);
				return;
			case '.':
				// there's an extension in this file name
				fext = filepath.substr(p-str+1);
				pext = p;
				break;
			}
		}
		filename = filepath;
	}

	// Extract extension from full specified file path
	// Returns
	//   pointer to the extension (without .) or pointer to an empty 0-terminated string
	inline const char* GetExt( const char *filepath )
	{
		const char *str = filepath;
		int len = strlen(filepath);
		for (const char* p = str + len-1; p >= str; --p)
		{
			switch(*p)
			{
				case ':':
				case '/':
				case '\\':
					// we've reached a path separator - it means there's no extension in this name
					return "";
				case '.':
					// there's an extension in this file name
					return p+1;
			}
		}
		return "";
	}

	//! Extract path from full specified file path.
	inline string GetPath( const string &filepath )
	{
		const char *str = filepath.c_str();
		for (const char* p = str + filepath.length()-1; p >= str; --p)
		{
			switch(*p)
			{
			case ':':
			case '/':
			case '\\':
				return filepath.substr(0,p-str+1);
			}
		}
		return filepath;
	}

	//! Extract file name with extension from full specified file path.
	inline string GetFile( const string &filepath )
	{
		const char *str = filepath.c_str();
		for (const char* p = str + filepath.length()-1; p >= str; --p)
		{
			switch(*p)
			{
			case ':':
			case '/':
			case '\\':
				return filepath.substr(p-str+1);
			}
		}
		return filepath;
	}

	inline const char* GetFile(const char* filepath)
	{
		const size_t len = strlen(filepath);
		for (const char* p = filepath + len -1; p >= filepath; --p)
		{
			switch(*p)
			{
			case ':':
			case '/':
			case '\\':
				return p+1;
			}
		}
		return filepath;
	}

	//! Replace extension for given file.
	inline void RemoveExtension( string &filepath )
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
				return;
			case '.':
				// there's an extension in this file name
				filepath = filepath.substr(0,p-str);
				return;
			}
		}
		// it seems the file name is a pure name, without path or extension
	}

	//! Extract file name without extension from full specified file path.
	inline string GetFileName( const string &filepath )
	{
		string file = filepath;
		RemoveExtension(file);
		return GetFile(file);
	}

	//! Removes the trailing slash or backslash from a given path.
	inline string RemoveSlash( const string &path )
	{
		if (path.empty() || (path[path.length()-1] != '/' && path[path.length()-1] != '\\'))
			return path;
		return path.substr(0,path.length()-1);
	}

	//! add a backslash if needed
	inline string AddSlash( const string &path )
	{
		if (path.empty() || path[path.length()-1] == '/')
			return path;
		if (path[path.length()-1] == '\\')
			return path.substr(0,path.length()-1) + "/";
		return path + "/";
	}

	//! Replace extension for given file.
	inline string ReplaceExtension( const string &filepath,const string &ext )
	{
		string str = filepath;
		RemoveExtension(str);
		if (!ext.empty() && ext[0] != '.')
		{
			str += ".";
		}
		str += ext;
		return str;
	}

	//! Makes a fully specified file path from path and file name.
	inline string Make( const string &path,const string &file )
	{
		return AddSlash(path) + file;
	}

	//! Makes a fully specified file path from path and file name.
	inline string Make( const string &dir,const string &filename,const string &ext )
	{
		string path = ReplaceExtension(filename,ext);
		path = AddSlash(dir) + path;
		return path;
	}

	//! Makes a fully specified file path from path and file name.
	inline string MakeFullPath( const string &relativePath )
	{
		return relativePath;
	}

	//////////////////////////////////////////////////////////////////////////
	// Description:
	//    Make a game correct path out of any input path.
	inline string MakeGamePath( const string &path )
	{
		string fullpath = MakeFullPath(path);
		string rootDataFolder = ToUnixPath(AddSlash(DATA_FOLDER));
		if (fullpath.length() > rootDataFolder.length() && strnicmp(fullpath.c_str(),rootDataFolder.c_str(),rootDataFolder.length()) == 0)
		{
			return fullpath.substr(rootDataFolder.length(), fullpath.length()-rootDataFolder.length());
		}
		//fullpath = GetRelativePath(path);
		return fullpath;
	}

	inline string GetParentDirectory (const string& strFilePath, int nGeneration = 1)
	{
		for (const char* p = strFilePath.c_str() + strFilePath.length() - 2; // -2 is for the possible trailing slash: there always must be some trailing symbol which is the file/directory name for which we should get the parent
			p >= strFilePath.c_str();
			--p)
		{
			switch (*p)
			{
			case ':':
				return string (strFilePath.c_str(), p);
				break;
			case '/':
			case '\\':
				// we've reached a path separator - return everything before it.
				if (!--nGeneration)
					return string(strFilePath.c_str(), p);
				break;
			}
		};
		// it seems the file name is a pure name, without path or extension
		return string();
	}

	// returns true if the string matches the wildcard
	inline bool MatchWildcard (const char* szString, const char* szWildcard)
	{
		const char* pString = szString, *pWildcard = szWildcard;
		// skip the obviously the same starting substring
		while (*pWildcard && *pWildcard != '*' && *pWildcard != '?')
			if (*pString != *pWildcard)
				return false; // must be exact match unless there's a wildcard character in the wildcard string
			else
				++pString, ++pWildcard;

		if (!*pString)
		{
			// this will only match if there are no non-wild characters in the wildcard
			for(; *pWildcard; ++pWildcard)
				if (*pWildcard != '*' && *pWildcard != '?')
					return false;
			return true;
		}

		switch(*pWildcard)
		{
		case '\0':
			return false; // the only way to match them after the leading non-wildcard characters is !*pString, which was already checked

			// we have a wildcard with wild character at the start.
		case '*':
			{
				// merge consecutive ? and *, since they are equivalent to a single *
				while (*pWildcard == '*' || *pWildcard == '?')
					++pWildcard;

				if (!*pWildcard)
					return true; // the rest of the string doesn't matter: the wildcard ends with *

				for (; *pString; ++pString)
					if (MatchWildcard(pString, pWildcard))
						return true;

				return false;
			}
		case '?':
			return MatchWildcard(pString+1, pWildcard + 1) || MatchWildcard(pString, pWildcard+1);
		default:
			assert (0);
			return false;
		}
	}

	inline string GetGameFolder() 
	{
		return (gEnv->pCryPak->GetAlias(DATA_FOLDER));
	}
};

#endif //__CryPath_h__
