///
/// @file
/// @details Contain and manage the information about each racecar on the server.
///
/// <!-- Copyright (c) 2023 Tyre Bytes LLC - All Rights Reserved -->
///------------------------------------------------------------------------------------------------------------------///

#include "../vehicle_physics/torque_curve.hpp"
#include "../logging.hpp"

#include <turtle_brains/game/diagnostics/tb_game_diagnostics.hpp>
#include <implot/implot.h>

#include <algorithm>

//--------------------------------------------------------------------------------------------------------------------//

Asteroids::VehiclePhysics::TorqueCurve Asteroids::VehiclePhysics::TorqueCurve::MiataTorqueCurve(void)
{	//http://www.automobile-catalog.com/curve/1999/1667030/mazda_mx-5_1_9.html
	TorqueCurve curve;
	curve.AddPlotPoint(500, 25.0);
	curve.AddPlotPoint(1000, 75.0);
	curve.AddPlotPoint(1500, 112.0);
	curve.AddPlotPoint(2000, 130.0);
	curve.AddPlotPoint(2500, 137.0);
	curve.AddPlotPoint(3000, 150.0);
	curve.AddPlotPoint(3500, 155.0);
	curve.AddPlotPoint(4000, 158.0);
	curve.AddPlotPoint(4500, 162.0);
	curve.AddPlotPoint(5000, 160.0);
	curve.AddPlotPoint(5500, 159.0);
	curve.AddPlotPoint(6000, 156.5);
	curve.AddPlotPoint(6500, 151.0);
	curve.AddPlotPoint(7000, 127.0);
	curve.AddPlotPoint(7500, 25.0);
	curve.AddPlotPoint(8000, 0.0);
	curve.NormalizeTorqueCurve();
	return curve;
}

//--------------------------------------------------------------------------------------------------------------------//

Asteroids::VehiclePhysics::TorqueCurve::TorqueCurve(void) :
	mTorqueTable(),
	mMaximumTorque(0.0),
	mIsNormalized(false)
{
}

//--------------------------------------------------------------------------------------------------------------------//

Asteroids::VehiclePhysics::TorqueCurve::~TorqueCurve(void)
{
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::VehiclePhysics::TorqueCurve::AddPlotPoint(const Scalar engineSpeedRPM, const Scalar torque)
{
	tb_error_if(true == mIsNormalized, "Cannot add more plot points to a table that is already normalized.");
	tb_error_if(engineSpeedRPM < 0.0, "Cannot add plot point for engine speeds less than zero.");
	tb_error_if(torque < 0.0, "Cannot add plot point for torque amounts that are less than zero.");

	auto findItr = std::find_if(mTorqueTable.begin(), mTorqueTable.end(), [engineSpeedRPM](PlotPoint& pt) { return fabs(pt.first - engineSpeedRPM) < 0.1; });
	tb_error_if(mTorqueTable.end() != findItr, "Cannot plot a point on top of another point!");

	PlotPoint pt(engineSpeedRPM, torque);
	mTorqueTable.push_back(pt);
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::VehiclePhysics::TorqueCurve::NormalizeTorqueCurve(void)
{
	tb_error_if(true == mTorqueTable.empty(), "Cannot normalize a table without plotted points. Call AddPlotPoint() to make it interesting.");

	std::sort(mTorqueTable.begin(), mTorqueTable.end(), [](PlotPoint& a, PlotPoint& b) { return a.second < b.second; });
	mMaximumTorque = mTorqueTable.back().second;

	std::sort(mTorqueTable.begin(), mTorqueTable.end(), [](PlotPoint& a, PlotPoint& b) { return a.first < b.first; });
	std::for_each(mTorqueTable.begin(), mTorqueTable.end(), [this](PlotPoint& pt) { pt.second /= mMaximumTorque; });

	mIsNormalized = true;
}

//--------------------------------------------------------------------------------------------------------------------//

Asteroids::Scalar Asteroids::VehiclePhysics::TorqueCurve::GetMaximumTorque(void) const
{
	return mMaximumTorque;
}

//--------------------------------------------------------------------------------------------------------------------//

Asteroids::Scalar Asteroids::VehiclePhysics::TorqueCurve::GetOutputTorque(const Scalar engineSpeedRPM) const
{
	return GetOutputValue(engineSpeedRPM) * mMaximumTorque;
}

//--------------------------------------------------------------------------------------------------------------------//

Asteroids::Scalar Asteroids::VehiclePhysics::TorqueCurve::GetOutputValue(const Scalar engineSpeedRPM) const
{
	tb_error_if(false == mIsNormalized, "Cannot get output of a TorqueCurve that has not been normalized. Call NormalizeTorqueCurve().");

	PlotPoint previousPoint = mTorqueTable.front();
	if (engineSpeedRPM < previousPoint.first)
	{	//The RPM of the engine is lower than the lowest in torque table.
		return previousPoint.second;
	}

	for (size_t index(1); index < mTorqueTable.size(); ++index)
	{
		const PlotPoint& currentPoint(mTorqueTable[index]);
		const Scalar& currentRPM(currentPoint.first);
		const Scalar& currentTorque(currentPoint.second);

		if (engineSpeedRPM > currentRPM)
		{
			previousPoint = currentPoint;
			continue;
		}

		const Scalar& previousRPM(previousPoint.first);
		const Scalar& previousTorque(previousPoint.second);
		const Scalar percentage = Scalar(1.0 - ((currentRPM - engineSpeedRPM) / (currentRPM - previousRPM)));
		return previousTorque + ((currentTorque - previousTorque) * percentage);
	}

	tb_debug_log(LogPhysics::Warning() << "Value not found for RPM: " << engineSpeedRPM << " in torque table.");
	return mTorqueTable.back().second;
}

//--------------------------------------------------------------------------------------------------------------------//

Asteroids::Scalar Asteroids::VehiclePhysics::TorqueCurve::GetMaximumRPM(void) const
{
	tb_error_if(false == mIsNormalized, "Cannot get the Maximum RPM of a TorqueCurve that has not been normalized. Call NormalizeTorqueCurve().");
	return mTorqueTable.back().first;
}

//--------------------------------------------------------------------------------------------------------------------//
//--------------------------------------------------------------------------------------------------------------------//
//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::VehiclePhysics::DisplayTorqueCurve(const TorqueCurve& torqueCurve, const Scalar engineRPM)
{
	if (true == ImGui::CollapsingHeader("Debug Torque Curve", ImGuiTreeNodeFlags_DefaultOpen))
	{
		const float rpmPerStep = 1.0f;
		const int maximumSize(static_cast<int>(torqueCurve.GetMaximumRPM() / rpmPerStep) + 2);

		std::vector<float> rpm;
		std::vector<float> torque;
		rpm.resize(maximumSize, 0.0f);
		torque.resize(maximumSize, 0.0f);

		for (int index = 0; index < maximumSize; ++index)
		{
			rpm[index] = index * rpmPerStep;
			torque[index] = torqueCurve.GetOutputTorque(tbMath::Clamp(rpm[index], 0.0f, torqueCurve.GetMaximumRPM()));
		}

		ImPlot::SetNextAxesLimits(0, torqueCurve.GetMaximumRPM(), 0, torqueCurve.GetMaximumTorque() + 20.0f, ImPlotCond_Always);
		if (true == ImPlot::BeginPlot("Torque", ImVec2(-1, -1), ImPlotFlags_NoTitle | ImPlotFlags_NoInputs))// | ImPlotFlags_NoLegend))
		{
			ImPlot::SetupAxis(ImAxis_X1, "RPM");

			ImPlot::PushStyleVar(ImPlotStyleVar_FillAlpha, 0.2f);
			ImPlot::PlotLine("Torque", rpm.data(), torque.data(), tbCore::size32(rpm.size()));

			ImPlot::PlotInfLines("current_rpm", &engineRPM, 1, ImPlotItemFlags_NoLegend);

			ImPlot::PopStyleVar();
			ImPlot::EndPlot();
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------//
