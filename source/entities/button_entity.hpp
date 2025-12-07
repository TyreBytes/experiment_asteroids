///
/// @file
/// @details A button to be placed wherever for the Asteroids project.
///
/// <!-- Copyright (c) 2025 Tyre Bytes LLC - All Rights Reserved -->
///------------------------------------------------------------------------------------------------------------------///

#ifndef Asteroids_ButtonEntity_hpp
#define Asteroids_ButtonEntity_hpp

#include "../asteroids.hpp"

#include <functional>

namespace Asteroids
{
	enum ButtonType {
		Primary,
		Secondary,
		SettingsButton,
		Iconic, //Would be the technology tree, square icon buttons.
	};

	class ButtonEntity;
	using ButtonPtr = std::unique_ptr<ButtonEntity>;

	namespace ButtonFactory
	{
		void SetupButton(ButtonEntity& button, const ButtonType type, const String& labelIconSprite);

		ButtonPtr CreateButton(ButtonType type, const String& labelIconSprite);
	};


	class ButtonEntity : public tbGame::Entity
	{
	public:
		ButtonEntity(void);
		virtual ~ButtonEntity(void);

		void SetEnabled(bool enabled) { mIsEnabled = enabled; }
		bool IsDisabled(void) const;

		void SetClickCallback(std::function<void()> onClick) { mOnClick = onClick; }
		void SetEnabler(std::function<bool()> enabler) { mEnabler = enabler; }

		void SetAvailable(const bool isAvailable);

#if defined(rusty_development)
		void Development_ForceClickCallback(void);
#endif

	protected:
		virtual void OnAdd(void) override;
		virtual void OnRemove(void) override;
		virtual void OnSimulate(void) override;
		virtual void OnUpdate(const float deltaTime) override;
		virtual void OnRender(void) const override;
		virtual void OnCollide(const tbGame::Entity& otherEntity) override;

	private:
		friend void ButtonFactory::SetupButton(ButtonEntity& button, const ButtonType type, const String& labelIconSprite);

		enum State { Hovered, Enabled, Disabled, NumberOfStates };
		std::array<tbGraphics::GraphicList, NumberOfStates> mVisuals;
		std::vector<tbGraphics::Sprite*> mHoverButtonSprites;

		String mPressedAudioEvent;
		std::function<void()> mOnClick;
		std::function<bool()> mEnabler;
		bool mIsEnabled;
		bool mIsHovered;

		float mRunningTimer;
	};

};	//namespace Asteroids

#endif /* Asteroids_ButtonEntity_hpp */
