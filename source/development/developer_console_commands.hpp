///
/// @file
/// @details Some quick commands for the developer console so the dev can give extra resources, see the unlocked keys or
///   unlock a key, etc for make progression and testing much faster.
///
/// <!-- Copyright (c) 2025 Tyre Bytes LLC - All Rights Reserved -->
///------------------------------------------------------------------------------------------------------------------///

#ifndef Asteroids_DeveloperConsoleCommands_hpp
#define Asteroids_DeveloperConsoleCommands_hpp

#include "asteroids.hpp"


namespace Asteroids::Development
{
	///
	/// @details Registers the custom commands for Asteroids developer console.
	///
	void RegisterDeveloperCommands(void);

}; /* Asteroids::Development */

#endif /* Asteroids_DeveloperConsoleCommands_hpp */
