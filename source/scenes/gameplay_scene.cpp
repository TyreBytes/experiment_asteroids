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
	AddGraphic(mSpaceBackdrop);

	AddEntity(mRocketShip);
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
	Vector2 movement(100.0f, 100.0f);
	mSpaceBackdrop.SetPosition(mSpaceBackdrop.GetPosition() + movement * deltaTime);

	BaseRustyScene::OnUpdate(deltaTime);

	if (false == IsSettingsOpen() && true == tbGame::Input::IsKeyPressed(tbApplication::tbKeyEscape))
	{
		theSceneManager->ChangeToScene(SceneId::kTitleScene);
	}

	if (true == tbApplication::Input::IsKeyPressed(Key::tbMouseLeft))
	{
		const Vector2 mouseInWorldSpace = ScreenSpaceToWorldSpace(tbGame::Input::GetMousePosition());

		const int asteroidSize = tbMath::RandomInt(0, 9);
		AddEntity(new AsteroidEntity(asteroidSize, mouseInWorldSpace));
	}
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::GameplayScene::OnRenderGameWorld(void) const
{
	BaseRustyScene::OnRenderGameWorld();

	const float length = 100.0f;
	const Vector2 direction = RotationToForwardVector2(mRocketShip.GetRotation());
	tbGraphics::Line(mRocketShip.GetPosition(), mRocketShip.GetPosition() + direction * length).Render();
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::GameplayScene::OnRenderInterface(void) const
{
	BaseRustyScene::OnRenderInterface();
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::GameplayScene::OnOpen(void)
{
	BaseRustyScene::OnOpen();
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::GameplayScene::OnClose(void)
{
	BaseRustyScene::OnClose();
}

//--------------------------------------------------------------------------------------------------------------------//
