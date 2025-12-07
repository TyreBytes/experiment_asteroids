///
/// @file
/// @details Create a Torque/Power curve for an engine to lookup how much torque it applies at a given RPM.
///
/// <!-- Copyright (c) 2025 Tyre Bytes LLC - All Rights Reserved -->
///------------------------------------------------------------------------------------------------------------------///

#ifndef Asteroids_TorqueCurve_hpp
#define Asteroids_TorqueCurve_hpp

#include "asteroids.hpp"

#include <vector>

namespace Asteroids::VehiclePhysics
{

	class TorqueCurve
	{
	public:
		static TorqueCurve MiataTorqueCurve(void);

		TorqueCurve(void);
		~TorqueCurve(void);

		///
		/// @details Inserts a point for the curve to follow a more realistic torque/power curve of an internal combustion engine.
		///
		/// @param engineSpeedRPM Must be a positive value representing the speed of the engine in revolutions-per-minute.
		/// @param torque Must be a positive value representing the torque produced at engineSpeedRPM.
		///
		/// @note Cannot be called once the TorqueCurve object has been normalized or an error condition will be triggered.
		///
		void AddPlotPoint(const Scalar engineSpeedRPM, const Scalar torque);

		///
		/// @details Finds the maximum torque value in the table and normalizes all values to be within 0.0 to 1.0.
		///
		void NormalizeTorqueCurve(void);

		///
		/// @details Will return true if the TorqueTable has been normalized, "set in stone."
		///
		inline bool IsNormalized(void) const { return mIsNormalized; }

		///
		/// @details Returns the maximum amount of torque in Nm (Newton-meters) of the engine.
		///
		Scalar GetMaximumTorque(void) const;

		///
		/// @details Returns the maximum torque output of the engine at the given engine speed in Nm (Newton-meters).
		///
		Scalar GetOutputTorque(const Scalar engineSpeedRPM) const;

		Scalar GetMaximumRPM(void) const;

	private:

		///
		/// @details Returns a value from 0 to 1 representing a percentage of the maximum torque at this given engine speed.
		///
		Scalar GetOutputValue(const Scalar engineSpeedRPM) const;

		typedef std::pair<Scalar, Scalar> PlotPoint; //RPM, NormalizedTorque
		std::vector<PlotPoint> mTorqueTable;
		Scalar mMaximumTorque;  //In Nm
		bool mIsNormalized;
	};

	void DisplayTorqueCurve(const TorqueCurve& torqueCurve, const Scalar engineRPM);

};	//namespace Asteroids::VehiclePhysics

#endif /* Asteroids_TorqueCurve_hpp */
