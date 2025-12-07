///
/// @file
/// @details Displays a shifter at mouse position for shitfting feedback.
///
/// <!-- Copyright (c) 2025 Tyre Bytes LLC - All Rights Reserved -->
///------------------------------------------------------------------------------------------------------------------///

#include "../graphics/event_box.hpp"
#include "../interface.hpp"

//--------------------------------------------------------------------------------------------------------------------//

Asteroids::EventBox::EventBox(void) :
	tbGraphics::GraphicList(),
	mDarkenBox(1, 1, Color(0x80000000)), //setup in OnUpdate() due to potential ScreenSize changes.
	mBorderBox(Vector2(kEventNoticeSize.x + kBorder * 2, kEventNoticeSize.y + kBorder * 2), Vector2::Zero(), AsteroidsColor::TernaryBackgroundColor, kRoundedCornerRadius, kBorder, AsteroidsColor::TernaryColor),
	mDarkenBackdrop(false)
{
	AddGraphic(mBorderBox);

	// 2025-10-29: Calling Update() because it will setup the positioning of the boxes for the EventBox. This is only
	//   helpful for the times we took shortcuts and created an EventBox directly in Render(), and really we shouldn't
	//   but... reality is messy.
	Update(0.0f);
}

//--------------------------------------------------------------------------------------------------------------------//

Asteroids::EventBox::~EventBox(void)
{
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::EventBox::SetColor(const Color& innerColor, const Color& borderColor)
{
	tb_unused(innerColor);
	tb_unused(borderColor);
	// mBorderBox.SetColor(borderColor);
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::EventBox::OnUpdate(const float deltaTime)
{
	GraphicList::OnUpdate(deltaTime);

	// 2025-11-05: The EventBox position SHOULD be the center of the interface area, which is commented out below - as
	//   you can read. The positions of the child boxes to make up the EventBox visual should then be position at
	//   Vector2::Zero with their origin's centered. I (Tim) can't think of why this would cause issues with placement,
	//   or with using GetAnchorPosition() but it does. It places the EventBox at the top-left of the screen/interface
	//   instead of the center.
	//
	//   There is a known issue with GetAnchorPosition() in which any parent positioning is ignored due to TurtleBrains
	//   Graphic not holding on to "parents" and some other information that would be necessary. That should be OKAY in
	//   this situation since the EventBox is the object we are setting up and calling GetAnchorPosition on... I don't
	//   wish to waste time debugging this, but there is an issue or some mismatch of expectations.
	//
	//   To work around this issue I just added an overloaded GetAnchorPosition() that uses the InnerBox graphic, which
	//   again shouldn't be necessary, but here we are.

	//SetOrigin(Anchor::Center);
	//SetPosition(Interface::GetAnchorPositionOfInterface(Anchor::Center));

	mDarkenBox = tbGraphics::BoxShape(Interface::TargetWidth(), Interface::TargetHeight(), Color(0xAA000000));
	mDarkenBox.SetVisible(mDarkenBackdrop);
	mDarkenBox.SetOrigin(Anchor::Center);
	mDarkenBox.SetPosition(Interface::GetAnchorPositionOfInterface(Anchor::Center));
//	mDarkenBox.SetPosition(Vector2::Zero());

	mBorderBox.SetOrigin(Anchor::Center);
	mBorderBox.SetPosition(Interface::GetAnchorPositionOfInterface(Anchor::Center));
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::EventBox::OnRender(void) const
{
	if (true == mDarkenBackdrop)
	{
		mDarkenBox.Render();
	}

	GraphicList::OnRender();
}

//--------------------------------------------------------------------------------------------------------------------//
