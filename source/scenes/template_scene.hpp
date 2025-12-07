///
/// @file
/// @details This scene is for the Asteroids project.
///
/// <!-- Copyright (c) 2025 Tyre Bytes LLC - All Rights Reserved -->
///------------------------------------------------------------------------------------------------------------------///

#ifndef Asteroids_TemplateScene_hpp
#define Asteroids_TemplateScene_hpp

#include "../scenes/base_rusty_scene.hpp"

#include <turtle_brains/game/tb_game_scene.hpp>

namespace Asteroids
{

	class TemplateScene : public BaseRustyScene
	{
	public:
		TemplateScene(void);
		virtual ~TemplateScene(void);

	protected:
		virtual void OnSimulate(void) override;
		virtual void OnUpdate(const float deltaTime) override;
		virtual void OnRenderGameWorld(void) const override;
		virtual void OnRenderInterface(void) const override;
		virtual void OnOpen(void) override;
		virtual void OnClose(void) override;

	private:
	};

};	//namespace Asteroids

#endif /* Asteroids_TemplateScene_hpp */
