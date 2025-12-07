///
/// @file
/// @details A place to create/destroy all the shaders in Asteroids while ensuring the uniforms have defaults.
///
/// <!-- Copyright (c) Tyre Bytes LLC 2025 - All Rights Reserved -->
///------------------------------------------------------------------------------------------------------------------///

#include "../shader_system/shaders.hpp"

//--------------------------------------------------------------------------------------------------------------------//

Asteroids::ShaderSystem::ShaderHandle Asteroids::ShaderSystem::theFogOfWildernessShader = InvalidShader();
Asteroids::ShaderSystem::ShaderHandle Asteroids::ShaderSystem::theSimpleFogShader = InvalidShader();
Asteroids::ShaderSystem::ShaderHandle Asteroids::ShaderSystem::theOutlineShader = InvalidShader();
Asteroids::ShaderSystem::ShaderHandle Asteroids::ShaderSystem::theUIOutlineShader = InvalidShader();

//--------------------------------------------------------------------------------------------------------------------//

namespace Asteroids::ShaderSystem::Implementation
{
	bool theShadersCreated = false;

	const String theShaderDirectory = "data/shaders/";

	/// 2025-11-13: Upon stumbling into Create/Destroy shaders I noticed theSimpleFogShader and theOutlineShader were
	///   not being destroyed. At first I didn't think much of it, just went to cleanup the code so they would be by
	///   adding this container of information for the shaders. But that also fixed the setting uniform crash that was
	///   what originally brought me investigating... Turns out leaving a "dangling shader" by not calling DestroyShader(),
	///   despite sometime being valid, will leave those shaders in an invalid state after a context load which was the
	///   cause of the crash. Lesson, proper cleanup can prevent crashing.
	std::vector<std::pair<ShaderHandle*, std::pair<String, String>>> theShaders = {
		{ &theFogOfWildernessShader, { "fog_of_wilderness_gl3_2.vert", "fog_of_wilderness_gl3_2.frag" } },
		{ &theSimpleFogShader, { "fog_gl3_2.vert", "fog_gl3_2.frag" } },
		{ &theOutlineShader, { "fog_gl3_2.vert", "outline_gl3_2.frag" } },
		{ &theUIOutlineShader, { "fog_gl3_2.vert", "ui_outline_gl3_2.frag" } },
	};
};

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::ShaderSystem::CreateShaders(void)
{
	tb_error_if(true == Implementation::theShadersCreated, "Calling CreateShaders() with shaders already existing.");

	for (auto& shaderFilesPair : Implementation::theShaders)
	{
		ShaderHandle& shader = *shaderFilesPair.first;
		const auto& files = shaderFilesPair.second;

		shader = ShaderSystem::theShaderManager.CreateShaderFromFile(
			Implementation::theShaderDirectory + files.first, Implementation::theShaderDirectory + files.second);

		tb_error_if(InvalidShader() == shader, "Error failed to load shader '%s' + '%s'.", files.first.c_str(), files.second.c_str());
	}

	// Whenever a new uniform is added to a Shader it must also get added to this to initialize the "ShaderUniforms"
	//   on the CPU side. These are the defaults that ACTUALLY matter. The specification essentially means the defaults
	//   set on uniform values in the shaders are not guarenteed or something strange. This is an old memory, but it has
	//   bitten me before.
	theShaderManager.SetShaderUniform("uPrimaryColor", Vector3(0.7f, 0.5f, 0.4f));
	theShaderManager.SetShaderUniform("uSecondaryColor", Vector3(0.1f, 0.2f, 0.2f));
	theShaderManager.SetShaderUniform("uFogSize", Vector2(0.0f, 0.0f));
	theShaderManager.SetShaderUniform("uScreenSize", Vector2(0.0f, 0.0f));
	theShaderManager.SetShaderUniform("uFogDirection", Vector2(0.3f, -0.3f));
	theShaderManager.SetShaderUniform("uForceTransparency", false);
	theShaderManager.SetShaderUniform("uFogTransparency", 0.1f);
	theShaderManager.SetShaderUniform("uFogSpeed", 0.3f);
	theShaderManager.SetShaderUniform("outlineThickness", 0.04f);
	theShaderManager.SetShaderUniform("uTime", 0.0f);

	// Common Uniforms, should kinda be set by engine but shaders aren't really in TurtleBrains.
	theShaderManager.SetShaderUniform("uObjectToProjection", Matrix4::Identity());
	theShaderManager.SetShaderUniform("diffuseTexture", 0);

	Implementation::theShadersCreated = true;
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::ShaderSystem::DestroyShaders(void)
{
	tb_error_if(false == Implementation::theShadersCreated, "Calling DestroyShaders() before CreateShaderes()????");

	for (auto& shaderFilesPair : Implementation::theShaders)
	{
		ShaderHandle& shader = *shaderFilesPair.first;

		theShaderManager.DestroyShader(shader);
		shader = InvalidShader();
	}

	Implementation::theShadersCreated = false;
}

//--------------------------------------------------------------------------------------------------------------------//
