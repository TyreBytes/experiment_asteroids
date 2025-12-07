///
/// @file
/// @details An entity within the Asteroids project.
///
/// <!-- Copyright (c) 2025 Tyre Bytes LLC - All Rights Reserved -->
///------------------------------------------------------------------------------------------------------------------///

#ifndef Asteroids_AsteroidEntity_hpp
#define Asteroids_AsteroidEntity_hpp

#include "../asteroids.hpp"
#include "../graphics/asteroid_shape.hpp"

namespace Asteroids
{
	namespace Implementation
	{
		Vector2 RandomLinearVelocity(void);
	};

	class AsteroidEntity : public tbGame::Entity
	{
	public:
		explicit AsteroidEntity(const int size, const Vector2& position = tbGraphics::ScreenCenter(),
			const Vector2& velocity = Implementation::RandomLinearVelocity());
		virtual ~AsteroidEntity(void);

		inline bool IsAlive(void) const { return mHitPoints > 0; }
		inline float GetRadius(void) const { return mShape.GetRadius(); }

	protected:
		virtual void OnAdd(void) override;
		virtual void OnRemove(void) override;
		virtual void OnSimulate(void) override;
		virtual void OnUpdate(const float deltaTime) override;
		virtual void OnRender(void) const override;
		virtual void OnCollide(const tbGame::Entity& otherEntity) override;

	private:
		// Direction the bullet, or projectile is traveling.
		void BreakApart(const Vector2& impactDirection);

		AsteroidShape mShape;
		Vector2 mLinearVelocity;
		Angle mAngularVelocity;

		int mSize;
		int mHitPoints;
	};

};	//namespace Asteroids

#endif /* Asteroids_AsteroidEntity_hpp */
