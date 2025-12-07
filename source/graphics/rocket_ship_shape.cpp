///
/// @file
/// @details Displays a rocket ship, of sorts.
///
/// <!-- Copyright (c) 2025 Tyre Bytes LLC - All Rights Reserved -->
///------------------------------------------------------------------------------------------------------------------///

#include "../graphics/rocket_ship_shape.hpp"
#include "../interface.hpp"

#include <turtle_brains/graphics/tb_basic_shapes.hpp>

//--------------------------------------------------------------------------------------------------------------------//

Asteroids::RocketShipShape::RocketShipShape(const tbGraphics::Color& color, const Vector2& position) :
	AsteroidShape(3, 48.0f, color, position)
{
}

//--------------------------------------------------------------------------------------------------------------------//

Asteroids::RocketShipShape::~RocketShipShape(void)
{
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::RocketShipShape::OnUpdate(const float deltaTime)
{
	PolygonShape::OnUpdate(deltaTime);
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::RocketShipShape::OnRender(void) const
{
	tbGraphics::BoxShape box(10.0f, 10.0f, ColorPalette::Gray);
	box.SetOrigin(Anchor::TopCenter);
	box.SetPosition(-20.0f, 20.0f);
	box.Render();

	box.SetPosition(20.0f, 20.0f);
	box.Render();

	PolygonShape::OnRender();
}

//--------------------------------------------------------------------------------------------------------------------//

