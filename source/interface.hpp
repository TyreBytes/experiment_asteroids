///
/// @file
/// @details A place for interface helpers.
///
/// <!-- Copyright (c) 2025 Tyre Bytes LLC - All Rights Reserved -->
///------------------------------------------------------------------------------------------------------------------///

#ifndef Asteroids_Interface_hpp
#define Asteroids_Interface_hpp

#include "asteroids.hpp"
#include "game_manager.hpp"

namespace Asteroids
{
	static const float kPadding = 30.0f;
	static const float kHalfPadding = kPadding / 2.0f;

	static const float kBorder = 4.0f;
	static const float kRoundedCornerRadius = 10.0f;

	static const float kIconSize = 96.0f;
	static const float kIconButtonSize = kIconSize;

	static const Vector2 kEventNoticeSize(1000.0f, 600.0f);

	// 2025-10-22: The amount of space from center to center of each tech tree icon in InterfaceSpace.
	static const float kTechTreeSpacing = 150.0f;

	namespace AsteroidsColor
	{
		enum class PaletteTheme
		{
			BlueVariation = 0,
			Vaporwave
		};

		static const PaletteTheme ActiveTheme = PaletteTheme::BlueVariation;

		struct ThemePalette
		{
			Color PrimaryColor;
			Color PrimaryColorHalf;
			Color PrimaryTextColor;
			Color PrimaryBackgroundColor;
			Color PrimaryBackgroundColorHalfTransparent;

			Color SecondaryColor;
			Color SecondaryColorHalf;
			Color SecondaryTextColor;
			Color SecondaryBackgroundColor;
			Color SecondaryBackgroundColorHalfTransparent;

			Color TernaryColor;
			Color TernaryColorHalf;
			Color TernaryTextColor;
			Color TernaryBackgroundColor;
			Color TernaryBackgroundColorHalfTransparent;

			Color DangerTextColor;
			Color DangerBackgroundColor;

			Color SuccessTextColor;
			Color SuccessBackgroundColor;

			// Quality, actually kinda important to be shades of this.
			Color PoorColor = ColorPalette::Red;
			Color OkayColor = ColorPalette::Red;
			Color GoodColor = ColorPalette::Blue;
			Color PerfectColor = ColorPalette::Green;
		};

		static const ThemePalette AllPalettes[] =
		{
			// ----------------------------------------------------------------------------------------------------------------------
			// [0] Blue Variation Palette
			// ----------------------------------------------------------------------------------------------------------------------
			{
				Color(0xFF365A8C), Color(0xFFFF00FF), Color(0xFFE9F2FF), Color(0xFF1B1D24), Color(0x771B1D24),
				Color(0xFF6C7B90), Color(0xFF353C47), Color(0xFFE0E5EB), Color(0xFF14161B), Color(0x7714161B),
				Color(0xFF32C7D6), Color(0xFFFF00FF), Color(0xFFE0FFFF), Color(0xFF0D1014), Color(0x770D1014),
				Color(0xFFFF9E9E), Color(0xFF3B100E),
				Color(0xFFD4E88C), Color(0xFF22361C)
			},

			// ----------------------------------------------------------------------------------------------------------------------
			// [1] Vaporwave Variation Palette
			// ----------------------------------------------------------------------------------------------------------------------
			{
				Color(0xFF8033A0), Color(0xFFFF00FF), Color(0xFFFFD1F7), Color(0xFF1A0A1F), Color(0x771A0A1F),
				Color(0xFF4D9BFF), Color(0xFF203E60), Color(0xFFE0FFFF), Color(0xFF0D0512), Color(0x770D0512),
				Color(0xFF40FFD0), Color(0xFF5b7b73), Color(0xFFF7FFFF), Color(0xFF000000), Color(0x77000000),
				Color(0xFFFF5599), Color(0xFF401025),
				Color(0xFF99FF55), Color(0xFF1A380A)
			}
		};

		static constexpr const ThemePalette& GetActivePalette()
		{
			return AllPalettes[static_cast<int>(ActiveTheme)];
		}

		// Error seems to be a bug: https://developercommunity.visualstudio.com/t/IntelliSense-shows-incorrect-expression/10800174
		inline const Color PrimaryColor = GetActivePalette().PrimaryColor;
		inline const Color PrimaryColorHalf = GetActivePalette().PrimaryColorHalf;
		inline const Color PrimaryTextColor = GetActivePalette().PrimaryTextColor;
		inline const Color PrimaryBackgroundColor = GetActivePalette().PrimaryBackgroundColor;
		inline const Color PrimaryBackgroundColorHalfTransparent = GetActivePalette().PrimaryBackgroundColorHalfTransparent;

		inline const Color SecondaryColor = GetActivePalette().SecondaryColor;
		inline const Color SecondaryColorHalf = GetActivePalette().SecondaryColorHalf;
		inline const Color SecondaryTextColor = GetActivePalette().SecondaryTextColor;
		inline const Color SecondaryBackgroundColor = GetActivePalette().SecondaryBackgroundColor;
		inline const Color SecondaryBackgroundColorHalfTransparent = GetActivePalette().SecondaryBackgroundColorHalfTransparent;

		inline const Color TernaryColor = GetActivePalette().TernaryColor;
		inline const Color TernaryColorHalf = GetActivePalette().TernaryColorHalf;
		inline const Color TernaryTextColor = GetActivePalette().TernaryTextColor;
		inline const Color TernaryBackgroundColor = GetActivePalette().TernaryBackgroundColor;
		inline const Color TernaryBackgroundColorHalfTransparent = GetActivePalette().TernaryBackgroundColorHalfTransparent;

		inline const Color DangerTextColor = GetActivePalette().DangerTextColor;
		inline const Color DangerBackgroundColor = GetActivePalette().DangerBackgroundColor;

		inline const Color SuccessTextColor = GetActivePalette().SuccessTextColor;
		inline const Color SuccessBackgroundColor = GetActivePalette().SuccessBackgroundColor;

		inline const Color PoorColor = GetActivePalette().PoorColor;
		inline const Color OkayColor = GetActivePalette().OkayColor;
		inline const Color GoodColor = GetActivePalette().GoodColor;
		inline const Color PerfectColor = GetActivePalette().PerfectColor;

	}
};

namespace Asteroids::Interface
{
	float Scale(void);
	float AspectRatio(void);

	tbGraphics::PixelSpace Width(void);
	tbGraphics::PixelSpace Height(void);
	Vector2 Size(void);

	tbGraphics::PixelSpace TargetWidth(void);
	tbGraphics::PixelSpace TargetHeight(void);
	Vector2 TargetSize(void);

	Vector2 GetAnchorPositionOfInterface(const tbGraphics::AnchorLocation& anchor, const float offsetX, const float offsetY);
	Vector2 GetAnchorPositionOfInterface(const tbGraphics::AnchorLocation& anchor, const Vector2& offset = Vector2::Zero());

	template<typename Type> Type ScreenSpaceToInterface(const Type & screenSpace)
	{
		return screenSpace / Scale();
	}

	template<typename Type> Type InterfaceToScreenSpace(const Type& interfaceSpace)
	{
		return interfaceSpace * Scale();
	}

	tbGraphics::Sprite MakeIconSprite(const String& iconName, const float size = kIconSize);
	tbGraphics::Sprite MakeStatIcon(const Stat& stat, const float size = kIconSize);

	void MakeTitleText(tbGraphics::Text& textObject, const String& textString);
	void MakeNormalText(tbGraphics::Text& textObject, const String& textString);
	void MakeSmallText(tbGraphics::Text& textObject, const String& textString);
	void MakeScalableText(tbGraphics::Text& textObject, const String& textString);
	void MakeScalableText(tbGraphics::Text* const &textObject, const String& textString);
};	//namespace Asteroids

#endif /* Asteroids_Interface_hpp */
