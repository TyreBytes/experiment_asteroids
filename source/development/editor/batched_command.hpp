///
/// @file
/// @details Takes a sequence of cammands and batches it into one undo/redo call.
///
/// <!-- Copyright (c) 2025 Tyre Bytes LLC - All Rights Reserved -->
///------------------------------------------------------------------------------------------------------------------///

#ifndef Asteroids_BatchedCommand_hpp
#define Asteroids_BatchedCommand_hpp

#include "../../development/editor/command_manager.hpp"

#if defined(rusty_development)

#include <list>

namespace Asteroids::Development::Editor
{

	class BatchedCommand : public CommandInterface
	{
	public:
		BatchedCommand(CommandSequence& commandSequence) :
			CommandInterface("BatchedCommand"),
			mCommandSequence(std::move(commandSequence))
		{
		}

		virtual ~BatchedCommand(void)
		{
		}

	protected:
		virtual void OnPerform(void) override
		{
			for (CommandSequence::iterator commandIterator = mCommandSequence.begin(); commandIterator != mCommandSequence.end(); ++commandIterator)
			{
				(*commandIterator)->OnPerform();
			}
		}

		virtual void OnRevert(void) override
		{
			for (CommandSequence::reverse_iterator commandIterator = mCommandSequence.rbegin(); commandIterator != mCommandSequence.rend(); ++commandIterator)
			{
				(*commandIterator)->OnRevert();
			}
		}

	private:
		CommandSequence mCommandSequence;
	};

};	//namespace Asteroids::Development::Editor

#endif /* rusty_development */
#endif /* Asteroids_BatchedCommand_hpp */
