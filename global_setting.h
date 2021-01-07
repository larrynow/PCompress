#pragma once
#ifndef _NC_GLOBAL_SETTING_H_
#define _NC_GLOBAL_SETTING_H_

class GlobalSetting
{
	/*
	This class set global values.
	*/

public:

	// Global decimal precision.
	static void SetDecimalPrecision(int i)
	{
		GetDecimalPrecision() = i;
	}

	static int& GetDecimalPrecision()
	{
		static int Precision = 2;// Default use 2 decimal bits.
		return Precision;
	}

	// Global curve error threshold.
	static void SetCurveThreshold(float t)
	{
		GetCurveThreshold() = t;
	}

	static float& GetCurveThreshold()
	{
		static float threshold = 10.f;// Curve error threshold.
		return threshold;
	}

	// Global curve error threshold.
	static void SetSamplePointStep(float n)
	{
		GetSamplePointStep() = n;
	}

	static float& GetSamplePointStep()
	{
		static float step = 5.f;// Curve error threshold.
		return step;
	}
	
};
#endif // !_NC_GLOBAL_SETTING_H_