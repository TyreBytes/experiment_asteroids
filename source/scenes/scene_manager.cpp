///
/// @file
/// @details Contains all of the scenes in the Asteroids project from splash, menus, options,
///   to gameplay to provide an  easy way for the scenes to be managed and changed between.
///   (Although TurtleBrains supplies a way to change between GameScene's it doesn't provide a great way to manage their memory.)
///
/// <!-- Copyright (c) 2025 Tyre Bytes LLC - All Rights Reserved -->
///------------------------------------------------------------------------------------------------------------------///

#include "../scenes/scene_manager.hpp"
#include "../scenes/title_scene.hpp"
#include "../scenes/gameplay_scene.hpp"

Asteroids::SceneManager* Asteroids::theSceneManager(nullptr);

//--------------------------------------------------------------------------------------------------------------------//

Asteroids::SceneManager::SceneManager(void) :
	mScenes()
{
	// 2025-10-15: Creating the SupplyRunScene must be before the GarageScene....
	// 2025-10-24: ChallengerScene must be created before DragRaceScene so that the VehicleInfo has loaded.

	mScenes.resize(kSceneCount, nullptr);
	mScenes[SceneId::kTitleScene] = new TitleScene();
	mScenes[SceneId::kGameplayScene] = new GameplayScene();

}

//--------------------------------------------------------------------------------------------------------------------//

Asteroids::SceneManager::~SceneManager(void)
{
	while (false == mScenes.empty())
	{
		delete mScenes.back();
		mScenes.pop_back();
	}
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::SceneManager::CreateSceneManager(void)
{
	tb_error_if(nullptr != theSceneManager, "Expected theSceneManager to be null when calling create, seems create has been called twice.");
	theSceneManager = new SceneManager();
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::SceneManager::DestroySceneManager(void)
{
	tb_error_if(nullptr == theSceneManager, "Expected theSceneManager to be a valid pointer when calling destroy, seems destroy has been called twice, or without create.");
	delete theSceneManager;
	theSceneManager = nullptr;
}

//--------------------------------------------------------------------------------------------------------------------//

tbGame::GameScene& Asteroids::SceneManager::GetScene(const SceneId& sceneIdentifier)
{
	tb_error_if(nullptr == theSceneManager, "Expected theSceneManager to be created and valid before ChangeToScene is called.");
	tb_error_if(sceneIdentifier < 0, "Expected a valid sceneIdentifier, below zero is out of range.");
	tb_error_if(static_cast<size_t>(sceneIdentifier) >= theSceneManager->mScenes.size(), "Expected a valid sceneIdentifier, out of range.  Was the scene added to manager?");
	return *theSceneManager->mScenes[sceneIdentifier];
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::SceneManager::ChangeToScene(const SceneId& sceneIdentifier)
{
	tbGame::GameScene::ChangeToScene(SceneManager::GetScene(sceneIdentifier));
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::SceneManager::QuitGame(void)
{
	tb_error_if(nullptr == theSceneManager, "Expected theSceneManager to be created and valid before ChangeToScene is called.");
	tbGame::GameApplication::MarkForClose();
}

//--------------------------------------------------------------------------------------------------------------------//
