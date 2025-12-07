///
/// @file
/// @details Some quick tools and helpers for development.
///
/// <!-- Copyright (c) 2025 Tyre Bytes LLC - All Rights Reserved -->
///------------------------------------------------------------------------------------------------------------------///

#ifndef Asteroids_Development_hpp
#define Asteroids_Development_hpp

#include "../asteroids.hpp"

#if defined(rusty_development)
#include "../development/screen_resolution_tool.hpp"

#include <turtle_brains/game/diagnostics/tb_game_diagnostics.hpp>

namespace Asteroids::Development
{

	bool IsDebugging(void);
	void ToggleDebug(void);

	bool IsRunTimerDisabled(void);

}; /* Asteroids::Development */

#endif /* rusty_development */
#endif /* Asteroids_Development_hpp */
