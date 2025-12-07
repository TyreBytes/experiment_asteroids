///
/// @file
/// @details Provide definitions for the debug channels in Asteroids.
///
/// <!-- Copyright (c) 2025 Tyre Bytes LLC - All Rights Reserved -->
///------------------------------------------------------------------------------------------------------------------///

#include "logging.hpp"
#include "game_manager.hpp"

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::SetLoggingLevels(void)
{
	tbCore::Debug::TheLogger().SetLogLevel("tbInternal", tbCore::Debug::LogLevel::kInfo);
	tbCore::Debug::TheLogger().SetLogLevel("Steam", tbCore::Debug::LogLevel::kTrace); //Tyre Bytes Tool Box

	tbCore::Debug::TheLogger().SetLogLevel(LogGame::AsString(), tbCore::Debug::LogLevel::kInfo);
	tbCore::Debug::TheLogger().SetLogLevel(LogState::AsString(), tbCore::Debug::LogLevel::kInfo);
	tbCore::Debug::TheLogger().SetLogLevel(LogAudio::AsString(), tbCore::Debug::LogLevel::kInfo);
	tbCore::Debug::TheLogger().SetLogLevel(LogPhysics::AsString(), tbCore::Debug::LogLevel::kInfo);
	tbCore::Debug::TheLogger().SetLogLevel(LogGraphics::AsString(), tbCore::Debug::LogLevel::kInfo);
}

//--------------------------------------------------------------------------------------------------------------------//
//--------------------------------------------------------------------------------------------------------------------//
//--------------------------------------------------------------------------------------------------------------------//

Asteroids::DebugInfo Asteroids::DebugInfo::PlayTimeHistory(void)
{
	DebugInfo info;
	std::stringstream ss;

	const String totalPlayTime = tbCore::StringUtilities::TimeToString(static_cast<tbCore::uint32>(GetStat(Stat::TotalPlayTime) * 1000.0f));
	const String totalActiveTime = tbCore::StringUtilities::TimeToString(static_cast<tbCore::uint32>(GetStat(Stat::TotalActiveTime) * 1000.0f));
	ss << "HISTORY: [ " << totalPlayTime << ", " << totalActiveTime << " ]:  ";
	info.mInfo = ss.str();
	return info;
}

//--------------------------------------------------------------------------------------------------------------------//
