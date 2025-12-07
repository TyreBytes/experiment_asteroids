///
/// @file
/// @details This is currently in early development and will be properly documented at a later date once
///   the details are more concrete. TODO: TurtleBrains: Documentation: Check over interface and documentation for
///   first public release.
///
/// <!-- Copyright (c) Tyre Bytes LLC 2021 - All Rights Reserved -->
///------------------------------------------------------------------------------------------------------------------///

#include "../shader_system/uniform_buffer.hpp"
#include "../shader_system/shader_manager.hpp" //For ShaderHandle and Getting the ProgramID

// 2025-11-19: The ShaderSystem Implementation depends on the TurtleBrains renderer, for check_gl_errors and other
//   implementation details. We 'know what we are doing'.
#define TurtleBrains_LetMeHave_Implementation
#include <turtle_brains/graphics/implementation/tbi_renderer.hpp>
#undef TurtleBrains_LetMeHave_Implementation


#include <list>

namespace Asteroids::ShaderSystem::Implementation
{
	//Exists in shader_manager.cpp
	tbCore::uint32 ShaderHandleToProgramID(const ShaderHandle& shaderHandle);

	std::list<UniformBuffer*>& GetAllUniformBuffers(void)
	{
		static std::list<UniformBuffer*> allUniformBuffers;
		return allUniformBuffers;
	}
};

//--------------------------------------------------------------------------------------------------------------------//

Asteroids::ShaderSystem::UniformBuffer::UniformBuffer(void) :
	tbCore::Noncopyable(),
	kBatchSize(2048),
	kElementSize(sizeof(Type)),
	mUniformBuffer(0),
	mBufferData(),
	mWriteIndex(0)
{
	ShaderSystem::Implementation::GetAllUniformBuffers().push_back(this);

	int maximumUniformBytes = 0;
	tb_check_gl_errors(glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &maximumUniformBytes));
	const GLint totalRequiredBytes = tbCore::size(kBatchSize * kElementSize);
	tb_error_if(maximumUniformBytes < totalRequiredBytes, "ShaderError: This GPU does not have a large enough block size available.");

	tb_check_gl_errors(glGenBuffers(1, &mUniformBuffer));
	tb_check_gl_errors(glBindBuffer(GL_UNIFORM_BUFFER, mUniformBuffer));
	tb_check_gl_errors(glBufferData(GL_UNIFORM_BUFFER, totalRequiredBytes, nullptr, GL_DYNAMIC_DRAW));
}

//--------------------------------------------------------------------------------------------------------------------//

Asteroids::ShaderSystem::UniformBuffer::~UniformBuffer(void)
{
	// 2025-11-13: Allov discovered a small leak without this happening, because we are recreating the UniformBuffer
	//   object every frame and copying the contents of the fog values into it. That actually means the following
	//   comments (which had described why the glDelete, glBind and buffer = 0 lines were all commented originally)
	//   are wrong since the UniformBuffer won't outlive the SupplyRun::Render() function. This fix wouldn't really
	//   work as-is in ICE, nor would it work terribly well if the UniformBuffer was an object that had lifetime
	//   outside that function, then again the leak would be much smaller in those cases.

	////This would be called when the GL context has already died, so we can't really clean it up.
	////Trust that the system will? (This code initially came from the ParticleManager)
	tb_check_gl_errors(glDeleteBuffers(1, &mUniformBuffer));
	tb_check_gl_errors(glBindBuffer(GL_UNIFORM_BUFFER, 0));
	mUniformBuffer = 0;

	ShaderSystem::Implementation::GetAllUniformBuffers().remove(this);
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::ShaderSystem::UniformBuffer::OnCreateGraphicsContext(void)
{
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::ShaderSystem::UniformBuffer::OnDestroyGraphicsContext(void)
{
	//mData = nullptr;
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::ShaderSystem::UniformBuffer::Clear(void)
{
	mWriteIndex = 0;
}

//--------------------------------------------------------------------------------------------------------------------//

bool Asteroids::ShaderSystem::UniformBuffer::PushData(const Type& data)
{
	tb_error_if(0 == mUniformBuffer, "UniformBuffer has not been properly created, or was somehow lost.");
	tb_error_if(mWriteIndex >= mBufferData.size(), "The UniformBuffer is filled and cannot accept more data.");
	if (mWriteIndex < mBufferData.size())
	{
		//The UniformBuffer was originally made to support the Animation system with transforms into the shader. This
		//code certainly should not be here, but allowed quick testing walking back up the chain to apply parent transforms.
		//It was simply a convenient place. Read up in the ice_animation.py script from the exporter, at least on the
		//testing-animation-support branch created 2021-12-25.
		//
		// 2022-04-14: I don't know what comment I was supposed to read in the ice_animation.py script... There was nothing
		//   related to this in there. Guess I'll be left hanging, and hopefully not leave future selves hanging too!
		//
		// 2024-01-11: When I started reinvestigating animation support, the PushData() contents just set the data to
		//   the writeIndex and incremented write index. There were no hacks for walking up the chain. I believe the top
		//   part of this comment (which was marked unknown in 2022-04-14 comment) is referring to the struggle of making
		//   the decision to export the data as boneToRoot or boneToParent. 99% sure that we were testing boneToParent
		//   when the hack code was in use. If we wind up using boneToRoot in the export than buffer[write] = data is
		//   all that is necessary, and as the hack says, all of this code should be outside the UniformBuffer anyway.
		//{	//HACKS
		//	tbMath::Matrix4 toRoot = tbMath::Matrix4::Identity();
		//	for (size_t index = 0; index < mWriteIndex; ++index)
		//	{
		//		//toRoot *= mBufferData[index];
		//		toRoot = mBufferData[index] * toRoot;
		//	}

		//	mBufferData[mWriteIndex] = data * toRoot;
		//}

		////const tbMath::Matrix4 parentToRoot = (true == mBufferData.empty()) ? tbMath::Matrix4::Identity() : mBufferData.back();
		////mBufferData[mWriteIndex] = data * parentToRoot;

		mBufferData[mWriteIndex] = data;
		++mWriteIndex;
	}

	return (mWriteIndex < mBufferData.size());
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::ShaderSystem::UniformBuffer::Bind(ShaderHandle& shader, const String& uniformBlockName)
{
	tbCore::uint32 shaderProgramID = ShaderSystem::Implementation::ShaderHandleToProgramID(shader);

	GLuint bufferIndex = 0;
	tb_check_gl_errors(bufferIndex = glGetUniformBlockIndex(shaderProgramID, uniformBlockName.c_str()));
	tb_check_gl_errors(glUniformBlockBinding(shaderProgramID, bufferIndex, 0));
	tb_check_gl_errors(glBindBufferBase(GL_UNIFORM_BUFFER, 0, mUniformBuffer));

	//tb_check_gl_errors(glBufferData(GL_UNIFORM_BUFFER, kElementSize * mWriteIndex, mBufferData.data(), GL_DYNAMIC_DRAW));

	Type* bufferData = nullptr;
	tb_check_gl_errors(bufferData = reinterpret_cast<Type*>(
		glMapBufferRange(GL_UNIFORM_BUFFER, 0, kElementSize * kBatchSize, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT)));
	memcpy(bufferData, mBufferData.data(), kElementSize * mWriteIndex);

	rand();
	tb_check_gl_errors(glUnmapBuffer(GL_UNIFORM_BUFFER));



	//tbMath::Matrix4* bufferData = nullptr;
	//tb_check_gl_errors(bufferData = reinterpret_cast<tbMath::Matrix4*>(
	//	glMapBufferRange(GL_UNIFORM_BUFFER, 0, kElementSize * kBatchSize, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT)));


	//If we want to support filling a buffer up, for instanced rendering with multiple batches we need to unmap the
	//buffer range once it is filled, then do the render call, and map the range again.  Basically loop over all the
	//elements and if currentIndex > batchSize do the following before continuing to iterate.
	//
	//     tb_check_gl_errors(glUnmapBuffer(GL_UNIFORM_BUFFER));
	//     theMeshManager.RenderInstancedMesh(itr->first, particleInstanceIndex);
	//
	//     tb_check_gl_errors(bufferData = reinterpret_cast<tbMath::Matrix4*>(
	//         glMapBufferRange(GL_UNIFORM_BUFFER, 0, kElementSize * kBatchSize,
	//         GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT)));
	//
	//Before continuing to add instanced data like we did in particle/billboards.
	//     bufferData[instanceIndex] = actualData;
	//     ++currentIndex;
	//
	//Instead, as of now 2021-12-25 we are just assuming you can't/won't overflow the buffer per single bind call.

	//Now free to render with UniformBuffer attached to the shader.
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::ShaderSystem::UniformBuffer::Unbind(void)
{
	//tb_check_gl_errors(glUnmapBuffer(GL_UNIFORM_BUFFER));
	//bufferData = nullptr;

	tb_check_gl_errors(glBindBufferBase(GL_UNIFORM_BUFFER, 0, 0));

	tb_check_recent_gl_errors("UniformBuffer Bind");
	tbGraphics::Implementation::Renderer::ClearErrors("Asteroids UniformBuffer Bind");
}

//--------------------------------------------------------------------------------------------------------------------//
