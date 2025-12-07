///
/// @file
/// @details Provide some information on the version of Asteroids.
///
/// <!-- Copyright (c) 2025 Tyre Bytes LLC - All Rights Reserved -->
///------------------------------------------------------------------------------------------------------------------///

#ifndef Asteroids_Version_hpp
#define Asteroids_Version_hpp

#include "utilities/string_table.hpp"

#include <turtle_brains/core/tb_types.hpp>
#include <turtle_brains/core/tb_string.hpp>

namespace Asteroids::Version
{

	constexpr tbCore::uint32 Major(void)
	{
#if defined(build_major)
		return tbCore::uint32(build_major);
#else
		return 0;
#endif
	}

	constexpr tbCore::uint32 Minor(void)
	{
#if defined(build_minor)
		return tbCore::uint32(build_minor);
#else
		return 0;
#endif
	}

	constexpr tbCore::uint32 Patch(void)
	{
#if defined(build_patch)
		return tbCore::uint32(build_patch);
#else
		return 0;
#endif
	}

	inline String BuildTag(void)
	{
#if defined(tb_headless_build)
		const static String headlessTag("-headless");
#else
		const static String headlessTag("");
#endif

#if defined(tb_debug_build)
		return tb_string(headlessTag + "-debug");
#elif defined(tb_development_build)
		return tb_string(headlessTag + "-dev");
#elif defined(tb_release_build)
		return tb_string(headlessTag + "-rel");
#elif defined(tb_public_build)
		return tb_string(headlessTag + "");
#else
#error Unknown build specificiers.
#endif
	}

	inline String VersionString(void)
	{
		const String separator(".");

		return tb_string(Major()) + separator + tb_string(Minor()) + separator + tb_string(Patch()) + BuildTag();
	}

	inline String ProjectVersionString(void)
	{
#if defined(asteroids_demo_build)
		return Localize(StringKey::GameDemo_Title) + " v" + VersionString();
#else
		return Localize(StringKey::Game_Title) + " v" + VersionString();
#endif
	}

};	//namespace Asteroids::Version

#endif /* Asteroids_Version_hpp */
