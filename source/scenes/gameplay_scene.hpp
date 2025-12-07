///
/// @file
/// @details This scene is the main title scene the player will see upon starting the game.
///
/// <!-- Copyright (c) 2025 Tyre Bytes LLC - All Rights Reserved -->
///------------------------------------------------------------------------------------------------------------------///

#ifndef Asteroids_GameplayScene_hpp
#define Asteroids_GameplayScene_hpp

#include "../scenes/base_rusty_scene.hpp"

#include <turtle_brains/game/tb_game_scene.hpp>
#include <turtle_brains/express/graphics/tbx_parallax_backdrop.hpp>

namespace Asteroids
{

	class GameplayScene : public BaseRustyScene
	{
	public:
		GameplayScene(void);
		virtual ~GameplayScene(void);

	protected:
		virtual void OnSimulate(void) override;
		virtual void OnUpdate(const float deltaTime) override;
		virtual void OnRenderGameWorld(void) const override;
		virtual void OnRenderInterface(void) const override;
		virtual void OnOpen(void) override;
		virtual void OnClose(void) override;

	private:
		tbxGraphics::ParallaxBackdrop mSpaceBackdrop;
	};

};	//namespace Asteroids

#endif /* Asteroids_GameplayScene_hpp */
