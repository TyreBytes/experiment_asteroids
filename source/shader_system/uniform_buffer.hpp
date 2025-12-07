///
/// @file
/// @details This is currently in early development and will be properly documented at a later date once
///   the details are more concrete. TODO: TurtleBrains: Documentation: Check over interface and documentation for
///   first public release.
///
/// <!-- Copyright (c) Tyre Bytes LLC 2021 - All Rights Reserved -->
///------------------------------------------------------------------------------------------------------------------///

#ifndef Asteroids_UniformBuffer_hpp
#define Asteroids_UniformBuffer_hpp

#include <turtle_brains/core/tb_noncopyable.hpp>
#include <turtle_brains/core/tb_opengl.hpp>
#include <turtle_brains/math/tb_matrix.hpp>

#include "../shader_system/shader_manager.hpp"

#include <array>

namespace Asteroids::ShaderSystem
{
	// 2025-10-21: This came from InternalCombustion::Unstable::UniformBuffer which was originally an implementation
	//   detail to support skinned animations. We are using it in Asteroids for the fog of wilderness effect, and
	//   going to try the template thing...
	//template<typename Type, size_t Size> class UniformBuffer : public tbCore::Noncopyable
	class UniformBuffer : public tbCore::Noncopyable
	{
	public:
		using Type = float;
		static const size_t Size = 2048;

		explicit UniformBuffer(void);
		~UniformBuffer(void);

		void Clear(void);
		bool PushData(const Type& data);
		void Bind(ShaderHandle& shader, const String& uniformBlockName);
		void Unbind(void);

		static void OnCreateGraphicsContext(void);
		static void OnDestroyGraphicsContext(void);

	private:
		const size_t kBatchSize;
		const size_t kElementSize;
		GLuint mUniformBuffer;

		std::array<Type, Size> mBufferData;
		size_t mWriteIndex;
	};

};	//namespace Asteroids::ShaderSystem

#endif /* Asteroids_UniformBuffer_hpp */
