///
/// @file
/// @details A place for interface helpers.
///
/// <!-- Copyright (c) 2025 Tyre Bytes LLC - All Rights Reserved -->
///------------------------------------------------------------------------------------------------------------------///

#include "interface.hpp"

#if defined(rusty_development)
  #include "development/screen_resolution_tool.hpp"
#endif

#include <unordered_map>

namespace
{
	namespace FontSize
	{
		static const tbGraphics::FontSize HighRes = 128;
		static const tbGraphics::FontSize Title = 52;
		static const tbGraphics::FontSize Normal = 38;
		static const tbGraphics::FontSize Small = 28;
	};
};

//--------------------------------------------------------------------------------------------------------------------//

float Asteroids::Interface::Scale(void)
{
	return static_cast<float>(tbGraphics::ScreenHeight()) / static_cast<float>(Interface::Height());
}

//--------------------------------------------------------------------------------------------------------------------//

float Asteroids::Interface::AspectRatio(void)
{
	//return tbGraphics::ScreenAspectRatio();
	return 1920.0f / 1080.0f;
}

//--------------------------------------------------------------------------------------------------------------------//

tbGraphics::PixelSpace Asteroids::Interface::Width(void)
{
	// TODO: Asteroids: Settings: 2025-10-22: This maximumInterfaceAspectRatio should be something in the settings to
	//   support very wide screens so that the player can adjust how wide the UI is. Maybe some want it on screen-edge.
	const float kMaximumInterfaceAspectRatio = 1920.0f / 1080.0f; //16:9, aka 1.77777...
	//const float kMaximumInterfaceAspectRatio = 21.0f / 9.0f;

	float appliedMaximumRatio = kMaximumInterfaceAspectRatio;
#if defined(rusty_development)
	if (true == Development::IsRenderingLayout())
	{
		appliedMaximumRatio = 4.0f / 3.0f;
	}
#endif

	const float scaledScreenWidth = tbGraphics::ScreenWidth() / Interface::Scale();
	const float widthValue = tbMath::Minimum<float>(scaledScreenWidth, Interface::Height() * appliedMaximumRatio);
	return static_cast<tbGraphics::PixelSpace>(std::round(widthValue));
}

//--------------------------------------------------------------------------------------------------------------------//

tbGraphics::PixelSpace Asteroids::Interface::Height(void)
{
	return 1080;
}

//--------------------------------------------------------------------------------------------------------------------//

Asteroids::Vector2 Asteroids::Interface::Size(void)
{
	return Vector2(Width(), Height());
}

//--------------------------------------------------------------------------------------------------------------------//

tbGraphics::PixelSpace Asteroids::Interface::TargetWidth(void)
{
	return static_cast<tbGraphics::PixelSpace>(tbGraphics::ScreenWidth() / Interface::Scale());
}

//--------------------------------------------------------------------------------------------------------------------//

tbGraphics::PixelSpace Asteroids::Interface::TargetHeight(void)
{
	return Height();
}

//--------------------------------------------------------------------------------------------------------------------//

Asteroids::Vector2 Asteroids::Interface::TargetSize(void)
{
	return Vector2(TargetWidth(), TargetHeight());
}

//--------------------------------------------------------------------------------------------------------------------//

tbMath::Vector2 Asteroids::Interface::GetAnchorPositionOfInterface(const tbGraphics::AnchorLocation& anchor,
	const float offsetX, const float offsetY)
{
	return GetAnchorPositionOfInterface(anchor, tbMath::Vector2(offsetX, offsetY));
}

//--------------------------------------------------------------------------------------------------------------------//

tbMath::Vector2 Asteroids::Interface::GetAnchorPositionOfInterface(const tbGraphics::AnchorLocation& anchor, const Vector2& offset)
{
	const float interfaceHeight = Interface::Height();
	const float interfaceWidth = Interface::Width();
	const Vector2 interfacePosition(((tbGraphics::ScreenWidth() / Scale()) - interfaceWidth) / 2.0f, 0.0f);

	switch (anchor)
	{
	case Anchor::TopLeft:      return interfacePosition + offset;
	case Anchor::TopCenter:    return interfacePosition + offset + tbMath::Vector2(0.5f * interfaceWidth, 0.0f * interfaceHeight);
	case Anchor::TopRight:     return interfacePosition + offset + tbMath::Vector2(1.0f * interfaceWidth, 0.0f * interfaceHeight);
	case Anchor::CenterLeft:   return interfacePosition + offset + tbMath::Vector2(0.0f * interfaceWidth, 0.5f * interfaceHeight);
	case Anchor::Center:       return interfacePosition + offset + tbMath::Vector2(0.5f * interfaceWidth, 0.5f * interfaceHeight);
	case Anchor::CenterRight:  return interfacePosition + offset + tbMath::Vector2(1.0f * interfaceWidth, 0.5f * interfaceHeight);
	case Anchor::BottomLeft:   return interfacePosition + offset + tbMath::Vector2(0.0f * interfaceWidth, 1.0f * interfaceHeight);
	case Anchor::BottomCenter: return interfacePosition + offset + tbMath::Vector2(0.5f * interfaceWidth, 1.0f * interfaceHeight);
	case Anchor::BottomRight:  return interfacePosition + offset + tbMath::Vector2(1.0f * interfaceWidth, 1.0f * interfaceHeight);
	};

	return offset;
}

//--------------------------------------------------------------------------------------------------------------------//

tbGraphics::Sprite Asteroids::Interface::MakeIconSprite(const String& iconName, const float size)
{
	//tbGraphics::Sprite iconSprite("data/interface/icons/" + iconName + ".png");
	//iconSprite.SetScale(size / 512.0f);
	//return iconSprite;

	const std::vector<String> nameParts = tbCore::StringUtilities::SeparateString(iconName, ":");
	tb_error_if(nameParts.size() != 2, "Expected a shortName and a spriteIndex in iconName like: short_name:42");

	const std::unordered_map<String, String> shortNameToTexture = {
		{ "accessories", "data/interface/icons/wasteland_icons_accessories.png" },
		{ "aid_and_misc", "data/interface/icons/wasteland_icons_aid_and_misc.png" },
		{ "equipment", "data/interface/icons/wasteland_icons_equipment.png" },
		{ "faction_gear", "data/interface/icons/wasteland_icons_faction_gear.png" },
		{ "junk_pack", "data/interface/icons/wasteland_icons_junk_pack.png" },
		{ "weapons", "data/interface/icons/wasteland_icons_weapons.png" },
	};

	tb_error_if(false == shortNameToTexture.contains(nameParts[0]), "Expected shortName('%s') to exist in texture table for icons.", nameParts[0].c_str());
	const String textureFilepath = shortNameToTexture.at(nameParts[0]);
	const tbGraphics::PixelSpace spriteIndex = FromString<tbGraphics::PixelSpace>(nameParts[1]);
	tb_error_if(spriteIndex >= 144, "Expected spriteIndex('%d') to be in range; 0 <= index < 144.", spriteIndex);

	// The wasteland pack 'as-is' was 32x32 pixel sprites in a 12x12 grid for each sheet.
	const tbGraphics::PixelSpace spriteSize = 32;
	const tbGraphics::PixelSpace spriteX = (spriteIndex % 12) * spriteSize;
	const tbGraphics::PixelSpace spriteY = (spriteIndex / 12) * spriteSize;

	tbGraphics::TextureHandle spritePack = tbGraphics::theTextureManager.CreateTextureFromFile(textureFilepath, false);
	tbGraphics::theTextureManager.SetTextureFiltering(spritePack, tbGraphics::TextureFilter::Closest);
	tb_unused(spritePack);

	tbGraphics::Sprite iconSprite(textureFilepath, spriteX, spriteY, spriteSize, spriteSize);
	iconSprite.SetScale(size / spriteSize);
	//iconSprite.SetScale(2.0f);

	return iconSprite;
}

//--------------------------------------------------------------------------------------------------------------------//

//tbGraphics::Sprite Asteroids::Interface::MakeStatIcon(const Stat& stat, const float size)
//{
//	const std::unordered_map<Stat, String> statToTexture = {
//		{ Stat::NutsAndBolts, "data/interface/icons/interface_icon_bolts.png" },
//		{ Stat::Unknown, "data/interface/icons/interface_icon_alert.png" }
//	};
//
//	tb_error_if(false == statToTexture.contains(stat), "Expected stat('%s') to exist in texture table for StatIcon().", ToString(stat).c_str());
//	const String textureFilepath = statToTexture.at(stat);
//
//	tbGraphics::Sprite iconSprite(textureFilepath);
//	iconSprite.SetScale(size / iconSprite.GetHeight());
//	return iconSprite;
//}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::Interface::MakeTitleText(tbGraphics::Text& textObject, const String& textString)
{
	textObject.SetText(textString, FontSize::Title, "data/font/comic_mono.ttf");
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::Interface::MakeNormalText(tbGraphics::Text& textObject, const String& textString)
{
	textObject.SetText(textString, FontSize::Normal, "data/font/comic_mono.ttf");
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::Interface::MakeSmallText(tbGraphics::Text& textObject, const String& textString)
{
	textObject.SetText(textString, FontSize::Small, "data/font/comic_mono.ttf");
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::Interface::MakeScalableText(tbGraphics::Text& textObject, const String& textString)
{
	MakeScalableText(&textObject, textString);
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::Interface::MakeScalableText(tbGraphics::Text* const &textObject , const String& textString)
{
	textObject->SetText(textString, FontSize::HighRes, "data/font/comic_mono.ttf");
}

//--------------------------------------------------------------------------------------------------------------------//
