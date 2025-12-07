///
/// @file
/// @details This is a small object that represents a full row of data in the TechTree csv table.
///
/// <!-- Copyright (c) 2025 Tyre Bytes LLC - All Rights Reserved -->
///------------------------------------------------------------------------------------------------------------------///

#include "../utilities/tech_tree_upgrade.hpp"

#include "../interface.hpp" //To create icon sprite for verification that everything is loading properly.
#include "../game_manager.hpp"

#include <unordered_map>

namespace Asteroids
{
	namespace Implementation
	{	// 2025-10-14: This lives in game_manager.cpp
		extern String theDebugInfo;

		class UpgradeDataKey
		{
		public:
			static constexpr std::string_view Key = "ID";
			static constexpr std::string_view Title = "TITLE";
			static constexpr std::string_view Parents = "PARENT";
			static constexpr std::string_view ModifyStat = "MODIFY_STAT";
			static constexpr std::string_view ModifyAdd = "MODIFY_ADD";
			static constexpr std::string_view ModifyMultiplier = "MODIFY_MULTIPLIER";
			static constexpr std::string_view CostStat = "COST_STAT";
			static constexpr std::string_view CostValue = "COST_VALUE";
			static constexpr std::string_view Levels = "LEVELS";
			static constexpr std::string_view UnlockKey = "UNLOCK_KEY";
			static constexpr std::string_view LockStat = "LOCK_STAT";
			static constexpr std::string_view LockValue = "LOCK_VALUE";
			static constexpr std::string_view VisibleStat = "VISIBLE_STAT";
			static constexpr std::string_view VisibleValue = "VISIBLE_VALUE";
			static constexpr std::string_view Column = "COL";
			static constexpr std::string_view Row = "ROW";
			static constexpr std::string_view Sprite = "SPRITE";
			static constexpr std::string_view Description = "DESCRIPTION";
		};
	};

	Asteroids::StatValueContainer StatValuePairsFrom(const DynamicStructure& rowData,
		const StringView& statColumn, const StringView& valueColumn, const String& debugInfo);

	Asteroids::StringContainer StringKeysFrom(const DynamicStructure& rowData, const StringView& columnName);
};

//--------------------------------------------------------------------------------------------------------------------//

Asteroids::StatValueContainer Asteroids::StatValuePairsFrom(const DynamicStructure& rowData,
	const StringView& statColumn, const StringView& valueColumn, const String& debugInfo = "")
{
	StringContainer stats = tbCore::StringUtilities::SeparateString(rowData[statColumn].AsString(), ",");
	if (true == stats.empty() || (1 == stats.size() && "none" == tbCore::StringUtilities::Lowercase(stats.back())))
	{
		return StatValueContainer();
	}

	StringContainer valueStrings = tbCore::StringUtilities::SeparateString(rowData[valueColumn].AsString(), ",");
	if (stats.size() != valueStrings.size())
	{
		tb_error_if(stats.size() != valueStrings.size(), "Error getting StatValues('%s', '%s') from a rowData from: '%s'",
			statColumn.data(), valueColumn.data(), debugInfo.c_str());
		return StatValueContainer();
	}

	// 2025-10-02: We are returning a pointer/reference to the stat value which is mutable.
	GameStats& gameStats = GameManager::GetMutableActiveStats();

	StatValueContainer pairs;
	pairs.reserve(stats.size());

	for (size_t index = 0; index < stats.size(); ++index)
	{
		if (true == gameStats.Contains(stats[index]))
		{
			pairs.emplace_back(StatValuePair{ FromString<Stat>(stats[index]), tbCore::FromString<double>(valueStrings[index]) });
		}
		else
		{
			tb_error("Error: There is no stat for '%s' in a rowData from: '%s'", stats[index].c_str(), debugInfo.c_str());
		}
	}

	return pairs;
}

//--------------------------------------------------------------------------------------------------------------------//

Asteroids::StringContainer Asteroids::StringKeysFrom(const DynamicStructure& rowData, const StringView& columnName)
{
	StringContainer values = tbCore::StringUtilities::SeparateString(rowData[columnName].AsString(), ",");
	if (true == values.empty() || (1 == values.size() && "none" == tbCore::StringUtilities::Lowercase(values.back())))
	{
		return StringContainer();
	}

	return values;
}

//--------------------------------------------------------------------------------------------------------------------//

Asteroids::String ToDataStats(const Asteroids::StatValueContainer& container)
{
	Asteroids::String value = "";

	if (true == container.empty())
	{
		return "NONE";
	}

	if (container.size() > 1)
	{
		value += "\"";
	}

	Asteroids::String columnSeparator = "";
	for (const auto& statValuePair : container)
	{
		value += columnSeparator + ToString(statValuePair.first);
		columnSeparator = ",";
	}

	if (container.size() > 1)
	{
		value += "\"";
	}

	return value;
}

//--------------------------------------------------------------------------------------------------------------------//

Asteroids::String ToDataValues(const Asteroids::StatValueContainer& container)
{
	Asteroids::String value = "";

	if (true == container.empty())
	{
		return "NONE";
	}

	if (container.size() > 1)
	{
		value += "\"";
	}

	Asteroids::String columnSeparator = "";
	for (const auto& statValuePair : container)
	{
		value += columnSeparator + tbCore::ToString(statValuePair.second);
		columnSeparator = ",";
	}

	if (container.size() > 1)
	{
		value += "\"";
	}

	return value;
}

//--------------------------------------------------------------------------------------------------------------------//

template<> Asteroids::DynamicStructure Asteroids::ToDynamicStructure<Asteroids::UpgradeData>(const UpgradeData& upgrade)
{
	using Asteroids::Implementation::UpgradeDataKey;

	String allParents = "";
	for (const UpgradeKey& parentKey : upgrade.mParentKeys)
	{
		if (false == allParents.empty())
		{
			allParents += ",";
		}

		allParents += parentKey.String();
	}

	String allUnlocks = "";
	for (const String& unlockKey : upgrade.mUnlocks)
	{
		if (unlockKey == UnlockKeys::DefaultKey)
		{	// Don't save default key.
			continue;
		}

		if (false == allUnlocks.empty())
		{
			allUnlocks += ",";
		}
		else
		{
			allUnlocks += "\"";
		}

		allUnlocks += unlockKey;
	}

	if (false == allUnlocks.empty())
	{
		allUnlocks += "\"";
	}

	if (true == allUnlocks.empty())
	{
		allUnlocks = "NONE";
	}

	return DynamicStructure{
		{ String(UpgradeDataKey::Key), upgrade.mKey.String() },
		{ String(UpgradeDataKey::Title), upgrade.mTitle },
		{ String(UpgradeDataKey::Parents), allParents },
		{ String(UpgradeDataKey::Levels), static_cast<int>(upgrade.mLevels) },
		{ String(UpgradeDataKey::UnlockKey), allUnlocks },

		{ String(UpgradeDataKey::CostStat), ToDataStats(upgrade.mStatCosts) },
		{ String(UpgradeDataKey::CostValue), ToDataValues(upgrade.mStatCosts) },
		{ String(UpgradeDataKey::LockStat), ToDataStats(upgrade.mStatLocks) },
		{ String(UpgradeDataKey::LockValue), ToDataValues(upgrade.mStatLocks) },
		{ String(UpgradeDataKey::ModifyStat), ToDataStats(upgrade.mStatModifyAdds) },
		{ String(UpgradeDataKey::ModifyAdd), ToDataValues(upgrade.mStatModifyAdds) },
		{ String(UpgradeDataKey::ModifyMultiplier), ToDataValues(upgrade.mStatModifyMults) },

		{ String(UpgradeDataKey::Column), upgrade.mColumn },
		{ String(UpgradeDataKey::Row), upgrade.mRow },
		{ String(UpgradeDataKey::Sprite), upgrade.mSpriteInfo },
		{ String(UpgradeDataKey::Description), upgrade.mDescription },
	};
}

//--------------------------------------------------------------------------------------------------------------------//

template<> Asteroids::UpgradeData Asteroids::FromDynamicStructure(const DynamicStructure& data)
{
	using Asteroids::Implementation::UpgradeDataKey;

	UpgradeData upgrade;

	upgrade.mTitle = data[UpgradeDataKey::Title].AsString();
	upgrade.mDescription = data[UpgradeDataKey::Description].AsString();

	upgrade.mSpriteInfo = data[UpgradeDataKey::Sprite].AsString();
	tbGraphics::Sprite upgradeSprite = Interface::MakeIconSprite(upgrade.mSpriteInfo);

	upgrade.mLevels = data[UpgradeDataKey::Levels].AsRangedIntegerWithDefault<tbCore::uint8>(1);

	upgrade.mKey = FromString<UpgradeKey>(data[UpgradeDataKey::Key].AsString());
	tb_error_if(UpgradeKey::Invalid() == upgrade.mKey, "Expected a valid key here.");

	const std::vector<String> parents = tbCore::StringUtilities::SeparateString(data[UpgradeDataKey::Parents].AsString(), ",");
	for (const String& parentString : parents)
	{
		const UpgradeKey parentKey = FromString<UpgradeKey>(parentString);
		if (UpgradeKey::Invalid() == parentKey)
		{	//No connection for -1 parents.
			continue;
		}

		upgrade.mParentKeys.emplace_back(parentKey);
	}

	upgrade.mColumn = data[UpgradeDataKey::Column].AsRangedInteger<int>();
	upgrade.mRow = data[UpgradeDataKey::Row].AsRangedInteger<int>();
	upgrade.mPosition = Vector2(upgrade.mColumn * kTechTreeSpacing, upgrade.mRow * kTechTreeSpacing);

	const String fullDebug = "Verifying( '" + Implementation::theDebugInfo + ":" + data[UpgradeDataKey::Key].AsString() + "' )";

	// StatValuePairFrom should be verifying all STAT names exist and will allow 0-n pairs to be returned. It will
	//   trigger an error if something was misspelled, which is important to know upon load of the game rather!
	upgrade.mStatCosts = StatValuePairsFrom(data, UpgradeDataKey::CostStat, UpgradeDataKey::CostValue, fullDebug);
	upgrade.mStatLocks = StatValuePairsFrom(data, UpgradeDataKey::LockStat, UpgradeDataKey::LockValue, fullDebug);
	upgrade.mStatModifyAdds = StatValuePairsFrom(data, UpgradeDataKey::ModifyStat, UpgradeDataKey::ModifyAdd, fullDebug);
	upgrade.mStatModifyMults = StatValuePairsFrom(data, UpgradeDataKey::ModifyStat, UpgradeDataKey::ModifyMultiplier, fullDebug);
	upgrade.mUnlocks = StringKeysFrom(data, UpgradeDataKey::UnlockKey);

	tb_error_if(upgrade.mStatModifyAdds.size() != upgrade.mStatModifyMults.size(), "Expected to have same number of ADDITIVE and MULITPLIER pairs.");
	for (size_t index = 0; index < upgrade.mStatModifyAdds.size(); ++index)
	{
		tb_error_if(upgrade.mStatModifyAdds[index].first != upgrade.mStatModifyMults[index].first, "Expected both ADDITIVE and MULTIPLIER pairs to have same MODIFY_STAT.");
	}

	// TODO: Asteroids: Verify: 2025-10-10: We should be verifying that each upgrade has a different COL,ROW from every other upgrade.
	//   This is simply too easy to mess up and not actually see, I accidentally upgraded to "BIG BOLTS" too early because it
	//   was at 0,0 overlapping the "MID BOLTS".
	//
	// TODO: Asteroids: Verify: 2025-10-10: Should verify that all upgrade parents are directly adjacent.
	//   Due to the way the tech tree is being built/designed we should ensure the parent is always one column or row
	//   away from the current one. That does add limitations, but I think it also decreases mistake potential.

	for (const String& unlockKey : upgrade.mUnlocks)
	{
		// 2025-10-06: Lookup LevenshteinDistance to compute how close a match could have been and suggest that as a
		//   replacement. This could potentially fit in the TurtleBrains::Core::String(Utilities).
		tb_error_if(false == UnlockKeys::VerifyKeyExists(unlockKey), "Error: There is no %s named '%s' in a rowData from: '%s'",
			UpgradeDataKey::UnlockKey.data(), unlockKey.c_str(), fullDebug.c_str());
	}

	return upgrade;
}

//--------------------------------------------------------------------------------------------------------------------//
