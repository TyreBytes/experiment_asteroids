///
/// @file
/// @details Draws a rounded box shape.
///
/// <!-- Copyright (c) 2025 Tyre Bytes LLC - All Rights Reserved -->
///------------------------------------------------------------------------------------------------------------------///

#ifndef Asteroids_RoundedBoxShape_hpp
#define Asteroids_RoundedBoxShape_hpp

#include "../interface.hpp"

#include <turtle_brains/graphics/tb_graphic.hpp>
#include <turtle_brains/graphics/tb_sprite.hpp>

namespace Asteroids
{

	class RoundedBoxShape : public tbGraphics::Graphic
	{
	public:
		explicit RoundedBoxShape(const tbMath::Vector2& size, const tbMath::Vector2& position,
			const Color& fillColor, const float& cornerRadius, const float& border = 0.0f,
			const Color& borderColor = ColorPalette::White,
			const bool& topCorners = true, const bool& bottomCorners = true);
		virtual ~RoundedBoxShape(void);

		inline virtual tbGraphics::PixelSpace GetPixelWidth(void) const override {
			return static_cast<tbGraphics::PixelSpace>(mSize.x);
		};
		inline virtual tbGraphics::PixelSpace GetPixelHeight(void) const override {
			return static_cast<tbGraphics::PixelSpace>(mSize.y);
		};

	protected:
		virtual void OnUpdate(const float deltaTime) override;
		virtual void OnRender(void) const override;

		void DrawRoundedBox(const Vector2& offset, const Vector2& size, const Color& color) const;

	private:
		Color mBorderColor;
		Vector2 mSize;
		Vector2 mBorder;
		float mCornerRadius;
		bool mTopCorners;
		bool mBottomCorners;
	};

};	//namespace Asteroids

#endif /* Asteroids_RoundedBoxShape_hpp */
