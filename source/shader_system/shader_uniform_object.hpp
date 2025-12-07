///
/// @file
/// @details This is currently in early development and will be properly documented at a later date once
///   the details are more concrete.  TODO: TIM: Documentation: Finish this documentation before release.
///
/// <!-- Copyright (c) Tyre Bytes LLC 2016 - All Rights Reserved -->
///------------------------------------------------------------------------------------------------------------------///

#ifndef Asteroids_ShaderUniformObject_hpp
#define Asteroids_ShaderUniformObject_hpp

#include <turtle_brains/math//tb_vector.hpp>
#include <turtle_brains/math//tb_matrix.hpp>
#include <turtle_brains/core/tb_noncopyable.hpp>
#include <turtle_brains/core/tb_opengl.hpp>
#include <turtle_brains/core/tb_types.hpp>

#include <array>
#include <vector>
#include <unordered_map>
#include <memory>

#if defined(tb_legacy_gl_forced)
#error Legacy OpenGL is NOT supported with the shader stuff.
#endif /* tb_legacy_gl_forced */

namespace Asteroids::ShaderSystem::Implementation
{
	class CleanableInterface
	{
	public:
		virtual ~CleanableInterface(void)
		{
		}

		virtual void OnClean(void) = 0;
	};

	class CleanableObserverInterface
	{
	public:
		virtual ~CleanableObserverInterface(void)
		{
		}

		virtual void OnNotifyDirty(CleanableInterface& cleanable) = 0;
	};

	class UniformObject : public CleanableInterface
	{
	public:
		UniformObject(int uniformLocation, const float *values, const tbCore::uint8 valueSize, CleanableObserverInterface* observer) :
			mFloatValues(),
			mIntegerValue(0),
			mObserver(observer),
			mUniformLocation(uniformLocation),
			mValueSize(valueSize),
			mIsDirty(true)
		{
			tb_error_if(valueSize > 16, "iceInternalError: Unexpected size, will cause buffer-overrun.");
			memcpy(mFloatValues.data(), values, mValueSize * sizeof(float));

			tb_error_if(nullptr == observer, "Expected a valid CleanableObserver.");
			mObserver->OnNotifyDirty(*this);
		}

		UniformObject(int uniformLocation, tbCore::int32 value, CleanableObserverInterface* observer) :
			mFloatValues(),
			mIntegerValue(value),
			mObserver(observer),
			mUniformLocation(uniformLocation),
			mValueSize(0),
			mIsDirty(true)
		{
			tb_error_if(nullptr == observer, "Expected a valid CleanableObserver.");
			mObserver->OnNotifyDirty(*this);
		}

		virtual ~UniformObject(void)
		{
		}

		void RefreshUniformLocation(int openglProgramId, const String& uniformName);
		virtual void OnClean(void) override;

		void SetValue(const float* values, tbCore::uint8 valueSize)
		{
			tb_error_if(valueSize != mValueSize, "iceInternalError: Expected size match with uniform values.");

			if (false == mIsDirty)
			{
				bool foundDifference = false;
				for (int index = 0; index < mValueSize; ++index)
				{
					if (mFloatValues[index] != values[index])
					{
						foundDifference = true;
						break;
					}
				}

				if (true == foundDifference)
				{
					memcpy(mFloatValues.data(), values, mValueSize * sizeof(float));
					mObserver->OnNotifyDirty(*this);
					mIsDirty = true;
				}
			}
			else
			{
				memcpy(mFloatValues.data(), values, mValueSize * sizeof(float));
			}
		}

		void SetValue(const tbCore::int32 value)
		{
			tb_error_if(0 != mValueSize, "iceInternalError: Expected size match with uniform values.");

			if (false == mIsDirty && mIntegerValue != value)
			{
				mObserver->OnNotifyDirty(*this);
				mIsDirty = true;
			}

			mIntegerValue = value;
		}

	private:
		std::array<float, 16> mFloatValues;
		tbCore::int32 mIntegerValue;
		CleanableObserverInterface* mObserver;
		tbCore::int32 mUniformLocation;
		const tbCore::uint8 mValueSize;
		bool mIsDirty;
	};



	class ShaderUniformTable : public CleanableObserverInterface
	{
	public:
		ShaderUniformTable(void)
		{
		}

		virtual ~ShaderUniformTable(void)
		{
		}

		virtual void OnNotifyDirty(CleanableInterface& cleanable)
		{
			mDirtyUniforms.push_back(&cleanable);
		}

		void ApplyUniformsForDraw(void)
		{
			for (auto& uniform : mDirtyUniforms)
			{
				uniform->OnClean();
			}

			mDirtyUniforms.clear();
		}

		void RefreshAllUniforms(int openglProgramId)
		{
			for (auto& uniformPair : mUniforms)
			{
				uniformPair.second->RefreshUniformLocation(openglProgramId, uniformPair.first);
			}
		}

		void CreateUniform(const String& uniformName, tbCore::int32 uniformLocation, const float* uniformValues, tbCore::uint8 valueSize)
		{
			std::unique_ptr<UniformObject> uniformObject = std::unique_ptr<UniformObject>(new UniformObject(uniformLocation, uniformValues, valueSize, this));
			mUniforms.emplace(uniformName, std::move(uniformObject));
		}

		void CreateUniform(const String& uniformName, tbCore::int32 uniformLocation, tbCore::uint32 value)
		{
			std::unique_ptr<UniformObject> uniformObject = std::unique_ptr<UniformObject>(new UniformObject(uniformLocation, value, this));
			mUniforms.emplace(uniformName, std::move(uniformObject));
		}

		bool FindAndSetUniform(const String& uniformName, const float* uniformValues, tbCore::uint8 valueSize)
		{
			const auto& uniformIterator = mUniforms.find(uniformName);
			if (uniformIterator != mUniforms.end())
			{
				uniformIterator->second->SetValue(uniformValues, valueSize);
				return true;
			}

			return false;
		}

		bool FindAndSetUniform(const String& uniformName, tbCore::uint32 value)
		{
			const auto& uniformIterator = mUniforms.find(uniformName);
			if (uniformIterator != mUniforms.end())
			{
				uniformIterator->second->SetValue(value);
				return true;
			}

			return false;
		}

	private:
		std::vector<CleanableInterface*> mDirtyUniforms;
		std::unordered_map<String, std::unique_ptr<UniformObject>> mUniforms;
	};

}; /* namespace Asteroids::ShaderSystem::Implementation */

#endif /* Asteroids_ShaderUniformObject_hpp */
