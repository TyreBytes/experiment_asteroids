///
/// @file
/// @details Some quick commands for the developer console so the dev can give extra resources, see the unlocked keys or
///   unlock a key, etc for make progression and testing much faster.
///
/// <!-- Copyright (c) 2025 Tyre Bytes LLC - All Rights Reserved -->
///------------------------------------------------------------------------------------------------------------------///

#include "../development/developer_console_commands.hpp"

#if defined(rusty_development)

#include "../game_manager.hpp"
#include "../scenes/scene_manager.hpp"

#include <turtle_brains/core/diagnostics/tb_console_command_system.hpp>

namespace Asteroids::Development
{	// 2025-10-16: Lives in development/development.cpp
	extern bool theRunTimerIsDisabled;
};

//--------------------------------------------------------------------------------------------------------------------//

// 2025-10-14: This absolutely needs a public API from tbCore::Diagnostics... But it wasn't so here we are for now.
namespace TurtleBrains::Core::Diagnostics::Implementation
{
	void CommandLog(const TurtleBrains::String& message);
	void CommandLog(const char* format, ...);
};

//--------------------------------------------------------------------------------------------------------------------//

namespace Asteroids::Development
{
	using TurtleBrains::Core::Diagnostics::Implementation::CommandLog;

	class GiveResourceCommand : public tbCore::Diagnostics::CommandDefinition
	{
	public:
		GiveResourceCommand(void) :
			CommandDefinition("give", "Give yourself an amount of some resource.")
		{
			AddSynopsis("<AMOUNT> <RESOURCE_NAME>");
		}

		virtual ~GiveResourceCommand(void)
		{
		}

		virtual void OnRunCommand(tbCore::Diagnostics::Command& command) override
		{
			if (2 != command.GetParameterCount())
			{
				CommandLog("Expected two parameters give AMOUNT RESOURCE_TYPE");
			}
			else
			{
				const StatType resourceAmount = FromString<StatType>(command.GetParameter(0));
				const String resourceType = command.GetParameter(1);

				if (false == GameManager::GetActiveStats().Contains(resourceType))
				{
					CommandLog("The provided resource type '%s' is unknown by GameStats.", resourceType.c_str());
				}
				else if (resourceAmount < 1.0)
				{
					CommandLog("give command expects to give at least 1 %s.", resourceType.c_str());
				}
				else
				{
					const StatType previousAmount = GameManager::GetActiveStats()[resourceType];
					GameManager::GetMutableActiveStats()[resourceType] += resourceAmount;
					const StatType currentAmount = GameManager::GetActiveStats()[resourceType];

					CommandLog("You had %d %s and now you have %d %s.",
						static_cast<int>(previousAmount), resourceType.c_str(),
						static_cast<int>(currentAmount), resourceType.c_str());

					if (Stat::Experience == FromString<Stat>(resourceType))
					{
						const int levelsGained = GameManager::CheckForLevelUp();
						CommandLog("   This gained you %d level too.", levelsGained);
					}
				}
			}
		}
	};

//--------------------------------------------------------------------------------------------------------------------//
//--------------------------------------------------------------------------------------------------------------------//
//--------------------------------------------------------------------------------------------------------------------//

	class GetResourceCommand : public tbCore::Diagnostics::CommandDefinition
	{
	public:
		GetResourceCommand(void) :
			CommandDefinition("get", "See the amount of some resource.")
		{
			AddSynopsis("<RESOURCE_NAME>");
		}

		virtual ~GetResourceCommand(void)
		{
		}

		virtual void OnRunCommand(tbCore::Diagnostics::Command& command) override
		{
			if (1 != command.GetParameterCount())
			{
				CommandLog("Expected two parameters get RESOURCE_TYPE");
			}
			else
			{
				const String resourceType = command.GetParameter(0);

				if (false == GameManager::GetActiveStats().Contains(resourceType))
				{
					CommandLog("The provided resource type '%s' is unknown by GameStats.", resourceType.c_str());
				}
				else
				{
					const StatType currentAmount = GameManager::GetActiveStats()[resourceType];
					CommandLog("You have %d %s.", static_cast<int>(currentAmount), resourceType.c_str());
				}
			}
		}
	};

//--------------------------------------------------------------------------------------------------------------------//
//--------------------------------------------------------------------------------------------------------------------//
//--------------------------------------------------------------------------------------------------------------------//

	class UnlocksCommand : public tbCore::Diagnostics::CommandDefinition
	{
	public:
		UnlocksCommand(void) :
			CommandDefinition("unlocks", "See the keys the player currently has unlocked.")
		{
			AddSynopsis("");
		}

		virtual ~UnlocksCommand(void)
		{
		}

		virtual void OnRunCommand(tbCore::Diagnostics::Command& /*command*/) override
		{
			//String partialMatch = "";
			//if (command.GetParameterCount() > 0)
			//{
			//	part
			//}

			CommandLog("The following keys are unlocked:");

			for (const std::string_view& key : UnlockKeys::theAllowedKeys)
			{
				if (true == GameManager::IsUnlocked(key))
				{
					CommandLog("    %s", key.data());
				}
			}
		}
	};

	class UnlockCommand : public tbCore::Diagnostics::CommandDefinition
	{
	public:
		UnlockCommand(void) :
			CommandDefinition("unlock", "Unlock a key for the player.")
		{
			AddSynopsis("");
			AddSynopsis("<KEY>");

			AddOption("all", { "Unlock ALL the keys." });
		}

		virtual ~UnlockCommand(void)
		{
		}

		virtual void OnRunCommand(tbCore::Diagnostics::Command& command) override
		{
			if (command.HasOption("all"))
			{
				for (const StringView& keyToUnlock : UnlockKeys::theAllowedKeys)
				{
					GameManager::UnlockKey(keyToUnlock);
				}

				CommandLog("All keys contained in UnlockKeys are unlocked.");
				CommandLog("This does NOT unlock every key like the challengers\nor tech tree upgrades because they are unknown.");
			}
			else if (command.GetParameterCount() < 1)
			{
				CommandLog("Expected at least one parameter; unlock <KEY>");
			}
			else
			{
				for (size_t parameterIndex = 0; parameterIndex < command.GetParameterCount(); ++parameterIndex)
				{
					const String keyToUnlock = command.GetParameter(parameterIndex);
					if (false == UnlockKeys::VerifyKeyExists(keyToUnlock))
					{
						CommandLog("The provided key '%s' is an unknown UnlockKey.", keyToUnlock.c_str());
					}
					else
					{
						CommandLog("The key '%s' is now unlocked.", keyToUnlock.c_str());
						GameManager::UnlockKey(keyToUnlock);
					}
				}
			}
		}
	};

	//class OpenEditorCommand : public tbCore::Diagnostics::CommandDefinition
	//{
	//public:
	//	OpenEditorCommand(void) :
	//		CommandDefinition("editor", "Open the editor for the tech tree.")
	//	{
	//		AddSynopsis("");
	//	}

	//	virtual ~OpenEditorCommand(void)
	//	{
	//	}

	//	virtual void OnRunCommand(tbCore::Diagnostics::Command& /*command*/) override
	//	{
	//		theSceneManager->ChangeToScene(SceneId::kTechTreeEditor);
	//	}
	//};

	//class ResetSavesEditorCommand : public tbCore::Diagnostics::CommandDefinition
	//{
	//public:
	//	ResetSavesEditorCommand(void) :
	//		CommandDefinition("reset", "Resets the game and save states for the active profile to a clear fresh state.")
	//	{
	//		AddSynopsis("");
	//		AddSynopsis("--all");

	//		AddOption("all", { "Resets all the game states, not just the active profile." });
	//	}

	//	virtual ~ResetSavesEditorCommand(void)
	//	{
	//	}

	//	virtual void OnRunCommand(tbCore::Diagnostics::Command& command) override
	//	{
	//		SceneId scene = SceneId::kTitleScene;

	//		if (true == command.HasOption("all"))
	//		{
	//			tb_always_log(LogState::Always() << DebugInfo::PlayTimeHistory() << "Developer ResetGame for ALL profiles.");
	//			for (size_t profileIndex = 0; profileIndex < GameManager::GetNumberOfProfiles(); ++profileIndex)
	//			{
	//				GameManager::ResetProfile(profileIndex);
	//			}
	//		}
	//		else
	//		{
	//			tb_always_log(LogState::Always() << DebugInfo::PlayTimeHistory() << "Developer ResetGame for active profile.");
	//			GameManager::ResetProfile();
	//			scene = SceneId::kSupplyRunScene;
	//		}

	//		GameManager::SaveGame();
	//		GameManager::LoadGame();
	//		theSceneManager->ChangeToScene(scene);
	//	}
	//};

	class RunTimerCommand : public tbCore::Diagnostics::CommandDefinition
	{
	public:
		RunTimerCommand(void) :
			CommandDefinition("timer", "Enable or disable the run timer.")
		{
			AddSynopsis("");
			AddSynopsis("on");
			AddSynopsis("off");
		}

		virtual ~RunTimerCommand(void)
		{
		}

		virtual void OnRunCommand(tbCore::Diagnostics::Command& command) override
		{
			if (1 == command.GetParameterCount())
			{
				if ("on" == command.GetParameter(0))
				{
					theRunTimerIsDisabled = false;
				}
				else
				{
					theRunTimerIsDisabled = true;
				}
			}
			else
			{
				theRunTimerIsDisabled = !theRunTimerIsDisabled;
			}

			CommandLog("The RunTimer is now %s.", (true == theRunTimerIsDisabled) ? "DISABLED" : "ENABLED");
		}
	};

};	// namespace Asteroids


void Asteroids::Development::RegisterDeveloperCommands(void)
{
	static GiveResourceCommand theGiveResourceCommand;
	static GetResourceCommand theGetResourceCommand;
	static UnlocksCommand theUnlocksCommand;
	static UnlockCommand theUnlockCommand;
//	static OpenEditorCommand theOpenEditorCommand;
	static RunTimerCommand theRunTimerCommand;
//	static ResetSavesEditorCommand theResetSavesEditorCommand;
}

#endif /* rusty_development */

//--------------------------------------------------------------------------------------------------------------------//
