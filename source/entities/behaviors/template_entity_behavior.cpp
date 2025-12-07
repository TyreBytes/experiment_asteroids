///
/// @file
/// @details A template to create a behavior for entities within the Asteroids project.
///
/// <!-- Copyright (c) 2025 Tyre Bytes LLC - All Rights Reserved -->
///------------------------------------------------------------------------------------------------------------------///

#include "../../entities/behaviors/template_entity_behavior.hpp"

//--------------------------------------------------------------------------------------------------------------------//

Asteroids::TemplateBehavior::TemplateBehavior(tbGame::Entity& entity) :
	tbGame::EntityBehaviorInterface(entity)
{
}

//--------------------------------------------------------------------------------------------------------------------//

Asteroids::TemplateBehavior::~TemplateBehavior(void)
{
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::TemplateBehavior::OnAdd(void)
{
	EntityBehaviorInterface::OnAdd();
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::TemplateBehavior::OnRemove(void)
{
	EntityBehaviorInterface::OnRemove();
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::TemplateBehavior::OnPause(void)
{
	EntityBehaviorInterface::OnPause();
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::TemplateBehavior::OnResume(void)
{
	EntityBehaviorInterface::OnResume();
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::TemplateBehavior::OnSimulate(void)
{
	EntityBehaviorInterface::OnSimulate();
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::TemplateBehavior::OnUpdate(const float deltaTime)
{
	EntityBehaviorInterface::OnUpdate(deltaTime);
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::TemplateBehavior::OnRender(void) const
{
	EntityBehaviorInterface::OnRender();
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::TemplateBehavior::OnCollide(const tbGame::Entity& withOther)
{
	EntityBehaviorInterface::OnCollide(withOther);
}

//--------------------------------------------------------------------------------------------------------------------//
