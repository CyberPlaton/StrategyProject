#include "Localization.h"

Localization* Localization::g_localization = nullptr;
size_t Localization::m_currentLanguage = 0;

Localization::Localization()
{
}
Localization::~Localization()
{
}
cherrysoda::String Localization::GetLocalizedString(loc_strings::ELocalizedString s)
{
	return _getLocString(s, m_currentLanguage);
}
cherrysoda::String Localization::GetLocalizedString(const cherrysoda::String& string)
{
	if (string.compare("Main Menu") == 0)
	{
		return GetLocalizedString(loc_strings::ELocalizedString::LS_MAIN_MENU);
	}
	else if (string.compare("Options") == 0)
	{
		return GetLocalizedString(loc_strings::ELocalizedString::LS_OPTIONS);
	}
	else if (string.compare("Exit") == 0)
	{
		return GetLocalizedString(loc_strings::ELocalizedString::LS_EXIT_GAME);
	}
	else if (string.compare("Single Player") == 0)
	{
		return GetLocalizedString(loc_strings::ELocalizedString::LS_SINGLE_PLAYER);
	}
	else
	{
		return u8"String not found \"" + string + "\"";
	}
}
void Localization::SetLanguage(const cherrysoda::String& name)
{
	if (name.compare("English") == 0)
	{
		m_currentLanguage = m_locEnglishId;
	}
	else if (name.compare("German") == 0)
	{
		m_currentLanguage = m_locGermanId;
	}
	else if (name.compare("Russian") == 0)
	{
		m_currentLanguage = m_locRussianId;
	}
	else
	{
		// Set Default to English.
		m_currentLanguage = m_locEnglishId;
	}
}
bool Localization::Initialize()
{
	Localization::get();
	return true;
}
cherrysoda::String Localization::_getLocString(loc_strings::ELocalizedString s, size_t lang)
{
	switch (s)
	{
	case loc_strings::ELocalizedString::LS_MAIN_MENU:
		switch (lang)
		{
		case 0:
			return loc_strings::main_menu::english;
		case 1:
			return loc_strings::main_menu::german;
		case 2:
			return loc_strings::main_menu::russian;
		}


	case loc_strings::ELocalizedString::LS_OPTIONS:
		switch (lang)
		{
		case 0:
			return loc_strings::options::english;
		case 1:
			return loc_strings::options::german;
		case 2:
			return loc_strings::options::russian;
		}

	case loc_strings::ELocalizedString::LS_EXIT_GAME:
		switch (lang)
		{
		case 0:
			return loc_strings::exit_game::english;
		case 1:
			return loc_strings::exit_game::german;
		case 2:
			return loc_strings::exit_game::russian;
		}

	case loc_strings::ELocalizedString::LS_SINGLE_PLAYER:
		switch (lang)
		{
		case 0:
			return loc_strings::single_player::english;
		case 1:
			return loc_strings::single_player::german;
		case 2:
			return loc_strings::single_player::russian;
		}




	}
}