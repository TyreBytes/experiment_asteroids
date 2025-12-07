///
/// @file
/// @details Provide definitions for the debug channels in Asteroids.
///
/// <!-- Copyright (c) 2025 Tyre Bytes LLC - All Rights Reserved -->
///------------------------------------------------------------------------------------------------------------------///

#ifndef Asteroids_Logging_hpp
#define Asteroids_Logging_hpp

//Do Not Include asteroids.hpp or any other Asteroids headers that will to avoid circular inclusion.

#include <turtle_brains/core/debug/tb_debug_logger.hpp>
#include <turtle_brains/system/tb_system_timer.hpp>

namespace Asteroids
{
	using String = TurtleBrains::String;
	using StringView = std::string_view;

	using LogWithColor = TurtleBrains::Core::Debug::LogWithColor;
	using LogGame = TurtleBrains::Core::Debug::LogGameplay;
	using LogGraphics = TurtleBrains::Core::Debug::LogGraphics;
	using LogPhysics = TurtleBrains::Core::Debug::LogPhysics;
	using LogNetwork = TurtleBrains::Core::Debug::LogNetwork;

	struct GameStateChannel { static String AsString(void) { return ("GameState"); } };
	using LogState = TurtleBrains::Core::Debug::LogChannelLevel<GameStateChannel>;

	struct AudioChannel { static String AsString(void) { return ("Audio"); } };
	using LogAudio = TurtleBrains::Core::Debug::LogChannelLevel<AudioChannel>;

	struct ShaderSystemChannel { static String AsString(void) { return ("ShaderSystem"); } };
	using LogShader = TurtleBrains::Core::Debug::LogChannelLevel<ShaderSystemChannel>;


	///
	/// @details Puts quotes in front of, and behind, the string of data when logging, which can be somewhat cleaner
	///   than writing log << "Hello \"" << data << "\" Turtles!";
	///
	class QuotedString
	{
	public:
		explicit QuotedString(const String& data) :
			mContents(data)
		{
		}

		friend tbCore::Debug::StreamLogger& operator<<(tbCore::Debug::StreamLogger& stream, const QuotedString& quotedString)
		{
			stream << "\"" << quotedString.mContents << "\"";
			return stream;
		}

	private:
		const String mContents;
	};



	class DebugInfo
	{
	public:
		static DebugInfo PlayTimeHistory(void);

		//DebugInfo(const GameState::RacecarState& racecar)
		//{
		//	std::stringstream ss;
		//	const String driverName = (GameState::InvalidDriver() == racecar.GetDriverIndex()) ? "no driver" :
		//		GameState::DriverState::Get(racecar.GetDriverIndex()).GetName();
		//	ss << "Racecar(" << static_cast<int>(racecar.GetRacecarIndex()) << ", \"" << driverName << "\")";
		//	mInfo = ss.str();
		//}

		//DebugInfo(const GameState::RacecarIndex racecarIndex) :
		//	mInfo("InvalidRacecar")
		//{
		//	if (true == GameState::IsValidRacecar(racecarIndex))
		//	{
		//		std::stringstream ss;
		//		const GameState::RacecarState& racecar = GameState::RacecarState::Get(racecarIndex);
		//		const String driverName = (GameState::InvalidDriver() == racecar.GetDriverIndex()) ? "no driver" :
		//			GameState::DriverState::Get(racecar.GetDriverIndex()).GetName();
		//		ss << "Racecar(" << static_cast<int>(racecar.GetDriverIndex()) << ", \"" << driverName << "\")";
		//		mInfo = ss.str();
		//	}
		//}

		friend std::ostream& operator<<(std::ostream& output, const DebugInfo& debugInfo)
		{
			output << debugInfo.mInfo;
			return output;
		}

	private:
		String mInfo;
	};

	void SetLoggingLevels(void);

};	//namespace Asteroids

#endif /* Asteroids_Logging_hpp */
