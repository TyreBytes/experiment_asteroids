///
/// @file
/// @details This is a simple shader management class to load and apply shader programs in OpenGL.  This is the first
///   attempt at such a class.
///
/// <!-- Copyright (c) Tyre Bytes LLC 2017 - All Rights Reserved -->
///------------------------------------------------------------------------------------------------------------------///

#include "../shader_system/shader_manager.hpp"
#include "../shader_system/shader_uniform_object.hpp"
#include "../shader_system/shaders.hpp"

#include "../logging.hpp"

#include <turtle_brains/core/tb_configuration.hpp>
#include <turtle_brains/core/tb_opengl.hpp>
#include <turtle_brains/core/tb_resource_cache.hpp>
#include <turtle_brains/core/tb_file_utilities.hpp>

// 2025-11-19: The ShaderSystem Implementation depends on the TurtleBrains renderer, for check_gl_errors and other
//   implementation details. We 'know what we are doing'.
#define TurtleBrains_LetMeHave_Implementation
#include <turtle_brains/graphics/implementation/tbi_renderer.hpp>
#include <turtle_brains/system/implementation/tbi_directory_watcher.hpp>
#undef TurtleBrains_LetMeHave_Implementation

#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <memory>

const tbCore::uint32 kInvalidOpenGLProgram(0); //This should be an invalid Program handle from OpenGL.
const tbCore::uint32 kInvalidOpenGLShader(0); //This should be an invalid Shader handle from OpenGL.

namespace Hacks
{	//Delete this some day when the comment about uniform locations doesn't apply.
	int glContextsCreated = 0;
}

namespace Asteroids::ShaderSystem::Implementation
{
	enum ShaderType
	{
		kVertexShader = 0,
		kFragmentShader,
		kGeometryShader,
	};

	class ShaderManagerCreator
	{
	public:
		ShaderManagerCreator(void) :
			mShaderManager()
		{
		}

		~ShaderManagerCreator(void)
		{
		}

		ShaderManager mShaderManager;
	};

	struct CachedName
	{	//NOTE: Though the variable names are "File" they could contain the "Source" of the shaders when the member
		//   mIsShaderSource is set to true. This can be an issue with debugging; attempting to output the "File"
		//   when it contains "Source" may result in the debug_log buffer being too small. Considered separating
		//   the Source into three different strings, but that does add slight overhead that may be unnecessary.
		//
		//   The debug buffer issue was "fixed" by just using a substring of the first 128 characters, each file added
		//   in this manner should contain the name at the top as the first possible comment.
		String mGeometryShaderFile = "";
		String mVertexShaderFile = "";
		String mFragmentShaderFile = "";
		bool mIsShaderSource = false;

		bool empty(void) const { return (mVertexShaderFile.empty() || mFragmentShaderFile.empty()); }

		bool operator<(const CachedName& other) const
		{
			return (mGeometryShaderFile + mVertexShaderFile + mFragmentShaderFile) < (other.mGeometryShaderFile + other.mVertexShaderFile + other.mFragmentShaderFile);
		}

		String CombinedString(void) const
		{
			return mGeometryShaderFile.substr(0, 128) + mVertexShaderFile.substr(0, 128) + mFragmentShaderFile.substr(0, 128);
		}
	};

	struct ShaderData
	{
		struct UniformInfo
		{
			String mName;
			int mLocation;
			size_t mSize;
			GLenum mType;
			bool mIsInitialized;
		};

		CachedName mCachedName;
		std::shared_ptr<ShaderUniformTable> mUniformTable;
		//Container of the active uniforms when this shader was linked to ensure that all the uniforms are initialized
		//from within code upon being bound for rendering. This is more a debug/development tool than necessary to ship.
		std::map<String, UniformInfo> mActiveUniforms;
		int mProgram;
		int mGeometryShader;
		int mVertexShader;
		int mFragmentShader;
	};

	typedef ShaderHandle ShaderHandle;
	typedef tbCore::ResourceCache<ShaderData, ShaderHandle> ShaderCache;

	typedef std::stack<ShaderHandle> ShaderStack;

	ShaderCache theShaderCache;

	ShaderStack theShaderStack;
	ShaderHandle theBoundShader;
	tbCore::uint32 theDataShaderCount = 0;
	int theOriginalShaderProgram = 0; //aka: the built-in TurtleBrains shader.

	//The following do NOT modify name or reference count, it only handles the GL logic.
	bool GenerateShaderFromOpenGL(ShaderData& shaderData);
	bool GenerateShaderFromOpenGL(ShaderData& shaderData, const String& vertexShaderSource,
		const String& fragmentShaderSource, const String& geometryShaderSource);
	bool DestroyShaderFromOpenGL(ShaderData& shaderData);

	///
	/// This is a pile of hackery, the filePath will have to have been found in the shader cache already, and then
	///   an attempt will be made to load a new vertex/fragment shader. If compiled it will then switch out the cached
	///   id's with the new ones and unload the old.
	///
	void TryReloadingShaderFromFile(const String& filePath);
	String LoadShaderSourceFromFile(const String& filepath);

	///
	/// Compiles an OpenGL GLSL shader for usage.
	///
	/// @param shaderFilename Used specifically for debugging purposes and displaying the shader name getting compiled
	///   should an error occur while compiling.
	///
	int CompileShader(const String& shaderSource, const ShaderType& shaderType, const String& shaderFilename);
	int LinkShader(ShaderData& shaderData);

	String theEngineShadersDirectory;
	TurtleBrains::System::Implementation::DirectoryWatcher theShaderDirectoryWatcher;

	tbCore::uint32 ShaderHandleToProgramID(const ShaderHandle& shaderHandle);
	tbCore::uint32 GetInternalBoundProgramID(void);

	String PreprocessShaderSource(const String& shaderSource);
};

using namespace Asteroids::ShaderSystem::Implementation;

ShaderManagerCreator theShaderManagerCreator;
Asteroids::ShaderSystem::ShaderManager& Asteroids::ShaderSystem::theShaderManager(theShaderManagerCreator.mShaderManager);

//--------------------------------------------------------------------------------------------------------------------//
//--------------------------------------------------------------------------------------------------------------------//
//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::ShaderSystem::Implementation::UniformObject::RefreshUniformLocation(int openglProgramId, const String& uniformName)
{
	tb_check_gl_errors(mUniformLocation = glGetUniformLocation(openglProgramId, uniformName.c_str()));
	mObserver->OnNotifyDirty(*this);
	mIsDirty = true;
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::ShaderSystem::Implementation::UniformObject::OnClean(void)
{
	if (-1 != mUniformLocation)
	{
		switch (mValueSize)
		{
		case 0: tb_check_gl_errors(glUniform1i(mUniformLocation, mIntegerValue)); break;
		case 1: tb_check_gl_errors(glUniform1f(mUniformLocation, mFloatValues[0])); break;
		case 2: tb_check_gl_errors(glUniform2f(mUniformLocation, mFloatValues[0], mFloatValues[1])); break;
		case 3: tb_check_gl_errors(glUniform3f(mUniformLocation, mFloatValues[0], mFloatValues[1], mFloatValues[2])); break;
		case 4: tb_check_gl_errors(glUniform4f(mUniformLocation, mFloatValues[0], mFloatValues[1], mFloatValues[2], mFloatValues[3])); break;
		case 9: tb_check_gl_errors(glUniformMatrix3fv(mUniformLocation, 1, true, mFloatValues.data())); break;
		case 16: tb_check_gl_errors(glUniformMatrix4fv(mUniformLocation, 1, true, mFloatValues.data())); break;
		default: tb_error("iceInternalError: Unknown case for uniform upload.");
		};
	}

	mIsDirty = false;
}

//--------------------------------------------------------------------------------------------------------------------//
//--------------------------------------------------------------------------------------------------------------------//
//--------------------------------------------------------------------------------------------------------------------//

tbCore::uint32 Asteroids::ShaderSystem::Implementation::ShaderHandleToProgramID(const ShaderHandle& shaderHandle)
{
	if (false == theShaderCache.IsValidHandle(shaderHandle))
	{
		return kInvalidOpenGLProgram;
	}

	return theShaderCache.GetResourceReference(shaderHandle).mProgram;
}

//--------------------------------------------------------------------------------------------------------------------//

tbCore::uint32 Asteroids::ShaderSystem::Implementation::GetInternalBoundProgramID(void)
{
	if (false == theShaderCache.IsValidHandle(theBoundShader))
	{
		return kInvalidOpenGLProgram;
	}

	return theShaderCache.GetResourceReference(theBoundShader).mProgram;
}

//--------------------------------------------------------------------------------------------------------------------//

bool Asteroids::ShaderSystem::Implementation::GenerateShaderFromOpenGL(ShaderData& shaderData)
{
	if (true == shaderData.mCachedName.mIsShaderSource)
	{
		return GenerateShaderFromOpenGL(shaderData, shaderData.mCachedName.mVertexShaderFile,
			shaderData.mCachedName.mFragmentShaderFile, shaderData.mCachedName.mGeometryShaderFile);
	}

	const String geometryShaderSource(shaderData.mCachedName.mGeometryShaderFile.empty() ? "" :
		PreprocessShaderSource(LoadShaderSourceFromFile(shaderData.mCachedName.mGeometryShaderFile)));
	const String vertexShaderSource(PreprocessShaderSource(LoadShaderSourceFromFile(shaderData.mCachedName.mVertexShaderFile)));
	const String fragmentShaderSource(PreprocessShaderSource(LoadShaderSourceFromFile(shaderData.mCachedName.mFragmentShaderFile)));

	return GenerateShaderFromOpenGL(shaderData, vertexShaderSource, fragmentShaderSource, geometryShaderSource);
}

//--------------------------------------------------------------------------------------------------------------------//

bool Asteroids::ShaderSystem::Implementation::GenerateShaderFromOpenGL(ShaderData& shaderData, const String& vertexShaderSource,
	const String& fragmentShaderSource, const String& geometryShaderSource)
{
	if (true == vertexShaderSource.empty())
	{
		tb_error("Failed to load vertex shader: \"%s\"", shaderData.mCachedName.mVertexShaderFile.c_str());
		return false;
	}

	if (true == fragmentShaderSource.empty())
	{
		tb_error("Failed to load fragment shader: \"%s\"", shaderData.mCachedName.mFragmentShaderFile.c_str());
		return false;
	}

	if (false == geometryShaderSource.empty())
	{
		shaderData.mGeometryShader = CompileShader(geometryShaderSource, kGeometryShader, shaderData.mCachedName.mGeometryShaderFile);
	}

	shaderData.mVertexShader = CompileShader(vertexShaderSource, kVertexShader, shaderData.mCachedName.mVertexShaderFile);
	shaderData.mFragmentShader = CompileShader(fragmentShaderSource, kFragmentShader, shaderData.mCachedName.mFragmentShaderFile);
	shaderData.mProgram = LinkShader(shaderData);

	return true;
}

//--------------------------------------------------------------------------------------------------------------------//

bool Asteroids::ShaderSystem::Implementation::DestroyShaderFromOpenGL(ShaderData& shaderData)
{
	tb_check_gl_errors(glDetachShader(shaderData.mProgram, shaderData.mFragmentShader));
	tb_check_gl_errors(glDetachShader(shaderData.mProgram, shaderData.mVertexShader));
	tb_check_gl_errors(glDeleteShader(shaderData.mFragmentShader));
	tb_check_gl_errors(glDeleteShader(shaderData.mVertexShader));
	tb_check_gl_errors(glDeleteProgram(shaderData.mProgram));

	shaderData.mProgram = kInvalidOpenGLProgram;
	shaderData.mGeometryShader = kInvalidOpenGLShader;
	shaderData.mVertexShader = kInvalidOpenGLShader;
	shaderData.mFragmentShader = kInvalidOpenGLShader;

	return true;
}

//--------------------------------------------------------------------------------------------------------------------//
//--------------------------------------------------------------------------------------------------------------------//
//--------------------------------------------------------------------------------------------------------------------//

//void Asteroids::ShaderSystem::LoadInternalShaders(void)
//{
//	RustyShaders::CreateShaders();
//}

//--------------------------------------------------------------------------------------------------------------------//
//--------------------------------------------------------------------------------------------------------------------//
//--------------------------------------------------------------------------------------------------------------------//

Asteroids::ShaderSystem::ShaderManager::ShaderManager(void)
{
}

//--------------------------------------------------------------------------------------------------------------------//

Asteroids::ShaderSystem::ShaderManager::~ShaderManager(void)
{
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::ShaderSystem::ShaderManager::SetEngineShaderDirectory(const String& shaderDirectory)
{
#if defined(tb_debug_build) || defined(tb_development_build)
	theEngineShadersDirectory = shaderDirectory;
#else
	tb_unused(shaderDirectory);
#endif /* debug or development build */
}

//--------------------------------------------------------------------------------------------------------------------//

Asteroids::ShaderSystem::ShaderHandle Asteroids::ShaderSystem::ShaderManager::CreateShaderFromFile(
	const String& vertexShaderFile, const String& fragmentShaderFile, const String& geometryShaderFile)
{
	//First check the cache to see if this shader is already loaded.
	const CachedName cachedName{ geometryShaderFile, vertexShaderFile, fragmentShaderFile, false };
	const String cachedNameString = cachedName.CombinedString();

	ShaderHandle shaderHandle = theShaderCache.GetResourceHandle(cachedNameString);
	if (shaderHandle.IsValid())
	{
		theShaderCache.IncrementReferenceOf(shaderHandle);
	}
	else
	{
		if (false == Implementation::theEngineShadersDirectory.empty())
		{
			theShaderDirectoryWatcher.AddWatchedFile(Implementation::theEngineShadersDirectory + "shader_api.vert");
			theShaderDirectoryWatcher.AddWatchedFile(Implementation::theEngineShadersDirectory + "shader_api.frag");
		}

		theShaderDirectoryWatcher.AddWatchedFile(geometryShaderFile);
		theShaderDirectoryWatcher.AddWatchedFile(vertexShaderFile);
		theShaderDirectoryWatcher.AddWatchedFile(fragmentShaderFile);

		//Wasn't already loaded, so lets load it up!
		ShaderData shaderData;
		shaderData.mCachedName = cachedName;
		shaderData.mProgram = kInvalidOpenGLProgram;
		shaderData.mGeometryShader = kInvalidOpenGLShader;
		shaderData.mVertexShader = kInvalidOpenGLShader;
		shaderData.mFragmentShader = kInvalidOpenGLShader;
		shaderData.mUniformTable = std::make_shared<Implementation::ShaderUniformTable>();

		Implementation::GenerateShaderFromOpenGL(shaderData);
		shaderHandle = theShaderCache.CreateResource(shaderData, cachedNameString);
		tb_debug_log(LogShader::Info() << "Loaded Shader Program:\n\tGeometry: '" << geometryShaderFile <<
			"'\n\tVertex: '" << vertexShaderFile << "'\n\tFragment: '" << fragmentShaderFile << "'.");
	}

	return shaderHandle;
}

//--------------------------------------------------------------------------------------------------------------------//

Asteroids::ShaderSystem::ShaderHandle Asteroids::ShaderSystem::ShaderManager::CreateShaderFromData(
	const String& vertexShaderSource, const String& fragmentShaderSource, const String& geometryShaderSource)
{
	const CachedName cachedName{
		PreprocessShaderSource(geometryShaderSource),
		PreprocessShaderSource(vertexShaderSource),
		PreprocessShaderSource(fragmentShaderSource), true
	};

	ShaderData shaderData;
	shaderData.mCachedName = cachedName;
	shaderData.mProgram = kInvalidOpenGLProgram;
	shaderData.mGeometryShader = kInvalidOpenGLShader;
	shaderData.mVertexShader = kInvalidOpenGLShader;
	shaderData.mFragmentShader = kInvalidOpenGLShader;
	shaderData.mUniformTable = std::make_shared<Implementation::ShaderUniformTable>();

	Implementation::GenerateShaderFromOpenGL(shaderData, cachedName.mVertexShaderFile,
		cachedName.mFragmentShaderFile, cachedName.mGeometryShaderFile);
	return theShaderCache.CreateResource(shaderData, "ShaderFromData" + tb_string(++theDataShaderCount));
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::ShaderSystem::ShaderManager::DestroyShader(const ShaderHandle& shaderHandle)
{
	if (false == tbGraphics::Implementation::Renderer::tbiIsRendererAvailable)
	{
		tb_always_log(tbiLog::Warning() << "TODO: TIM: Skipping the unloading of a shader, need refactor.");
		return;
	}

	tb_error_if(false == theShaderCache.IsValidHandle(shaderHandle), "Shader ExternalError: Expected a valid shader handle, this one is out of range.");

	if (true == theShaderCache.DecrementReferenceOf(shaderHandle))
	{
		ShaderData& shaderData = theShaderCache.GetResourceReference(shaderHandle);

		if (theBoundShader == shaderHandle)
		{
			BindShader(ShaderHandle());
		}

		shaderData.mUniformTable = nullptr;
		DestroyShaderFromOpenGL(shaderData);
		shaderData.mProgram = kInvalidOpenGLProgram;

		theShaderCache.DestroyResource(shaderHandle);

		//TODO: InternalCombustion: Remove shader files from theShaderDirectoryWatcher
	}
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::ShaderSystem::ShaderManager::CheckAndReloadShaders(void)
{
	const auto modifiedFiles = theShaderDirectoryWatcher.CheckForModifiedFiles();
	for (const auto& modifiedFile : modifiedFiles)
	{
#if defined(tb_debug_build) || defined(tb_development_build)
		if (false == Implementation::theEngineShadersDirectory.empty())
		{
			if (modifiedFile == Implementation::theEngineShadersDirectory + "shader_api.vert" ||
				modifiedFile == Implementation::theEngineShadersDirectory + "shader_api.frag")
			{
				theShaderDirectoryWatcher.ForEachWatchedFile([](const String& watchedFile){

					if (watchedFile != Implementation::theEngineShadersDirectory + "shader_api.vert" &&
						watchedFile != Implementation::theEngineShadersDirectory + "shader_api.frag")
					{
						TryReloadingShaderFromFile(watchedFile);
					}
				});

				continue; //Do not TryReloading shader_api below...
			}
		}
#endif /* debug or development build */

		TryReloadingShaderFromFile(modifiedFile);
	}
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::ShaderSystem::Implementation::TryReloadingShaderFromFile(const String& filePath)
{
	const ShaderHandle boundShader = theBoundShader;
	theShaderManager.BindShader(InvalidShader());

	theShaderCache.ForEachValidResource([&filePath](ShaderData& shaderData, ShaderHandle shaderHandle){
		tb_unused(shaderHandle);

		if (filePath == shaderData.mCachedName.mGeometryShaderFile ||
			filePath == shaderData.mCachedName.mVertexShaderFile ||
			filePath == shaderData.mCachedName.mFragmentShaderFile)
		{
			ShaderData newShaderData;
			newShaderData.mCachedName.mGeometryShaderFile = shaderData.mCachedName.mGeometryShaderFile;
			newShaderData.mCachedName.mVertexShaderFile = shaderData.mCachedName.mVertexShaderFile;
			newShaderData.mCachedName.mFragmentShaderFile = shaderData.mCachedName.mFragmentShaderFile;
			newShaderData.mCachedName.mIsShaderSource = false;
			newShaderData.mProgram = kInvalidOpenGLProgram;
			newShaderData.mGeometryShader = kInvalidOpenGLShader;
			newShaderData.mVertexShader = kInvalidOpenGLShader;
			newShaderData.mFragmentShader = kInvalidOpenGLShader;

			bool validData = true;
			tbCore::Error::EnableThrowOnError();
			try
			{
				GenerateShaderFromOpenGL(newShaderData);
			}
			catch (...)
			{
				validData = false;
			}
			tbCore::Error::DisableThrowOnError();

			if (validData)
			{
				//Destroy the old shader.
				DestroyShaderFromOpenGL(shaderData);

				//Swap to the new shader.
				shaderData.mProgram = newShaderData.mProgram;
				shaderData.mGeometryShader = newShaderData.mGeometryShader;
				shaderData.mVertexShader = newShaderData.mVertexShader;
				shaderData.mFragmentShader = newShaderData.mFragmentShader;

				theShaderManager.BindShader(shaderHandle);
				shaderData.mUniformTable->RefreshAllUniforms(shaderData.mProgram);
			}
		}
	});

	tb_check_recent_gl_errors("ShaderManager::TryReloadingShader");
	tbGraphics::Implementation::Renderer::ClearErrors("TryReloadingShader");

	theShaderManager.BindShader(boundShader);
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::ShaderSystem::ShaderManager::PushAndBindShader(const ShaderHandle& shaderHandle)
{
	theShaderStack.push(theBoundShader);
	BindShader(shaderHandle);
	//The following reloads all the matrix uniforms to the shader.
	//TODO: Asteroids: 2025-10-21: Since we should be loading those uniforms into all loaded shaders, this
	//  should no longer be necessary. Not changing at this moment because it might be needed and I don't want to guess.
	tbGraphics::Implementation::Renderer::SetObjectToWorld(tbGraphics::Implementation::Renderer::GetObjectToWorld());
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::ShaderSystem::ShaderManager::PopShader(void)
{
	BindShader(theShaderStack.top());
	theShaderStack.pop();
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::ShaderSystem::ShaderManager::BindShader(const ShaderHandle& shaderHandle)
{
	tb_error_if(0 == theOriginalShaderProgram, "Error: Expected to have valid original shader program here.");

	if (theBoundShader == shaderHandle)
	{	//No need to set this if it is already set.
		return;
	}

	theBoundShader = shaderHandle;
	if (InvalidShader() == theBoundShader)
	{
		tb_check_gl_errors(glUseProgram(theOriginalShaderProgram));
	}
	else
	{
		const ShaderData& shaderData = theShaderCache.GetResourceReference(shaderHandle);
		tb_error_if(0 == shaderData.mProgram, "Error: Shader is being bound to invalid shader!?");
		tb_check_gl_errors(glUseProgram(shaderData.mProgram));

		ShaderSystem::theShaderManager.SetShaderUniform("diffuseTexture", 0);
	}
}

//--------------------------------------------------------------------------------------------------------------------//

const Asteroids::ShaderSystem::ShaderHandle& Asteroids::ShaderSystem::ShaderManager::GetCurrentShader(void) const
{
	return theBoundShader;
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::ShaderSystem::ShaderManager::ApplyUniformsForDraw(void)
{
	if (InvalidShader() != theBoundShader)
	{	//Do NOT try holding ShaderData pointer to the boundShader as the container in the ResourceCache could shift.
		Implementation::ShaderData& shaderData = theShaderCache.GetResourceReference(theBoundShader);
		shaderData.mUniformTable->ApplyUniformsForDraw();

		if (true) //This should indicate a shader uniform was not initialized from within code, something that is important to do.
		{
			const auto& activeUniforms = shaderData.mActiveUniforms;
			for (const auto& uniform : activeUniforms)
			{
				if (String::npos != uniform.second.mName.find('.'))
				{	//Don't care about structures or arrays of structures...
					continue;
				}

				if (String::npos != uniform.second.mName.find('['))
				{	//Don't care about arrays ???? It seemed ubInstancedObjectTransforms in no detail shader failed here as
					//a false negative; though this could be because it is a uniform buffer? ... Are all uniform arrays
					//technically a uniform buffer?  It was complaining of instancedTransforms[128] which is within the
					//ubInstancedObjectTransforms.
					continue;
				}

				//Do not care about some transforms that are _actually_ set using Turtle Brains matrix operations despite
				//not going through the ShaderManager SetUniform(), which fails this uniform initialization check.
				if ("uObjectToProjection" == uniform.second.mName) { continue; }

				//The following check only works for first OpenGL context created. The mLocation in UniformInfo is only
				//used for this particular debugging / early warning detection system and does NOT (exactly) match the
				//location of the UniformObject within the uniforms table. This detection system is useful as some GPUs
				//expect uniforms to be set from code, and this will ensure they were.
				//
				//The glContextsCreated hack allows us to ensure it only runs on the first context until the locations
				//match the uniform table locations better.
				tb_error_if(1 == Hacks::glContextsCreated && -1 == uniform.second.mLocation, "ShaderError: Expected uniform \"%s\" to be initialized by code and it is not. %s",
					uniform.second.mName.c_str(), shaderData.mCachedName.CombinedString().c_str());
			}
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::ShaderSystem::ShaderManager::SetShaderAttribute(const ShaderHandle& shaderHandle, const String& attributeName, const float value)
{
	int attributeLocation = 0;
	tb_check_gl_errors(glGetAttribLocation(theShaderCache.GetResourceReference(shaderHandle).mProgram, attributeName.c_str()));
	tb_check_gl_errors(glVertexAttrib1f(attributeLocation, value));
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::ShaderSystem::ShaderManager::SetShaderAttribute(const ShaderHandle& shaderHandle, const String& attributeName, const Vector2& value)
{
	int attributeLocation = 0;
	tb_check_gl_errors(glGetAttribLocation(theShaderCache.GetResourceReference(shaderHandle).mProgram, attributeName.c_str()));
	tb_check_gl_errors(glVertexAttrib2f(attributeLocation, value.x, value.y));
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::ShaderSystem::ShaderManager::SetShaderAttribute(const ShaderHandle& shaderHandle, const String& attributeName, const Vector3& value)
{
	int attributeLocation = 0;
	tb_check_gl_errors(glGetAttribLocation(theShaderCache.GetResourceReference(shaderHandle).mProgram, attributeName.c_str()));
	tb_check_gl_errors(glVertexAttrib3f(attributeLocation, value.x, value.y, value.z));
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::ShaderSystem::ShaderManager::SetShaderAttribute(const ShaderHandle& shaderHandle, const String& attributeName, const Vector4& value)
{
	int attributeLocation = 0;
	tb_check_gl_errors(glGetAttribLocation(theShaderCache.GetResourceReference(shaderHandle).mProgram, attributeName.c_str()));
	tb_check_gl_errors(glVertexAttrib4f(attributeLocation, value.x, value.y, value.z, value.w));
}

//--------------------------------------------------------------------------------------------------------------------//

//This returns the OpenGL location, yay for tightly-coupling with OpenGL, but meh.
int GetShaderUniformLocation(Asteroids::ShaderSystem::Implementation::ShaderData& shaderData, const Asteroids::String& uniformName)
{
	int uniformLocation = 0;
	tb_check_gl_errors(uniformLocation = glGetUniformLocation(shaderData.mProgram, uniformName.c_str()));
	if (uniformLocation != -1)
	{
		//This error condition represents a state of which the shader had a uniform by the given name, but it was not
		//discovered upon collecting the active uniforms for the shader during the LinkShader() process. Something
		//we are certainly assuming should always be collected there.
		//
		//Maybe we shouldn't assume this is okay as it appears some arrays don't do what we need them to do; arrays
		//seem to only show [0] in the list; perhaps just a special case if uniformName contains [] to replaces contents
		//with [0] before checking the uniforms... ugh.

		Asteroids::String arraySafeUniformName = uniformName;
		const std::size_t startIndex = uniformName.find_first_of('[');
		if (Asteroids::String::npos != startIndex)
		{
			arraySafeUniformName = uniformName.substr(0, startIndex + 1) + "0]";
		}

		auto uniformInformationItr = shaderData.mActiveUniforms.find(arraySafeUniformName);

		//if (uniformInformationItr != shaderData.mActiveUniforms.end())
		{
			tb_error_if(uniformInformationItr == shaderData.mActiveUniforms.end(), "Shader InteralError: Expected to find the uniform named: %s in shader %s",
				arraySafeUniformName.c_str(), shaderData.mCachedName.CombinedString().c_str());
			uniformInformationItr->second.mLocation = uniformLocation;
		}
	}

	return uniformLocation;
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::ShaderSystem::ShaderManager::SetShaderUniform(const ShaderHandle& shaderHandle,
	const String& uniformName, const int value)
{
	Implementation::ShaderData& shaderData = theShaderCache.GetResourceReference(shaderHandle);
	if (false == shaderData.mUniformTable->FindAndSetUniform(uniformName, value))
	{
		int uniformLocation = GetShaderUniformLocation(shaderData, uniformName);
		shaderData.mUniformTable->CreateUniform(uniformName, uniformLocation, value);
	}
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::ShaderSystem::ShaderManager::SetShaderUniform(const ShaderHandle& shaderHandle,
	const String& uniformName, const float value)
{
	SetShaderUniform(shaderHandle, uniformName, &value, 1);
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::ShaderSystem::ShaderManager::SetShaderUniform(const ShaderHandle& shaderHandle,
	const String& uniformName, const Vector2& value)
{
	SetShaderUniform(shaderHandle, uniformName, static_cast<const float*>(value), 2);
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::ShaderSystem::ShaderManager::SetShaderUniform(const ShaderHandle& shaderHandle,
	const String& uniformName, const Vector3& value)
{
	SetShaderUniform(shaderHandle, uniformName, static_cast<const float*>(value), 3);
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::ShaderSystem::ShaderManager::SetShaderUniform(const ShaderHandle& shaderHandle,
	const String& uniformName, const Vector4& value)
{
	SetShaderUniform(shaderHandle, uniformName, static_cast<const float*>(value), 4);
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::ShaderSystem::ShaderManager::SetShaderUniform(const ShaderHandle& shaderHandle,
	const String& uniformName,const Matrix3& value, bool transposeOnPassing)
{
	tb_error_if(false == transposeOnPassing, "This is not longer supported, so, whoops.");
	SetShaderUniform(shaderHandle, uniformName, static_cast<const float*>(value), 9);
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::ShaderSystem::ShaderManager::SetShaderUniform(const ShaderHandle& shaderHandle,
	const String& uniformName, const Matrix4& value, bool transposeOnPassing)
{
	tb_error_if(false == transposeOnPassing, "This is not longer supported, so, whoops.");
	SetShaderUniform(shaderHandle, uniformName, static_cast<const float*>(value), 16);
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::ShaderSystem::ShaderManager::SetShaderUniform(const ShaderHandle& shaderHandle,
	const String& uniformName, const float* values, tbCore::uint8 numberOfValues)
{
	Implementation::ShaderData& shaderData = theShaderCache.GetResourceReference(shaderHandle);
	if (false == shaderData.mUniformTable->FindAndSetUniform(uniformName, values, numberOfValues))
	{
		int uniformLocation = GetShaderUniformLocation(shaderData, uniformName);
		shaderData.mUniformTable->CreateUniform(uniformName, uniformLocation, values, numberOfValues);
	}
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::ShaderSystem::ShaderManager::SetShaderUniform(const String& uniformName, const float* values, tbCore::uint8 numberOfValues)
{
	theShaderCache.ForEachValidResource([this, uniformName, values, numberOfValues](Implementation::ShaderData& /*shaderData*/, ShaderHandle handle) {
		SetShaderUniform(handle, uniformName, values, numberOfValues);
	});
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::ShaderSystem::ShaderManager::SetShaderUniform(const String& uniformName, const int value)
{
	theShaderCache.ForEachValidResource([this, uniformName, value](Implementation::ShaderData& /*shaderData*/, ShaderHandle handle) {
		SetShaderUniform(handle, uniformName, value);
	});
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::ShaderSystem::ShaderManager::CreateGraphicsContext(void)
{
	tb_error_if(0 != theOriginalShaderProgram, "Expected theOriginalShaderProgram to be in an invalid state.");
	tb_check_gl_errors(glGetIntegerv(GL_CURRENT_PROGRAM, &theOriginalShaderProgram));

	++Hacks::glContextsCreated;

	theShaderCache.ForEachValidResource([](ShaderData& shaderData, ShaderHandle shaderHandle) {
		tb_unused(shaderHandle);

		//2019-02-13: Replacing internal caching with ResourceCache; I believe the following
		//name sets are unnecessary, although if there are bugs with context swapping this would
		//be a place to look.

		//Wasn't already loaded, so lets load it up!
		//shaderData.mCachedName.mGeometryShaderFile = cacheEntry.first.mGeometryShaderFile;
		//shaderData.mCachedName.mVertexShaderFile = cacheEntry.first.mVertexShaderFile;
		//shaderData.mCachedName.mFragmentShaderFile = cacheEntry.first.mFragmentShaderFile;
		//shaderData.mReferences = 1;
		shaderData.mProgram = kInvalidOpenGLProgram;
		shaderData.mVertexShader = kInvalidOpenGLShader;
		shaderData.mFragmentShader = kInvalidOpenGLShader;
		shaderData.mGeometryShader = kInvalidOpenGLShader;

		Implementation::GenerateShaderFromOpenGL(shaderData);
		tb_debug_log_if(false == shaderData.mCachedName.mIsShaderSource, LogShader::Info() << "Reloading Shader Program:\n\t\'" <<
			shaderData.mCachedName.mGeometryShaderFile << "'\n\t'" << shaderData.mCachedName.mVertexShaderFile <<
			"'\n\t'" << shaderData.mCachedName.mFragmentShaderFile << "'\n");
		tb_debug_log_if(true == shaderData.mCachedName.mIsShaderSource, LogShader::Info() << "Reloading Shader Program:\n\t\"Data Shader\"\n");

		shaderData.mUniformTable->RefreshAllUniforms(shaderData.mProgram);
	});

	// Asteroids is unlikely to have a internal shaders, it is afterall the project and not an engine...
	//if (InvalidShader() == RustyShaders::theSkyShader)
	//{
	//	LoadInternalShaders();
	//}
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::ShaderSystem::ShaderManager::DestroyGraphicsContext(void)
{
	theShaderCache.ForEachValidResource([](ShaderData& shaderData, ShaderHandle shaderHandle) {
		tb_unused(shaderHandle);
		DestroyShaderFromOpenGL(shaderData);
	});

	theOriginalShaderProgram = 0;
}

//--------------------------------------------------------------------------------------------------------------------//

int Asteroids::ShaderSystem::Implementation::CompileShader(const String& shaderSource, const ShaderType& shaderType, const String& shaderFilename)
{
	GLuint shaderId = kInvalidOpenGLShader;
	switch (shaderType)
	{
	case kVertexShader: shaderId = glCreateShader(GL_VERTEX_SHADER); break;
	case kFragmentShader: shaderId = glCreateShader(GL_FRAGMENT_SHADER); break;
#if defined(tb_web)
	case kGeometryShader: return kInvalidOpenGLShader;
#else
	case kGeometryShader: shaderId = glCreateShader(GL_GEOMETRY_SHADER); break;
#endif
	};

	const GLchar* data = &shaderSource[0];

	glShaderSource(shaderId, 1, &data, NULL);
	glCompileShader(shaderId);
	tb_check_recent_gl_errors("ShaderManager::CompileShader");
	tb_error_if(false == tbGraphics::Implementation::Renderer::DidShaderCompile(shaderId), "ShaderManager: Failed to compile shader '%s' of type(%d).\n",
		shaderFilename.c_str(), shaderType);

	tbGraphics::Implementation::Renderer::ClearErrors("Compile Shader");
	return shaderId;
}

//--------------------------------------------------------------------------------------------------------------------//

int Asteroids::ShaderSystem::Implementation::LinkShader(ShaderData& shaderData)
{
	if (kInvalidOpenGLShader == shaderData.mVertexShader || kInvalidOpenGLShader == shaderData.mFragmentShader)
	{
		tb_error("Shader InternalError: Expected to have valid vertex and fragment shaders.");
		return kInvalidOpenGLProgram;
	}

	GLuint programId = glCreateProgram();

	if (kInvalidOpenGLShader != shaderData.mGeometryShader)
	{
		tb_check_gl_errors(glAttachShader(programId, shaderData.mGeometryShader));
	}

	tb_check_gl_errors(glAttachShader(programId, shaderData.mVertexShader));
	tb_check_gl_errors(glAttachShader(programId, shaderData.mFragmentShader));

	tb_check_gl_errors(glBindAttribLocation(programId, 0, "vsPosition"));
	tb_check_gl_errors(glBindAttribLocation(programId, 1, "vsNormal"));
	tb_check_gl_errors(glBindAttribLocation(programId, 2, "vsColor"));
	tb_check_gl_errors(glBindAttribLocation(programId, 3, "vsTextureUV"));
	tb_check_gl_errors(glBindAttribLocation(programId, 4, "vsBoneIndex"));

	tb_check_gl_errors(glLinkProgram(programId));	//gDEBugger claims this is passing an Invalid parameter, not created by OpenGL.
	tb_check_recent_gl_errors("ShaderManager::LinkShader");

	if (false == tbGraphics::Implementation::Renderer::DidProgramLink(programId))
	{
		tb_error("Shader Error: ShaderManager Failed to link shader program \"%s\" + \"%s\" + \"%s\"", shaderData.mCachedName.mGeometryShaderFile.c_str(),
			shaderData.mCachedName.mVertexShaderFile.c_str(), shaderData.mCachedName.mFragmentShaderFile.c_str());
		glDeleteProgram(programId);
		return kInvalidOpenGLProgram;
	}

	{	//Find all the available uniforms
		shaderData.mActiveUniforms.clear();

		int totalUniformCount = 0;
		tb_check_gl_errors(glGetProgramiv(programId, GL_ACTIVE_UNIFORMS, &totalUniformCount));

		int uniformNameLength = 0;
		int maximumNameLength = 0;
		tb_check_gl_errors(glGetProgramiv(programId, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maximumNameLength));
		std::unique_ptr<char[]> nameOfUniformVariable(new char[maximumNameLength + 1]);

		int sizeOfUniformVariable = 0;
		GLenum typeOfUniformVariable = 0;

		tb_debug_log(tbiLog::Trace() << "Active Shader Uniforms:\n" << tbCore::Debug::ContinueEntry());
		for (int uniformIndex = 0; uniformIndex < totalUniformCount; ++uniformIndex)
		{
			tb_check_gl_errors(glGetActiveUniform(programId, uniformIndex, maximumNameLength, &uniformNameLength, &sizeOfUniformVariable,
				&typeOfUniformVariable, nameOfUniformVariable.get()));
			nameOfUniformVariable[uniformNameLength] = '\0';

			tb_debug_log("\t" << nameOfUniformVariable << "\n" << tbCore::Debug::ContinueEntry());
			shaderData.mActiveUniforms.insert(std::pair<String, ShaderData::UniformInfo>{ nameOfUniformVariable.get(),
				{ nameOfUniformVariable.get(), -1, tbCore::RangedCast<size_t>(sizeOfUniformVariable), typeOfUniformVariable, false } });
		}
		tb_debug_log(""); //end the entry.
	}

	tbGraphics::Implementation::Renderer::ClearErrors("LinkShader");
	return programId;
}

//--------------------------------------------------------------------------------------------------------------------//

Asteroids::String Asteroids::ShaderSystem::Implementation::LoadShaderSourceFromFile(const String& filename)
{
	String shaderSource = tbCore::FileUtilities::LoadFileContentsToString(filename);
//#if defined(rusty_development)
//	if (false == Implementation::theEngineShadersDirectory.empty() &&
//		true == tbCore::StringContains(filename, Implementation::theEngineShadersDirectory))
//	{
//		// 2025-01-30: TODO: InternalCombustion: Optimization: This could use something like RemoveAllInstanceOf() which
//		//   could also offer a InPlace() version. The Replace doesn't do that because the new contents may be larger.
//		shaderSource = tbCore::String::ReplaceAllInstancesOf(shaderSource, "R\"(", ""); //find and remove R"(
//		shaderSource = tbCore::String::ReplaceAllInstancesOf(shaderSource, ")\"", "");  //find and remove )"
//	}
//#endif /* debug or development build */

	return shaderSource;

	// 2025-01-30: Prior to this day we were loading directly like so, idk why the unique_ptr<char[]> but, TurtleBrains
	//   now has a LoadFileContentsToString() function and might as well leverage that!! One other difference noticed
	//   was the below specified std::ios::in while LoadFileContentsToString() doesn't but that should be default.
	//
	//std::ifstream file(filename.c_str(), std::ios::in);
	//if (true == file.is_open() && true == file.good())
	//{
	//	file.seekg(0, std::ios::end);
	//	size_t fileLength = static_cast<size_t>(file.tellg());
	//	file.seekg(std::ios::beg);

	//	std::unique_ptr<char[]> shaderSource(new char[fileLength + 1]);
	//	if (nullptr != shaderSource)
	//	{
	//		shaderSource[fileLength] = '\0';

	//		unsigned int i = 0;
	//		int value = 0;
	//		while (true == file.good())
	//		{
	//			value = file.get();
	//			shaderSource[i] = tbCore::RangedCast<char>(value);
	//			++i;
	//		}

	//		shaderSource[i - 1] = '\0';

	//		return String(shaderSource.get());
	//	}

	//	file.close();
	//}

	//return "";
}


//--------------------------------------------------------------------------------------------------------------------//
//--------------------------------------------------------------------------------------------------------------------//
//--------------------------------------------------------------------------------------------------------------------//

size_t CountTheLines(const Asteroids::String& contents)
{
	const Asteroids::String& token = "\n";

	size_t count = 0;

	size_t currentIndex(0);
	size_t previousIndex(0);
	while (std::string::npos != (currentIndex = contents.find(token, previousIndex)))
	{
		previousIndex = currentIndex + token.size();
		++count;
	}
	return count;
}

//--------------------------------------------------------------------------------------------------------------------//

Asteroids::String LoadIncludedShaderSource(const Asteroids::String& filename)
{


//#if defined(rusty_development)
//#if !defined(tb_web)
//	if (String::npos != filename.find("shader_api.vert") ||
//		String::npos != filename.find("shader_api.frag"))
//	{
//		if (false == Implementation::theEngineShadersDirectory.empty())
//		{
//			const String& shaderPath = Implementation::theEngineShadersDirectory;
//			return LoadShaderSourceFromFile(shaderPath + filename);
//		}
//	}
//#endif
//#endif
//
//
//	if (String::npos != filename.find("shader_api.vert"))
//	{
//		return String(
//#if defined(tb_web)
//#include "implementation/shaders_gles3_0/shader_api.vert"
//#else
//#include "implementation/shaders_gl3_2/shader_api.vert"
//#endif /* tb_web */
//		);
//	}
//	else if (String::npos != filename.find("shader_api.frag"))
//	{
//		return String(
//#if defined(tb_web)
//#include "implementation/shaders_gles3_0/shader_api.frag"
//#else
//#include "implementation/shaders_gl3_2/shader_api.frag"
//#endif /* tb_web */
//		);
//	}

	return LoadShaderSourceFromFile(filename);
}

//--------------------------------------------------------------------------------------------------------------------//

Asteroids::String Asteroids::ShaderSystem::Implementation::PreprocessShaderSource(const String& shaderSource)
{
	//This is assuming #include has a whitespace of exactly one SPACE character, followed by double-quote.
	//Also, can't exactly comment out a #include... so, yay.
	const String includeToken = "#include \"";

	size_t previousIndex(0);
	size_t includePosition = shaderSource.find(includeToken);
	if (String::npos == includePosition)
	{
		return shaderSource;
	}

	int shaderIncludeIndex = 1;
	String actualShaderSource;

	do
	{
		const size_t startQuotePosition = includePosition + includeToken.size();
		const size_t finalQuotePosition = shaderSource.find("\"", startQuotePosition + 1);
		tb_error_if(String::npos == finalQuotePosition, "Could not #include from the shader, no end quote found.");

		const String includeFile = shaderSource.substr(startQuotePosition, finalQuotePosition - startQuotePosition);
		tb_debug_log(tbiLog::Debug() << "Including a shader: '" << includeFile  << "'.");

		actualShaderSource += shaderSource.substr(previousIndex, includePosition);
		const size_t oldLineCount = CountTheLines(actualShaderSource);

		actualShaderSource += "#line 0 1\n";
		actualShaderSource += LoadIncludedShaderSource(includeFile);
		actualShaderSource += "#line " + tb_string(oldLineCount + 1) + " " + tb_string(shaderIncludeIndex) + "\n";
		actualShaderSource += shaderSource.substr(finalQuotePosition + 1);

		++shaderIncludeIndex;
		previousIndex = finalQuotePosition;
	} while (std::string::npos != (includePosition = shaderSource.find(includeToken, previousIndex)));

	return actualShaderSource;
}

//--------------------------------------------------------------------------------------------------------------------//
//--------------------------------------------------------------------------------------------------------------------//
//--------------------------------------------------------------------------------------------------------------------//

Asteroids::ShaderSystem::ShaderHandle Asteroids::ShaderSystem::InvalidShader(void)
{
	return ShaderHandle();
}

//--------------------------------------------------------------------------------------------------------------------//
