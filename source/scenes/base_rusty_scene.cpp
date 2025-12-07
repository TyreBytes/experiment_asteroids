///
/// @file
/// @details This scene is a simple base scene that ALL scenes in Asteroids will extend.
///
/// <!-- Copyright (c) 2025 Tyre Bytes LLC - All Rights Reserved -->
///------------------------------------------------------------------------------------------------------------------///

#include "../scenes/base_rusty_scene.hpp"
#include "../shader_system/shader_manager.hpp"
#include "../interface.hpp"

#if defined(rusty_development)
  #include "development/screen_resolution_tool.hpp"

  #include <turtle_brains/game/diagnostics/tb_game_diagnostics.hpp>
  #include <turtle_brains/game/diagnostics/tb_diagnostics_console.hpp>
  #include <turtle_brains/game/diagnostics/tb_time_profiler.hpp>
  #include <turtle_brains/development/tb_performance_timer.hpp>
#endif /* rusty_development */

// 2025-11-19: Seem to need BeginDraw() to setup and render the RenderTarget view correctly.
#define TurtleBrains_LetMeHave_Implementation
#include <turtle_brains/graphics/implementation/tbi_renderer.hpp>
#undef TurtleBrains_LetMeHave_Implementation

namespace Asteroids
{
	extern tbGame::GameApplication* theGameApplication;
	extern bool theGameIsActive;
};

//--------------------------------------------------------------------------------------------------------------------//

Asteroids::BaseRustyScene::BaseRustyScene(void) :
	tbGame::GameScene(),
	mInterfaceEntities(),
	mSettingsScreen(),
	mSettingsButton(),
	mWorldSpaceTarget(nullptr),
	mInterfaceTarget(nullptr),
	mLastScreenSize{ tbGraphics::ScreenWidth(), tbGraphics::ScreenHeight() }
{
	// 2025-12-02: Watch out for scenes that call ClearInterfaceEntities(), since they would not have the settings button.
	//   This is why the settings button does not display on the SupplyRunScene.
	ButtonFactory::SetupButton(mSettingsButton, ButtonType::SettingsButton, "");
	AddInterfaceEntity(mSettingsButton);

	mSettingsButton.SetClickCallback([this]() {
		if (false == IsSettingsOpen())
		{
			AddInterfaceEntity(mSettingsScreen);
		}

		tb_always_log("GOTCHYA!");
	});
}

//--------------------------------------------------------------------------------------------------------------------//

Asteroids::BaseRustyScene::~BaseRustyScene(void)
{
}

//--------------------------------------------------------------------------------------------------------------------//

bool Asteroids::BaseRustyScene::IsSettingsOpen(void) const
{
	// 2025-11-12: This is a dumb. I am adding it because TurtleBrains::Game::EntityManager doesn't have a const correct
	//   GetEntitiesByType() and we are 100% sure that mInterfaceEntities can be non-const because we own it, and it
	//   isn't declared with const... Still const_cast should be avoided, but the finger points to TurtleBrains...
	return (0 == const_cast<EntityManager&>(mInterfaceEntities).GetEntitiesByType("SettingsScreenEntity").size()) ? false : true;
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::BaseRustyScene::OnSimulate(void)
{
	tbGame::GameScene::OnSimulate();
	mInterfaceEntities.Simulate();
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::BaseRustyScene::OnUpdate(const float deltaTime)
{
	mSettingsButton.SetOrigin(Anchor::TopRight);
	mSettingsButton.SetPosition(Interface::GetAnchorPositionOfInterface(Anchor::TopRight, -kPadding, kPadding));

	tbGame::GameScene::OnUpdate(deltaTime);
	mInterfaceEntities.Update(deltaTime);

	{
		static tbSystem::Timer::Timer theElapsedTimer;
		theElapsedTimer.Update();

		GameManager::GetMutableStat(Stat::TotalPlayTime) += theElapsedTimer.GetDeltaTime();

		if (true == theGameIsActive)
		{
			GameManager::GetMutableStat(Stat::TotalActiveTime) += theElapsedTimer.GetDeltaTime();
		}
	}

#if defined(rusty_development)
	GameManager::TimeMultiplier(false == tbGame::Diagnostics::WantsKeyboardInput() &&
		(tbGame::Input::IsKeyDown(tbApplication::tbKeySpace)) ? 4.0f : 1.0f);

	Development::UpdateDeveloperResolution();

	if (true == tbGame::Input::IsKeyPressed(tbApplication::tbKeyTilde))
	{
		tbGame::Diagnostics::ToggleDiagnostics();
		tbGame::Diagnostics::ToggleDeveloperConsole();
	}
#endif

	if (nullptr == mWorldSpaceTarget || nullptr == mInterfaceTarget ||
		mLastScreenSize.x != tbGraphics::ScreenWidth() || mLastScreenSize.y != tbGraphics::ScreenHeight())
	{
		mLastScreenSize.x = tbGraphics::ScreenWidth();
		mLastScreenSize.y = tbGraphics::ScreenHeight();

		mWorldSpaceTarget = std::make_unique<tbGraphics::RenderTarget>(WorldTargetWidth(), WorldTargetHeight());
		mInterfaceTarget = std::make_unique<tbGraphics::RenderTarget>(Interface::TargetWidth(), Interface::TargetHeight());
	}

	// 2025-10-21: Technically starting the Render process here. It may fall into there "Update" time block!
	tb_error_if(nullptr == mWorldSpaceTarget, "Expected a valid world-space target to bind/update.");
	tb_error_if(nullptr == mInterfaceTarget, "Expected a valid interface target to bind/update.");

	{
		mWorldSpaceTarget->BindRenderTarget();
		mWorldSpaceTarget->ClearRenderTarget(ColorPalette::Black);
		tbGraphics::Implementation::Renderer::BeginDraw();
		OnRenderGameWorld();
	//	mWorldSpaceTarget->UnbindRenderTarget();
	//}

	//{

	//	mInterfaceTarget->BindRenderTarget();
	//	mInterfaceTarget->ClearRenderTarget(ColorPalette::Transparent);
	//	tbImplementation::Renderer::BeginDraw();
		OnRenderInterface();
		mInterfaceTarget->UnbindRenderTarget();
	}
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::BaseRustyScene::OnRenderGameWorld(void) const
{
	tbGame::GameScene::OnRender();
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::BaseRustyScene::OnRenderInterface(void) const
{
	mInterfaceEntities.Render();
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::BaseRustyScene::OnRender(void) const
{
	ShaderSystem::theShaderManager.BindShader(ShaderSystem::InvalidShader());

	tb_error_if(nullptr == mWorldSpaceTarget, "Expected a valid world-space target to render.");
	tb_error_if(nullptr == mInterfaceTarget, "Expected a valid interface target to render.");

	tbGraphics::Sprite gameWorldSprite(mWorldSpaceTarget->GetColorTextureHandle());
	gameWorldSprite.SetOrigin(Anchor::Center);
	gameWorldSprite.SetPosition(tbGraphics::ScreenCenter());
	gameWorldSprite.SetFlippedVertically(true);
	gameWorldSprite.SetScale(GameScale());
	gameWorldSprite.Render();

	//tbGraphics::Sprite interfaceSprite(mInterfaceTarget->GetColorTextureHandle());
	//interfaceSprite.SetOrigin(Anchor::Center);
	//interfaceSprite.SetPosition(tbGraphics::ScreenCenter());
	//interfaceSprite.SetFlippedVertically(true);
	//interfaceSprite.SetScale(Interface::Scale());
	//interfaceSprite.Render();

#if defined(rusty_development)
	Development::RenderSafeZone();

	//tbGraphics::Text fps(tb_string(theGameApplication->GetFrameRate()));
	//fps.SetOrigin(tbGraphics::kAnchorBottomRight);
	//fps.SetPosition(tbGraphics::ScreenWidth() - kPadding, tbGraphics::ScreenHeight() - kPadding);
	//fps.Render();
#endif
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::BaseRustyScene::OnOpen(void)
{
	tbGame::GameScene::OnOpen();
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::BaseRustyScene::OnClose(void)
{
	tbGame::GameScene::OnClose();
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::BaseRustyScene::OnRuntimeReload(void)
{
#if defined(rusty_development)
	ShaderSystem::theShaderManager.CheckAndReloadShaders();
#endif
}

//--------------------------------------------------------------------------------------------------------------------//
