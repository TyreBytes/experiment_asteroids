///
/// @file
/// @details A place to hold the state of the game that is known as 'global state' while keeping it somewhat contained
///    and in one place. Yes, globals are here.
///
/// <!-- Copyright (c) 2025 Tyre Bytes LLC - All Rights Reserved -->
///------------------------------------------------------------------------------------------------------------------///

#include "game_manager.hpp"
#include "asteroids.hpp"

// The scenes and garage are necessary for Saving/Loading the tech tree.
#include "scenes/scene_manager.hpp"

#include <turtle_brains/core/tb_file_utilities.hpp>

#include <unordered_set>

namespace Asteroids
{
	namespace Implementation
	{
		String theDebugInfo = "";

		tbCore::uint32 kAsteroidsSaveVersion = 1;
	};

	const std::vector<std::pair<Stat, String>> kStatToNameTable{
		{ Stat::TotalPlayTime, "TOTAL_PLAY_TIME" },
		{ Stat::TotalActiveTime, "TOTAL_ACTIVE_TIME" },

		{ Stat::Level, "LEVEL" },
		{ Stat::Experience, "XP" },
	};

	namespace GameManager
	{
		std::array<GameStats, GetNumberOfProfiles()> theGameStats;
		std::array<std::unordered_set<String>, GetNumberOfProfiles()> theUnlockedKeys;

		int theActiveProfile = 0;

		float theTimeMultiplier = 1.0f;
		bool theGameIsPaused = false;
	}
};

//--------------------------------------------------------------------------------------------------------------------//

Asteroids::GameStats::GameStats(void) :
	mIncrementalStats{ 0.0f, }
{
	for (StatType& statValue : mIncrementalStats)
	{
		statValue = 0.0;
	}

	(*this)[Stat::Level] = 1.0;
}

//--------------------------------------------------------------------------------------------------------------------//
//--------------------------------------------------------------------------------------------------------------------//
//--------------------------------------------------------------------------------------------------------------------//

Asteroids::StatType Asteroids::GameManager::CalculateExperienceForLevel(const int level)
{
	return tbMath::Maximum(50.0 * level * level + 50.0 * level, StatType(0.0));
}

//--------------------------------------------------------------------------------------------------------------------//
//--------------------------------------------------------------------------------------------------------------------//
//--------------------------------------------------------------------------------------------------------------------//

Asteroids::String Asteroids::MakePlayerFacingName(const Stat& input, bool plural)
{
	const std::unordered_map<Stat, std::pair<String, String>> kStatToPlayerName{
		{ Stat::Experience, { NotLocalized("XP"), NotLocalized("XP") } },
		{ Stat::Level, { NotLocalized("Level"), NotLocalized("Levels") } },
	};

	tb_error_if(false == kStatToPlayerName.contains(input), "Expected to find Stat(%d) to convert for UI string.", input);
	return (true == plural) ? kStatToPlayerName.at(input).second : kStatToPlayerName.at(input).first;
}

//--------------------------------------------------------------------------------------------------------------------//
//--------------------------------------------------------------------------------------------------------------------//
//--------------------------------------------------------------------------------------------------------------------//

bool Asteroids::UnlockKeys::VerifyKeyExists(const StringView& key)
{
	for (const StringView& availableKey : UnlockKeys::theAllowedKeys)
	{
		if (tbCore::StringUtilities::Lowercase(String(key)) == tbCore::StringUtilities::Lowercase(String(availableKey)))
		{
			return true;
		}
	}

	return false;
}

//--------------------------------------------------------------------------------------------------------------------//

bool Asteroids::UnlockKeys::VerifyKeyExists(const String& key)
{
	for (const StringView& availableKey : UnlockKeys::theAllowedKeys)
	{
		if (tbCore::StringUtilities::Lowercase(key) == tbCore::StringUtilities::Lowercase(String(availableKey)))
		{
			return true;
		}
	}

	return false;
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::VerifyStatsHaveNames(void)
{
#if defined(rusty_development)
	for (int statIndex = 0; statIndex < static_cast<int>(Stat::NumberOfStats); ++statIndex)
	{
		tb_error_if("UNKNOWN_STAT" == ToString(static_cast<Stat>(statIndex)), "Error: Unknown name for statIndex: %d.", statIndex);
	}
#endif /* rusty_development */
}

//--------------------------------------------------------------------------------------------------------------------//
//--------------------------------------------------------------------------------------------------------------------//
//--------------------------------------------------------------------------------------------------------------------//

template<> Asteroids::Stat Asteroids::FromString(const String& input)
{
	const String upper = tbCore::StringUtilities::Uppercase(input);

	for (auto& statNamePair : kStatToNameTable)
	{
		if (statNamePair.second == upper)
		{
			return statNamePair.first;
		}
	}

	return Stat::Unknown;
}

//--------------------------------------------------------------------------------------------------------------------//

template<> Asteroids::String Asteroids::ToString(const Stat& input)
{
	for (auto& statNamePair : kStatToNameTable)
	{
		if (statNamePair.first == input)
		{
			return statNamePair.second;
		}
	}

	return "UNKNOWN_STAT";
}

//--------------------------------------------------------------------------------------------------------------------//
//--------------------------------------------------------------------------------------------------------------------//
//--------------------------------------------------------------------------------------------------------------------//

size_t Asteroids::GameManager::GetActiveProfile(void)
{
	return static_cast<size_t>(theActiveProfile);
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::GameManager::SetActiveProfile(size_t profileIndex)
{
	tb_error_if(profileIndex >= GetNumberOfProfiles(), "Expected valid profileIndex.");
	theActiveProfile = static_cast<int>(profileIndex);
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::GameManager::ResetProfile(void)
{
	ResetProfile(static_cast<size_t>(theActiveProfile));
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::GameManager::ResetProfile(const size_t profileIndex)
{
	theGameStats[profileIndex] = GameStats();
	theUnlockedKeys[profileIndex].clear();
}

//--------------------------------------------------------------------------------------------------------------------//

const Asteroids::GameStats& Asteroids::GameManager::GetActiveStats(void)
{
	return theGameStats[theActiveProfile];
}

//--------------------------------------------------------------------------------------------------------------------//

Asteroids::GameStats& Asteroids::GameManager::GetMutableActiveStats()
{
	return theGameStats[theActiveProfile];
}

//--------------------------------------------------------------------------------------------------------------------//

const Asteroids::StatType& Asteroids::GameManager::GetStat(const Stat& statIndex)
{
	return GetActiveStats()[statIndex];
}

//--------------------------------------------------------------------------------------------------------------------//

const Asteroids::StatType& Asteroids::GameManager::GetStat(const StringView& statName)
{
	return GetActiveStats()[String(statName)];
}

//--------------------------------------------------------------------------------------------------------------------//

Asteroids::StatType& Asteroids::GameManager::GetMutableStat(const Stat& statIndex)
{
	return GetMutableActiveStats()[statIndex];
}

//--------------------------------------------------------------------------------------------------------------------//

Asteroids::StatType& Asteroids::GameManager::GetMutableStat(const StringView& statName)
{
	return GetMutableActiveStats()[statName];
}

//--------------------------------------------------------------------------------------------------------------------//

//const Asteroids::StatType& Asteroids::GameManager::GetStat(const String& statName)
//{
//	return GetActiveStats()[statName];
//}

//--------------------------------------------------------------------------------------------------------------------//

//Asteroids::StatType& Asteroids::GameManager::GetMutableStat(const String& statName)
//{
//	return GetMutableActiveStats()[statName];
//}

//--------------------------------------------------------------------------------------------------------------------//

bool Asteroids::GameManager::RollForStat(const Stat& statIndex)
{
	return tbMath::RandomFloat() <= GameManager::GetStat(statIndex);
}

//--------------------------------------------------------------------------------------------------------------------//

int Asteroids::GameManager::RollForStatRangeAsInteger(const Stat& statIndex, const StatType rangePercentage,
	const int rangeMinimum)
{
	const StatType& value = GameManager::GetStat(statIndex);
	const int range = tbMath::Maximum(MakeInteger(value * rangePercentage), rangeMinimum);
	const int valueAsInteger = GetStatAsInteger(statIndex);

	return tbMath::RandomInt(valueAsInteger - range, valueAsInteger + range);
}

//--------------------------------------------------------------------------------------------------------------------//

Asteroids::StatType Asteroids::GameManager::GetExperienceForNextLevel(const GameStats& gameStats)
{
	const int level = MakeInteger(gameStats[Stat::Level]);
	return CalculateExperienceForLevel(level);
}

//--------------------------------------------------------------------------------------------------------------------//

Asteroids::StatType Asteroids::GameManager::GetExperienceForNextLevel(void)
{
	return GetExperienceForNextLevel(theGameStats[theActiveProfile]);
}

//--------------------------------------------------------------------------------------------------------------------//

Asteroids::StatType Asteroids::GameManager::GetExperienceForCurrentLevel(const GameStats& gameStats)
{
	const int level = MakeInteger(gameStats[Stat::Level]) - 1;
	return CalculateExperienceForLevel(level);
}

//--------------------------------------------------------------------------------------------------------------------//

Asteroids::StatType Asteroids::GameManager::GetExperienceForCurrentLevel(void)
{
	return GetExperienceForCurrentLevel(theGameStats[theActiveProfile]);
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::GameManager::GainExperience(const StatType& value)
{
	GetMutableStat(Stat::Experience) += value;
	CheckForLevelUp();
}

//--------------------------------------------------------------------------------------------------------------------//

int Asteroids::GameManager::CheckForLevelUp(void)
{
	int levelsGained = 0;
	while (GameManager::GetStat(Stat::Experience) > GetExperienceForNextLevel())
	{
		GetMutableStat(Stat::Level) += 1.0;

		{	// TODO: Asteroids: Develoment: 2025-10-16: This would be a great tool to write out the history of play and
			//   track through multiple sessions if necessary.
			tb_always_log(LogState::Always() << DebugInfo::PlayTimeHistory() << "Level Up " <<
				GetStatAsIntegerString(Stat::Level) << " with a total of " << GetStatAsIntegerString(Stat::Experience) << " fame.");
		}

		//if (GetStatAsInteger(Stat::Level) >= 10 && 0 == GetStatAsInteger(Stat::Level) % 5)
		//{	// 2025-10-22: This is a special chain link that is given to unlock the ChallengerMode through tech tree.
		//	//   Once player buys their "race car" they can gain chain links through minions that then allow them to
		//	//   continue racing the 'bosses' of the challenge/race mode.
		//	GameManager::GetMutableStat(Stat::ChainLinks) += 1.0;
		//}

		++levelsGained;
	}

	return levelsGained;
}

//--------------------------------------------------------------------------------------------------------------------//

bool Asteroids::GameManager::IsUnlocked(const String& keyName)
{
	return theUnlockedKeys[theActiveProfile].contains(tbCore::StringUtilities::Lowercase(keyName));
}

//--------------------------------------------------------------------------------------------------------------------//

bool Asteroids::GameManager::IsUnlocked(const StringView& keyName)
{
	return IsUnlocked(String(keyName));
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::GameManager::UnlockKey(const String& keyName)
{
	if (true == theUnlockedKeys[theActiveProfile].insert(tbCore::StringUtilities::Lowercase(keyName)).second)
	{
		//const std::unordered_map<StringView, String> unlockKeyToSteamAchievements = {
		//	{ UnlockKeys::FirstLootPickedUp, "collected_first_bolt" },
		//	{ UnlockKeys::Quest_LittleLeader, "the_little_leader" },
		//};

		////if (true == unlockKeyToSteamAchievements.contains(keyName))
		//const auto& steamAchievementIterator = unlockKeyToSteamAchievements.find(keyName);
		//if (steamAchievementIterator != unlockKeyToSteamAchievements.end())
		//{
		//	ToolBox::Achievements::UnlockAchievement(steamAchievementIterator->second);
		//}
	}
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::GameManager::UnlockKey(const StringView& keyName)
{
	return UnlockKey(String(keyName));
}

//--------------------------------------------------------------------------------------------------------------------//

bool Asteroids::GameManager::UnlockFirstTimeKey(const StringView& firstKey, const StringView& firstKeyDone)
{
	if (true == IsUnlocked(firstKey) && false == IsUnlocked(firstKeyDone))
	{
		UnlockKey(firstKeyDone);
		return true;
	}

	return false;
}

//--------------------------------------------------------------------------------------------------------------------//

bool Asteroids::GameManager::LockKey(const String& keyName, const ThisShouldBeRare /*reallyLock*/)
{
	return (0 != theUnlockedKeys[theActiveProfile].erase(tbCore::StringUtilities::Lowercase(String(keyName))));
}

//--------------------------------------------------------------------------------------------------------------------//

float Asteroids::GameManager::TimeMultiplier(void)
{
	return theTimeMultiplier;
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::GameManager::TimeMultiplier(float multiplier)
{
	theTimeMultiplier = multiplier;
}

//--------------------------------------------------------------------------------------------------------------------//

bool Asteroids::GameManager::IsPaused(void)
{
	return theGameIsPaused;
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::GameManager::SetPaused(bool paused)
{
	theGameIsPaused = paused;
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::GameManager::ResetGameToDefaults(void)
{
	for (size_t profileIndex = 0; profileIndex < GetNumberOfProfiles(); ++profileIndex)
	{
		theUnlockedKeys[profileIndex].clear();
		theGameStats[profileIndex] = GameStats();
	}
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::GameManager::SaveGame(void)
{
	for (size_t profileIndex = 0; profileIndex < GetNumberOfProfiles(); ++profileIndex)
	{
		DynamicStructure unlockedKeysData;
		for (const String& unlockedKey : theUnlockedKeys[profileIndex])
		{
			unlockedKeysData.PushValue(unlockedKey);
		}

		DynamicStructure statsData;
		const GameStats& profileStats = theGameStats[profileIndex];
		for (int statIndex = 0; statIndex < static_cast<int>(Stat::NumberOfStats); ++statIndex)
		{
			const Stat currentStat = static_cast<Stat>(statIndex);
			statsData.AddMember(ToString(currentStat), GetStatAsFloat(currentStat, profileStats));
		}

		DynamicStructure profileData;
		profileData.AddMember("version", tbCore::RangedCast<tbCore::int64>(Implementation::kAsteroidsSaveVersion));
		profileData.AddMember("unlocked_keys", unlockedKeysData);
		profileData.AddMember("stats", statsData);
		//profileData.AddMember("tech_tree", theSceneManager->GetSceneAs<GarageScene>(SceneId::kGarageScene).GetSaveData());

		const String profileFilepath = GetSaveDirectory() + "profile_" + ToString(profileIndex + 1) + ".dat";

#if defined(rusty_development)
		tbCore::SaveJsonFile(profileFilepath + ".json", profileData);
#endif

		std::ofstream outputFile(profileFilepath, std::ios::binary);
		if (outputFile.is_open())
		{
			tbCore::FileUtilities::WriteVariableLengthEncoding(Implementation::kAsteroidsSaveVersion, outputFile);

			// 2025-11-05: Due to this area of the TurtleBrains api being "in-development" ReadBinary with the DynamicStructure
			//   is different from FileUtilities;:ReadBinary<> and this is another situation where TurtleBrains probably
			//   wants a global template specialization to ensure the consistency "just works", much like the idea of
			//   consistency with To/FromString, and To/FromDynamicStructure we should have Read/WriteBinary consistency too.
			//tbCore::FileUtilities::WriteBinary(profileData, outputFile);
			//tbCore::FileUtilities::WriteBinary(profileData, outputFile);

			WriteBinary(profileData, outputFile);
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::GameManager::LoadGame(void)
{
	bool failedToLoad = false;
	const int originalActiveProfile = theActiveProfile;

	for (int profileIndex = 0; profileIndex < static_cast<int>(GetNumberOfProfiles()); ++profileIndex)
	{
		theActiveProfile = profileIndex;
		const String profileFilepath = GetSaveDirectory() + "profile_" + ToString(profileIndex + 1) + ".dat";

		std::ifstream inputFile(profileFilepath, std::ios::binary);
		if (false == inputFile.is_open())
		{
			failedToLoad = true;
			continue;
		}

		const tbCore::int64 version = tbCore::FileUtilities::ReadVariableLengthEncoding(inputFile);
		if (Implementation::kAsteroidsSaveVersion != version)
		{
			tb_always_log(LogGame::Error() << "Failed to load profile data due to version mismatch.");
			failedToLoad = true;
			continue;
		}

#if defined(rusty_development)
		const DynamicStructure& profileData = tbCore::LoadJsonFile(profileFilepath + ".json");
#else
		// 2025-11-05: Due to this area of the TurtleBrains api being "in-development" ReadBinary with the DynamicStructure
		//   is different from FileUtilities;:ReadBinary<> and this is another situation where TurtleBrains probably
		//   wants a global template specialization to ensure the consistency "just works", much like the idea of
		//   consistency with To/FromString, and To/FromDynamicStructure we should have Read/WriteBinary consistency too.
		//const DynamicStructure profileData = tbCore::FileUtilities::ReadBinary<DynamicStructure>(inputFile);
		const DynamicStructure profileData = ReadBinary<DynamicStructure>(inputFile);
#endif

		// Process the data...
		if (Implementation::kAsteroidsSaveVersion != profileData["version"].AsIntegerWithDefault(0))
		{
			tb_always_log(LogGame::Error() << "Failed to load profile data due to internal data version mismatch.");
			failedToLoad = true;
			continue;
		}

		const DynamicStructure& unlockedKeysData = profileData["unlocked_keys"];
		const DynamicStructure& statsData = profileData["stats"];
		//const DynamicStructure& techTreeData = profileData["tech_tree"];

		for (const DynamicStructure& unlockKeyData : unlockedKeysData.AsArray())
		{
			UnlockKey(unlockKeyData.AsString());
		}

		for (const auto& keyValuePair : statsData.AsStructure())
		{
			const String& statName = keyValuePair.first;
			const StatType statValue = keyValuePair.second.AsFloat();

			if (true == GetActiveStats().Contains(statName))
			{
				GetMutableStat(statName) = statValue;
			}
			else
			{
				tb_always_log(LogGame::Warning() << "Stat: '" << statName << "' is an unknown stat name, was it removed?");
			}
		}

		//theSceneManager->GetSceneAs<GarageScene>(SceneId::kGarageScene).LoadFromData(techTreeData);
	}

	theActiveProfile = originalActiveProfile;

	tb_debug_log_if(false, "Pretending to use failed to load: " << failedToLoad);
}

//--------------------------------------------------------------------------------------------------------------------//
