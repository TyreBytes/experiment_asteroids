///
/// @file
/// @details Displays a shifter at mouse position for shitfting feedback.
///
/// <!-- Copyright (c) 2025 Tyre Bytes LLC - All Rights Reserved -->
///------------------------------------------------------------------------------------------------------------------///

#ifndef Asteroids_EventBox_hpp
#define Asteroids_EventBox_hpp

#include "../graphics/rounded_box_shape.hpp"

#include <turtle_brains/graphics/tb_graphic.hpp>
#include <turtle_brains/graphics/tb_sprite.hpp>

namespace Asteroids
{

	class EventBox : public tbGraphics::GraphicList
	{
	public:
		EventBox(void);
		virtual ~EventBox(void);

		inline Vector2 FakeGetAnchorPosition(const tbGraphics::AnchorLocation& anchor,
			const float offsetX, const float offsetY) const {
			return mBorderBox.GetAnchorPosition(anchor, offsetX, offsetY);
		}

		inline void SetDarkenBackdrop(const bool darkenBackdrop) { mDarkenBackdrop = darkenBackdrop; }

		void SetColor(const Color& innerColor, const Color& borderColor);

		/// @note: 2025-11-05: This is a hack to get things to work, see the note inside OnUpdate().
		inline tbMath::Vector2 GetAnchorPosition(const tbGraphics::AnchorLocation& anchor,
			const float offsetX, const float offsetY) const
		{
			return mBorderBox.GetAnchorPosition(anchor, offsetX, offsetY);
		}

		/// @note: 2025-11-05: This is a hack to get things to work, see the note inside OnUpdate().
		inline tbMath::Vector2 GetAnchorPosition(const tbGraphics::AnchorLocation& anchor,
			const tbMath::Vector2& offset = tbMath::Vector2::Zero()) const
		{
			return mBorderBox.GetAnchorPosition(anchor, offset);
		}

	protected:
		virtual void OnUpdate(const float deltaTime) override;
		virtual void OnRender(void) const override;

	private:
		tbGraphics::BoxShape mDarkenBox;
		RoundedBoxShape mBorderBox;
		bool mDarkenBackdrop;
	};

};	//namespace Asteroids

#endif /* Asteroids_RacercarShift_hpp */
