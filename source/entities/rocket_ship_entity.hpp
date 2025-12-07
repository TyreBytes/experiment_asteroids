///
/// @file
/// @details An entity within the Asteroids project.
///
/// <!-- Copyright (c) 2025 Tyre Bytes LLC - All Rights Reserved -->
///------------------------------------------------------------------------------------------------------------------///

#ifndef Asteroids_RocketShipEntity_hpp
#define Asteroids_RocketShipEntity_hpp

#include "../asteroids.hpp"
#include "../graphics/rocket_ship_shape.hpp"

namespace Asteroids
{

	class RocketShipEntity : public tbGame::Entity
	{
	public:
		explicit RocketShipEntity(const Vector2& position = tbGraphics::ScreenCenter());
		virtual ~RocketShipEntity(void);

	protected:
		virtual void OnAdd(void) override;
		virtual void OnRemove(void) override;
		virtual void OnSimulate(void) override;
		virtual void OnUpdate(const float deltaTime) override;
		virtual void OnRender(void) const override;
		virtual void OnCollide(const tbGame::Entity& otherEntity) override;

	private:
		tbGame::InputAction mThrustForward;
		tbGame::InputAction mThrustBackward;
		tbGame::InputAction mThrustRight;
		tbGame::InputAction mThrustLeft;

		RocketShipShape mShape;
		Vector2 mLinearVelocity;
		Angle mAngularVelocity;
	};

};	//namespace Asteroids

#endif /* Asteroids_RocketShipEntity_hpp */
