///
/// @file
/// @details Displays a half circle.
///
/// <!-- Copyright (c) 2025 Tyre Bytes LLC - All Rights Reserved -->
///------------------------------------------------------------------------------------------------------------------///

#include "../graphics/partial_circle_shape.hpp"
#include "../interface.hpp"

#include <turtle_brains/graphics/tb_basic_shapes.hpp>

//--------------------------------------------------------------------------------------------------------------------//

Asteroids::PartialCircleShape::PartialCircleShape(const float radius, const tbGraphics::Color& color, const tbMath::Vector2& position,
	const int sectionCount, const float startAngleRadians, const float endAngleRadians, const float thickness) :
	mRadius(radius)
{
	SetPosition(position);
	SetColor(color);

	SetAsTriangles();

	const tbMath::Vector2 circleCenter(mRadius, mRadius);

	const float outer = mRadius;
	const float inner = mRadius - thickness;

	for (int section = 0; section < sectionCount; ++section)
	{
		const float t0 = static_cast<float>(section) / static_cast<float>(sectionCount);
		const float t1 = static_cast<float>(section + 1) / static_cast<float>(sectionCount);

		const float angle0 = startAngleRadians + t0 * (endAngleRadians - startAngleRadians);
		const float angle1 = startAngleRadians + t1 * (endAngleRadians - startAngleRadians);

		const tbMath::Vector2 dir0(cos(angle0), sin(angle0));
		const tbMath::Vector2 dir1(cos(angle1), sin(angle1));

		const tbMath::Vector2 outer0 = circleCenter + dir0 * outer;
		const tbMath::Vector2 outer1 = circleCenter + dir1 * outer;

		const tbMath::Vector2 inner0 = circleCenter + dir0 * inner;
		const tbMath::Vector2 inner1 = circleCenter + dir1 * inner;

		// Triangle 1 (CCW)
		AddVertex(outer0, color);
		AddVertex(outer1, color);
		AddVertex(inner0, color);

		// Triangle 2 (CCW)
		AddVertex(outer1, color);
		AddVertex(inner1, color);
		AddVertex(inner0, color);

	}
}



//--------------------------------------------------------------------------------------------------------------------//

Asteroids::PartialCircleShape::~PartialCircleShape(void)
{
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::PartialCircleShape::OnUpdate(const float deltaTime)
{
	PolygonShape::OnUpdate(deltaTime);
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::PartialCircleShape::OnRender(void) const
{
	PolygonShape::OnRender();
}

//--------------------------------------------------------------------------------------------------------------------//

