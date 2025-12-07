///
/// @file
/// @details This command will make any type of modification to the tech tree, and allow it to be undone. Trade off is
///   keeping the old tech tree in memory, so it might be a little memory intesive; it's probably fine.
///
/// <!-- Copyright (c) 2025 Tyre Bytes LLC - All Rights Reserved -->
///------------------------------------------------------------------------------------------------------------------///

#ifndef Asteroids_ModifyTechTreeCommand_hpp
#define Asteroids_ModifyTechTreeCommand_hpp

#include "../../development/editor/command_manager.hpp"

#if defined(rusty_development)

#include "../../scenes/development/tech_tree_editor_scene.hpp"

namespace Asteroids::Development::Editor
{

	class ModifyTechTreeCommand : public CommandInterface
	{
	public:
		inline ModifyTechTreeCommand(TechTreeEditorScene& editor, const TechTreeEditorScene::UpgradeTable& upgradeTable,
			const UpgradeKey& selectedUpgrade, const UpgradeKey& hoveredUpgrade) :
			CommandInterface("ModifyTechTreeCommand"),
			mEditor(editor),
			mOriginalUpgrades(editor.mAllUpgrades),
			mModifiedUpgrades(upgradeTable),
			mOriginalSelectedUpgrade(editor.mSelectedUpgrade),
			mModifiedSelectedUpgrade(selectedUpgrade),
			mOriginalHoveredUpgrade(editor.mHoveredUpgrade),
			mModifiedHoveredUpgrade(hoveredUpgrade)
		{
		}

		virtual ~ModifyTechTreeCommand(void)
		{
		}

	protected:
		inline virtual void OnPerform(void) override
		{
			mEditor.mAllUpgrades = mModifiedUpgrades;
			mEditor.mSelectedUpgrade = mModifiedSelectedUpgrade;
			mEditor.mHoveredUpgrade = mModifiedHoveredUpgrade;
		}

		inline virtual void OnRevert(void) override
		{
			mEditor.mAllUpgrades = mOriginalUpgrades;
			mEditor.mSelectedUpgrade = mOriginalSelectedUpgrade;
			mEditor.mHoveredUpgrade = mOriginalHoveredUpgrade;
		}

	private:
		TechTreeEditorScene& mEditor;

		const TechTreeEditorScene::UpgradeTable mOriginalUpgrades;
		const TechTreeEditorScene::UpgradeTable mModifiedUpgrades;
		const UpgradeKey mOriginalSelectedUpgrade;
		const UpgradeKey mModifiedSelectedUpgrade;
		const UpgradeKey mOriginalHoveredUpgrade;
		const UpgradeKey mModifiedHoveredUpgrade;
	};

};	//namespace Asteroids::Development::Editor

#endif /* rusty_development */
#endif /* Asteroids_ModifyTechTreeCommand_hpp */
