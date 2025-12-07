///
/// @file
/// @details Manages all the commands the designer can use to manipulate the tech tree so we can have a undo/redo system.
///
/// <!-- Copyright (c) 2025 Tyre Bytes LLC - All Rights Reserved -->
///------------------------------------------------------------------------------------------------------------------///

#include "../../development/editor/command_manager.hpp"
#include "../../development/editor/batched_command.hpp"

#if defined(rusty_development)

//#include "../scenes/track_editor_scene.hpp"

#include <memory>

namespace Asteroids::Development::Editor::Implementation
{
	class CommandManagerCreator
	{
	public:
		CommandManagerCreator(void) :
			mCommandManager(new CommandManager())
		{
		}

		~CommandManagerCreator(void)
		{
		}

		std::unique_ptr<CommandManager> mCommandManager;
	};

	CommandManagerCreator theCommandManagerCreator;
};

Asteroids::Development::Editor::CommandManager& Asteroids::Development::Editor::theCommandManager(*
	Asteroids::Development::Editor::Implementation::theCommandManagerCreator.mCommandManager);

//--------------------------------------------------------------------------------------------------------------------//

Asteroids::Development::Editor::CommandInterface::CommandInterface(const String& commandName) :
	mCommandName(commandName)
{
}

//--------------------------------------------------------------------------------------------------------------------//

Asteroids::Development::Editor::CommandInterface::~CommandInterface(void)
{
}

//--------------------------------------------------------------------------------------------------------------------//
//--------------------------------------------------------------------------------------------------------------------//
//--------------------------------------------------------------------------------------------------------------------//

Asteroids::Development::Editor::CommandManager::CommandManager(void) :
	mUndoHistory(),
	mRedoHistory(),
	mCommandSequences()
{
}

//--------------------------------------------------------------------------------------------------------------------//

Asteroids::Development::Editor::CommandManager::~CommandManager(void)
{
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::Development::Editor::CommandManager::ClearHistory(void)
{
	while (false == mRedoHistory.empty())
	{
		mRedoHistory.pop();
	}

	while (false == mUndoHistory.empty())
	{
		mUndoHistory.pop();
	}
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::Development::Editor::CommandManager::ReallyPerformCommand(CommandInterfacePtr command)
{
	while (false == mRedoHistory.empty())
	{
		mRedoHistory.pop();
	}

	//TrackEditorScene::MakeModification();
	mUndoHistory.emplace(std::move(command));
	mUndoHistory.top()->OnPerform();
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::Development::Editor::CommandManager::Redo(void)
{
	if (false == mRedoHistory.empty())
	{
		//TrackEditorScene::MakeModification();
		mRedoHistory.top()->OnPerform();
		mUndoHistory.emplace(std::move(mRedoHistory.top()));
		mRedoHistory.pop();
	}
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::Development::Editor::CommandManager::Undo(void)
{
	if (false == mUndoHistory.empty())
	{
		//TrackEditorScene::MakeModification();
		mUndoHistory.top()->OnRevert();
		mRedoHistory.emplace(std::move(mUndoHistory.top()));
		mUndoHistory.pop();
	}
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::Development::Editor::CommandManager::BeginCommandBatch(void)
{
	mCommandSequences.push_back(CommandSequence());
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::Development::Editor::CommandManager::EndCommandBatch(void)
{
	tb_error_if(true == mCommandSequences.empty(), "Error the CommandManager does not have a CommandBatch to end.");
	CommandSequence sequence(std::move(mCommandSequences.back()));
	mCommandSequences.pop_back();

	PerformCommand<BatchedCommand>(sequence);
}

//--------------------------------------------------------------------------------------------------------------------//

#endif /* rusty_development */
