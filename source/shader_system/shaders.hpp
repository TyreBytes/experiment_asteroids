///
/// @file
/// @details A place to create/destroy all the shaders in Asteroids while ensuring the uniforms have defaults.
///
/// <!-- Copyright (c) Tyre Bytes LLC 2025 - All Rights Reserved -->
///------------------------------------------------------------------------------------------------------------------///

#ifndef Asteroids_Shaders_hpp
#define Asteroids_Shaders_hpp

#include "../shader_system/shader_manager.hpp"

namespace Asteroids::ShaderSystem
{

	extern ShaderHandle theFogOfWildernessShader;
	extern ShaderHandle theSimpleFogShader;
	extern ShaderHandle theOutlineShader;
	extern ShaderHandle theUIOutlineShader;

	void CreateShaders(void);
	void DestroyShaders(void);

}; /* namespace Asteroids::ShaderSystem */

#endif /* Asteroids_Shaders_hpp */
