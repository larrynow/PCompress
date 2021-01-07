#pragma once
#include "test_base.h"
#include "neuron_logic.h"
#include "file_io.h"
#include"spline_curve.h"

#include <algorithm>

namespace NCTest
{
	class TestSpline :
		public TestBase
	{
	public:

		virtual void Test() override
		{
			using namespace NCNeuron;

			Branch br = Branch();
			for (int i = 0; i < 10; i++)
			{
				auto x = static_cast<float>(i * PI / 10.0);
				br.push_back({ x,
					static_cast<float>(sin(x)), 0.f });
			}
			std::cout << br;

			NCSplineCurve::SCParams s;
			NCSplineCurve::FitCurve(br, s);

			Branch n_b;
			NCSplineCurve::RemakeCurve(s, n_b, 10);

			std::cout << n_b;

			SaveBranch(br, ".//neurons//br.swc");
			SaveBranch(n_b, ".//neurons//n_br.swc");
		}
	};
}


