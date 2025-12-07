///
/// @file
/// @details Manages all the commands the designer can use to manipulate the tech tree so we can have a undo/redo system.
///
/// <!-- Copyright (c) 2025 Tyre Bytes LLC - All Rights Reserved -->
///------------------------------------------------------------------------------------------------------------------///

#ifndef Asteroids_CommandManager_hpp
#define Asteroids_CommandManager_hpp

#include "../../asteroids.hpp"

#if defined(rusty_development)

#include <turtle_brains/core/tb_string.hpp>
#include <turtle_brains/core/tb_noncopyable.hpp>

#include <stack>
#include <list>
#include <memory>

namespace Asteroids::Development::Editor
{
	namespace Implementation { class CommandManagerCreator; }

	class BatchedCommand;

	class CommandInterface
	{
	public:
		explicit CommandInterface(const String& commandName);
		virtual ~CommandInterface(void) = 0;

		inline const String& GetCommandName(void) { return mCommandName; }

	protected:
		virtual void OnPerform(void) = 0;
		virtual void OnRevert(void) = 0;

	private:
		friend class CommandManager;
		friend class BatchedCommand;

		const String mCommandName;
	};

	typedef std::unique_ptr<CommandInterface> CommandInterfacePtr;
	typedef std::list<CommandInterfacePtr> CommandSequence;

	class CommandManager : public tbCore::Noncopyable
	{
	public:
		~CommandManager(void);

		///
		/// @details Removes all the history front and back. Use when starting new file, or opening existing.
		///
		void ClearHistory(void);

		///
		/// @details Performs a command for the first time. If there was any buffer/command history waiting to be
		///   'redone' than it will get destroyed with this command being performed.
		///
		template<typename Type, class... _Types> void PerformCommand(_Types&&... arguments)
		{
			if (false  == mCommandSequences.empty())
			{
				mCommandSequences.back().emplace_back(new Type(std::forward<_Types>(arguments)...));
			}
			else
			{
				ReallyPerformCommand(CommandInterfacePtr(new Type(std::forward<_Types>(arguments)...)));
			}
		}

		///
		/// @details If available the next command in history will be 'redone' and the history remains unchanged.
		///
		void Redo(void);

		///
		/// @details If available the previous command in history will be 'undone' and the history remains unchanged.
		///
		void Undo(void);

		///
		/// @details Any commands performed after Begin will be batched into a single undo/redo command.
		///
		void BeginCommandBatch();

		///
		///
		///
		void EndCommandBatch();

	protected:

	private:
		void ReallyPerformCommand(CommandInterfacePtr command);

		std::stack<CommandInterfacePtr> mUndoHistory;
		std::stack<CommandInterfacePtr> mRedoHistory;
		std::list<CommandSequence> mCommandSequences;

		friend class Implementation::CommandManagerCreator;

		CommandManager(void);
	};

	extern CommandManager& theCommandManager;

};	//namespace Asteroids::Development::Editor

#endif /* rusty_development */
#endif /* Asteroids_CommandManager_hpp */
