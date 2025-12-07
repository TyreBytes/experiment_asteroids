///
/// @file
/// @details This is an area for Asteroids Utility Functions...
///
/// <!-- Copyright (c) 2025 Tyre Bytes LLC - All Rights Reserved -->
///------------------------------------------------------------------------------------------------------------------///

#ifndef Asteroids_Utilities_hpp
#define Asteroids_Utilities_hpp

#include "../asteroids.hpp"

#include <vector>

namespace Asteroids
{	// 2025-10-31: Currently these are forced into Asteroids, eventually TurtleBrains may force this into Global Space?
	//   Unsure if there would be a way to make it as Utilities, or such.
	template<> Asteroids::AnchorLocation FromDynamicStructure(const DynamicStructure& data);
};

namespace Asteroids::Utilities
{

	DynamicStructure LoadJsonFile(const String& filepath);
	DynamicStructure CreateActualDataFromPotentialBases(const DynamicStructure& childData,
		const String& baseMemberName, const DynamicStructure& basesArray);

	template<typename Type> void Shuffle(std::vector<Type>& container)
	{
		std::random_device rd;
		std::mt19937 g(rd());
		std::shuffle(container.begin(), container.end(), g);
	}

	template<typename Type> std::vector<Type> Shuffle(const std::vector<Type>& container)
	{
		std::vector<Type> copyOfContainer = container;

		std::random_device rd;
		std::mt19937 g(rd());
		std::shuffle(copyOfContainer.begin(), copyOfContainer.end(), g);

		return copyOfContainer;
	}

}; /* namespace Asteroids::Utilities */

#endif /* Asteroids_Utilities_hpp */
