///
/// @file
/// @details This is a small object that represents a full row of data in the TechTree csv table.
///
/// <!-- Copyright (c) 2025 Tyre Bytes LLC - All Rights Reserved -->
///------------------------------------------------------------------------------------------------------------------///

#ifndef Asteroids_TechTreeUpgrade_hpp
#define Asteroids_TechTreeUpgrade_hpp

#include "../asteroids.hpp"
#include "../game_manager.hpp"

#include <turtle_brains/core/tb_types.hpp>
#include <turtle_brains/core/tb_string.hpp>

namespace Asteroids
{
	using StatValuePair = std::pair<Stat, StatType>;
	using StatValueContainer = std::vector<StatValuePair>;
	using StringContainer = std::vector<String>;

	// While not strictly necessary, it would be a good idea to use this for very common string Keys to prevent potential
	//   spelling mistakes. If there is a spelling mistake in a key it can't be known at the start of the game, and in
	//   a public build a "<UNKNOWN>" string will appear, where debug/dev builds will/could trigger an error.
	class UpgradeData
	{
	public:
		String mTitle;
		String mDescription;
		String mSpriteInfo;

		StatValueContainer mStatCosts;
		StatValueContainer mStatLocks;
		StatValueContainer mStatModifyAdds;
		StatValueContainer mStatModifyMults;
		StringContainer mUnlocks;

		Vector2 mPosition;

		UpgradeKey mKey;
		std::vector<UpgradeKey> mParentKeys;
		int mColumn;
		int mRow;
		tbCore::uint8 mLevels;
	};

	template<> DynamicStructure ToDynamicStructure<UpgradeData>(const UpgradeData& session);
	template<> UpgradeData FromDynamicStructure(const DynamicStructure& data);

}; /* namespace Asteroids */

#endif /* Asteroids_TechTreeUpgrade_hpp */
