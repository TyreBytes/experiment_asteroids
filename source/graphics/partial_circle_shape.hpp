///
/// @file
/// @details Displays a half circle.
///
/// <!-- Copyright (c) 2025 Tyre Bytes LLC - All Rights Reserved -->
///------------------------------------------------------------------------------------------------------------------///

#ifndef Asteroids_ParialCircleShape_hpp
#define Asteroids_ParialCircleShape_hpp

#include <turtle_brains/graphics/tb_graphic.hpp>
#include <turtle_brains/graphics/tb_basic_shapes.hpp>
#include <turtle_brains/graphics/tb_sprite.hpp>

namespace Asteroids
{

	class PartialCircleShape : public tbGraphics::PolygonShape
	{
	public:
		PartialCircleShape(const float radius = 64.0f, const tbGraphics::Color& color = tbGraphics::ColorPalette::White,
			const tbMath::Vector2& position = tbMath::Vector2::Zero(), const int sectionCount = 30,
			const float startAngleRadians = 0.0f, const float endAngleRadians = tbMath::kTwoPi,
			const float thickness = 1.0f);
		virtual ~PartialCircleShape(void);

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
