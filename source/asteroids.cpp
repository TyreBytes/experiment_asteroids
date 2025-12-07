///
/// @file
/// @details Entry point of the Asteroids project.
///
/// <!-- Copyright (c) 2025 Tyre Bytes LLC - All Rights Reserved -->
///------------------------------------------------------------------------------------------------------------------///

#include "asteroids.hpp"
#include "version.hpp"
#include "logging.hpp"
#include "game_manager.hpp"

#include "scenes/scene_manager.hpp"
#include "shader_system/shaders.hpp"
#include "shader_system/shader_manager.hpp"

#include <turtle_brains/core/tb_platform_utilities.hpp>
#include <turtle_brains/core/unit_test/tb_unit_test.hpp>
#include <turtle_brains/core/tb_version.hpp>
#include <turtle_brains/system/tb_system_timer.hpp>
#include <turtle_brains/game/diagnostics/tb_game_diagnostics.hpp>

#if defined(rusty_development)
  #include "development/developer_console_commands.hpp"
#endif

//--------------------------------------------------------------------------------------------------------------------//

namespace Asteroids
{
#if defined(asteroids_demo_build)
	const tbCore::int32 kSteamApplicationID = 4176700;
#else
	const tbCore::int32 kSteamApplicationID = 2695500;
#endif /* game / demo build */

	tbGame::GameApplication* theGameApplication = nullptr;
	bool theGameIsActive = false;

	int Main(int argumentCount, const char* argumentValues[]);

	String theQuickPlayRacetrackPath = "";

	class AsteroidsHandler : public tbApplication::ApplicationHandlerInterface
	{
	public:
		virtual void OnCreateGraphicsContext(void) override
		{
			tb_debug_log(LogGraphics::Always() << "Asteroids handling CreateGraphicsContext().");
			ApplicationHandlerInterface::OnCreateGraphicsContext();
			ShaderSystem::theShaderManager.CreateGraphicsContext();
			ShaderSystem::CreateShaders();
		}

		virtual void OnDestroyGraphicsContext(void) override
		{
			tb_debug_log(LogGraphics::Always() << "Asteroids handling DestroyGraphicsContext().");
			ShaderSystem::DestroyShaders();
			ShaderSystem::theShaderManager.DestroyGraphicsContext();
			ApplicationHandlerInterface::OnDestroyGraphicsContext();
		}

		virtual void OnBecomeActive(void) override
		{
			theGameIsActive = true;
			ApplicationHandlerInterface::OnBecomeActive();
		}

		virtual void OnBecomeInactive(void) override
		{
			theGameIsActive = false;
			ApplicationHandlerInterface::OnBecomeInactive();
		}
	};
};

//--------------------------------------------------------------------------------------------------------------------//

int Asteroids::Main(int argumentCount, const char* argumentValues[])
{
	const UserSettings launchSettings = ParseLaunchParameters(argumentCount, argumentValues);

	if (true == launchSettings.GetBoolean("developer"))
	{
	//	RacecarSelectionScene::SetDeveloperSelectedCarID(1);
	//	const String developerKey = tbCore::FileUtilities::LoadFileContentsToString(GetSaveDirectory() + "developer_key", true);
	//	tb_always_log(LogGame::Warning() << ((true == developerKey.empty()) ? "No DeveloperKey was found or loaded." : "[DeveloperKey] successfully loaded."));
	//	if (false == developerKey.empty())
	//	{
	//		Network::ClientPacketHandler::SetUserAccessKey(developerKey, Network::AuthenticationService::Developer);
	//	}
	}

	TurtleBrains::Core::Debug::SetCrashReportFilepath(GetDiagnosticDirectory() + "crash_report.txt");
	TurtleBrains::Game::Diagnostics::SetSettingsFile(GetDiagnosticDirectory() + "imgui.ini");

	TheUserSettings().CreateDefaultSettings();
	TheUserSettings().LoadSettings("settings.cfg");

	const bool kFullscreenDefault = false;

	// TODO: Asteroids: TurtleBrains does not support ensuring the application is in a viewable area. There is/was
	//   a comment talking about Windows managing that for us with CW_USEDEFAULT in TrailingBrakes (2023-10-22).
	tbApplication::WindowProperties windowProperties;
	windowProperties.mWindowMode = launchSettings.GetBoolean(Settings::Fullscreen(), TheUserSettings().GetBoolean(Settings::Fullscreen(), kFullscreenDefault))
		? tbApplication::WindowMode::FullScreen : tbApplication::WindowMode::Windowed;

	windowProperties.mVerticalSync = launchSettings.GetBoolean(Settings::VerticalSync(), TheUserSettings().GetBoolean(Settings::VerticalSync(), true));
	windowProperties.mWindowPositionX = static_cast<tbCore::int16>(launchSettings.GetInteger(Settings::WindowPositionX(), TheUserSettings().GetInteger(Settings::WindowPositionX(), 100)));
	windowProperties.mWindowPositionY = static_cast<tbCore::int16>(launchSettings.GetInteger(Settings::WindowPositionY(), TheUserSettings().GetInteger(Settings::WindowPositionY(), 100)));
	windowProperties.mWindowWidth = static_cast<tbCore::int16>(launchSettings.GetInteger(Settings::WindowWidth(), TheUserSettings().GetInteger(Settings::WindowWidth(), 1280)));
	windowProperties.mWindowHeight = static_cast<tbCore::int16>(launchSettings.GetInteger(Settings::WindowHeight(), TheUserSettings().GetInteger(Settings::WindowHeight(), 720)));

	AsteroidsHandler handler;
	tbGame::GameApplication gameApplication(windowProperties, true, &handler);
	theGameApplication = &gameApplication;
	gameApplication.SetWindowTitle(Version::ProjectVersionString());

	tbAudio::theAudioManager.LoadEventTable("audio_events", "data/audio/audio_events.json");

#if defined(rusty_development)
	Development::RegisterDeveloperCommands();

#if defined(tb_with_development_tools)
	tbDevelopment::ConfigObject::AddConfigurationFile("data/runtime_configuration.json");
#endif
#endif

	SceneManager::CreateSceneManager();

	// Needs to wait until SceneManager is created because GarageScene needs to exist.
	GameManager::LoadGame();

	tbAudio::theAudioManager.SetGlobalMusicVolume(0.6899f);
	tbAudio::theAudioManager.SetGlobalEffectVolume(0.6899f);

	gameApplication.RunGame(SceneManager::GetScene(SceneId::kTitleScene));

	GameManager::SaveGame();

	// 2025-12-07: This is where we should be unloading the audio events to prevent warning, however ikiwixz found that
	//   tbInternal: tbWarning: EventTable 'audio_events' was not unloaded before shutdown. is coming from RunGame()
	//   before we gain control. And calling this here actually causes an error condition since TurtleBrains already
	//   cleaned up...
	//tbAudio::theAudioManager.UnloadEventTable("audio_events");


	// Save the window properties and other user settings before complete shutting down.
	const tbApplication::WindowProperties currentWindowProperties = gameApplication.GetWindowProperties();
	TheUserSettings().SetInteger(Settings::WindowPositionX(), currentWindowProperties.mWindowPositionX);
	TheUserSettings().SetInteger(Settings::WindowPositionY(), currentWindowProperties.mWindowPositionY);
	TheUserSettings().SetInteger(Settings::WindowWidth(), currentWindowProperties.mWindowWidth);
	TheUserSettings().SetInteger(Settings::WindowHeight(), currentWindowProperties.mWindowHeight);
	TheUserSettings().SetInteger(Settings::Fullscreen(), tbApplication::WindowMode::FullScreen == currentWindowProperties.mWindowMode);

	TheUserSettings().SaveSettings("settings.cfg");

	SceneManager::DestroySceneManager();
	theGameApplication = nullptr;

	return 0;
}

//--------------------------------------------------------------------------------------------------------------------//

Asteroids::UserSettings Asteroids::ParseLaunchParameters(int argumentCount, const char* argumentValues[])
{
	UserSettings launchSettings;

	const std::map<String, String> booleanArgumentToKeys = {
		{ "--race", "race" },
		{ "--developer", "developer" },
	};

	const std::map<String, String> intArgumentToKeys = {
		{ "--x", "window_position_x" },
		{ "--y", "window_position_y" },
		{ "--width", "window_width" },
		{ "--height", "window_height" },
	};

	const std::map<String, String> stringArgumentToKeys = {
		//{ "--name", "name" },
	};

	for (int argumentIndex = 0; argumentIndex < argumentCount; ++argumentIndex)
	{
		for (const auto& nameKeyPair : booleanArgumentToKeys)
		{
			if (argumentValues[argumentIndex] == nameKeyPair.first)
			{
				launchSettings.SetBoolean(nameKeyPair.second, true);
			}
		}

		for (const auto& nameKeyPair : intArgumentToKeys)
		{
			if (argumentValues[argumentIndex] == nameKeyPair.first && argumentIndex + 1 < argumentCount)
			{
				++argumentIndex;
				launchSettings.SetInteger(nameKeyPair.second, FromString<tbCore::int64>(argumentValues[argumentIndex]));
			}
		}

		for (const auto& nameKeyPair : stringArgumentToKeys)
		{
			if (argumentValues[argumentIndex] == nameKeyPair.first && argumentIndex + 1 < argumentCount)
			{
				++argumentIndex;
				launchSettings.SetString(nameKeyPair.second, argumentValues[argumentIndex]);
			}
		}
	}

	return launchSettings;
}

//--------------------------------------------------------------------------------------------------------------------//

//#include <sstream>
//
//struct History
//{
//	std::stringstream history;
//	//tbCore::Debug::StreamLogger* mOriginalLogger;
//	std::ostream* mOriginalOutput;
//
//	History(void)
//	{
//	}
//
//	~History(void)
//	{
//		std::ofstream("history.txt", std::ios_base::app) << history.str();
//		tb_debug_log("Making history");
//
//		if (nullptr != mOriginalOutput)
//		{
//			*mOriginalOutput << history.str();
//		}
//
//		//if (nullptr != mOriginalLogger)
//		//{
//		//	*mOriginalLogger << history.str();
//		//}
//	}
//
//	//friend std::stringstream& operator<<(tbCore::Debug::StreamLogger& output, const History& history)
//	//{
//	//	History& mhistory = const_cast<History&>(history);
//	//	mhistory.mOriginalLogger = &output;
//	//	mhistory.history << "writing history: ";
//	//	return mhistory.history;
//	//}
//
//	friend std::stringstream& operator<<(std::ostream& output, const History& history)
//	{
//		History& mhistory = const_cast<History&>(history);
//		mhistory.mOriginalOutput = &output;
//		mhistory.history << "writing history: ";
//		return mhistory.history;
//	}
//};
//
//
//static std::stringstream& LogHistory(void)
//{
//	static History theRealHistory;
//	return theRealHistory.history;
//}

int main(const int argumentCount, const char* argumentValues[])
{
	for (int argumentIndex = 0; argumentIndex < argumentCount; ++argumentIndex)
	{	//Run unit tests if --test is present as an argument.
		if (Asteroids::String("--test") == argumentValues[argumentIndex])
		{
			Asteroids::String testHeader = "Testing " + Asteroids::Version::ProjectVersionString();
			return (true == TurtleBrains::Core::UnitTest::RunAllTests(testHeader)) ? 0 : 1;
		}
	}

	// Run --test above this so it can output the results into the nightly build emails.

	const Asteroids::UserSettings launchSettings = Asteroids::ParseLaunchParameters(argumentCount, argumentValues);

#if defined(tb_public_build)
	const bool openDebugConsole = false;
#else
	const bool openDebugConsole = true;
#endif

	const Asteroids::String diagnosticDirectory = Asteroids::GetDiagnosticDirectory();
	const Asteroids::String debugLogFilepath = diagnosticDirectory + launchSettings.GetString("debug_log", "debug_log.txt");
	tbCore::Debug::OpenLog(debugLogFilepath, openDebugConsole);
	Asteroids::SetLoggingLevels();
	tb_always_log(Asteroids::LogGame::Always() << "Starting " << Asteroids::Version::ProjectVersionString());
	tb_always_log(Asteroids::LogGame::Always() << "    " << tbCore::Version::ProjectVersionString());

	{	// TODO: Asteroids: Develoment: 2025-10-16: This would be a great tool to write out the history of play and
		//   track through multiple sessions if necessary.
		tb_always_log(Asteroids::LogState::Always() << Asteroids::DebugInfo::PlayTimeHistory() << "Launching Asteroids!");
	}

	// 2025-11-12: This was added because pressing [X] from the Console will skip the proper shutdown, it kills the
	//   process immediately in a way that TurtleBrains never exits from RunGame() which means Asteroids couldn't
	//   call SaveGame() or settings or gracefully shutdown. This is slightly hacky and should only be used if we have
	//   a console. It simply disables the [X] in probably the worst manner possible.
#if defined(tb_windows) && !defined(tb_public_build)
	HWND console = GetConsoleWindow();
	if (true == openDebugConsole && console)
	{
		DeleteMenu(GetSystemMenu(console, FALSE), SC_CLOSE, MF_BYCOMMAND);
	}
#endif

	int returnCode = tb_debug_project_entry_point_with(Asteroids::Main, argumentCount, argumentValues);

	tb_always_log(Asteroids::LogGame::Always() << "Clean shutdown.");
	tbCore::Debug::CloseLog();
	return returnCode;
}

//--------------------------------------------------------------------------------------------------------------------//

float Asteroids::GameScale(void)
{
	return static_cast<float>(tbGraphics::ScreenHeight()) / static_cast<float>(WorldTargetHeight());
}

//--------------------------------------------------------------------------------------------------------------------//

tbGraphics::PixelSpace Asteroids::WorldTargetWidth(void)
{
	return static_cast<tbGraphics::PixelSpace>(tbGraphics::ScreenWidth() / GameScale());
}

//--------------------------------------------------------------------------------------------------------------------//

tbGraphics::PixelSpace Asteroids::WorldTargetHeight(void)
{
	return 1080;
}

//--------------------------------------------------------------------------------------------------------------------//

float Asteroids::FixedTime(void)
{
	static const float kFixedTime(0.01f);
	return kFixedTime * GameManager::TimeMultiplier();
}

//--------------------------------------------------------------------------------------------------------------------//

Asteroids::String Asteroids::GetSaveDirectory(void)
{
	const String saveDirectory = tbCore::PlatformUtilities::UserDirectoryPath() + "TyreBytes/Asteroids/";
	tbCore::PlatformUtilities::CreateDirectoryPath(tbCore::PlatformUtilities::PathToSystemSlashes(saveDirectory));
	return tbCore::PlatformUtilities::PathToNormalSlashes(saveDirectory);
}

//--------------------------------------------------------------------------------------------------------------------//

Asteroids::String Asteroids::GetDiagnosticDirectory(void)
{
	const String diagnosticDirectory = GetSaveDirectory() + "diagnostics/";
	tbCore::PlatformUtilities::CreateDirectoryPath(tbCore::PlatformUtilities::PathToSystemSlashes(diagnosticDirectory));
	return tbCore::PlatformUtilities::PathToNormalSlashes(diagnosticDirectory);
}

//--------------------------------------------------------------------------------------------------------------------//
