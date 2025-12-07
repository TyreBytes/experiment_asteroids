///
/// @file
/// @details Some quick tools and helpers for adjusting the screen resolution in real-time for development purposes.
///
/// <!-- Copyright (c) 2025 Tyre Bytes LLC - All Rights Reserved -->
///------------------------------------------------------------------------------------------------------------------///

#include "../development/screen_resolution_tool.hpp"
#include "../development/development.hpp"

#include <turtle_brains/game/tb_input_action.hpp>
#include <turtle_brains/game/tb_game_application.hpp>

#include <turtle_brains/application/tb_application_window.hpp>

//--------------------------------------------------------------------------------------------------------------------//

#if defined(rusty_development)

namespace Asteroids
{	// This exists in asteroids.cpp
	extern tbGame::GameApplication* theGameApplication;
};

namespace
{
	tbGame::InputAction theControlAction(tbApplication::tbKeyControlModifier);
	tbGame::InputAction theShiftAction(tbApplication::tbKeyShiftModifier);

	struct ResolutionAction
	{
		tbGame::InputAction mAction;
		tbCore::uint16 mWindowWidth;
		tbCore::uint16 mWindowHeight;
	};

	const std::vector<ResolutionAction> theResolutionActions{
		{ tbGame::InputAction(tbApplication::tbKeyF1), 1920, 1080 },
		{ tbGame::InputAction(tbApplication::tbKeyF2), 1680, 1050 },
		{ tbGame::InputAction(tbApplication::tbKeyF3), 1440, 900 },
		{ tbGame::InputAction(tbApplication::tbKeyF4), 1280, 1024 },
		{ tbGame::InputAction(tbApplication::tbKeyF5), 1280, 720 },
		{ tbGame::InputAction(tbApplication::tbKeyF6), 1024, 768 },
	};

	bool theSafeZoneIsVisible = false;
};

#endif /* rusty_development */

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::Development::UpdateDeveloperResolution(void)
{
#if defined(rusty_development)
	if (true == tbGame::Diagnostics::WantsKeyboardInput())
	{	// Skip any inputs in the function if keyboard is taken by ImGui
		return;
	}

	tbApplication::WindowProperties windowProperties;
	windowProperties.mVerticalSync = false;
	windowProperties.mWindowWidth = 1280;
	windowProperties.mWindowHeight = 720;
	windowProperties.mWindowMode = tbApplication::WindowMode::Windowed;
	windowProperties.mWindowPositionX = windowProperties.mWindowPositionY = 0; //Ignored in the SetWindowProperties function.

	if (true == theControlAction.IsDown())
	{
		const bool fullscreen = theShiftAction.IsDown();
		windowProperties.mWindowMode = fullscreen ? tbApplication::WindowMode::FullScreen : tbApplication::WindowMode::Windowed;

		for (const auto& resolutionAction : theResolutionActions)
		{
			if (true == resolutionAction.mAction.IsPressed())
			{
				windowProperties.mWindowWidth = resolutionAction.mWindowWidth;
				windowProperties.mWindowHeight = resolutionAction.mWindowHeight;
				theGameApplication->SetWindowProperties(windowProperties);
				break;
			}
		}
	}
	else
	{
		if (true == tbGame::Input::IsKeyPressed(tbApplication::tbKey0))
		{
			Development::ToggleDebug();
		}
	}

	if (true == tbGame::Input::IsKeyPressed(tbApplication::tbKey9))
	{
		theSafeZoneIsVisible = !theSafeZoneIsVisible;
	}
#endif /* rusty_development */
}

//--------------------------------------------------------------------------------------------------------------------//

bool Asteroids::Development::IsRenderingLayout(void)
{
#if defined(rusty_development)
	return (true == Development::IsDebugging() && true == theSafeZoneIsVisible);
#else
	return false;
#endif
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::Development::RenderSafeZone(void)
{
#if defined(rusty_development)
	if (true == Development::IsDebugging() && true == Development::IsRenderingLayout())
	{
		//4:3 safe area in the center of the screen.
		const float safeAspectRatio = 4.0f / 3.0f;

		const float safeWidth = tbGraphics::ScreenHeight() * safeAspectRatio;
		const float halfUnsafeWidth = (tbGraphics::ScreenWidth() - safeWidth) / 2.0f;

		const Color unsafeZoneColor = Color(0x80FF0000);
		tbGraphics::BoxShape unsafeZoneBox(halfUnsafeWidth, tbGraphics::ScreenHeight(), unsafeZoneColor);
		unsafeZoneBox.Render();

		unsafeZoneBox.SetPosition(tbGraphics::ScreenWidth() - unsafeZoneBox.GetWidth(), 0.0f);
		unsafeZoneBox.Render();

		//const Color safeZoneColor = Color(0x8000FF00);
		//tbGraphics::BoxShape safeZoneBox(tbGraphics::ScreenHeight() * safeAspectRatio, tbGraphics::ScreenHeight(), safeZoneColor);
		//safeZoneBox.SetOrigin(tbGraphics::kAnchorCenter);
		//safeZoneBox.SetPosition(tbGraphics::ScreenCenter());
		//safeZoneBox.Render();
	}
#endif /* rusty_development */
}

//--------------------------------------------------------------------------------------------------------------------//
