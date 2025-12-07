///
/// @file
/// @details Entry point of the Asteroids project.
///
/// <!-- Copyright (c) 2025 Tyre Bytes LLC - All Rights Reserved -->
///------------------------------------------------------------------------------------------------------------------///

#ifndef Asteroids_Asteroids_hpp
#define Asteroids_Asteroids_hpp

#if defined(tb_debug_build) || defined(tb_development_build)
  #define rusty_development
#endif /* debug or development build */

#include "utilities/string_table.hpp"
#include "utilities/rusty_timer.hpp"
#include "user_settings.hpp"
#include "logging.hpp"

//Do Not Include game_manager.hpp to avoid circular inclusion.

#include <turtle_brains/tb_application_kit.hpp>
#include <turtle_brains/tb_audio_kit.hpp>
#include <turtle_brains/tb_core_kit.hpp>
#include <turtle_brains/tb_debug_kit.hpp>
#include <turtle_brains/tb_game_kit.hpp>
#include <turtle_brains/tb_graphics_kit.hpp>
#include <turtle_brains/tb_math_kit.hpp>
#include <turtle_brains/tb_development_kit.hpp>

#include <turtle_brains/core/tb_node.hpp>
#include <turtle_brains/core/tb_component.hpp>
#include <turtle_brains/core/tb_dynamic_structure.hpp>

#include <turtle_brains/graphics/tb_camera.hpp>

#include <turtle_brains/express/game/tbx_basic_tech_tree.hpp>

#include <cstdint>

namespace Asteroids
{
	using StringView = std::string_view;

	using UpgradeKey = tbxGame::BasicTechTree::UpgradeKey;

	using NodeKey = TurtleBrains::Core::NodeKey;
	using ComponentKey = TurtleBrains::Core::ComponentKey;
	using ComponentTypeKey = TurtleBrains::Core::ComponentTypeKey;

	using DynamicStructure = TurtleBrains::Core::DynamicStructure;

	using String = TurtleBrains::String;
	using Scalar = float;// TurtleBrains::Math::Scalar;
	using Vector2 = TurtleBrains::Math::Vector2;
	using Vector3 = TurtleBrains::Math::Vector3;
	using Vector4 = TurtleBrains::Math::Vector4;
	using Matrix3 = TurtleBrains::Math::Matrix3;
	using Matrix4 = TurtleBrains::Math::Matrix4;
	using Angle = TurtleBrains::Math::Angle;

	using Color = TurtleBrains::Graphics::Color;
	namespace ColorPalette = TurtleBrains::Graphics::ColorPalette;
	using ShakeIntensity = TurtleBrains::Graphics::Camera::IntensityLevel;

	using Anchor = TurtleBrains::Graphics::Anchor;
	using AnchorLocation = TurtleBrains::Graphics::AnchorLocation;

	using Key = TurtleBrains::Application::Key;
	namespace Input = TurtleBrains::Game::Input;

	struct IntVector2
	{
		int x;
		int y;
	};

	using GameTimer = TurtleBrains::Game::GameTimer;

	// 2025-10-08: All hard-coded positions and such should be designed as if the game is being played at resolution of
	//   1920x1080. (aka 1080p). When the game is actually at that resolution the scale will remain 1.0. The scale of
	//   all scenes will keep 100% of the height, and lose some details to the left/right unless anchoring a UI element
	//   to a screen edge.
	//
	//   This is a change going FORWARD and prior to today(2025-10-08) it was hard-coded at 720p, so there is some old
	//   position that don't fit correctly.
	//
	// This might not even be the best way to tackle the problem since Behaviors might not update to screen-scale... Say
	//   setting the floor when creating LaunchAndFall() to 980 * gameScale, then the screen changes mid-way through the
	//   behavior, it would land in the wrong spot. If we use gameScale in the middle of the Behavior we are more likely
	//   to have issues in the code-base getting confused when things have and have not already applied the scale. YIKES.
	float GameScale(void);

	tbGraphics::PixelSpace WorldTargetWidth(void);
	tbGraphics::PixelSpace WorldTargetHeight(void);

	// 2025-10-16: WorldSpace and GameSpace are the same and designed to as 1920x1080 (or rather 1080 tall by whatever width).
	template<typename Type> Type WorldSpaceToScreenSpace(const Type& worldSpace)
	{
		return worldSpace * GameScale();
	}

	// 2025-10-16: WorldSpace and GameSpace are the same and designed to as 1920x1080 (or rather 1080 tall by whatever width).
	template<typename Type> Type ScreenSpaceToWorldSpace(const Type& screenSpace)
	{
		return screenSpace / GameScale();
	}

	Vector2 RotationToForwardVector2(const Angle& orientation);

	// Define the FromString/ToString so that we can create custom TemplateSpecializations.
	template<typename Type> String ToString(const Type& object) {
		return tbCore::ToString(object);
	}
	template<typename Type> Type FromString(const String& string) {
		return tbCore::FromString<Type>(string);
	}

	template <typename Type> Type FromDynamicStructure(const DynamicStructure& data) {
		return TurtleBrains::FromDynamicStructure<Type>(data);
	}
	template <typename Type> DynamicStructure ToDynamicStructure(const Type& value) {
		return TurtleBrains::ToDynamicStructure(value);
	}

	template<typename Type> void WriteBinary(const Type& object, tbCore::OutputFile& outputFile) {
		tbCore::FileUtilities::WriteBinary(object, outputFile);
	}
	template<typename Type> void ReadBinary(const Type& object, tbCore::InputFile& inputFile) {
		tbCore::FileUtilities::ReadBinary(object, inputFile);
	}
	template<typename Type> Type ReadBinary(tbCore::InputFile& inputFile) {
		return tbCore::FileUtilities::ReadBinary<Type>(inputFile);
	}

	// 2025-11-11: This is defined in utilities.cpp
	// 2025-10-31: Currently these are forced into Asteroids, eventually TurtleBrains may force this into Global Space?
	//   Unsure if there would be a way to make it as Utilities, or such.
	template<> AnchorLocation FromDynamicStructure(const DynamicStructure& data);

	typedef tbCore::uint32 MillisecondTimer;

	float FixedTime(void);

	static const float kInvalidTime(-1.0f);

	String GetSaveDirectory(void);
	String GetDiagnosticDirectory(void);

	UserSettings ParseLaunchParameters(int argumentCount, const char* argumentValues[]);

}; /* namespace Asteroids */

#endif /* Asteroids_Asteroids_hpp */
