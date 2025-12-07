///
/// @file
/// @details An entity within the Asteroids project that allows the player to make adjustments to their settings.
///
/// <!-- Copyright (c) 2025 Tyre Bytes LLC - All Rights Reserved -->
///------------------------------------------------------------------------------------------------------------------///

#include "../entities/settings_screen_entity.hpp"
#include "../user_settings.hpp"
#include "../interface.hpp"

//#include <turtle_brains/application/implementation/tbi_realtime_system_application.hpp>
namespace tbImplementation //TurtleBrains::Application::Implementation
{
	std::vector<tbApplication::WindowProperties> GetSupportedResolutions(void);
};

namespace Asteroids
{
	extern tbGame::GameApplication* theGameApplication;
};

namespace
{
	const tbGraphics::PixelSpace kElementWidth = 300;
	const tbGraphics::PixelSpace kElementHeight = 50;

	tbApplication::WindowProperties MakeResolution(tbGraphics::PixelSpace width, tbGraphics::PixelSpace height, bool fullscreen)
	{
		tbApplication::WindowProperties window = Asteroids::theGameApplication->GetWindowProperties();
		window.mWindowWidth = width;
		window.mWindowHeight = height;
		window.mWindowMode = (true == fullscreen) ? tbApplication::WindowMode::FullScreen : tbApplication::WindowMode::Windowed;
		return window;
	}

	std::vector<tbApplication::WindowProperties> SupportedResolutions(void)
	{
		// 2025-11-13: TurtleBrains does have an internal/implementation of tbImplementation::GetSupportedResolutions()
		//   which works only on Windows. This was attempted to be used with #if defined(tb_windows) but it actually
		//   fills the options in with TOO MANY OPTIONS. Currently the dropdown box doesn't support a scrollbar so it
		//   will eh, fall of the screen... Hence we will run with "just guessing" some common resolutions and let the
		//   players choose what works.
		const std::vector<tbApplication::WindowProperties> maybeSupported = {
			MakeResolution(1024, 768, false),
			MakeResolution(1280, 720, false),
			MakeResolution(1280, 1024, false),
			MakeResolution(1440, 900, false),
			MakeResolution(1680, 1050, false),
			MakeResolution(1920, 1080, false),
			MakeResolution(3840, 2160, false),
		};

		return maybeSupported;
	}

	std::vector<Asteroids::String> SupportedResolutionNames(void)
	{
		const auto& resolutions = SupportedResolutions();

		std::vector<Asteroids::String> names;
		names.reserve(resolutions.size());

		for (const tbApplication::WindowProperties& windowProperties : resolutions)
		{
			names.push_back(Asteroids::ToString(windowProperties.mWindowWidth) + Asteroids::String("x") +
				Asteroids::ToString(windowProperties.mWindowHeight));
		}

		return names;
	}

	tbApplication::WindowProperties GetSupportedResolution(const size_t index)
	{
		const auto& resolutions = SupportedResolutions();
		tb_error_if(index >= resolutions.size(), "Error: Index for supported resolution is out of rang.");
		return resolutions[index];
	}
};

//--------------------------------------------------------------------------------------------------------------------//

Asteroids::SettingsScreenEntity::SettingsScreenEntity(void) :
	tbGame::Entity("SettingsScreenEntity"),
	mSettingsBox(),
	mInterfaceController(),
	//mInterface(20, tbxInterface::Unstable::Point(960 - 80, 250)),
	mInterface(20, tbxInterface::Unstable::Point(0, 250)),
	mVolumeMusicSlider(kElementWidth, kElementHeight),
	mVolumeEffectSlider(kElementWidth, kElementHeight),
	mFullscreenCheckbox(NotLocalized("Fullscreen"), kElementWidth, kElementHeight),
	mResolutionDropdown(SupportedResolutionNames(), 0, kElementWidth, kElementHeight),
	mApplyButton(NotLocalized("Apply"), kElementWidth, kElementHeight)
{
	mSettingsBox.SetDarkenBackdrop(true);
	AddGraphic(mSettingsBox);

	mInterface.AddElement(mResolutionDropdown);
	mInterface.AddElement(mFullscreenCheckbox);
	mInterface.AddElement(mVolumeMusicSlider);
	mInterface.AddElement(mVolumeEffectSlider);
	mInterface.AddElement(mApplyButton);

	const float initialMusic = TheUserSettings().GetFloat(Settings::MusicVolume());
	mVolumeMusicSlider.SetValue(std::sqrt(initialMusic));
	mVolumeMusicSlider.SetFunction([this]() {
		const float musicVolume = mVolumeMusicSlider.GetValue() * mVolumeMusicSlider.GetValue();
		TheUserSettings().SetFloat(Settings::MusicVolume(), musicVolume);
		tbAudio::theAudioManager.SetGlobalMusicVolume(musicVolume);
	});

	const float initialEffect = TheUserSettings().GetFloat(Settings::SoundVolume());
	mVolumeEffectSlider.SetValue(std::sqrt(initialEffect));
	mVolumeEffectSlider.SetFunction([this]() {
		const float effectVolume = mVolumeEffectSlider.GetValue() * mVolumeEffectSlider.GetValue();
		TheUserSettings().SetFloat(Settings::SoundVolume(), effectVolume);
		tbAudio::theAudioManager.SetGlobalEffectVolume(effectVolume);
	});

	mResolutionDropdown.SetFunction([this]() {
		mApplyButton.SetState(tbxInterface::Unstable::Element::State::Enabled);
	});

	mFullscreenCheckbox.SetChecked(tbApplication::WindowMode::FullScreen == theGameApplication->GetWindowProperties().mWindowMode);
	mFullscreenCheckbox.SetFunction([this]() {
		mApplyButton.SetState(tbxInterface::Unstable::Element::State::Enabled);
	});

	mApplyButton.SetState(tbxInterface::Unstable::Element::State::Disabled);

	mApplyButton.SetFunction([this]() {
		const tbApplication::WindowProperties currentWindow = theGameApplication->GetWindowProperties();
		TheUserSettings().SetBoolean(Settings::Fullscreen(), mFullscreenCheckbox.IsChecked());

		tbApplication::WindowProperties otherWindow = GetSupportedResolution(mResolutionDropdown.GetSelectedIndex());
		otherWindow.mWindowMode = (true == mFullscreenCheckbox.IsChecked()) ? tbApplication::WindowMode::FullScreen :
			tbApplication::WindowMode::Windowed;

		if (currentWindow.mWindowWidth != otherWindow.mWindowWidth ||
			currentWindow.mWindowHeight != otherWindow.mWindowHeight ||
			currentWindow.mWindowMode != otherWindow.mWindowMode)
		{
			theGameApplication->SetWindowProperties(otherWindow);
		}
	});
}

//--------------------------------------------------------------------------------------------------------------------//

Asteroids::SettingsScreenEntity::~SettingsScreenEntity(void)
{
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::SettingsScreenEntity::OnAdd(void)
{
	tbGame::Entity::OnAdd();

	mInterface.SetState(tbxInterface::Unstable::Element::State::Focused);
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::SettingsScreenEntity::OnRemove(void)
{
	tbGame::Entity::OnRemove();
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::SettingsScreenEntity::OnSimulate(void)
{
	tbGame::Entity::OnSimulate();
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::SettingsScreenEntity::OnUpdate(const float deltaTime)
{
	tbGame::Entity::OnUpdate(deltaTime);

	if (true == tbGame::Input::IsKeyReleased(tbApplication::tbKeyEscape))
	{
		GetEntityManager()->RemoveEntity(this);
	}
	mResolutionDropdown.SetPosition(static_cast<tbGraphics::PixelSpace>(
		Interface::GetAnchorPositionOfInterface(Anchor::Center).x) - (mResolutionDropdown.GetPixelWidth() / 2),
		static_cast<tbGraphics::PixelSpace>(mResolutionDropdown.GetPosition().y));
	mFullscreenCheckbox.SetPosition(static_cast<tbGraphics::PixelSpace>(
		Interface::GetAnchorPositionOfInterface(Anchor::Center).x) - (mFullscreenCheckbox.GetPixelWidth() / 2),
		static_cast<tbGraphics::PixelSpace>(mFullscreenCheckbox.GetPosition().y));
	mVolumeMusicSlider.SetPosition(static_cast<tbGraphics::PixelSpace>(
		Interface::GetAnchorPositionOfInterface(Anchor::Center).x) - (mVolumeMusicSlider.GetPixelWidth() / 2),
		static_cast<tbGraphics::PixelSpace>(mVolumeMusicSlider.GetPosition().y));
	mVolumeEffectSlider.SetPosition(static_cast<tbGraphics::PixelSpace>(
		Interface::GetAnchorPositionOfInterface(Anchor::Center).x) - (mVolumeEffectSlider.GetPixelWidth() / 2),
		static_cast<tbGraphics::PixelSpace>(mVolumeEffectSlider.GetPosition().y));
	mApplyButton.SetPosition(static_cast<tbGraphics::PixelSpace>(
		Interface::GetAnchorPositionOfInterface(Anchor::Center).x) - (mApplyButton.GetPixelWidth() / 2),
		static_cast<tbGraphics::PixelSpace>(mApplyButton.GetPosition().y));


	//const tbxInterface::Unstable::Point interfaceOffset(
	//	static_cast<tbCore::int32>(Interface::GetAnchorPositionOfInterface(Anchor::Center).x),
	//	static_cast<tbCore::int32>(Interface::GetAnchorPositionOfInterface(Anchor::Center).y));

	const tbxInterface::Unstable::Point interfaceOffset(
		static_cast<tbCore::int32>(Interface::GetAnchorPositionOfInterface(Anchor::TopLeft).x),
		static_cast<tbCore::int32>(Interface::GetAnchorPositionOfInterface(Anchor::TopLeft).y));

	mInterfaceController.SetInterfaceOffset(interfaceOffset);
	mInterfaceController.SetInterfaceScale(1.0f / Interface::Scale());

	mInterface.Update(mInterfaceController);
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::SettingsScreenEntity::OnRender(void) const
{
	tbGame::Entity::OnRender();
	mInterface.Render();
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::SettingsScreenEntity::OnCollide(const tbGame::Entity& otherEntity)
{
	tbGame::Entity::OnCollide(otherEntity);
}

//--------------------------------------------------------------------------------------------------------------------//
