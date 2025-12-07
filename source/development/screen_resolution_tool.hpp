///
/// @file
/// @details Some quick tools and helpers for adjusting the screen resolution in real-time for development purposes.
///
/// <!-- Copyright (c) 2025 Tyre Bytes LLC - All Rights Reserved -->
///------------------------------------------------------------------------------------------------------------------///

#ifndef Undefined_ScreenResolutionTool_hpp
#define Undefined_ScreenResolutionTool_hpp

#include "asteroids.hpp"

namespace Asteroids::Development
{
	///
	/// @details If certain key-combinations are pressed this will update the resolution in TurtleBrains to a different
	///   size or full-screen/windowed state.
	///
	/// @note The buttons used currently are F1-F8 with CTRL and/or SHIFT pressed.
	///
	void UpdateDeveloperResolution(void);

	bool IsRenderingLayout(void);

	void RenderSafeZone(void);

}; /* Asteroids::Development */

#endif /* Undefined_ScreenResolutionTool_hpp */
