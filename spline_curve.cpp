#include "spline_curve.h"
#include "global_setting.h"
#include"utils.h"

void NCSplineCurve::FitCurve(const NCNeuron::Branch& branch, SCParams& ret_param)
{
	// Fit a curve without last point.
	NCNeuron::Branch nb(branch);

	auto N = nb.size();
	if (N < 2) std::cout << "Too Small" << std::endl;
	float step = 1.f / N;

	// X.
	{
		float tangent_0 = (nb[1].x - nb[0].x) / step;
		float tangent_n = (nb[N - 1].x - nb[N - 2].x) / step;
		ret_param.XParams = VEC4({ nb[0].x, nb[N - 1].x, tangent_0, tangent_n });
		DebugLog("Tangent : ", tangent_0, " ", tangent_n);
	}

	// Y.
	{
		float tangent_0 = (nb[1].y - nb[0].y) / step;
		float tangent_n = (nb[N - 1].y - nb[N - 2].y) / step;
		ret_param.YParams = VEC4({ nb[0].y, nb[N - 1].y, tangent_0, tangent_n });
		DebugLog("Tangent : ", tangent_0, " ", tangent_n);
	}

	// Z.
	{
		float tangent_0 = (nb[1].z - nb[0].z) / step;
		float tangent_n = (nb[N - 1].z - nb[N - 2].z) / step;
		ret_param.ZParams = VEC4({ nb[0].z, nb[N - 1].z, tangent_0, tangent_n });
		DebugLog("Tangent : ", tangent_0, " ", tangent_n);
	}
}

void NCSplineCurve::RemakeCurve(const SCParams& param, NCNeuron::Branch& ret_branch,
	int node_num)
{
	auto hermite = GetHermite();

	for (int i = 0; i < node_num; i++)
	{
		float t = static_cast<float>(i) / static_cast<float>(node_num - 1);

		auto qx = hermite * param.XParams;
		auto x = qx.x * (t * t * t) + qx.y * (t * t) + qx.z * t + qx.w;

		auto qy = hermite * param.YParams;
		auto y = qy.x * (t * t * t) + qy.y * (t * t) + qy.z * t + qy.w;

		auto qz = hermite * param.ZParams;
		auto z = qz.x * (t * t * t) + qz.y * (t * t) + qz.z * t + qz.w;

		ret_branch.push_back({ x, y, z });
	}
}

NCSplineCurve::ErrorUnit NCSplineCurve::CurveError(const NCNeuron::Branch& branch, 
	const SCParams& param)
{
	NCNeuron::Branch nb(branch);
	//nb.MoveToO();

	auto N = nb.size();
	NCNeuron::Branch curve_b;

	RemakeCurve(param, curve_b, N);

	float max_e = 0.0;
	std::size_t id=0;
	float sum_e = 0.f;
	for (std::size_t i = 0; i < N; i++)
	{
		auto e = curve_b[i].SqrDistance(nb[i]);
		if (e >= 0.0) e = sqrt(e);
		DebugLog(e);
		sum_e += e;
		if (e > max_e) { max_e = e; id = i; }
	}

	return {sum_e, id};
}


