///
/// @file
/// @details An entity within the Asteroids project that allows the player to make adjustments to their settings.
///
/// <!-- Copyright (c) 2025 Tyre Bytes LLC - All Rights Reserved -->
///------------------------------------------------------------------------------------------------------------------///

#ifndef Asteroids_SettingsScreenEntity_hpp
#define Asteroids_SettingsScreenEntity_hpp

#include "../asteroids.hpp"
#include "../graphics/event_box.hpp"
#include "../interface/dropdown.hpp"

#include <turtle_brains/express/interface/tbx_interface_controller.hpp>
#include <turtle_brains/express/interface/tbx_interface_screen.hpp>
#include <turtle_brains/express/interface/tbx_interface_dropdown.hpp>
#include <turtle_brains/express/interface/tbx_interface_slider.hpp>
#include <turtle_brains/express/interface/tbx_interface_button.hpp>
#include <turtle_brains/express/interface/tbx_interface_checkbox.hpp>

namespace Asteroids
{

	class SettingsScreenEntity : public tbGame::Entity
	{
	public:
		SettingsScreenEntity(void);
		virtual ~SettingsScreenEntity(void);

	protected:
		virtual void OnAdd(void) override;
		virtual void OnRemove(void) override;
		virtual void OnSimulate(void) override;
		virtual void OnUpdate(const float deltaTime) override;
		virtual void OnRender(void) const override;
		virtual void OnCollide(const tbGame::Entity& otherEntity) override;

	private:
		EventBox mSettingsBox;

		tbxInterface::Unstable::Controller mInterfaceController;

		tbxInterface::Unstable::SingleColumnScreen mInterface;
		tbxInterface::Unstable::SimpleSlider mVolumeMusicSlider;
		tbxInterface::Unstable::SimpleSlider mVolumeEffectSlider;
		tbxInterface::Unstable::SimpleCheckbox mFullscreenCheckbox;
		Interface::SimpleDropdown mResolutionDropdown;
		tbxInterface::Unstable::SimpleButton mApplyButton;
	};

};	//namespace Rushcremental

#endif Asteroidstal_SettingsScreenEntity_hpp */
