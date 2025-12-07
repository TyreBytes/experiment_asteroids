///
/// @file
/// @details Some quick tools and helpers for development.
///
/// <!-- Copyright (c) 2025 Tyre Bytes LLC - All Rights Reserved -->
///------------------------------------------------------------------------------------------------------------------///

#include "../development/development.hpp"
#if defined(rusty_development)

#include <turtle_brains/game/tb_input_action.hpp>
#include <turtle_brains/game/tb_game_application.hpp>

#include <turtle_brains/application/tb_application_window.hpp>

//--------------------------------------------------------------------------------------------------------------------//

namespace Asteroids::Development
{
	bool theDebuggerIsActive = false;
	bool theRunTimerIsDisabled = false;
};

//--------------------------------------------------------------------------------------------------------------------//

bool Asteroids::Development::IsDebugging(void)
{
	return theDebuggerIsActive;
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::Development::ToggleDebug(void)
{
	theDebuggerIsActive = !theDebuggerIsActive;
}

//--------------------------------------------------------------------------------------------------------------------//

bool Asteroids::Development::IsRunTimerDisabled(void)
{
	return theRunTimerIsDisabled;
}

//--------------------------------------------------------------------------------------------------------------------//

#endif /* rusty_development */
