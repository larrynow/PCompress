#include"test_file_io.h"
#include"test_desc.h"
#include"test_encoder.h"
#include"test_parser.h"
#include"test_compress.h"
#include"global_setting.h"
#include"test_spline.h"

int main(int argc, char argv[])
{
	using namespace NCTest;

	GlobalSetting::SetDecimalPrecision(6);

	// File io test.
	/*auto fileioTest = new TestFileIO();
	fileioTest->Test();*/

	// Descriptor test.
	/*auto descTest = new TestDesc();
	descTest->Test();*/

	// Encoder test.
	/*auto encoderTest = new TestEncoder();
	encoderTest->Test();*/

	// Parser test.
	/*auto parseTest = new TestParser();
	parseTest->Test();*/

	// Compress test.
	auto compressTest = new TestCompress();
	compressTest->Test();

	// Spline test.
	/*auto spline_tset = TestSpline();
	spline_tset.Test();*/
}