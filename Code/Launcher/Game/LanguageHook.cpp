#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "CryCommon/CrySystem/IConsole.h"
#include "CryCommon/CrySystem/ISystem.h"

#include "Library/OS.h"
#include "Library/StringFormat.h"

#include "LanguageHook.h"

struct ILocalizationManager
{
	virtual bool SetLanguage(const char* language) = 0;

	// ...
};

static const char* GetLanguageFromSystem()
{
	static const struct { char code[2 + 1]; const char* name; } languages[] = {
		{ "en", "English" },
		{ "de", "German" },
		{ "cs", "Czech" },
		{ "sk", "Czech" },  // Slovak -> Czech
		{ "pl", "Polish" },
		{ "es", "Spanish" },
		{ "fr", "French" },
		{ "it", "Italian" },
		{ "hu", "Hungarian" },
		{ "ru", "Russian" },
		{ "be", "Russian" },  // Belarusian -> Russian
	//	{ "uk", "Russian" },  // Ukrainian -> Russian, disabled for political reasons
		{ "tr", "Turkish" },
		{ "ja", "Japanese" },
		{ "zn", "Chinese" },
		{ "th", "Thai" },
	};

	const int languageCount = sizeof(languages) / sizeof(languages[0]);

	char code[8] = {};
	OS::GetSystemLanguageCode(code, sizeof(code));

	for (int i = 0; i < languageCount; i++)
	{
		if (std::memcmp(languages[i].code, code, 2 + 1) == 0)
		{
			return languages[i].name;
		}
	}

	return NULL;
}

static bool LanguagePakExists(const char* language)
{
	std::FILE* pak = std::fopen(StringFormat("Game/Localized/%s.pak", language).c_str(), "rb");
	if (!pak)
	{
		return false;
	}

	std::fclose(pak);

	return true;
}

static const char* ChooseLanguage(const char* defaultLanguage, ICVar* pLanguageCVar)
{
	const char* language = OS::CmdLine::GetArgValue("-language");
	if (*language)
	{
		return language;
	}

	if (pLanguageCVar)
	{
		const char* value = pLanguageCVar->GetString();
		if (*value)
		{
			return value;
		}
	}

	language = defaultLanguage;
	if (!language || !*language)
	{
		CryLogErrorAlways("[Localization] Missing or invalid Game/Localized/Default.lng file!");
		CryLogErrorAlways("[Localization] Trying to guess language from the system!");
		language = GetLanguageFromSystem();

		if (!language)
		{
			CryLogErrorAlways("[Localization] Failed to guess language from the system!");
			CryLogErrorAlways("[Localization] Falling back to English language!");
			language = "English";
		}
	}

	bool exists = LanguagePakExists(language);
	if (!exists)
	{
		if (std::strcmp(language, "English") == 0)
		{
			CryLogErrorAlways("[Localization] Not even English language exists!");
		}
		else
		{
			CryLogErrorAlways("[Localization] %s language does not exist!", language);
			CryLogErrorAlways("[Localization] Falling back to English language!");
			language = "English";

			exists = LanguagePakExists(language);
			if (!exists)
			{
				CryLogErrorAlways("[Localization] Not even English language exists!");
			}
		}
	}

	if (!exists)
	{
		CryLogErrorAlways("[Localization] No suitable language found!");

		OS::ErrorBox(
			"No suitable language found!\n"
			"\n"
			"Localization files are incomplete!\n"
			"This is a known issue in the Steam version of Crysis.\n"
			"\n"
			"You can try the following:\n"
			"    1. Go to Game/Localized\n"
			"    2. Choose a suitable *.lng file\n"
			"    3. Make a copy of that file\n"
			"    4. Rename the copy to Default.lng\n"
			"\n"
			"One or more *.pak files of the chosen language must exist!"
		);

		// throwing an exception through the engine is undefined behavior
		std::exit(1);
	}

	return language;
}

void LanguageHook::OnInit(const char* defaultLanguage, ILocalizationManager* pLocalizationManager)
{
	ICVar* pLanguageCVar = gEnv->pConsole->GetCVar("g_language");
	const char* language = ChooseLanguage(defaultLanguage, pLanguageCVar);

	CryLogAlways("[Localization] Using %s language", language);

	if (pLanguageCVar)
	{
		pLanguageCVar->Set(language);
	}

	pLocalizationManager->SetLanguage(language);
}
