///
/// @file
/// @details A place to hold configuration/preferences/settings/values for the player.
///
/// <!-- Copyright (c) 2025 Tyre Bytes LLC - All Rights Reserved -->
///------------------------------------------------------------------------------------------------------------------///

#include "user_settings.hpp"
#include "asteroids.hpp" //GetSaveDirectory()
#include "logging.hpp"

#include <turtle_brains/core/tb_dynamic_structure.hpp>
#include <turtle_brains/core/tb_platform_utilities.hpp>
#include <turtle_brains/application/tb_application_input.hpp>
#include <turtle_brains/math/tb_math.hpp>

#include <fstream>

using tbCore::Debug::LogGameplay;

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::UserSettings::CreateDefaultSettings(void)
{
	SetBoolean(Settings::Fullscreen(), false);
	SetBoolean(Settings::VerticalSync(), false);
	SetInteger(Settings::WindowPositionX(), 100);
	SetInteger(Settings::WindowPositionY(), 100);
	SetInteger(Settings::WindowWidth(), 1280);
	SetInteger(Settings::WindowHeight(), 720);
	SetFloat(Settings::MusicVolume(), 0.7f);
	SetFloat(Settings::SoundVolume(), 0.75f);
	SetFloat(Settings::ShakeIntensity(), 1.0f);
}

//--------------------------------------------------------------------------------------------------------------------//

Asteroids::UserSettings& Asteroids::TheUserSettings(void)
{
	static UserSettings theUserSettings;
	return theUserSettings;
}

//--------------------------------------------------------------------------------------------------------------------//

Asteroids::UserSettings::UserSettings(void)
{
}

//--------------------------------------------------------------------------------------------------------------------//

Asteroids::UserSettings::~UserSettings(void)
{
}

//--------------------------------------------------------------------------------------------------------------------//

bool Asteroids::UserSettings::GetBoolean(const String& keyName, const bool defaultValue) const
{
	return mUserSettings[keyName].AsBooleanWithDefault(defaultValue);
}

//--------------------------------------------------------------------------------------------------------------------//

tbCore::int64 Asteroids::UserSettings::GetInteger(const String& keyName, const tbCore::int64 defaultValue) const
{
	return mUserSettings[keyName].AsIntegerWithDefault(defaultValue);
}

//--------------------------------------------------------------------------------------------------------------------//

float Asteroids::UserSettings::GetFloat(const String& keyName, const float defaultValue) const
{
	return mUserSettings[keyName].AsFloatWithDefault(defaultValue);
}

//--------------------------------------------------------------------------------------------------------------------//

Asteroids::String Asteroids::UserSettings::GetString(const String& keyName, const String& defaultValue) const
{
	return mUserSettings[keyName].AsStringWithDefault(defaultValue);
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::UserSettings::SetBoolean(const String& keyName, const bool value)
{
	mUserSettings[keyName] = value;
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::UserSettings::SetInteger(const String& keyName, const tbCore::int64 value)
{
	mUserSettings[keyName] = value;
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::UserSettings::SetFloat(const String& keyName, const float value)
{
	mUserSettings[keyName] = value;
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::UserSettings::SetString(const String& keyName, const String& value)
{
	mUserSettings[keyName] = value;
}

//--------------------------------------------------------------------------------------------------------------------//

bool Asteroids::UserSettings::HasKey(const String& keyName)
{
	return mUserSettings.HasMember(keyName);
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::UserSettings::DeleteKey(const String& keyName)
{
	mUserSettings.RemoveMember(keyName);
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::UserSettings::LoadSettings(const String& filename)
{
	String settingsFilepath = filename; //Try next to executable first.

	bool settingsFileExists = tbCore::PlatformUtilities::DoesFileExist(settingsFilepath);
	if (true == settingsFileExists)
	{
		mIsLocalSettings = true;
	}
	else
	{
		settingsFilepath = Asteroids::GetSaveDirectory() + filename;
		settingsFileExists = tbCore::PlatformUtilities::DoesFileExist(settingsFilepath);
	}

	if (false == settingsFileExists)
	{	//Not loading the file is not exactly an error, but LoadJsonFile would go boom.
		return;
	}

	const tbCore::DynamicStructure settingsData = tbCore::LoadJsonFile(settingsFilepath);
	if (true == settingsData.IsNil())
	{
		tb_always_log(LogGame::Error() << "Failed to load the " << QuotedString(settingsFilepath) << " file.");
		return;
	}

	// 2024-09-03: We can't just assign like: mUserSettings = settingsData; because that will remove any of the defaults
	//   setup by CreateDefaultSettings().
	for (auto& keyValueIterator : settingsData.AsStructure())
	{
		mUserSettings[keyValueIterator.first] = keyValueIterator.second;
	}
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::UserSettings::SaveSettings(const String& filename) const
{
	const String settingsFilepath(((mIsLocalSettings) ? "" : Asteroids::GetSaveDirectory()) + filename);
	if (false == tbCore::SaveJsonFile(settingsFilepath, mUserSettings, true))
	{
		tb_always_log(LogGame::Error() << "Failed to save the settings at: " << QuotedString(settingsFilepath));
	}
}

//--------------------------------------------------------------------------------------------------------------------//
