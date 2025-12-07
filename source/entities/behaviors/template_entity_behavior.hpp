///
/// @file
/// @details A template to create a behavior for entities within the Asteroids project.
///
/// <!-- Copyright (c) 2025 Tyre Bytes LLC - All Rights Reserved -->
///------------------------------------------------------------------------------------------------------------------///

#ifndef Asteroids_TemplateEntityBehavior_hpp
#define Asteroids_TemplateEntityBehavior_hpp

#include "../../asteroids.hpp"

namespace Asteroids
{

	class TemplateBehavior : public tbGame::EntityBehaviorInterface
	{
	public:
		explicit TemplateBehavior(tbGame::Entity& entity);
		virtual ~TemplateBehavior(void);

	protected:
		virtual void OnAdd(void) override;
		virtual void OnRemove(void) override;
		virtual void OnPause(void) override;
		virtual void OnResume(void) override;

		virtual void OnSimulate(void) override;
		virtual void OnUpdate(const float deltaTime) override;
		virtual void OnRender(void) const override;
		virtual void OnCollide(const tbGame::Entity& withOther) override;

	private:
	};

};	//namespace Asteroids

#endif /* Asteroids_TemplateEntityBehavior_hpp */
