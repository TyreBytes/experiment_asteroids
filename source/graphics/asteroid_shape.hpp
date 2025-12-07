///
/// @file
/// @details Displays a half circle.
///
/// <!-- Copyright (c) 2025 Tyre Bytes LLC - All Rights Reserved -->
///------------------------------------------------------------------------------------------------------------------///

#ifndef Asteroids_ParialCircleShape_hpp
#define Asteroids_ParialCircleShape_hpp

#include "../asteroids.hpp"

#include <turtle_brains/graphics/tb_graphic.hpp>
#include <turtle_brains/graphics/tb_basic_shapes.hpp>
#include <turtle_brains/graphics/tb_sprite.hpp>

namespace Asteroids
{

	class AsteroidShape : public tbGraphics::PolygonShape
	{
	public:
		explicit AsteroidShape(const int corners = 5, const float radius = 64.0f,
			const Color& color = tbGraphics::ColorPalette::White, const Vector2& position = Vector2::Zero());

		virtual ~AsteroidShape(void);

		inline float GetRadius(void) const { return mRadius; }
		inline virtual tbGraphics::PixelSpace GetPixelWidth(void) const override { return static_cast<tbGraphics::PixelSpace>(mRadius * 2); }
		inline virtual tbGraphics::PixelSpace GetPixelHeight(void) const override { return static_cast<tbGraphics::PixelSpace>(mRadius * 2); }

	protected:
		virtual void OnUpdate(const float deltaTime) override;
		virtual void OnRender(void) const override;

	private:
		float mRadius;
	};

};	//namespace Asteroids

#endif /* Asteroids_ParialCircleShape_hpp */
