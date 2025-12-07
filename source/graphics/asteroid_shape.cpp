///
/// @file
/// @details Displays a half circle.
///
/// <!-- Copyright (c) 2025 Tyre Bytes LLC - All Rights Reserved -->
///------------------------------------------------------------------------------------------------------------------///

#include "../graphics/asteroid_shape.hpp"
#include "../interface.hpp"

#include <turtle_brains/graphics/tb_basic_shapes.hpp>

//--------------------------------------------------------------------------------------------------------------------//

Asteroids::AsteroidShape::AsteroidShape(const int corners, const float radius, const Color& color, const Vector2& position) :
	mRadius(radius)
{
	SetPosition(position);
	SetColor(color);

	//const tbMath::Vector2 circleCenter(radius, radius);
	const tbMath::Vector2 circleCenter = Vector2::Zero();

	SetAsTriangleFan();
	AddVertex(circleCenter, color, tbMath::Vector2(0.5f, 0.5f));

	for (int section = 0; section <= corners; ++section)
	{
		const float percentage(static_cast<float>(section) / static_cast<float>(corners));
		//tbMath::Vector2 direction(cos(percentage * tbMath::kTwoPi), sin(percentage * tbMath::kTwoPi));


		tbMath::Vector2 direction(sin(percentage * tbMath::kTwoPi), -cos(percentage * tbMath::kTwoPi));

		AddVertex(circleCenter + (direction * radius), color, (direction + tbMath::Vector2(1.0f, 1.0f)) / 2.0f);
	}

	RecomputeBounds();
	//SetOrigin(Anchor::Center);
}

//--------------------------------------------------------------------------------------------------------------------//

Asteroids::AsteroidShape::~AsteroidShape(void)
{
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::AsteroidShape::OnUpdate(const float deltaTime)
{
	PolygonShape::OnUpdate(deltaTime);
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::AsteroidShape::OnRender(void) const
{
	PolygonShape::OnRender();
}

//--------------------------------------------------------------------------------------------------------------------//

