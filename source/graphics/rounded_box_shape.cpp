///
/// @file
/// @details Draws a rounded box shape.
///
/// <!-- Copyright (c) 2025 Tyre Bytes LLC - All Rights Reserved -->
///------------------------------------------------------------------------------------------------------------------///

#include "../graphics/rounded_box_shape.hpp"

//--------------------------------------------------------------------------------------------------------------------//

Asteroids::RoundedBoxShape::RoundedBoxShape(const tbMath::Vector2& size, const tbMath::Vector2& position,
		const Color& fillColor, const float& cornerRadius, const float& border, const Color& borderColor,
		const bool& topCorners, const bool& bottomCorners) :
	mBorderColor(borderColor),
	mSize(size),
	mBorder(border, border),
	mCornerRadius(cornerRadius),
	mTopCorners(topCorners),
	mBottomCorners(bottomCorners)
{
	mCornerRadius = 10.0f;

	SetPosition(position);
	SetColor(fillColor);
	tb_unused(border);
}

//--------------------------------------------------------------------------------------------------------------------//

Asteroids::RoundedBoxShape::~RoundedBoxShape(void)
{
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::RoundedBoxShape::OnUpdate(const float deltaTime)
{
	Graphic::OnUpdate(deltaTime);
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::RoundedBoxShape::OnRender(void) const
{
	Graphic::OnRender();

	DrawRoundedBox(Vector2::Zero() - mBorder, mSize + mBorder * 2.0f, mBorderColor);
	DrawRoundedBox(Vector2::Zero(), mSize, GetColor());
}

void Asteroids::RoundedBoxShape::DrawRoundedBox(const Vector2& offset, const Vector2& size, const Color& color) const
{
	float width = size.x;
	float height = size.y;
	float radius = tbMath::Clamp(mCornerRadius, 0.0f, std::min(width, height) * 0.5f);
	tbMath::Vector2 position = offset;

	float innerWidth = size.x - mBorder.x;
	float innerHeight = size.y - mBorder.y;

	tbMath::Vector2 innerPosition = position + size / 2.0f;

	const Color& fillColor = color;

	tbGraphics::CircleShape circle(radius, fillColor);

	// Rounded corners for fill
	if (true == mTopCorners)
	{
		circle.SetPosition(innerPosition + tbMath::Vector2(+(innerWidth / 2 - radius), -(innerHeight / 2 - radius)));
		circle.SetOrigin(Anchor::Center);
		circle.Render();

		// 2025-12-02: The SetRotation() is actually being useful here, even if we set it back immediately after. For
		//   some reason the TurtleBrains::CircleShape is slightly 'flattened' on the top-left. I'm not sure why, likely
		//   something to do with integral pixel positions vs floating point direction/distances, but this also fixes
		//   the issue, was easy and ... smoke and mirrors, lets keep moving.
		circle.SetPosition(innerPosition + tbMath::Vector2(-(innerWidth / 2 - radius), -(innerHeight / 2 - radius)));
		circle.SetOrigin(Anchor::Center);
		circle.SetRotation(180.0_degrees);
		circle.Render();
		circle.SetRotation(0.0_degrees);
	}

	if (true == mBottomCorners)
	{
		circle.SetPosition(innerPosition + tbMath::Vector2(-(innerWidth / 2 - radius), +(innerHeight / 2 - radius)));
		circle.SetOrigin(Anchor::Center);
		circle.Render();

		circle.SetPosition(innerPosition + tbMath::Vector2(+(innerWidth / 2 - radius), +(innerHeight / 2 - radius)));
		circle.SetOrigin(Anchor::Center);
		circle.Render();
	}

	float topWidth = innerWidth;
	if (true == mTopCorners)
	{
		topWidth = innerWidth - 2 * radius;
	}
	tbGraphics::BoxShape fillBoxWidthTop(topWidth, innerHeight / 2.0f, fillColor, innerPosition);
	fillBoxWidthTop.SetOrigin(Anchor::BottomCenter);
	fillBoxWidthTop.Render();

	float bottomWidth = innerWidth;
	if (true == mBottomCorners)
	{
		bottomWidth = innerWidth - 2 * radius;
	}
	tbGraphics::BoxShape fillBoxWidthBottom(bottomWidth, innerHeight / 2.0f, fillColor, innerPosition);
	fillBoxWidthBottom.SetOrigin(Anchor::TopCenter);
	fillBoxWidthBottom.Render();

	tbGraphics::BoxShape fillBoxHeight(innerWidth, innerHeight - 2 * radius, fillColor, innerPosition);
	fillBoxHeight.SetOrigin(Anchor::Center);
	fillBoxHeight.Render();
}

//--------------------------------------------------------------------------------------------------------------------//
