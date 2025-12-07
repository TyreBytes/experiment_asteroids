///
/// @file
/// @details This is using the tbGame::GameTimer API but with a floating point type representation of the timer. It is
///   slighly less specific, but allows easier modification of time multipliers. Perhaps this can go into TurtleBrains
///   in the future.
///
/// <!-- Copyright (c) 2025 Tyre Bytes LLC - All Rights Reserved -->
///------------------------------------------------------------------------------------------------------------------///

#include "../utilities/rusty_timer.hpp"
#include "../asteroids.hpp"
#include "../game_manager.hpp"

#include <limits>

//Asteroids::RustyTimer::Seconds Asteroids::RustyTimer::sSecondsPerStep(Asteroids::kFixedTime);
Asteroids::RustyTimer::Seconds Asteroids::RustyTimer::sSecondsPerStep(0.01f);

Asteroids::RustyTimer Asteroids::RustyTimer::Zero(void) { return RustyTimer(0.0); }
Asteroids::RustyTimer Asteroids::RustyTimer::Infinity(void) { return RustyTimer(std::numeric_limits<Seconds>::infinity()); }

namespace tbImplementation
{
#if defined(tb_with_audio) //Found in tb_audio_manager.cpp
	extern tbCore::uint32 tbiAudioMillisecondsPerStep;
#endif /* tb_with_audio */
};

//--------------------------------------------------------------------------------------------------------------------//

Asteroids::RustyTimer::RustyTimer(void) :
	mSecondTimer(0.0)
{
}

//--------------------------------------------------------------------------------------------------------------------//

Asteroids::RustyTimer::RustyTimer(const Milliseconds& valueInMilliseconds) :
	mSecondTimer(ToSeconds(valueInMilliseconds))
{
}

//--------------------------------------------------------------------------------------------------------------------//

Asteroids::RustyTimer::RustyTimer(const Seconds& valueSeconds) :
	mSecondTimer(valueSeconds)
{
}

//--------------------------------------------------------------------------------------------------------------------//

Asteroids::RustyTimer::RustyTimer(const RustyTimer& other) :
	mSecondTimer(other.mSecondTimer)
{
}

//--------------------------------------------------------------------------------------------------------------------//

Asteroids::RustyTimer::~RustyTimer(void)
{
}

//--------------------------------------------------------------------------------------------------------------------//

Asteroids::RustyTimer& Asteroids::RustyTimer::operator=(const RustyTimer& other)
{
	mSecondTimer = other.mSecondTimer;
	return *this;
}

//--------------------------------------------------------------------------------------------------------------------//

bool Asteroids::RustyTimer::DecrementStep(const double timeMultiplier)
{
	const Seconds decrementAmount = sSecondsPerStep * timeMultiplier * GameManager::TimeMultiplier();
	if (mSecondTimer <= decrementAmount)
	{
		mSecondTimer = 0.0;
		return true;
	}

	mSecondTimer -= decrementAmount;
	return false;
}

//--------------------------------------------------------------------------------------------------------------------//

bool Asteroids::RustyTimer::IncrementStep(const RustyTimer& maximumValue, const double timeMultiplier)
{
	tb_error_if(maximumValue.mSecondTimer < sSecondsPerStep, "tbExternalError: maximumValue must be larger than secondsPerStep.");

	const Seconds incrementAmount = sSecondsPerStep * timeMultiplier * GameManager::TimeMultiplier();
	if (maximumValue.mSecondTimer - incrementAmount <= mSecondTimer)
	{
		mSecondTimer = maximumValue.mSecondTimer;
		return true;
	}

	mSecondTimer += incrementAmount;
	return false;
}

//--------------------------------------------------------------------------------------------------------------------//

bool Asteroids::RustyTimer::IsZero(void) const
{
	return (0 == mSecondTimer) ? true : false;
}

//--------------------------------------------------------------------------------------------------------------------//

const Asteroids::RustyTimer::Seconds& Asteroids::RustyTimer::GetElapsedTime(void) const
{
	return mSecondTimer;
}

//--------------------------------------------------------------------------------------------------------------------//

const Asteroids::RustyTimer::Seconds& Asteroids::RustyTimer::GetRemainingTime(void) const
{
	return mSecondTimer;
}

//--------------------------------------------------------------------------------------------------------------------//

float Asteroids::RustyTimer::GetPercentageOf(const RustyTimer& timeValue) const
{
	tb_error_if(0 == timeValue.mSecondTimer, "tbExternalError: Expected timeValue to be a non-zero time value.");
	return static_cast<float>(mSecondTimer / timeValue.mSecondTimer);
}

//--------------------------------------------------------------------------------------------------------------------//

Asteroids::RustyTimer::Seconds Asteroids::RustyTimer::ToSeconds(const Milliseconds& valueMilliseconds)
{
	return valueMilliseconds / 1000.0;
}

//--------------------------------------------------------------------------------------------------------------------//

Asteroids::RustyTimer::Milliseconds Asteroids::RustyTimer::ToMilliseconds(const Seconds& valueSeconds)
{
	return static_cast<Milliseconds>(valueSeconds * 1000.0);
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::RustyTimer::SetSecondsPerStep(const Seconds& secondsPerStep)
{
	tb_error_if(secondsPerStep <= 0, "ExternalError: Expected secondsPerStep to be greater than zero.");

	sSecondsPerStep = secondsPerStep;
}

//--------------------------------------------------------------------------------------------------------------------//

Asteroids::RustyTimer::Seconds Asteroids::RustyTimer::GetSecondsPerStep(void)
{
	tb_error_if(sSecondsPerStep <= 0, "InternalError: Expected sSecondsPerStep to be greater than zero.");
	return sSecondsPerStep;
}

//--------------------------------------------------------------------------------------------------------------------//

Asteroids::RustyTimer Asteroids::RustyTimer::operator+(const RustyTimer& right) const
{
	return RustyTimer(GetElapsedTime() + right.GetElapsedTime());
}

//--------------------------------------------------------------------------------------------------------------------//

Asteroids::RustyTimer& Asteroids::RustyTimer::operator+=(const RustyTimer& right)
{
	mSecondTimer += right.GetElapsedTime();
	return *this;
}

//--------------------------------------------------------------------------------------------------------------------//

Asteroids::RustyTimer Asteroids::RustyTimer::operator-(const Asteroids::RustyTimer& right) const
{
	return RustyTimer((right.GetElapsedTime() > GetElapsedTime()) ? 0 : GetElapsedTime() - right.GetElapsedTime());
}

//--------------------------------------------------------------------------------------------------------------------//

Asteroids::RustyTimer& Asteroids::RustyTimer::operator-=(const RustyTimer& right)
{	//Subtract right if it won't cause underflow, else subtract self's timer to set it to 0.
	mSecondTimer -= (right.GetElapsedTime() > mSecondTimer) ? mSecondTimer : right.GetElapsedTime();
	return *this;
}

//--------------------------------------------------------------------------------------------------------------------//
