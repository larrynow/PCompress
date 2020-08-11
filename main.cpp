#include"test_file_io.h"
#include"test_desc.h"
#include"test_encoder.h"

int main(int argc, char argv[])
{
	using namespace NCTest;

	// File io test.
	/*auto fileioTest = new TestFileIO();
	fileioTest->Test();*/

	// Descriptor test.
	/*auto descTest = new TestDesc();
	descTest->Test();*/

	// Encoder test.
	auto encoderTest = new TestEncoder();
	encoderTest->Test();
}