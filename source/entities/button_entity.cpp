///
/// @file
/// @details A button to be placed wherever for the Asteroids project.
///
/// <!-- Copyright (c) 2025 Tyre Bytes LLC - All Rights Reserved -->
///------------------------------------------------------------------------------------------------------------------///

#include "../entities/button_entity.hpp"
#include "../graphics/rounded_box_shape.hpp"
#include "../interface.hpp"
#include "../shader_system/shaders.hpp"

namespace
{
	const float kButtonPadding = 80.0f;
};

//--------------------------------------------------------------------------------------------------------------------//
//--------------------------------------------------------------------------------------------------------------------//
//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::ButtonFactory::SetupButton(ButtonEntity& button, const ButtonType type, const String& labelIconSprite)
{
	tb_unused(labelIconSprite);

	Vector2 size(64.0f, 64.0f);

	switch (type)
	{
	case ButtonType::Primary: {
		button.mPressedAudioEvent = "primary_button_pressed";
		size = Vector2(300.0f, 120.0f);
		break; }
	case ButtonType::Secondary: {
		button.mPressedAudioEvent = "secondary_button_pressed";
		size = Vector2(225.0f, 90.0f);
		break; }

	case ButtonType::SettingsButton:
	case ButtonType::Iconic: {
		break; }
	};

	for (int index = 0; index < ButtonEntity::State::NumberOfStates; ++index)
	{
		tbGraphics::GraphicList& visuals = button.mVisuals[index];

		switch (type)
		{
		case ButtonType::Primary: {
			Asteroids::RoundedBoxShape* roundedBox = new Asteroids::RoundedBoxShape(size - Vector2(16.0f, 16.0f), size / 2.0f,
				(ButtonEntity::State::Hovered == index) ? AsteroidsColor::PrimaryColorHalf : AsteroidsColor::PrimaryColor, kRoundedCornerRadius);
			roundedBox->SetOrigin(Anchor::Center);
			visuals.AddGraphic(roundedBox);

			tbGraphics::Text* text = new tbGraphics::Text();
			Interface::MakeScalableText(text, labelIconSprite);
			text->SetColor(AsteroidsColor::PrimaryTextColor);
			text->SetOrigin(Anchor::Center);
			text->SetScale((size.x - kButtonPadding) / text->GetWidth());
			text->SetPosition(size / 2.0f);
			visuals.AddGraphic(text);

			break; }
		case ButtonType::Secondary: {
			Asteroids::RoundedBoxShape* roundedBox = new Asteroids::RoundedBoxShape(size - Vector2(16.0f, 16.0f), size / 2.0f,
				(ButtonEntity::State::Hovered == index) ? AsteroidsColor::SecondaryColorHalf : AsteroidsColor::SecondaryColor, kRoundedCornerRadius);
			roundedBox->SetOrigin(Anchor::Center);
			visuals.AddGraphic(roundedBox);

			tbGraphics::Text* text = new tbGraphics::Text();
			Interface::MakeScalableText(text, labelIconSprite);
			text->SetOrigin(Anchor::Center);
			text->SetColor(AsteroidsColor::SecondaryTextColor);
			text->SetPosition(size / 2.0f);
			text->SetScale((size.x - kButtonPadding) / text->GetWidth());
			visuals.AddGraphic(text);

			break;}

		case ButtonType::SettingsButton: {
			//tbGraphics::Sprite* settingsCog = new tbGraphics::Sprite("data/interface/icons/settings_cog.png");
			//settingsCog->SetOrigin(Anchor::Center);
			//settingsCog->SetPosition(size / 2.0f);
			//settingsCog->SetColor(AsteroidsColor::SecondaryColor);
			//visuals.AddGraphic(settingsCog);

			tbGraphics::CircleShape* settingsCog = new tbGraphics::CircleShape(24.0f, AsteroidsColor::SecondaryColor);
			settingsCog->SetOrigin(Anchor::Center);
			settingsCog->SetPosition(size / 2.0f);
			settingsCog->SetColor(AsteroidsColor::SecondaryColor);
			visuals.AddGraphic(settingsCog);

			break; }

		case ButtonType::Iconic: {
			break; }
		};

		visuals.RecalculateWidthAndHeight();
	}

	button.RecalculateWidthAndHeight();
}

//--------------------------------------------------------------------------------------------------------------------//

Asteroids::ButtonPtr Asteroids::ButtonFactory::CreateButton(ButtonType type, const String& labelIconSprite)
{
	ButtonPtr button = std::make_unique<ButtonEntity>();
	SetupButton(*button, type, labelIconSprite);
	return button;
}

//--------------------------------------------------------------------------------------------------------------------//
//--------------------------------------------------------------------------------------------------------------------//
//--------------------------------------------------------------------------------------------------------------------//

Asteroids::ButtonEntity::ButtonEntity(void) :
	tbGame::Entity("ButtonEntity"),
	mPressedAudioEvent(""),
	mOnClick(nullptr),
	mEnabler(nullptr),
	mIsEnabled(true),
	mIsHovered(false),
	mRunningTimer(0.0f)
{
	AddGraphic(mVisuals[State::Disabled]);
	AddGraphic(mVisuals[State::Enabled]);
	AddGraphic(mVisuals[State::Hovered]);
}

//--------------------------------------------------------------------------------------------------------------------//

Asteroids::ButtonEntity::~ButtonEntity(void)
{
}

//--------------------------------------------------------------------------------------------------------------------//

bool Asteroids::ButtonEntity::IsDisabled(void) const
{
	return (nullptr == mOnClick || false == mIsEnabled);
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::ButtonEntity::SetAvailable(const bool isAvailable)
{
	SetVisible(isAvailable);
}

//--------------------------------------------------------------------------------------------------------------------//

#if defined(rusty_development)
void Asteroids::ButtonEntity::Development_ForceClickCallback(void)
{
	mOnClick();
}
#endif

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::ButtonEntity::OnAdd(void)
{
	tbGame::Entity::OnAdd();
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::ButtonEntity::OnRemove(void)
{
	tbGame::Entity::OnRemove();
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::ButtonEntity::OnSimulate(void)
{
	tbGame::Entity::OnSimulate();
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::ButtonEntity::OnUpdate(const float deltaTime)
{
	tbGame::Entity::OnUpdate(deltaTime);

	mRunningTimer += deltaTime;

	if (nullptr != mEnabler)
	{
		bool result = mEnabler();
		SetEnabled(result);
	}

	const Vector2 mousePositionInterface = Interface::ScreenSpaceToInterface(tbGame::Input::GetMousePosition());
	mIsHovered = UnstableIsPointContained(mousePositionInterface);

	if (false == IsDisabled() && true == mIsHovered && true == tbGame::Input::IsKeyPressed(tbApplication::tbMouseLeft))
	{
		if (false == mPressedAudioEvent.empty())
		{
			tbAudio::theAudioManager.PlayEvent("audio_events", mPressedAudioEvent);
		}

		mOnClick();
	}

	mVisuals[State::Hovered].SetVisible(true == mIsEnabled && true == mIsHovered);
	mVisuals[State::Enabled].SetVisible(true == mIsEnabled && false == mIsHovered);
	mVisuals[State::Disabled].SetVisible(false == mIsEnabled);
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::ButtonEntity::OnRender(void) const
{
	tbGame::Entity::OnRender();

	if (true == mVisuals[State::Hovered].IsVisible())
	{
		for (auto* sprite : mHoverButtonSprites)
		{
			ShaderSystem::theShaderManager.SetShaderUniform("uTime", mRunningTimer);
			ShaderSystem::theShaderManager.SetShaderUniform("uPrimaryColor", Vector3(0.988f, 0.804f, 0.376f));
			ShaderSystem::theShaderManager.SetShaderUniform("uSecondaryColor", Vector3(0.65f, 0.935f, 1.0f));
			ShaderSystem::theShaderManager.PushAndBindShader(ShaderSystem::theUIOutlineShader);
			ShaderSystem::theShaderManager.ApplyUniformsForDraw();

			sprite->Render();

			ShaderSystem::theShaderManager.PopShader();
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::ButtonEntity::OnCollide(const tbGame::Entity& otherEntity)
{
	tbGame::Entity::OnCollide(otherEntity);
}

//--------------------------------------------------------------------------------------------------------------------//
