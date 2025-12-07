///
/// @file
/// @details A place to hold the state of the game that is known as 'global state' while keeping it somewhat contained
///    and in one place. Yes, globals are here.
///
/// <!-- Copyright (c) 2025 Tyre Bytes LLC - All Rights Reserved -->
///------------------------------------------------------------------------------------------------------------------///

#ifndef Asteroids_GameManager_hpp
#define Asteroids_GameManager_hpp

#include "asteroids.hpp"

#include <turtle_brains/tb_core_kit.hpp>
#include <turtle_brains/core/tb_dynamic_structure.hpp>

#include <array>

namespace Asteroids
{
	// 2025-10-06: To add a new unlock key, simply add a new string_view member with the name and key value and then
	//   append that key into theAllowedKeys. This is the ONLY place in the code-base that we should be typing a key
	//   name. Every other place in code should use UnlockKeys::Debug_ChallengerMode or such.
	//
	//   It took literally less than 3 hours to come up with typo from code, by trying to concatenate strings to build
	//   the key: challenger_fred_won. So much for "ONLY" place to type them key there... Be careful, it's dangerous.
	class UnlockKeys
	{
	public:
		static constexpr StringView DefaultKey = "default_key";

		static bool VerifyKeyExists(const StringView& key);
		static bool VerifyKeyExists(const String& key);

		// 2025-10-06: Let C++ figure out this is an array of Strings and the size. Class Template Argument Deduction (CTAD)
		//   a C++17 feature.
		static constexpr std::array theAllowedKeys = {
			DefaultKey,

		};

		// 2025-10-16: If you want a key to be selectable in the TechTreeEditor it needs to be placed in this container.
		static constexpr std::array theEditorKeys = {
			DefaultKey,
		};
	};

	// 2025-10-06: To add a new game stat, whether that is a resource to collect or a stat that changes how the resources
	//   get collected, simply add to this enum the name of the 'variable'. It will be a double (or whatever StatType is)
	//   and if you need to set a default value jump to the GameStats constructor and set it after the loop.
	enum class Stat {
		TotalPlayTime,
		TotalActiveTime,

		Level,
		Experience,


		NumberOfStats,
		Unknown
	};

	void VerifyStatsHaveNames(void);

	// 2025-10-24: This is used in the TechTreeEditor, or so I thought, but gcc says not used ... sooo maybe it was the static keyword?
	//inline constexpr std::array theResourceStats = { Stat::NutsAndBolts, Stat::Fame, Stat::Gears, Stat::Fuel, Stat::Plastic, Stat::Steel, Stat::ChainLinks, };

	using StatType = double;

	template<> Stat FromString(const String& input);
	template<> String ToString(const Stat& input);

	String MakePlayerFacingName(const Stat& input, bool plural);

	struct GameStats
	{
		std::array<StatType, static_cast<int>(Stat::NumberOfStats)> mIncrementalStats = { 0.0, };

		GameStats(void);

		bool Contains(const StringView& statName) const
		{
			return (Stat::Unknown != FromString<Stat>(String(statName)));
		}

		const StatType& operator[](const Stat& statIndex) const
		{
			return mIncrementalStats[static_cast<int>(statIndex)];
		}

		StatType& operator[](const Stat& statIndex)
		{
			return mIncrementalStats[static_cast<int>(statIndex)];
		}

		const StatType& operator[](const StringView& statName) const
		{
#if defined(tb_debug_build)
			tb_error_if(false == Contains(statName), "Expected statName='%s' to exist as a StatIndex, potential typo?", statName.data());
#endif /* debug_build */

			return (*this)[FromString<Stat>(String(statName))];
		}

		// The const StatType* that gets const_casted to a StatType* is part of THIS object. THIS object is non-const as per
		//   which guarantees that the members are also non-const. The NameToStatTable() is const-correct for the const
		//   version of this operator.
		StatType& operator[](const StringView& statName)
		{
#if defined(tb_debug_build)
			tb_error_if(false == Contains(statName), "Expected statName='%s' to exist as a StatIndex, potential typo?", statName.data());
#endif /* debug_build */

			return (*this)[FromString<Stat>(String(statName))];
		}
	};

	namespace GameManager
	{
		///
		/// Area for Constants that control game pacing or anything...
		///
		StatType CalculateExperienceForLevel(const int level);

		///
		constexpr size_t GetNumberOfProfiles(void) { return 3; }
		size_t GetActiveProfile(void);
		void SetActiveProfile(size_t profileIndex);

		void ResetProfile(void);
		void ResetProfile(const size_t profileIndex);

		const GameStats& GetActiveStats();
		GameStats& GetMutableActiveStats();

		const StatType& GetStat(const StringView& statName);
		const StatType& GetStat(const Stat& statIndex);
		StatType& GetMutableStat(const StringView& statName);
		StatType& GetMutableStat(const Stat& statIndex);
		//const StatType& GetStat(const String& statName);
		//StatType& GetMutableStat(const String& statName);

		bool RollForStat(const Stat& statIndex);

		/// @details This is a helper roll function that can roll +/- range by percentage with a minimum. So you could
		///   say RollForStatRange(WorldLootDrop, 0.1, 1) to get a returned value that is +/- 10% of the value held
		///   by WorldLootDrop, and if that 10% is less than 1 it would become +/- 1.
		int RollForStatRangeAsInteger(const Stat& statIndex, const StatType rangePercentage, const int rangeMinimum);

		StatType GetExperienceForNextLevel(const GameStats& gameStats);
		StatType GetExperienceForNextLevel(void);
		StatType GetExperienceForCurrentLevel(const GameStats& gameStats);
		StatType GetExperienceForCurrentLevel(void);

		// Should be called immediately ANYTIME fame is modified.
		int CheckForLevelUp(void);

		// Keys for progression and unlocking features etc.
		bool IsUnlocked(const String& keyName);
		bool IsUnlocked(const StringView& keyName);
		void UnlockKey(const String& keyName);
		void UnlockKey(const StringView& keyName);
		bool UnlockFirstTimeKey(const StringView& firstKey, const StringView& firstKeyDone);

		///
		/// @details LockKey should rarely be used, if even rarely. This was added so the logic of GetCurrentChallenger()
		///   and GetNextChallenger() could still work in the ChallengerScene while causing the player to 'rechallenge'
		///   after they lose a race. There were probably _other_ solutions to that problem, but it would have required
		///   much more refactoring and work.
		///
		/// @note ThisShouldBeRare is only a slight detterent but it should help ensure future usage is consider heavily.
		///
		enum class ThisShouldBeRare { LockKey };
		bool LockKey(const String& keyName, const ThisShouldBeRare reallyLock);

		float TimeMultiplier(void);
		void TimeMultiplier(float multiplier);

		void ResetGameToDefaults(void);
		void SaveGame(void);
		void LoadGame(void);

	};	//namespace GameManager

	inline float MakeFloat(const StatType& value) { return static_cast<float>(value); }
	inline int MakeInteger(const StatType& value) { return static_cast<int>(value + 0.5f); }
	inline String MakeString(const StatType& value, const int precision = 3) { return tbCore::ToString(value, precision); }
	inline String MakeIntegerString(const StatType& value) { return tb_string(static_cast<int>(value)); }
	inline String MakePercentageString(const StatType& value) { return tb_string(static_cast<int>(value * 100)) + "%"; }

	// This will be used so often that it is better to avoid the need to call GameManager:: each time. Although we should
	//   require the long-form for non-const, mutable, access. GameManager::GetMutableStat() to keep that easier to find
	//   and a little more "thinking" involved before invoking it.
	//
	// These should be how you access the stats by default, it may become enforced if there are are any bugs from maintaining
	//   the various access methods. This came up when trying to get as StatAsInteger for levels, we performing rounding
	//   here, and that same rounding would have been necessary elsewhere...
	inline const StatType& GetStat(const StringView& statName) { return GameManager::GetStat(statName); }
	inline const StatType& GetStat(const StringView& statName, const GameStats& stats) { return stats[statName]; }
	inline const StatType& GetStat(const Stat& statIndex) { return GameManager::GetStat(statIndex); }
	inline const StatType& GetStat(const Stat& statIndex, const GameStats& stats) { return stats[statIndex]; }

	inline float GetStatAsFloat(const Stat& statIndex) { return MakeFloat(GameManager::GetStat(statIndex)); }
	inline float GetStatAsFloat(const Stat& statIndex, const GameStats& stats) { return MakeFloat(stats[statIndex]); }
	inline int GetStatAsInteger(const Stat& statIndex) { return MakeInteger(GameManager::GetStat(statIndex)); }
	inline int GetStatAsInteger(const Stat& statIndex, const GameStats& stats) { return MakeInteger(stats[statIndex]); }
	inline String GetStatString(const Stat& statIndex) { return MakeString(GetStat(statIndex)); }
	inline String GetStatAsIntegerString(const Stat& statIndex) { return MakeIntegerString(GetStat(statIndex)); }
	inline String GetStatAsPercentageString(const Stat& statIndex) { return MakePercentageString(GetStat(statIndex)); }

};	//namespace Asteroids

#endif /* Asteroids_GameManager_hpp */
