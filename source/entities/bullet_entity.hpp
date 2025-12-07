///
/// @file
/// @details An entity within the Asteroids project that is a projectile flying through space to destroy an asteroid!
///
/// <!-- Copyright (c) 2025 Tyre Bytes LLC - All Rights Reserved -->
///------------------------------------------------------------------------------------------------------------------///

#ifndef Asteroids_BulletEntity_hpp
#define Asteroids_BulletEntity_hpp

#include "../asteroids.hpp"

namespace Asteroids
{

	class BulletEntity : public tbGame::Entity
	{
	public:
		explicit BulletEntity(const Vector2& position, const Vector2& velocity);
		virtual ~BulletEntity(void);

	protected:
		virtual void OnAdd(void) override;
		virtual void OnRemove(void) override;
		virtual void OnSimulate(void) override;
		virtual void OnUpdate(const float deltaTime) override;
		virtual void OnRender(void) const override;
		virtual void OnCollide(const tbGame::Entity& otherEntity) override;

	private:
		tbGraphics::Sprite mSprite;
		Vector2 mLinearVelocity;
		float mRadius;
	};

};	//namespace Asteroids

#endif /* Asteroids_BulletEntity_hpp */
