///
/// @file
/// @details This scene is a simple base scene that adds a few buttons that will jump between the different upgrade
///   scenes and into a scavenge mode, ensuring they all have a common base for DRY code.
///
/// <!-- Copyright (c) 2025 Tyre Bytes LLC - All Rights Reserved -->
///------------------------------------------------------------------------------------------------------------------///

#ifndef Asteroids_BaseUpgradeScene_hpp
#define Asteroids_BaseUpgradeScene_hpp

#include "../scenes/base_rusty_scene.hpp"
#include "../scenes/scene_manager.hpp"

#include "../entities/button_entity.hpp"

#include <turtle_brains/game/tb_game_scene.hpp>
#include <turtle_brains/express/game/tbx_basic_tech_tree.hpp>

namespace Asteroids
{

	class BaseUpgradeScene : public BaseRustyScene
	{
	public:
		explicit BaseUpgradeScene(const SceneId childScene);
		virtual ~BaseUpgradeScene(void);

	protected:
		virtual void OnSimulate(void) override;
		virtual void OnUpdate(const float deltaTime) override;
		virtual void OnRenderGameWorld(void) const override;
		virtual void OnRenderInterface(void) const override;
		virtual void OnOpen(void) override;
		virtual void OnClose(void) override;

	private:
		ButtonEntity mScavengeButton;
		ButtonEntity mGarageButton;
		ButtonEntity mChallengerButton;
		ButtonEntity mQuestButton;
		ButtonEntity mTheCampButton;
	};

};	//namespace Asteroids

#endif /* Asteroids_BaseUpgradeScene_hpp */
