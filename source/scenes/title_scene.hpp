///
/// @file
/// @details This scene is the main title scene the player will see upon starting the game.
///
/// <!-- Copyright (c) 2025 Tyre Bytes LLC - All Rights Reserved -->
///------------------------------------------------------------------------------------------------------------------///

#ifndef Asteroids_TitleScene_hpp
#define Asteroids_TitleScene_hpp

#include "../scenes/base_rusty_scene.hpp"

#include <turtle_brains/game/tb_game_scene.hpp>

namespace Asteroids
{

	class TitleScene : public BaseRustyScene
	{
	public:
		TitleScene(void);
		virtual ~TitleScene(void);

	protected:
		virtual void OnSimulate(void) override;
		virtual void OnUpdate(const float deltaTime) override;
		virtual void OnRenderGameWorld(void) const override;
		virtual void OnRenderInterface(void) const override;
		virtual void OnOpen(void) override;
		virtual void OnClose(void) override;

	private:
		void LoadProfileAndLaunchGame(const int profile);

		tbGraphics::Text mTitleText;
		//std::array<ButtonEntity, GameManager::GetNumberOfProfiles()> mProfileButtons;
		ButtonEntity mPlayButton;
		ButtonEntity mQuitButton;

		tbMath::Angle mCogRotationSpeed;
	};

};	//namespace Asteroids

#endif /* Asteroids_TitleScene_hpp */
