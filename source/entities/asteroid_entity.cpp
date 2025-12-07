///
/// @file
/// @details An entity within the Asteroids project.
///
/// <!-- Copyright (c) 2025 Tyre Bytes LLC - All Rights Reserved -->
///------------------------------------------------------------------------------------------------------------------///

#include "asteroid_entity.hpp"

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
		return 15.0f + asteroidSize * 5.0f;
	}

	Vector2 RandomLinearVelocity(void)
	{
		const float speed = 500.0f;// tbMath::RandomFloat(10.0f, 100.0f);
		return Vector2(tbMath::RandomFloat(-1.0f, 1.0f), tbMath::RandomFloat(-1.0f, 1.0f)).GetNormalized() * speed;
	}

	Angle RandomAngularVelocity(void)
	{
		const Angle kMaximumAngularSpeed = 45.0_degrees; //per second
		return kMaximumAngularSpeed * tbMath::RandomFloat(-1.0f, 1.0f);
	}

};

//--------------------------------------------------------------------------------------------------------------------//

Asteroids::AsteroidEntity::AsteroidEntity(const int size) :
	tbGame::Entity("AsteroidEntity"),
	mLinearVelocity(Implementation::RandomLinearVelocity()),
	mAngularVelocity(Implementation::RandomAngularVelocity()),
	mShape(Implementation::CalculateSides(size), Implementation::CalculateRadius(size))
{
	mShape.SetOrigin(Anchor::Center);
	AddGraphic(mShape);
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

	const Vector2 worldSize(WorldTargetWidth(), WorldTargetHeight());
	//const Vector2 worldSize(static_cast<float>(ScreenSpaceToWorldSpace(tbGraphics::ScreenWidth())),
	//	static_cast<float>(ScreenSpaceToWorldSpace(tbGraphics::ScreenHeight())));

	const float radius = mShape.GetRadius();
	Vector2 position = GetPosition();
	if (position.x > worldSize.x + radius) { position.x -= worldSize.x; }
	if (position.x < -radius) { position.x += worldSize.x; }
	if (position.y > worldSize.y + radius) { position.y -= worldSize.y; }
	if (position.y < -radius) { position.y += worldSize.y; }
	SetPosition(position);

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
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::AsteroidEntity::OnCollide(const tbGame::Entity& otherEntity)
{
	tbGame::Entity::OnCollide(otherEntity);
}

//--------------------------------------------------------------------------------------------------------------------//
