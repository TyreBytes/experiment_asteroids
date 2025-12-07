///
/// @file
/// @details This is an extremely simple StringTable to lookup the localized strings in the game from a key.
///
/// <!-- Copyright (c) 2025 Tyre Bytes LLC - All Rights Reserved -->
///------------------------------------------------------------------------------------------------------------------///

#include "../utilities/string_table.hpp"
#include "../asteroids.hpp"
#include "../game_manager.hpp"

#include <turtle_brains/core/unit_test/tb_unit_test.hpp>

#include <unordered_map>

namespace
{
	Asteroids::String kParamHolder = "%s%";
};

namespace Asteroids::Implementation
{
	static const std::unordered_map<std::string_view, String>& StringTable(void)
	{
		static std::unordered_map<std::string_view, String> theStringTable;

		if (false == theStringTable.empty())
		{
			return theStringTable;
		}

		theStringTable[StringKey::Test_ParamNone] = "This is a test without a parameter.";
		theStringTable[StringKey::Test_ParamStart] = kParamHolder + " was a test starting with a parameter.";
		theStringTable[StringKey::Test_ParamMiddle] = "This is a test with " + kParamHolder + " a parameter in the middle.";
		theStringTable[StringKey::Test_ParamEnd] = "This is a test ending with a parameter: " + kParamHolder;

		theStringTable[StringKey::Game_Title] = "Asteroids";
		theStringTable[StringKey::GameDemo_Title] = "Asteroids Demo";

		theStringTable[StringKey::Common_Okay] = "Okay";
		theStringTable[StringKey::Common_Button_Next] = "Next";
		theStringTable[StringKey::Common_Button_Done] = "Done";

		theStringTable[StringKey::Title_ClickToPlay] = "Click To Play";
		theStringTable[StringKey::Title_Play] = "Play";
		theStringTable[StringKey::Title_Profile] = "Profile " + kParamHolder;
		theStringTable[StringKey::Title_Settings] = "Settings";
		theStringTable[StringKey::Title_Quit] = "Quit";

		return theStringTable;
	}
};

//--------------------------------------------------------------------------------------------------------------------//

Asteroids::String Asteroids::Localize(const StringView& key, const String& param)
{
#if defined(rusty_development)
	tb_error_if(false == Implementation::StringTable().contains(key), "StringTable does not contain a key: '%s'", key.data())
#endif /* rusty_development */

	String value = Implementation::StringTable().at(key);

	size_t paramSpot = value.find(kParamHolder);
	if (String::npos != paramSpot)
	{
		value = value.substr(0, paramSpot) + param + value.substr(paramSpot + kParamHolder.size());
	}

	return value;
}

//--------------------------------------------------------------------------------------------------------------------//

Asteroids::String Asteroids::Localize(const String& key, const String& param)
{
	return Localize(std::string_view(key.data()), param);
}

//--------------------------------------------------------------------------------------------------------------------//
//--------------------------------------------------------------------------------------------------------------------//
//--------------------------------------------------------------------------------------------------------------------//

namespace Asteroids::UnitTesting
{

	class StringTableTest : public tbCore::UnitTest::TestCaseInterface
	{
	public:
		StringTableTest(void) :
			tbCore::UnitTest::TestCaseInterface("Asteroids::StringTableTest")
		{
		}

	protected:
		virtual bool OnRunTest(void) override
		{
			ExpectedValue<String>(Localize(StringKey::Test_ParamNone), "This is a test without a parameter.",
				"Expected a string lookup without parameter to pass.");

			ExpectedValue<String>(Localize(StringKey::Test_ParamStart, "MyParameter is Here"), "MyParameter is Here was a test starting with a parameter.",
				"Expected a string lookup with start parameter to pass.");

			ExpectedValue<String>(Localize(StringKey::Test_ParamMiddle, "Test Parameter"), "This is a test with Test Parameter a parameter in the middle.",
				"Expected a string lookup with a middle parameter to pass.");

			ExpectedValue<String>(Localize(StringKey::Test_ParamEnd, "Test Parameter!"), "This is a test ending with a parameter: Test Parameter!",
				"Expected a string lookup with a ending parameter to pass.");

			return true;
		}
	};

	StringTableTest theStringTableTest;

};

//--------------------------------------------------------------------------------------------------------------------//
