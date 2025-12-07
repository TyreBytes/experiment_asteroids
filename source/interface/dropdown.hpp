///
/// @file
/// @details Provide a dropdown selection box for the Asteroids settings screen.
///
/// <!-- Copyright (c) 2019-2025 Tyre Bytes LLC - All Rights Reserved -->
///------------------------------------------------------------------------------------------------------------------///

#ifndef Asteroids_Dropdown_hpp
#define Asteroids_Dropdown_hpp

#include "../asteroids.hpp"

#include <turtle_brains/graphics/tb_sprite.hpp>
#include <turtle_brains/express/graphics/tbx_shadowed_text.hpp>
#include <turtle_brains/express/interface/tbx_interface_dropdown.hpp>
#include <turtle_brains/graphics/tb_basic_shapes.hpp>

namespace Asteroids::Interface
{

	class SimpleDropdown : public tbxInterface::Unstable::Dropdown
	{
	public:
		SimpleDropdown(const ItemContainer& itemNames, const size_t selectedIndex,
			const tbGraphics::PixelSpace& width, const tbGraphics::PixelSpace& height) :

			Dropdown(itemNames, selectedIndex),
			mSelectedText(itemNames[selectedIndex]),
			mDropdownBox(static_cast<float>(width), static_cast<float>(height* itemNames.size()), ColorPalette::DarkGray,
				tbMath::Vector2(0.0f, 0.0f)),
			mFocusedItemBox(static_cast<float>(width), static_cast<float>(height), ColorPalette::LightGray,
				tbMath::Vector2(0.0f, static_cast<float>(height* selectedIndex)))
		{
			const float floatWidth = static_cast<float>(width);
			const float floatHeight = static_cast<float>(height);

			//Left aligned text, magic number 10 is some padding from left edge of element.
			mSelectedText.SetOrigin(Anchor::CenterLeft);
			mSelectedText.SetPosition(10.0f, floatHeight * 0.5f);

			AddGraphic(State::Hidden, new tbGraphics::BoxShape(floatWidth, floatHeight, ColorPalette::White));

			AddGraphic(State::Disabled, new tbGraphics::BoxShape(floatWidth, floatHeight, ColorPalette::Gray));
			AddGraphic(State::Disabled, new TriangleShape(48.0f, ColorPalette::LightGray, tbMath::Vector2(floatWidth - 48.0f - 8.0f, 8.0f)));
			AddGraphic(State::Disabled, mSelectedText);

			AddGraphic(State::Enabled, new tbGraphics::BoxShape(floatWidth, floatHeight, ColorPalette::Blue));
			AddGraphic(State::Enabled, new TriangleShape(48.0f, ColorPalette::White, tbMath::Vector2(floatWidth - 48.0f - 8.0f, 8.0f)));
			AddGraphic(State::Enabled, mSelectedText);

			AddGraphic(State::Focused, new tbGraphics::BoxShape(floatWidth, floatHeight, ColorPalette::Orange));
			AddGraphic(State::Focused, new TriangleShape(48.0f, ColorPalette::White, tbMath::Vector2(floatWidth - 48.0f - 8.0f, 8.0f)));
			AddGraphic(State::Focused, mSelectedText);

			SetPixelSize(width, height);

			RecreateActiveGraphics(selectedIndex);
		}

		virtual ~SimpleDropdown(void)
		{
		}

	protected:
		virtual void OnSelectionChange(const size_t selectedIndex) override
		{
			RecreateActiveGraphics(selectedIndex);
		}

	private:
		inline void RecreateActiveGraphics(const size_t selectedIndex)
		{
			ClearGraphics(State::Activated);

			AddGraphic(State::Activated, new tbGraphics::BoxShape(static_cast<float>(GetPixelWidth()),
				static_cast<float>(GetPixelHeight()), tbGraphics::ColorPalette::Green));

			mDropdownBox.SetDepth(1.0f);
			AddGraphic(State::Activated, mDropdownBox);

			mFocusedItemBox.SetPosition(0.0f, static_cast<float>(GetPixelHeight() * GetFocusedIndex()));
			mFocusedItemBox.SetDepth(1.0f);
			AddGraphic(State::Activated, mFocusedItemBox);

			size_t currentIndex = 0;
			for (const tbCore::String& item : GetItemNames())
			{
				tbGraphics::Text* itemLabel = new tbGraphics::Text(item);
				itemLabel->SetOrigin(Anchor::CenterLeft);
				itemLabel->SetPosition(10.0f, GetPixelHeight() * (static_cast<float>(currentIndex) + 0.5f));
				if (currentIndex == GetSelectedIndex())
				{
					itemLabel->SetColor(ColorPalette::Blue);
				}

				itemLabel->SetDepth(1.0f);
				AddGraphic(State::Activated, itemLabel);
				++currentIndex;
			}

			mSelectedText.SetText(GetItemNames().at(selectedIndex)),
				mSelectedText.SetOrigin(Anchor::CenterLeft);
			mSelectedText.SetPosition(10.0f, GetPixelHeight() * 0.5f);
		}

		class TriangleShape : public tbGraphics::PolygonShape
		{
		public:
			TriangleShape(float size = 48.0f, const tbGraphics::Color& color = ColorPalette::White,
				const tbMath::Vector2& position = tbMath::Vector2::Zero()) :
				PolygonShape()
			{
				SetColor(color);
				AddVertex(tbMath::Vector2(0.0f, 0.0f));
				AddVertex(tbMath::Vector2(size, 0.0f));
				AddVertex(tbMath::Vector2(size * 0.5f, size));
				SetAsTriangles();
				SetPosition(position);
			}
		};

		tbGraphics::Text mSelectedText; //When closed

		tbGraphics::BoxShape mDropdownBox;
		tbGraphics::BoxShape mFocusedItemBox;
	};

//--------------------------------------------------------------------------------------------------------------------//
//--------------------------------------------------------------------------------------------------------------------//
//--------------------------------------------------------------------------------------------------------------------//


	//class Dropdown : public tbxInterface::Unstable::Dropdown
	//{
	//public:
	//	Dropdown(const tbxInterface::Unstable::Dropdown::ItemContainer& itemNames, const size_t selectedIndex) :
	//		tbxInterface::Unstable::Dropdown(itemNames, selectedIndex),
	//		mNormalSprite("game_icons_sheet", "setup_dropdown_normal"),
	//		mFocusedSprite("game_icons_sheet", "setup_dropdown_focused"),
	//		mActiveSprite("game_icons_sheet", "setup_dropdown_active"),
	//		mDisabledSprite("game_icons_sheet", "setup_dropdown_disabled"),
	//		mNormalItemSprite("game_icons_sheet", "setup_dropdown_middle_item"),
	//		mFinalItemSprite("game_icons_sheet", "setup_dropdown_final_item"),
	//		mSelectedItemSprite("game_icons_sheet", "setup_dropdown_selected_item"),
	//		mSelectedText("")
	//	{
	//		CreateItemText("", mSelectedText);

	//		{
	//			mNormalSprite.SetScale(TurboBoom::kInterfaceScale);
	//			mFocusedSprite.SetScale(TurboBoom::kInterfaceScale);
	//			mActiveSprite.SetScale(TurboBoom::kInterfaceScale);
	//			mDisabledSprite.SetScale(TurboBoom::kInterfaceScale);
	//			mNormalItemSprite.SetScale(TurboBoom::kInterfaceScale);
	//			mFinalItemSprite.SetScale(TurboBoom::kInterfaceScale);
	//			mSelectedItemSprite.SetScale(TurboBoom::kInterfaceScale);
	//			mSelectedText.SetScale(TurboBoom::kInterfaceScale);

	//			// 2025-04-29: Each of the bits in the dropdown are moved 4px to the left so there is no weird black
	//			//   dot artifacts on the graphics settings page when the box is opened, say from the checkboxes being
	//			//   behind it. Also note the mSelectedItemSprite is modified with each change of selection.
	//			mNormalSprite.SetPosition(-4.0f, 0.0f);
	//			mFocusedSprite.SetPosition(-4.0f, 0.0f);
	//			mActiveSprite.SetPosition(-4.0f, 0.0f);
	//			mDisabledSprite.SetPosition(-4.0f, 0.0f);
	//			mNormalItemSprite.SetPosition(-4.0f, 0.0f);
	//			mFinalItemSprite.SetPosition(-4.0f, 0.0f);
	//			mSelectedItemSprite.SetPosition(-4.0f, 0.0f);
	//			mSelectedText.SetPosition(-4.0f, 0.0f);
	//		}

	//		//TurboBoom::Interface::CreateControlText(itemNames[selectedIndex], mSelectedText);

	//		AddGraphic(tbxInterface::Unstable::Element::State::Enabled, mNormalSprite);
	//		AddGraphic(State::Enabled, mSelectedText);

	//		AddGraphic(tbxInterface::Unstable::Element::State::Disabled, mDisabledSprite);
	//		AddGraphic(State::Disabled, mSelectedText);

	//		AddGraphic(tbxInterface::Unstable::Element::State::Activated, mActiveSprite);
	//		AddGraphic(tbxInterface::Unstable::Element::State::Focused, mFocusedSprite);
	//		AddGraphic(State::Focused, mSelectedText);

	//		SetPixelSize(static_cast<tbGraphics::PixelSpace>(mNormalSprite.GetWidth() * TurboBoom::kInterfaceScale),
	//			static_cast<tbGraphics::PixelSpace>(mNormalSprite.GetHeight() * TurboBoom::kInterfaceScale));

	//		RecreateActiveGraphics(selectedIndex);
	//	}

	//	virtual ~Dropdown(void)
	//	{
	//	}

	//protected:
	//	virtual void OnSelectionChange(const size_t selectedIndex) override
	//	{
	//		RecreateActiveGraphics(selectedIndex);
	//	}

	//private:
	//	void RecreateActiveGraphics(const size_t selectedIndex)
	//	{
	//		ClearGraphics(State::Activated);

	//		AddGraphic(tbxInterface::Unstable::Element::State::Activated, mActiveSprite);

	//		size_t currentIndex = 0;
	//		for (const tbCore::tbString& item : GetItemNames())
	//		{
	//			if (0 != currentIndex)
	//			{
	//				tbGraphics::Sprite* itemSprite = new tbGraphics::Sprite((currentIndex == GetItemNames().size() - 1) ? mFinalItemSprite : mNormalItemSprite);
	//				itemSprite->SetPosition(itemSprite->GetPosition() + tbMath::Vector2(0.0f, currentIndex * mNormalItemSprite.GetScaledHeight()));
	//				itemSprite->SetDepth(1.0f);
	//				AddGraphic(State::Activated, itemSprite);
	//			}

	//			if (GetFocusedIndex() == currentIndex)
	//			{
	//				// 2025-04-29: Move 4px to left because the whole dropbox is moved to the left, see constructor.
	//				mSelectedItemSprite.SetPosition(-4.0f, GetFocusedIndex() * mNormalItemSprite.GetScaledHeight());
	//				mSelectedItemSprite.SetDepth(1.0f);
	//				AddGraphic(State::Activated, mSelectedItemSprite);
	//			}

	//			tbGraphics::Text* itemLabel = new tbGraphics::Text("");
	//			CreateItemText(item, *itemLabel);

	//			itemLabel->SetScale(mSelectedText.GetScale());
	//			itemLabel->SetOrigin(tbGraphics::kAnchorCenterLeft);
	//			//itemLabel->SetPosition(10.0f, GetPixelHeight() * (static_cast<float>(currentIndex) + 0.5f));
	//			itemLabel->SetPosition(15.0f, mNormalItemSprite.GetScaledHeight() * (static_cast<float>(currentIndex) + 0.5f) + 2.0f);
	//			itemLabel->SetDepth(1.0f);

	//			if (currentIndex == GetSelectedIndex())
	//			{
	//				itemLabel->SetColor(tbGraphics::ColorPalette::MonkyBlue);
	//			}
	//			AddGraphic(State::Activated, itemLabel);

	//			++currentIndex;
	//		}

	//		mSelectedText.SetDepth(1.0f);
	//		mSelectedText.SetText(GetItemNames().at(selectedIndex)),
	//			mSelectedText.SetOrigin(tbGraphics::kAnchorCenterLeft);
	//		mSelectedText.SetPosition(10.0f, GetPixelHeight() * 0.5f);
	//	}

	//	tbGraphics::Sprite mNormalSprite;
	//	tbGraphics::Sprite mFocusedSprite;
	//	tbGraphics::Sprite mActiveSprite;
	//	tbGraphics::Sprite mDisabledSprite;
	//	tbGraphics::Sprite mNormalItemSprite;
	//	tbGraphics::Sprite mFinalItemSprite;
	//	tbGraphics::Sprite mSelectedItemSprite;

	//	tbGraphics::Text mSelectedText; //When closed
	//};

};	//namespace Asteroids::Interface

#endif /* Asteroids_Dropdown_hpp */
