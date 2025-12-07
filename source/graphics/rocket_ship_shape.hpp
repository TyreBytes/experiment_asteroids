///
/// @file
/// @details Displays a rocket ship, of sorts.
///
/// <!-- Copyright (c) 2025 Tyre Bytes LLC - All Rights Reserved -->
///------------------------------------------------------------------------------------------------------------------///

#ifndef Asteroids_RocketShipShape_hpp
#define Asteroids_RocketShipShape_hpp

#include "../asteroids.hpp"

#include "../graphics/asteroid_shape.hpp"

#include <turtle_brains/graphics/tb_graphic.hpp>
#include <turtle_brains/graphics/tb_basic_shapes.hpp>
#include <turtle_brains/graphics/tb_sprite.hpp>

namespace Asteroids
{

	class RocketShipShape : public AsteroidShape
	{
	public:
		explicit RocketShipShape(const tbGraphics::Color& color = tbGraphics::ColorPalette::White, const Vector2& position = Vector2::Zero());

		virtual ~RocketShipShape(void);

	protected:
		virtual void OnUpdate(const float deltaTime) override;
		virtual void OnRender(void) const override;

	private:
	};

};	//namespace Asteroids

#endif /* Asteroids_RocketShipShape_hpp */
