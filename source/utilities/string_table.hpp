///
/// @file
/// @details This is an extremely simple StringTable to lookup the localized strings in the game from a key.
///
/// <!-- Copyright (c) 2025 Tyre Bytes LLC - All Rights Reserved -->
///------------------------------------------------------------------------------------------------------------------///

#ifndef Asteroids_StringTable_hpp
#define Asteroids_StringTable_hpp

//Do Not Include asteroids.hpp or any other Asteroids headers that will to avoid circular inclusion.

#include <turtle_brains/core/tb_types.hpp>
#include <turtle_brains/core/tb_string.hpp>

namespace Asteroids
{
	using String = TurtleBrains::String;
	using StringView = std::string_view;

	// While not strictly necessary, it would be a good idea to use this for very common string Keys to prevent potential
	//   spelling mistakes. If there is a spelling mistake in a key it can't be known at the start of the game, and in
	//   a public build a "<UNKNOWN>" string will appear, where debug/dev builds will/could trigger an error.
	class StringKey
	{
	public:
		static constexpr StringView Test_ParamNone = "test_param_none";
		static constexpr StringView Test_ParamStart = "test_param_start";
		static constexpr StringView Test_ParamEnd = "test_param_end";
		static constexpr StringView Test_ParamMiddle = "test_param_middle";

		static constexpr StringView Game_Title = "game_title";
		static constexpr StringView GameDemo_Title = "game_demo_title";

		static constexpr StringView Common_Okay = "common_okay";
		static constexpr StringView Common_Button_Next = "common_button_next";
		static constexpr StringView Common_Button_Done = "common_button_done";

		static constexpr StringView Title_ClickToPlay = "title_click_to_play";
		static constexpr StringView Title_Play = "title_play";
		static constexpr StringView Title_Profile = "title_profile";
		static constexpr StringView Title_Settings = "title_settings";
		static constexpr StringView Title_Quit = "title_quit";

	};

	String Localize(const StringView& key, const String& param = "");
	String Localize(const String& key, const String& param = "");

	// 2025-10-23: This is a helper function so that we can simply search it out and replace with Localize() and use
	//   StringKeys. This is meant ONLY to speed up development while prototyping/proof-of-concept etc. As the strings
	//   get more finalized and certain, they should use Localize() with a StringKey...
	inline const String& NotLocalized(const String& notLocalized) { return notLocalized; }

	//String Localize(const StringView& key, const String& param = "");
	//String Localize(const String& key, const String& param = "");


}; /* namespace Asteroids */

#endif /* Asteroids_StringTable_hpp */
