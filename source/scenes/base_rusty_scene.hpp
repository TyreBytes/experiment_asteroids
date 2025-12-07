///
/// @file
/// @details This scene is a simple base scene that ALL scenes in Asteroids will extend.
///
/// <!-- Copyright (c) 2025 Tyre Bytes LLC - All Rights Reserved -->
///------------------------------------------------------------------------------------------------------------------///

#ifndef Asteroids_BaseRustyScene_hpp
#define Asteroids_BaseRustyScene_hpp

#include "../asteroids.hpp"
#include "../entities/button_entity.hpp"
#include "../entities/settings_screen_entity.hpp"

#include <turtle_brains/game/tb_game_scene.hpp>
#include <turtle_brains/graphics/tb_render_target.hpp>

namespace Asteroids
{

	class BaseRustyScene : public tbGame::GameScene
	{
	public:
		BaseRustyScene(void);
		virtual ~BaseRustyScene(void);

		bool IsSettingsOpen(void) const;

		inline void ClearInterfaceGraphics(void) { mInterfaceEntities.ClearGraphics(); }
		inline void AddInterfaceGraphic(Graphic* graphic) { mInterfaceEntities.AddGraphic(graphic); }
		inline void AddInterfaceGraphic(Graphic& graphic) { mInterfaceEntities.AddGraphic(graphic); }
		inline void RemoveInterfaceGraphic(Graphic* graphic) { mInterfaceEntities.RemoveGraphic(graphic); }

		inline void ClearInterfaceEntities(void) { mInterfaceEntities.ClearEntities(); }
		inline void AddInterfaceEntity(tbGame::Entity* entity) { mInterfaceEntities.AddEntity(entity); }
		inline void AddInterfaceEntity(tbGame::Entity& entity) { mInterfaceEntities.AddEntity(entity); }
		inline void RemoveInterfaceEntity(tbGame::Entity* entity) { mInterfaceEntities.RemoveEntity(entity); }
		inline void RemoveInterfaceEntities(const tbGame::EntityType& byType = tbGame::Entity::kInvalidType) { mInterfaceEntities.RemoveEntities(byType); }

	protected:
		virtual void OnOpen(void) override;
		virtual void OnClose(void) override;

		virtual void OnSimulate(void) override;
		virtual void OnUpdate(const float deltaTime) override;

		virtual void OnRenderGameWorld(void) const;
		virtual void OnRenderInterface(void) const;

	private:
		virtual void OnRender(void) const override;
		virtual void OnRuntimeReload(void);

	private:
		tbGame::EntityManager mInterfaceEntities;
		SettingsScreenEntity mSettingsScreen;
		ButtonEntity mSettingsButton;

		std::unique_ptr<tbGraphics::RenderTarget> mWorldSpaceTarget;
		std::unique_ptr<tbGraphics::RenderTarget> mInterfaceTarget;
		IntVector2 mLastScreenSize;
	};

};	//namespace Asteroids

#endif /* Asteroids_BaseRustyScene_hpp */
