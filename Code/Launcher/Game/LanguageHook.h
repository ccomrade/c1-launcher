#pragma once

struct ILocalizationManager;

namespace LanguageHook
{
	void OnInit(const char* defaultLanguage, ILocalizationManager* pLocalizationManager);
}
