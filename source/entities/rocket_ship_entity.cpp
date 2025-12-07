///
/// @file
/// @details An entity within the Asteroids project.
///
/// <!-- Copyright (c) 2025 Tyre Bytes LLC - All Rights Reserved -->
///------------------------------------------------------------------------------------------------------------------///

#include "../entities/rocket_ship_entity.hpp"

//--------------------------------------------------------------------------------------------------------------------//

Asteroids::RocketShipEntity::RocketShipEntity(const Vector2& position) :
	tbGame::Entity("RocketShipEntity"),
	mThrustForward(Key::tbKeyUp),
	mThrustBackward(Key::tbKeyDown),
	mThrustRight(Key::tbKeyRight),
	mThrustLeft(Key::tbKeyLeft),
	mShape(TyreBytes::ColorPalette::Pink),
	mLinearVelocity(Vector2::Zero()),
	mAngularVelocity(Angle::Zero())
{
	mThrustForward.AddBinding(tbApplication::tbKeyW);
	mThrustBackward.AddBinding(tbApplication::tbKeyS);
	mThrustRight.AddBinding(tbApplication::tbKeyD);
	mThrustLeft.AddBinding(tbApplication::tbKeyA);

	//mShape.SetOrigin(Anchor::Center);
	AddGraphic(mShape);

	SetPosition(position);
}

//--------------------------------------------------------------------------------------------------------------------//

Asteroids::RocketShipEntity::~RocketShipEntity(void)
{
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::RocketShipEntity::OnAdd(void)
{
	tbGame::Entity::OnAdd();
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::RocketShipEntity::OnRemove(void)
{
	tbGame::Entity::OnRemove();
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::RocketShipEntity::OnSimulate(void)
{
	tbGame::Entity::OnSimulate();

	const float kMaximumLinearSpeed = 500.0f;
	const Angle kMaximumAngularSpeed = 360.0_degrees; //per second
	const float kLinearAcceleration = 500.0f; //per second^2
	const Angle kAngularAcceleration = 1080.0_degrees; //per second^2

	//Drag.
	//mLinearVelocity = tbMath::Interpolation::Linear(0.05f, mLinearVelocity, Vector2::Zero());
	mAngularVelocity = tbMath::Interpolation::Linear(0.05f, mAngularVelocity, 0.0_degrees);

	if (mLinearVelocity.Magnitude() < 0.1f)
	{
		mLinearVelocity = Vector2::Zero();
	}

	if (std::abs(mAngularVelocity.AsDegrees()) < 0.1f)
	{
		mAngularVelocity = Angle::Zero();
	}

	// 2025-12-07: positive angles are suppose to be clockwise ...
	const Vector2 direction = RotationToForwardVector2(GetRotation());

	if (true == mThrustForward.IsDown()) { mLinearVelocity += direction * kLinearAcceleration * FixedTime(); }
	if (true == mThrustBackward.IsDown()) { mLinearVelocity -= direction * 0.4f * kLinearAcceleration * FixedTime(); }
	if (true == mThrustRight.IsDown()) { mAngularVelocity -= kAngularAcceleration * FixedTime(); }
	if (true == mThrustLeft.IsDown()) { mAngularVelocity += kAngularAcceleration * FixedTime(); }

	if (mLinearVelocity.Magnitude() > kMaximumLinearSpeed)
	{
		mLinearVelocity.SetLength(kMaximumLinearSpeed);
	}

	tb_always_log("RocketShip Speed: " << mLinearVelocity.Magnitude());

	if (std::abs(mAngularVelocity.AsDegrees()) > kMaximumAngularSpeed.AsDegrees())
	{
		mAngularVelocity = kMaximumAngularSpeed * tbMath::Sign(mAngularVelocity.AsDegrees());
	}

	SetPosition(GetPosition() + mLinearVelocity * FixedTime());
	SetRotation(GetRotation() + mAngularVelocity * FixedTime());

	// This is duplicated in both RocketShipEntity and AsteroidEntity
	const Vector2 worldSize(WorldTargetWidth(), WorldTargetHeight());
	const float radius = mShape.GetRadius();
	Vector2 position = GetPosition();
	if (position.x > worldSize.x + radius) { position.x -= worldSize.x; }
	if (position.x < -radius) { position.x += worldSize.x; }
	if (position.y > worldSize.y + radius) { position.y -= worldSize.y; }
	if (position.y < -radius) { position.y += worldSize.y; }
	SetPosition(position);
	// End duplication.
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::RocketShipEntity::OnUpdate(const float deltaTime)
{
	tbGame::Entity::OnUpdate(deltaTime);
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::RocketShipEntity::OnRender(void) const
{
	tbGame::Entity::OnRender();

	//const float length = 100.0f;
	//const Vector2 direction = RotationToForwardVector2(GetRotation());
	//tbGraphics::Line(GetPosition(), GetPosition() + direction * length).Render();
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::RocketShipEntity::OnCollide(const tbGame::Entity& otherEntity)
{
	tbGame::Entity::OnCollide(otherEntity);
}

//--------------------------------------------------------------------------------------------------------------------//
