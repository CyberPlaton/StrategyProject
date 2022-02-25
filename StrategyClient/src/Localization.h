#pragma once

#include <CherrySoda/Util/String.h>

#include <stdio.h>
#include <string>


namespace loc_strings
{
	// WHICH STRINGS ARE LOCALIZED
	enum ELocalizedString
	{
		LS_MAIN_MENU,
		LS_OPTIONS,
		LS_EXIT_GAME,
		LS_SINGLE_PLAYER,
	};


	// LOCALIZED STRINGS USED IN-GAME
	namespace main_menu
	{
		static const cherrysoda::String english =		u8"Main Menu"; 
		static const cherrysoda::String german =		u8"Hauptmenü";
		static const cherrysoda::String russian =		u8"Главное меню";
	};
	namespace options
	{
		static const cherrysoda::String english = u8"Options";
		static const cherrysoda::String german = u8"Optionen";
		static const cherrysoda::String russian = u8"Опции";
	};
	namespace single_player
	{
		static const cherrysoda::String english = u8"Single Player";
		static const cherrysoda::String german = u8"Einzelspieler";
		static const cherrysoda::String russian = u8"Одиночная игра";
	};
	namespace exit_game
	{
		static const cherrysoda::String english = u8"Exit";
		static const cherrysoda::String german = u8"Verlassen";
		static const cherrysoda::String russian = u8"Выйти";
	};


}


#define STATIC_GET_DEL(CLASS, STATIC_MEMBER) \
static CLASS* get() \
{ if(!STATIC_MEMBER){STATIC_MEMBER = new CLASS();} return STATIC_MEMBER;} \
static void del() \
{if(STATIC_MEMBER){delete STATIC_MEMBER; STATIC_MEMBER = nullptr;}}


class Localization
{
public:
	STATIC_GET_DEL(Localization, g_localization);

	static bool Initialize();

	static cherrysoda::String GetLocalizedString(loc_strings::ELocalizedString s);
	static cherrysoda::String GetLocalizedString(const cherrysoda::String& string);

	static void SetLanguage(const cherrysoda::String& name);

private:
	static Localization* g_localization;

	static const size_t m_locEnglishId = 0; // Default Language.
	static const size_t m_locGermanId = 1;
	static const size_t m_locRussianId = 2;

	static size_t m_currentLanguage;

private:

	Localization();
	~Localization();
	Localization(const Localization& rhs) = delete;
	Localization& operator=(const Localization& rhs) = delete;

	static cherrysoda::String _getLocString(loc_strings::ELocalizedString s, size_t lang);
};
