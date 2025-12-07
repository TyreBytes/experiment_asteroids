///
/// @file
/// @details This is using the tbGame::GameTimer API but with a floating point type representation of the timer. It is
///   slighly less specific, but allows easier modification of time multipliers. Perhaps this can go into TurtleBrains
///   in the future.
///
/// <!-- Copyright (c) 2025 Tyre Bytes LLC - All Rights Reserved -->
///------------------------------------------------------------------------------------------------------------------///

#ifndef Asteroids_RustyTimer_hpp
#define Asteroids_RustyTimer_hpp

#include <turtle_brains/core/tb_types.hpp>

namespace Asteroids
{
	///
	///	@details The GameTimer provides a simple interface for creating a timer to manually increment and decrement
	///   each Simulate() step for game logic.  Internally seconds are stored and can be greater than or equal to
	///   zero allowing use cases to count the elapsed time since an event (by setting the timer to zero and incrementing
	///   each step), or to count down to some event.
	///
	class RustyTimer
	{
	public:
		using Seconds = double;
		using Milliseconds = tbCore::uint32;

		///
		/// @details A constant GameTimer object representing a time value of zero.
		///
		static RustyTimer Zero(void);

		///
		/// @details A constant GameTimer object representing the maximum value for the timer, infinity.
		///
		static RustyTimer Infinity(void);

		///
		///	@details Constructs a GameTimer object with an internal value of 0 seconds elapsed/remaining.
		///
		RustyTimer(void);


		RustyTimer(const Milliseconds& valueInMilliseconds);

		///
		///	@details Constructs a GameTimer object with the desired value in seconds.
		///
		/// @param valueSeconds The initial value of the timer object, in seconds, to track time from.
		///
		RustyTimer(const Seconds& valueInSeconds);

		///
		///	@details Copy constructor to create a GameTimer object from another object, copying all internal values.
		///
		RustyTimer(const RustyTimer& other);

		///
		///	@details Destructs a GameTimer object, which is minimal since the timer does not need to release any resources.
		///
		~RustyTimer(void);

		///
		///	@details Set the value of the GameTimer to the time value of another timer object.
		///
		RustyTimer& operator=(const RustyTimer& other);

		///
		///	@details Decrement the value of the timer by the seconds in a single Simulate step (kFixedTime) and returns
		///   true if the internal value is now at 0, including if it was already at 0.
		///
		/// @note Will not allow the timer object to underflow, and will remain at 0 even if step size is greater than value.
		///
		bool DecrementStep(const double timeMultiplier = 1.0);

		///
		///	@details Increments the value of the timer by the seconds in a single Simulate step so that GetElapsedTime()
		///   can be used to track how long the timer has been running for. This will also prevent the timer from overflowing
		///   should it ever get large enough, or approach a value greater than maximumValue supplied. Returns true if the
		///   increment reached or stopped at the maximumValue during the call, false otherwise.
		///
		/// @param maximumValue The highest value the timer can increment to, defaults to kInfinity, the maximum value possible.
		///
		/// @note It is possible that the timer overflows, in this version of the GameTimer the second value is stored
		///   as a 32-bit unsigned integer value which will hold approximately 49 days.  This may be changed to 64-bit in
		///   the future.
		///
		bool IncrementStep(const RustyTimer& maximumValue = Infinity(), const double timeMultiplier = 1.0);

		///
		///	@details Checks if the timer is at zero returning true if it is or false it if the value is greater than 0.
		///
		bool IsZero(void) const;

		///
		///	@details Returns the value of the timer in seconds, typically used with a timer object that is counting
		///   upwards using IncrementStep() for clarity that some amount of time has passed.
		///
		const Seconds& GetElapsedTime(void) const;

		///
		///	@details Returns the value of the timer in seconds, typically used with a timer object that is counting
		///   down towards zero using DecrementStep() for clarity that some amount of time may be remaining.
		///
		const Seconds& GetRemainingTime(void) const;

		///
		///	@details Computes a percentage value from 0.0f to 1.0f where the timer object is between 0 and the timeValue
		///   passed in.  If the internal seconds value is larger than that of the timeValue parameter, the returned
		///   percentage may be greater than 1.0f.
		///
		/// @param timeValue The range of seconds representing "100 percent" or a return value of 1.0f.
		///
		/// @note The returned result may be greater than 1.0f if the timers value is greater than the provided value.
		///
		float GetPercentageOf(const RustyTimer& timeValue) const;

		///
		///	@details Converts a value in milliseconds to a value in partial seconds, being returned as a floating point.
		///
		/// @note The actual assigned value will be converted to seconds and in the process some precision may be lost.
		///
		static Seconds ToSeconds(const Milliseconds& valueMilliseconds);

		///
		///	@details Converts a time value from partial seconds to milliseconds, being returned as an integer type.
		///
		/// @note The returned value will be the input parameter converted to milliseconds and in the process precision
		///   may be lost, the returned value may be slightly smaller than the input.  0.0018 seconds may become 1 millisecond.
		///
		static Milliseconds ToMilliseconds(const Seconds& valueSeconds);

		///
		///	@details Changes the number of seconds per Simulate step in TurtleBrains.
		///
		/// @param secondsPerStep The number of seconds that pass for each simulate step.  kFixedTime in Asteroids is
		///   the default per step and results in Simulate being called at roughly 100hz.
		///
		static void SetSecondsPerStep(const Seconds& secondsPerStep);

		///
		///	@details Returns the partial seconds per Simulate step in TurtleBrains as set by SetSecondsPerStep().
		///
		/// @note The returned value will be converted from seconds and in the process precision may be lost, the
		///   returned value may be slightly smaller than the actual value. Ex. 0.0018 seconds may become 1 second.
		///
		static Seconds GetSecondsPerStep(void);

		RustyTimer operator+(const RustyTimer& right) const;
		RustyTimer& operator+=(const RustyTimer& right);
		RustyTimer operator-(const RustyTimer& right) const;
		RustyTimer& operator-=(const RustyTimer& right);

	private:
		Seconds mSecondTimer; //not the first timer...
		static Seconds sSecondsPerStep;
	};

	inline bool operator==(const RustyTimer& left, const RustyTimer& right)
	{
		return (left.GetElapsedTime() == right.GetElapsedTime()) ? true : false;
	}

	inline bool operator!=(const RustyTimer& left, const RustyTimer& right)
	{
		return (left.GetElapsedTime() != right.GetElapsedTime()) ? true : false;
	}

	inline bool operator>=(const RustyTimer& left, const RustyTimer& right)
	{
		return (left.GetElapsedTime() >= right.GetElapsedTime()) ? true : false;
	}

	inline bool operator<=(const RustyTimer& left, const RustyTimer& right)
	{
		return (left.GetElapsedTime() <= right.GetElapsedTime()) ? true : false;
	}

	inline bool operator>(const RustyTimer& left, const RustyTimer& right)
	{
		return (left.GetElapsedTime() > right.GetElapsedTime()) ? true : false;
	}

	inline bool operator<(const RustyTimer& left, const RustyTimer& right)
	{
		return (left.GetElapsedTime() < right.GetElapsedTime()) ? true : false;
	}

}; /* namespace Asteroids */

#endif /* Asteroids_RustyTimer_hpp */
