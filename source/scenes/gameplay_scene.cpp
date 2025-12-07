///
/// @file
/// @details This scene is the main title scene the player will see upon starting the game.
///
/// <!-- Copyright (c) 2025 Tyre Bytes LLC - All Rights Reserved -->
///------------------------------------------------------------------------------------------------------------------///

#include "../scenes/gameplay_scene.hpp"
#include "../scenes/scene_manager.hpp"
#include "../interface.hpp"
#include "../development/development.hpp"
#include "../music_manager.hpp"
#include "../asteroids.hpp"
#include "../entities/asteroid_entity.hpp"

//--------------------------------------------------------------------------------------------------------------------//

Asteroids::GameplayScene::GameplayScene(void) :
	BaseRustyScene(),
	mSpaceBackdrop(),
	mRocketShip(ScreenSpaceToWorldSpace(tbGraphics::ScreenCenter()))
{
	mSpaceBackdrop.ResetTargetArea(WorldTargetWidth(), WorldTargetHeight());
	tbGraphics::Sprite spaceSprite("data/space/space_blue_nebula_08.png");
	spaceSprite.SetColor(Color(0x99FFFFFF));
	mSpaceBackdrop.AddParallaxLayer(spaceSprite, 1.0f);
}

//--------------------------------------------------------------------------------------------------------------------//

Asteroids::GameplayScene::~GameplayScene(void)
{
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::GameplayScene::OnSimulate(void)
{
	BaseRustyScene::OnSimulate();
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::GameplayScene::OnUpdate(const float deltaTime)
{
	if (true == tbGame::Input::IsKeyPressed(Key::tbKeyP))
	{
		GameManager::SetPaused(!GameManager::IsPaused());
	}

	Vector2 movement(100.0f, 100.0f);
	mSpaceBackdrop.SetPosition(mSpaceBackdrop.GetPosition() + movement * deltaTime);

	BaseRustyScene::OnUpdate(deltaTime);

	if (false == IsSettingsOpen() && true == tbGame::Input::IsKeyPressed(tbApplication::tbKeyEscape))
	{
		theSceneManager->ChangeToScene(SceneId::kTitleScene);
	}

	//Hacks:::
	if (true == tbApplication::Input::IsKeyPressed(Key::tbMouseRight))
	{
		const Vector2 mouseInWorldSpace = ScreenSpaceToWorldSpace(tbGame::Input::GetMousePosition());

		//const int asteroidSize = tbMath::RandomInt(0, 9);
		const int asteroidSize = 12;
		AddEntity(new AsteroidEntity(asteroidSize, mouseInWorldSpace));
	}
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::GameplayScene::OnRenderGameWorld(void) const
{
	BaseRustyScene::OnRenderGameWorld();

#if defined(rusty_development)
	if (true == Development::IsDebugging())
	{
		const float length = 100.0f;
		const Vector2 direction = RotationToForwardVector2(mRocketShip.GetRotation());
		tbGraphics::Line(mRocketShip.GetPosition(), mRocketShip.GetPosition() + direction * length).Render();
	}
#endif
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::GameplayScene::OnRenderInterface(void) const
{
	BaseRustyScene::OnRenderInterface();

	tbGraphics::Text levelText;
	Interface::MakeTitleText(levelText, NotLocalized("Level ") + GetStatAsIntegerString(Stat::Level));
	levelText.SetOrigin(Anchor::TopLeft);
	levelText.SetPosition(Interface::GetAnchorPositionOfInterface(Anchor::TopLeft, kPadding, kPadding));
	levelText.Render();

	tbGraphics::Text experienceText;
	Interface::MakeNormalText(experienceText, GetStatAsIntegerString(Stat::Experience) +
		NotLocalized(" / ") + MakeIntegerString(GameManager::GetExperienceForNextLevel()) + NotLocalized(" xp"));
	experienceText.SetOrigin(Anchor::BottomLeft);
	experienceText.SetPosition(levelText.GetAnchorPosition(Anchor::BottomRight, kPadding * 2.0f, 0.0f));
	experienceText.Render();
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::GameplayScene::OnOpen(void)
{
	BaseRustyScene::OnOpen();

	ClearEntities();
	ClearGraphics();

	AddGraphic(mSpaceBackdrop);

	AddEntity(mRocketShip);

	AddEntity(new AsteroidEntity(4, Vector2(tbMath::RandomFloat(50.0f, tbGraphics::ScreenWidth() - 50.0f),
		tbMath::RandomFloat(50.0f, tbGraphics::ScreenHeight() - 50.0f))));
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::GameplayScene::OnClose(void)
{
	BaseRustyScene::OnClose();
}

//--------------------------------------------------------------------------------------------------------------------//
