///
/// @file
/// @details An entity within the Asteroids project.
///
/// <!-- Copyright (c) 2025 Tyre Bytes LLC - All Rights Reserved -->
///------------------------------------------------------------------------------------------------------------------///

#ifndef Asteroids_TemplateEntity_hpp
#define Asteroids_TemplateEntity_hpp

#include "../asteroids.hpp"

namespace Asteroids
{

	class TemplateEntity : public tbGame::Entity
	{
	public:
		TemplateEntity(void);
		virtual ~TemplateEntity(void);

	protected:
		virtual void OnAdd(void) override;
		virtual void OnRemove(void) override;
		virtual void OnSimulate(void) override;
		virtual void OnUpdate(const float deltaTime) override;
		virtual void OnRender(void) const override;
		virtual void OnCollide(const tbGame::Entity& otherEntity) override;

	private:
		tbGraphics::Sprite mSprite;
	};

};	//namespace Asteroids

#endif /* Asteroids_TemplateEntity_hpp */
