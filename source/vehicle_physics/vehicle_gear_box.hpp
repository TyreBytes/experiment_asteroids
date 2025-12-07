///
/// @file
/// @details A simple helper to take simulate a basic gear box for a vehicle.
///
/// <!-- Copyright (c) 2025 Tyre Bytes LLC - All Rights Reserved -->
///------------------------------------------------------------------------------------------------------------------///

#ifndef Asteroids_VehicleGearBox_hpp
#define Asteroids_VehicleGearBox_hpp

#include "../asteroids.hpp"

#include <utility>

namespace Asteroids::VehiclePhysics
{
	enum class Gear : tbCore::uint8 { Neutral, First, Second, Third, Fourth, Fifth, Sixth, Reverse };

	inline std::ostream& operator<<(std::ostream& output, const Gear gear)
	{
		switch (gear)
		{
		case Gear::Neutral: output << "N"; break;
		case Gear::First: output << "1"; break;
		case Gear::Second: output << "2"; break;
		case Gear::Third: output << "3"; break;
		case Gear::Fourth: output << "4"; break;
		case Gear::Fifth: output << "5"; break;
		case Gear::Sixth: output << "6"; break;
		case Gear::Reverse: output << "R"; break;
		};
		return output;
	}

	struct VehicleGearBox
	{
		std::array<Scalar, 8> mGearRatios = {//Gear Ratios for ... you guessed it, the 1999 Apex5
			Scalar(0.0), Scalar(3.163), Scalar(1.888), Scalar(1.333), Scalar(1.000), Scalar(0.814), Scalar(0.0), Scalar(-3.163),
		};
		Scalar mFinalRatio = Scalar(4.10);

		Gear mCurrentGear = Gear::Neutral;
		Gear mMaximumGear = Gear::Sixth;
		bool mCanShift = true;
		bool mIsAutomatic = true;

		explicit VehicleGearBox(const Gear maximumGear = Gear::Fifth) :
			mMaximumGear(maximumGear)
		{
		}

		inline void Reset(void)
		{
			mCurrentGear = Gear::Neutral;
			mCanShift = true;
		}

		///
		/// @details Given an engineSpeed (rpms), what would be the speed of the vehicle if in the current gear. This will
		///   return 0.0f if the car is in neutral.
		///
		inline Scalar CalculateVehicleSpeed(const Scalar& engineSpeed, const Scalar wheelRadius) const
		{
			if (Gear::Neutral == mCurrentGear)
			{
				return Scalar(0.0);
			}

			const Scalar engineSpeedRadiansSecond = tbMath::Convert::RevolutionsMinuteToRadiansSecond(engineSpeed);
			const Scalar wheelSpeed = engineSpeedRadiansSecond / (GetFinalGearRatio() * GetCurrentGearRatio());
			const Scalar vehicleSpeed = wheelSpeed * wheelRadius;
			return vehicleSpeed;
		}

		inline Scalar CalculateWheelTorque(const Scalar& engineTorque) const
		{
			return engineTorque * GetCurrentGearRatio() * GetFinalGearRatio();
		}

		///
		/// @note 2024-09-04: Not saying this is right, but, it was how OG auto shifting worked :D
		///
		inline Gear SimulateAutomaticShifting(const Scalar vehicleGroundSpeed, Scalar& throttle, Scalar& brake)
		{
			if (true == mIsAutomatic)
			{
				if (vehicleGroundSpeed < 0.01f)
				{
					mCurrentGear = Gear::Neutral;
				}

				if (Gear::Neutral == mCurrentGear)
				{
					if (brake > Scalar(0.1) && brake > throttle)
					{
						mCurrentGear = Gear::Reverse;
					}
					else if (throttle > Scalar(0.1))
					{
						mCurrentGear = Gear::First;
					}
				}

				if (Gear::Reverse == mCurrentGear)
				{
					std::swap(throttle, brake);
				}
			}

			return mCurrentGear;
		}

		///
		/// @note engineSpeed is in and out!
		///
		inline Scalar SimulateGearBox(const Scalar engineSpeed, const Scalar wheelRadius, const Scalar vehicleSpeed,
			const bool isShiftingUp = false, const bool isShiftingDown = false)
		{
			bool automaticShiftUp = false;
			bool automaticShiftDown = false;

			if (true == mIsAutomatic && Gear::Reverse != mCurrentGear)
			{
				if (engineSpeed > 5000)
				{
					automaticShiftUp = true;
				}
				if (engineSpeed < 3000)
				{
					if (Gear::First != mCurrentGear && Gear::Neutral != mCurrentGear && Gear::Reverse != mCurrentGear)
					{
						automaticShiftDown = true;
					}
				}
			}

			if (true == mCanShift)
			{
				if (true == automaticShiftUp || true == isShiftingUp)
				{
					if (mCurrentGear < mMaximumGear)
					{
						mCurrentGear = static_cast<Gear>(static_cast<int>(mCurrentGear) + 1);
					}
					else if (mCurrentGear == Gear::Reverse)
					{
						mCurrentGear = Gear::Neutral;
					}

					mCanShift = false;
				}
				if (true == automaticShiftDown || true == isShiftingDown)
				{
					if (mCurrentGear == Gear::Neutral)
					{
						mCurrentGear = Gear::Reverse;
					}
					else if (mCurrentGear > Gear::Neutral && mCurrentGear != Gear::Reverse)
					{
						mCurrentGear = static_cast<Gear>(static_cast<int>(mCurrentGear) - 1);
					}

					mCanShift = false;
				}
			}
			else
			{
				if (false == isShiftingUp && false == isShiftingDown)
				{
					mCanShift = true;
				}
			}

			//Set the engine speed based on vehicle speed and current gear, it isn't perfect, but we don't have engine inertia yet.
			if (Gear::Neutral == mCurrentGear)
			{	//This isn't correct, but for a faux gearbox it will work for today.
				return Scalar(0.0);
			}

			const Scalar reverseGearRatio = Scalar(3.163);
			const Scalar gearRatio = (vehicleSpeed < 0.0f) ? -reverseGearRatio : GetCurrentGearRatio();
			const Scalar wheelSpeed = vehicleSpeed / wheelRadius;
			return tbMath::Convert::RadiansSecondToRevolutionsMinute(wheelSpeed * GetFinalGearRatio() * gearRatio);
		}

		Scalar GetCurrentGearRatio(void) const
		{
			return mGearRatios[static_cast<int>(mCurrentGear)];
		}

		Scalar GetFinalGearRatio(void) const
		{
			return mFinalRatio;
		}

		void SetFinalGearRatio(const Scalar finalRatio)
		{
			mFinalRatio = finalRatio;
		}

		void SetGearRatio(const Gear gear, const Scalar gearRatio)
		{
			Scalar ackshuallyRatio = gearRatio;

			if (Gear::Reverse == gear && gearRatio > Scalar(0.0))
			{
				ackshuallyRatio = -gearRatio;
			}
			if (Gear::Neutral == gear)
			{
				ackshuallyRatio = 0.0f;
			}

			mGearRatios[static_cast<int>(gear)] = ackshuallyRatio;
		}

	};

};	//namespace Asteroids::VehiclePhysics

#endif /* Asteroids_VehicleGearBox_hpp */
