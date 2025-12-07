///
/// @file
/// @details This scene is the main title scene the player will see upon starting the game.
///
/// <!-- Copyright (c) 2025 Tyre Bytes LLC - All Rights Reserved -->
///------------------------------------------------------------------------------------------------------------------///

#include "../scenes/title_scene.hpp"
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

Asteroids::TitleScene::TitleScene(void) :
	BaseRustyScene(),
	mTitleText("Asteroids"),
	//mProfileButtons(),
	mPlayButton(),
	mQuitButton(),
	mCogRotationSpeed(kCogRotationSpeed)
{
	AddInterfaceGraphic(mTitleText);

	//int profileIndex = 0;
	//for (ButtonEntity& profileButton : mProfileButtons)
	//{
	//	const String profileString = ToString(profileIndex + 1);
	//	ButtonFactory::SetupButton(profileButton, ButtonType::Primary, Localize(StringKey::Title_Profile, profileString));
	//	AddInterfaceEntity(profileButton);
	//	profileButton.SetClickCallback([this, profileIndex]() {
	//		LoadProfileAndLaunchGame(profileIndex);
	//	});

	//	++profileIndex;
	//}

	ButtonFactory::SetupButton(mPlayButton, ButtonType::Primary, Localize(StringKey::Title_Play));
	AddInterfaceEntity(mPlayButton);
	mPlayButton.SetClickCallback([this]() {
		LoadProfileAndLaunchGame(0);
	});

	ButtonFactory::SetupButton(mQuitButton, ButtonType::Secondary, Localize(StringKey::Title_Quit));
	AddInterfaceEntity(mQuitButton);
	mQuitButton.SetClickCallback([]() {
		SceneManager::QuitGame();
	});
}

//--------------------------------------------------------------------------------------------------------------------//

Asteroids::TitleScene::~TitleScene(void)
{
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::TitleScene::OnSimulate(void)
{
	BaseRustyScene::OnSimulate();
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::TitleScene::OnUpdate(const float deltaTime)
{
	BaseRustyScene::OnUpdate(deltaTime);

	//int profileIndex = 0;
	//for (ButtonEntity& profileButton : mProfileButtons)
	//{
	//	const float width = 300.0f + kPadding * 2.0f;
	//	const float offsetX = width * profileIndex - ((width * 3.0f) / 2.0f) + kPadding;
	//	const float quarterInterfaceHeight = Interface::Height() / 3.0f;
	//	profileButton.SetOrigin(Anchor::CenterLeft);
	//	profileButton.SetPosition(Interface::GetAnchorPositionOfInterface(Anchor::BottomCenter, offsetX, -quarterInterfaceHeight));

	//	++profileIndex;
	//}

	const float quarterInterfaceHeight = Interface::Height() / 3.0f;
	mPlayButton.SetOrigin(Anchor::Center);
	mPlayButton.SetPosition(Interface::GetAnchorPositionOfInterface(Anchor::BottomCenter, 0.0f, -quarterInterfaceHeight));

	mQuitButton.SetOrigin(Anchor::BottomCenter);
	mQuitButton.SetPosition(Interface::GetAnchorPositionOfInterface(Anchor::BottomCenter, 0.0f, - kPadding));

	if (false == IsSettingsOpen())
	{
		if (true == tbGame::Input::IsKeyPressed(tbApplication::tbKeyEscape))
		{
			SceneManager::QuitGame();
		}

		mTitleText.SetOrigin(Anchor::Center);
		mTitleText.SetPosition(Interface::GetAnchorPositionOfInterface(Anchor::Center));

		mQuitButton.SetActive(true);
		mPlayButton.SetActive(true);
		//for (ButtonEntity& profileButton : mProfileButtons)
		//{
		//	profileButton.SetActive(true);
		//}
	}
	else
	{
		mQuitButton.SetActive(false);
		mPlayButton.SetActive(false);
		//for (ButtonEntity& profileButton : mProfileButtons)
		//{
		//	profileButton.SetActive(false);
		//}
	}

#if defined(rusty_development)
	if (true == tbGame::Input::IsKeyPressed(tbApplication::tbMouseRight))
	{
		tb_always_log(LogGame::Always() << "The developer right-clicked to play drag race!");
		SceneManager::ChangeToScene(SceneId::kDragRaceScene);
	}
#endif
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::TitleScene::OnRenderGameWorld(void) const
{
	BaseRustyScene::OnRenderGameWorld();
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::TitleScene::OnRenderInterface(void) const
{
	BaseRustyScene::OnRenderInterface();
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::TitleScene::OnOpen(void)
{
	BaseRustyScene::OnOpen();

	MusicSystem::theMusicManager.PlayMusic(MusicSystem::MusicTrack::Opening);
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::TitleScene::OnClose(void)
{
	BaseRustyScene::OnClose();
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::TitleScene::LoadProfileAndLaunchGame(const int profile)
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
