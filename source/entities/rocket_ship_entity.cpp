///
/// @file
/// @details An entity within the Asteroids project.
///
/// <!-- Copyright (c) 2025 Tyre Bytes LLC - All Rights Reserved -->
///------------------------------------------------------------------------------------------------------------------///

#include "../entities/rocket_ship_entity.hpp"
#include "../entities/bullet_entity.hpp"

#include "../development/development.hpp"

//--------------------------------------------------------------------------------------------------------------------//

Asteroids::RocketShipEntity::RocketShipEntity(const Vector2& position) :
	tbGame::Entity("RocketShipEntity"),
	mThrustForward(Key::tbKeyUp),
	mThrustBackward(Key::tbKeyDown),
	mThrustRight(Key::tbKeyRight),
	mThrustLeft(Key::tbKeyLeft),
	mShootWeapon(Key::tbMouseLeft),
	mWeaponReloadTimer(RustyTimer::Zero()),
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

	AddBoundingCircle(mShape.GetRadius() * 0.7f);
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
	mLinearVelocity -= mLinearVelocity * 0.75f * FixedTime();

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

	if (std::abs(mAngularVelocity.AsDegrees()) > kMaximumAngularSpeed.AsDegrees())
	{
		mAngularVelocity = kMaximumAngularSpeed * tbMath::Sign(mAngularVelocity.AsDegrees());
	}

	SetPosition(GetPosition() + mLinearVelocity * FixedTime());
	SetRotation(GetRotation() + mAngularVelocity * FixedTime());

	// This is duplicated in both RocketShipEntity, AsteroidEntity and kinda BulletEntity
	const Vector2 worldSize(WorldTargetWidth(), WorldTargetHeight());
	const float radius = mShape.GetRadius();
	Vector2 position = GetPosition();
	if (position.x > worldSize.x + radius) { position.x -= worldSize.x; }
	if (position.x < -radius) { position.x += worldSize.x; }
	if (position.y > worldSize.y + radius) { position.y -= worldSize.y; }
	if (position.y < -radius) { position.y += worldSize.y; }
	SetPosition(position);
	// End duplication.


	if (true == mShootWeapon.IsDown())
	{
		mWeaponReloadTimer.DecrementStep();
	}

	if (true == mWeaponReloadTimer.IsZero() || true == mShootWeapon.IsPressed())
	{
		const Vector2 mouseInWorldSpace = ScreenSpaceToWorldSpace(tbGame::Input::GetMousePosition());
		const Vector2 shootDirection = GetPosition().DirectionTo(mouseInWorldSpace);

		const Vector2 weaponPosition = GetPosition();
		GetEntityManager()->AddEntity(new BulletEntity(weaponPosition, shootDirection * 800.0f));

		mWeaponReloadTimer += 0.5f;
	}
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

#if defined(rusty_development)
	if (true == Development::IsDebugging())
	{
		OnDebugRender();
	}
#endif
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::RocketShipEntity::OnCollide(const tbGame::Entity& otherEntity)
{
	tbGame::Entity::OnCollide(otherEntity);

	if (true == otherEntity.IsEntityOfType("AsteroidEntity"))
	{
		tb_always_log("Player died...");
	}
}

//--------------------------------------------------------------------------------------------------------------------//
