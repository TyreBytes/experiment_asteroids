///
/// @file
/// @details An entity within the Asteroids project.
///
/// <!-- Copyright (c) 2025 Tyre Bytes LLC - All Rights Reserved -->
///------------------------------------------------------------------------------------------------------------------///

#include "../entities/asteroid_entity.hpp"
#include "../entities/bullet_entity.hpp"
#include "../development/development.hpp"

//--------------------------------------------------------------------------------------------------------------------//

namespace Asteroids::Implementation
{
	int CalculateSides(const int asteroidSize)
	{
		// 5, 6, 7, 8,
		return 5 + ((asteroidSize) % 4);
	}

	float CalculateRadius(const int asteroidSize)
	{
		return 25.0f + asteroidSize * 5.0f;
	}

	Vector2 RandomLinearVelocity(void)
	{
		const float speed = tbMath::RandomFloat(50.0f, 100.0f);
		return Vector2(tbMath::RandomFloat(-1.0f, 1.0f), tbMath::RandomFloat(-1.0f, 1.0f)).GetNormalized() * speed;
	}

	Angle RandomAngularVelocity(void)
	{
		const Angle kMaximumAngularSpeed = 45.0_degrees; //per second
		return kMaximumAngularSpeed * tbMath::RandomFloat(-1.0f, 1.0f);
	}
};

//--------------------------------------------------------------------------------------------------------------------//

Asteroids::AsteroidEntity::AsteroidEntity(const int size, const Vector2& position, const Vector2& velocity) :
	tbGame::Entity("AsteroidEntity"),
	mShape(Implementation::CalculateSides(size), Implementation::CalculateRadius(size)),
	mLinearVelocity(velocity),
	mAngularVelocity(Implementation::RandomAngularVelocity()),
	mSize(size),
	mHitPoints(10)
{
	SetPosition(position);

	//mShape.SetOrigin(Anchor::Center);
	AddGraphic(mShape);

	AddBoundingCircle(mShape.GetRadius() * 0.95f);
}

//--------------------------------------------------------------------------------------------------------------------//

Asteroids::AsteroidEntity::~AsteroidEntity(void)
{
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::AsteroidEntity::OnAdd(void)
{
	tbGame::Entity::OnAdd();
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::AsteroidEntity::OnRemove(void)
{
	tbGame::Entity::OnRemove();
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::AsteroidEntity::OnSimulate(void)
{
	tbGame::Entity::OnSimulate();

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

	tb_error_if(nullptr == GetEntityManager(), "Shouldn't be in OnSimulate() without an EntityManager.");

	// 2025-12-07: Turns out collision with other asteroids is going to present some interesting issues, at least if
	//   we do it here. We might be able to do this same stuff in the Gameplay scene a little better since ALL asteroids
	//   would have moved for the time-step already.
	//for (tbGame::Entity* entity: GetEntityManager()->GetEntitiesByType("AsteroidEntity"))
	//{
	//	AsteroidEntity* asteroidPointer = dynamic_cast<AsteroidEntity*>(entity);
	//	tb_error_if(nullptr == asteroidPointer, "Expected entity type AsteroidEntity to be an asteroid...");
	//	const AsteroidEntity& asteroid = *asteroidPointer;

	//	const float distanceSquared = GetPosition().SquaredDistanceTo(asteroid.GetPosition());
	//	if (distanceSquared < (GetRadius() * GetRadius() + asteroid.GetRadius() * asteroid.GetRadius()))
	//	{
	//		//Bonk.
	//	}
	//}
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::AsteroidEntity::OnUpdate(const float deltaTime)
{
	tbGame::Entity::OnUpdate(deltaTime);
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::AsteroidEntity::OnRender(void) const
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

void Asteroids::AsteroidEntity::OnCollide(const tbGame::Entity& otherEntity)
{
	tbGame::Entity::OnCollide(otherEntity);

	if (true == otherEntity.IsEntityOfType("BulletEntity"))
	{
		const BulletEntity& bullet = *dynamic_cast<const BulletEntity*>(&otherEntity);

		mHitPoints -= 1;
		if (mHitPoints <= 0)
		{
			BreakApart(bullet.GetLinearVelocity().GetNormalized());
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::AsteroidEntity::BreakApart(const Vector2& impactDirection)
{
	tbGame::EntityManager* entityManager = GetEntityManager();

	if (mSize >= 1)
	{
		const float speed = mLinearVelocity.Magnitude() * 1.1f;
		const Vector2 direction1 = Asteroids::RotationToForwardVector2(Asteroids::ForwardVector2ToRotation(impactDirection) - 45.0_degrees);
		const Vector2 direction2 = Asteroids::RotationToForwardVector2(Asteroids::ForwardVector2ToRotation(impactDirection) + 45.0_degrees);

		const float radius = Implementation::CalculateRadius(mSize - 1) * 0.7f;
		entityManager->AddEntity(new AsteroidEntity(mSize - 1, GetPosition() + direction1 * radius, direction1 * speed));
		entityManager->AddEntity(new AsteroidEntity(mSize - 1, GetPosition() + direction2 * radius, direction2 * speed));
	}

	entityManager->RemoveEntity(this);
}

//--------------------------------------------------------------------------------------------------------------------//
