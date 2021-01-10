#pragma once
#ifndef _NC_SPLINE_CURVE_H_
#define _NC_SPLINE_CURVE_H_

#include"math.h"
#include"branch.h"

namespace NCSplineCurve
{
	using namespace NCMath;

	struct SCParams
	{
		NCMath::VEC4 XParams;
		NCMath::VEC4 YParams;
		NCMath::VEC4 ZParams;

		VEC3 GetStartPoint()
		{
			return VEC3(XParams.x, YParams.x, ZParams.x);
		}

		VEC3 GetEndPoint()
		{
			return VEC3(XParams.y, YParams.y, ZParams.y);
		}

		VEC3 GetStartTangent()
		{
			return VEC3(XParams.z, YParams.z, ZParams.z);
		}

		VEC3 GetEndTangent()
		{
			return VEC3(XParams.w, YParams.w, ZParams.w);
		}
	};

	inline float GetBranchLength(const SCParams& p)
	{
		auto dis = VEC3(p.XParams[0], p.YParams[0], p.ZParams[0]).SqrDistance(
			VEC3(p.XParams[1], p.YParams[1], p.ZParams[1])
		);
		if (dis > 0.f) return sqrt(dis);
		else return 0.f;
	}

	struct ErrorUnit
	{
		float total_error;
		std::size_t max_e_id;
	};

	void FitCurve(const NCNeuron::Branch& branch, SCParams& ret_param);

	void RemakeCurve(const SCParams& param, NCNeuron::Branch& ret_branch, int node_num);

	ErrorUnit CurveError(const NCNeuron::Branch& branch, const SCParams& param);

	inline NCMath::MAT4 GetHermite()
	{
		// Construct hermite mat.
		MAT4 hermite;
		hermite.SetRow(0, { 2, -2, 1, 1 });
		hermite.SetRow(1, { -3, 3, -2, -1 });
		hermite.SetRow(2, { 0, 0, 1, 0 });
		hermite.SetRow(3, { 1, 0, 0, 0 });

		return hermite;
	}
}

#endif // !_NC_SPLINE_CURVE_H_
