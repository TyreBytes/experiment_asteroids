///
/// @file
/// @details An entity within the Asteroids project.
///
/// <!-- Copyright (c) 2025 Tyre Bytes LLC - All Rights Reserved -->
///------------------------------------------------------------------------------------------------------------------///

#include "../entities/template_entity.hpp"

//--------------------------------------------------------------------------------------------------------------------//

Asteroids::TemplateEntity::TemplateEntity(void) :
	tbGame::Entity("TemplateEntity"),
	mSprite("concept_art", "solid_square")
{
	AddGraphic(mSprite);
}

//--------------------------------------------------------------------------------------------------------------------//

Asteroids::TemplateEntity::~TemplateEntity(void)
{
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::TemplateEntity::OnAdd(void)
{
	tbGame::Entity::OnAdd();
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::TemplateEntity::OnRemove(void)
{
	tbGame::Entity::OnRemove();
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::TemplateEntity::OnSimulate(void)
{
	tbGame::Entity::OnSimulate();
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::TemplateEntity::OnUpdate(const float deltaTime)
{
	tbGame::Entity::OnUpdate(deltaTime);
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::TemplateEntity::OnRender(void) const
{
	tbGame::Entity::OnRender();
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::TemplateEntity::OnCollide(const tbGame::Entity& otherEntity)
{
	tbGame::Entity::OnCollide(otherEntity);
}

//--------------------------------------------------------------------------------------------------------------------//
