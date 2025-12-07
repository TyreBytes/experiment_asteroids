///
/// @file
/// @details This scene is a simple base scene that adds a few buttons that will jump between the different upgrade
///   scenes and into a scavenge mode, ensuring they all have a common base for DRY code.
///
/// <!-- Copyright (c) 2025 Tyre Bytes LLC - All Rights Reserved -->
///------------------------------------------------------------------------------------------------------------------///

#include "../scenes/base_upgrade_scene.hpp"
#include "../scenes/scene_manager.hpp"

#include "../game_manager.hpp"
#include "../interface.hpp"

//--------------------------------------------------------------------------------------------------------------------//

Asteroids::BaseUpgradeScene::BaseUpgradeScene(const SceneId /*childScene*/) :
	BaseRustyScene(),
	mScavengeButton(),
	mGarageButton(),
	mChallengerButton(),
	mQuestButton(),
	mTheCampButton()
{
	ButtonFactory::SetupButton(mScavengeButton, ButtonType::Primary, NotLocalized("Play"));
	mScavengeButton.SetClickCallback([]() {
		theSceneManager->ChangeToScene(SceneId::kSupplyRunScene);
	});
	AddInterfaceEntity(mScavengeButton);


	//ButtonFactory::SetupButton(mGarageButton, (SceneId::kGarageScene == childScene) ?
	//	ButtonType::UpgradeTabOpen : ButtonType::UpgradeTab, Localize(StringKey::Common_Button_Upgrade));
	//mGarageButton.SetClickCallback([]() {
	//	theSceneManager->ChangeToScene(SceneId::kGarageScene);
	//});
	//AddInterfaceEntity(mGarageButton);

	//ButtonFactory::SetupButton(mChallengerButton, (SceneId::kChallengerScene == childScene) ?
	//	ButtonType::UpgradeTabOpen : ButtonType::UpgradeTab, Localize(StringKey::Common_Button_Challengers));
	//mChallengerButton.SetClickCallback([]() {
	//	theSceneManager->ChangeToScene(SceneId::kChallengerScene);
	//});
	//AddInterfaceEntity(mChallengerButton);

	//ButtonFactory::SetupButton(mQuestButton, (SceneId::kQuestScene == childScene) ?
	//	ButtonType::UpgradeTabOpen : ButtonType::UpgradeTab, Localize(StringKey::Common_Button_Quests));
	//mQuestButton.SetClickCallback([]() {
	//	theSceneManager->ChangeToScene(SceneId::kQuestScene);
	//});
	//AddInterfaceEntity(mQuestButton);

	//ButtonFactory::SetupButton(mTheCampButton, (SceneId::kTheCampScene == childScene) ?
	//	ButtonType::UpgradeTabOpen : ButtonType::UpgradeTab, Localize(StringKey::Common_Button_TheCamp));
	//mTheCampButton.SetClickCallback([]() {
	//	theSceneManager->ChangeToScene(SceneId::kTheCampScene);
	//});
	//AddInterfaceEntity(mTheCampButton);
}

//--------------------------------------------------------------------------------------------------------------------//

Asteroids::BaseUpgradeScene::~BaseUpgradeScene(void)
{
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::BaseUpgradeScene::OnSimulate(void)
{
	BaseRustyScene::OnSimulate();
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::BaseUpgradeScene::OnUpdate(const float deltaTime)
{
	BaseRustyScene::OnUpdate(deltaTime);

	if (false == IsSettingsOpen() && true == tbGame::Input::IsKeyPressed(tbApplication::tbKeyEscape))
	{
		theSceneManager->ChangeToScene(SceneId::kTitleScene);
	}

	//static_cast<tbGraphics::Graphic&>(mScavengeButton).SetOrigin(Anchor::BottomRight);
	//mScavengeButton.SetPosition(Interface::GetAnchorPositionOfInterface(Anchor::BottomRight, -kPadding, -kPadding));

	//// Enable or disable the buttons based on current progression.
	//const bool canChallenge = (true == GameManager::IsUnlocked(UnlockKeys::ChallengerMode));
	//const bool canHaveQuest = (true == GameManager::IsUnlocked(UnlockKeys::QuestMode));
	//const bool canJumpToCamp = (true == GameManager::IsUnlocked(UnlockKeys::CampMode));

	////As the code is written right now:
	////  std::vector<tbGraphics::Graphic*> order { &mGarageButton, &mChallengerButton, &mQuestButton, &mTheCampButton };
	//std::vector<ButtonEntity*> order { &mGarageButton, &mQuestButton, &mChallengerButton, &mTheCampButton };

	//tb_error_if(order.size() < 2, "Expected at least two buttons in the order above.");

	//Vector2 nextButtonPosition = Interface::GetAnchorPositionOfInterface(Anchor::TopLeft, kPadding, kPadding);

	//for (ButtonEntity* button : order)
	//{
	//	button->SetOrigin(Anchor::TopLeft);
	//	button->SetPosition(nextButtonPosition);
	//	nextButtonPosition = button->GetAnchorPosition(Anchor::TopRight, kPadding, 0.0f);
	//}

	//mChallengerButton.SetActive(canChallenge);
	//mChallengerButton.SetVisible(canChallenge);
	//mQuestButton.SetActive(canHaveQuest);
	//mQuestButton.SetVisible(canHaveQuest);
	//mTheCampButton.SetActive(canJumpToCamp);
	//mTheCampButton.SetVisible(canJumpToCamp);
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::BaseUpgradeScene::OnRenderGameWorld(void) const
{
	BaseRustyScene::OnRenderGameWorld();
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::BaseUpgradeScene::OnRenderInterface(void) const
{
	BaseRustyScene::OnRenderInterface();
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::BaseUpgradeScene::OnOpen(void)
{
	BaseRustyScene::OnOpen();
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::BaseUpgradeScene::OnClose(void)
{
	BaseRustyScene::OnClose();
}

//--------------------------------------------------------------------------------------------------------------------//
