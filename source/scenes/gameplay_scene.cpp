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

//--------------------------------------------------------------------------------------------------------------------//

Asteroids::GameplayScene::GameplayScene(void) :
	BaseRustyScene(),
	mSpaceBackdrop()
{
	mSpaceBackdrop.ResetTargetArea(WorldTargetWidth(), WorldTargetHeight());
	tbGraphics::Sprite spaceSprite("data/space/space_blue_nebula_08.png");
	spaceSprite.SetColor(Color(0x99FFFFFF));
	mSpaceBackdrop.AddParallaxLayer(spaceSprite, 1.0f);
	AddGraphic(mSpaceBackdrop);
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
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::GameplayScene::OnRenderGameWorld(void) const
{
	BaseRustyScene::OnRenderGameWorld();
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
