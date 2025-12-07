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

namespace
{
	const float kPaddingMultiplier = 6.0f;
	const tbMath::Angle kCogRotationSpeed = 3.0_degrees; //per second
}

//--------------------------------------------------------------------------------------------------------------------//

Asteroids::GameplayScene::GameplayScene(void) :
	BaseRustyScene()
{
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

	MusicSystem::theMusicManager.PlayMusic(MusicSystem::MusicTrack::Opening);
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::GameplayScene::OnClose(void)
{
	BaseRustyScene::OnClose();
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::GameplayScene::LoadProfileAndLaunchGame(const int profile)
{
	GameManager::SetActiveProfile(profile);

	// 2025-11-11: The state for all the profiles is always loaded, but if we are going to call LoadGame here, we should
	//   also be calling SaveGame() before it to save any of the changes to the previous active profile, otherwise the
	//   load would wipe the progress.
	//GameManager::SaveGame();
	//GameManager::LoadGame();

	tb_always_log(LogGame::Info() << "The player is trying to start the game...");

	//if (GameManager::IsUnlocked(UnlockKeys::FirstGameLaunch))
	//{
	//	theSceneManager->ChangeToScene(SceneId::kGarageScene);
	//}
	//else
	//{
	//	GameManager::UnlockKey(UnlockKeys::FirstGameLaunch);
	//	theSceneManager->ChangeToScene(SceneId::kSupplyRunScene);
	//}
}

//--------------------------------------------------------------------------------------------------------------------//
