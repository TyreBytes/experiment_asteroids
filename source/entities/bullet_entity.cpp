///
/// @file
/// @details An entity within the Asteroids project that is a projectile flying through space to destroy an asteroid!
///
/// <!-- Copyright (c) 2025 Tyre Bytes LLC - All Rights Reserved -->
///------------------------------------------------------------------------------------------------------------------///

#include "../entities/bullet_entity.hpp"
#include "../development/development.hpp"

//--------------------------------------------------------------------------------------------------------------------//

Asteroids::BulletEntity::BulletEntity(const Vector2& position, const Vector2& velocity) :
	tbGame::Entity("BulletEntity"),
	mSprite("data/laser_sprites/01.png"),
	mLinearVelocity(velocity),
	mRadius(11.0f)
{
	mSprite.SetOrigin(Anchor::Center);
	mSprite.SetRotation(90.0_degrees);
	AddGraphic(mSprite);

	SetPosition(position);
	SetRotation(Asteroids::ForwardVector2ToRotation(velocity.GetNormalized()));

	AddBoundingCircle(mRadius, Vector2(0.0f, -6.0f));
}

//--------------------------------------------------------------------------------------------------------------------//

Asteroids::BulletEntity::~BulletEntity(void)
{
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::BulletEntity::OnAdd(void)
{
	tbGame::Entity::OnAdd();
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::BulletEntity::OnRemove(void)
{
	tbGame::Entity::OnRemove();
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::BulletEntity::OnSimulate(void)
{
	tbGame::Entity::OnSimulate();

	SetPosition(GetPosition() + mLinearVelocity * FixedTime());

	// This is duplicated in both RocketShipEntity, AsteroidEntity and kinda BulletEntity
	const Vector2 worldSize(WorldTargetWidth(), WorldTargetHeight());
	const float radius = mRadius;
	const Vector2 position = GetPosition();
	if (position.x > worldSize.x + radius || position.x < -radius ||
		position.y > worldSize.y + radius || position.y < -radius)
	{
		GetEntityManager()->RemoveEntity(this);
	}

	SetPosition(position);
	// End duplication.

}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::BulletEntity::OnUpdate(const float deltaTime)
{
	tbGame::Entity::OnUpdate(deltaTime);
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::BulletEntity::OnRender(void) const
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

void Asteroids::BulletEntity::OnCollide(const tbGame::Entity& otherEntity)
{
	tbGame::Entity::OnCollide(otherEntity);


	if (true == otherEntity.IsEntityOfType("AsteroidEntity"))
	{
		GetEntityManager()->RemoveEntity(this);
	}
}

//--------------------------------------------------------------------------------------------------------------------//
