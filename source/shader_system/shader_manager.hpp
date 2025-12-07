///
/// @file
/// @details This is a simple shader management class to load and apply shader programs in OpenGL.  This is the first
///   attempt at such a class.
///
/// <!-- Copyright (c) Tyre Bytes LLC 2017 - All Rights Reserved -->
///------------------------------------------------------------------------------------------------------------------///

#ifndef Asteroids_ShaderManager_hpp
#define Asteroids_ShaderManager_hpp

#include "../asteroids.hpp"

#include <turtle_brains/core/tb_noncopyable.hpp>
#include <turtle_brains/core/tb_types.hpp>
#include <turtle_brains/core/tb_string.hpp>
#include <turtle_brains/core/tb_resource_handle.hpp>
#include <turtle_brains/math/tb_matrix.hpp>

#include <stack>
#include <string>

namespace Asteroids::ShaderSystem
{
	namespace Implementation
	{
		class ShaderManagerCreator;
		class ShaderHandleSaver {};
	};

	///
	/// @details Loads up the shaders for the engine, this must be called between the Application starting up and
	///   any GameScene's getting instatiated. It is labeled Hacky because it will DEFINITELY move location/name
	///   and with any luck be automated in the future.
	///
	//void LoadInternalShaders(void);

	///
	/// @details Holds a handle/reference to a shader resource within InternalCombustion engine. Multiple shader handles
	///   can point to/share the same shader resource.
	///
	typedef tbCore::ResourceHandle<Implementation::ShaderHandleSaver> ShaderHandle;

	///
	/// @details Returns a value the represents the invalid state for a ShaderHandle.
	///
	ShaderHandle InvalidShader(void);

	class ShaderManager : public tbCore::Noncopyable
	{
	public:
		static void SetEngineShaderDirectory(const String& shaderDirectory);

		ShaderHandle CreateShaderFromFile(const String& vertexShaderFile, const String& fragmentShaderFile, const String& geometryShaderFile = "");
		ShaderHandle CreateShaderFromData(const String& vertexShaderSource, const String& fragmentShaderSource, const String& geometryShaderSource = "");

		void DestroyShader(const ShaderHandle& shaderHandle);
		void CheckAndReloadShaders(void);

		///
		/// @details For the currently bound shader, any dirty uniforms will get sent to the GPU.
		///
		void ApplyUniformsForDraw(void);

		void PushAndBindShader(const ShaderHandle& shaderHandle);
		void PopShader(void);
		void BindShader(const ShaderHandle& shaderHandle);
		const ShaderHandle& GetCurrentShader(void) const;

		void SetShaderAttribute(const ShaderHandle& shaderHandle, const String& attributeName, const float value);
		void SetShaderAttribute(const ShaderHandle& shaderHandle, const String& attributeName, const tbMath::Vector2& value);
		void SetShaderAttribute(const ShaderHandle& shaderHandle, const String& attributeName, const tbMath::Vector3& value);
		void SetShaderAttribute(const ShaderHandle& shaderHandle, const String& attributeName, const tbMath::Vector4& value);

		void SetShaderUniform(const ShaderHandle& shaderHandle, const String& uniformName, const int value);
		void SetShaderUniform(const ShaderHandle& shaderHandle, const String& uniformName, const float value);
		void SetShaderUniform(const ShaderHandle& shaderHandle, const String& uniformName, const Vector2& value);
		void SetShaderUniform(const ShaderHandle& shaderHandle, const String& uniformName, const Vector3& value);
		void SetShaderUniform(const ShaderHandle& shaderHandle, const String& uniformName, const Vector4& value);
		void SetShaderUniform(const ShaderHandle& shaderHandle, const String& uniformName, const Matrix3& value, const bool transposeOnPassing = true);
		void SetShaderUniform(const ShaderHandle& shaderHandle, const String& uniformName, const Matrix4& value, const bool transposeOnPassing = true);
		void SetShaderUniform(const ShaderHandle& shaderHandle, const String& uniformName, const float* values, tbCore::uint8 numberOfValues);


		/// @details This is current unstable, use at your own risk, may be deprecated at will.
		///   Objective here is to set the values for the provided uniform across ALL shaders, effectively making it
		///   easier to set engine uniforms once in a frame rather than for each shader in a frame. That doesn't help
		///   for those times where a shader is loaded after the engine uniform has been set, but if the engine sets
		///   all engine uniform per Render() this shouldn't be an issue... who would load a shader during render?
		void SetShaderUniform(const String& uniformName, const float* values, tbCore::uint8 numberOfValues);
		void SetShaderUniform(const String& uniformName, const int value);

		inline void SetShaderUniform(const String& uniformName, const float value) { SetShaderUniform(uniformName, &value, 1); }
		inline void SetShaderUniform(const String& uniformName, const Vector2& value) { SetShaderUniform(uniformName, static_cast<const float*>(value), 2); }
		inline void SetShaderUniform(const String& uniformName, const Vector3& value) { SetShaderUniform(uniformName, static_cast<const float*>(value), 3); }
		inline void SetShaderUniform(const String& uniformName, const Vector4& value) { SetShaderUniform(uniformName, static_cast<const float*>(value), 4); }
		inline void SetShaderUniform(const String& uniformName, const Matrix3& value) { SetShaderUniform(uniformName, static_cast<const float*>(value), 9); }
		inline void SetShaderUniform(const String& uniformName, const Matrix4& value) { SetShaderUniform(uniformName, static_cast<const float*>(value), 16); }

		void CreateGraphicsContext(void);
		void DestroyGraphicsContext(void);

	private:
		ShaderManager(void);
		~ShaderManager(void);

		friend class Implementation::ShaderManagerCreator;
	};

	extern ShaderManager& theShaderManager;

};	//namespace Asteroids::ShaderSystem

#endif /* Asteroids_ShaderManager_hpp */
