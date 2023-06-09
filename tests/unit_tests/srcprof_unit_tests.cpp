///                                 __ 
///   ___ _ __ ___ _ __  _ __ ___  / _|
///  / __| '__/ __| '_ \| '__/ _ \| |_   srcprof - invasive c++ profiler 
///  \__ \ | | (__| |_) | | | (_) |  _|  --------------------------------
///  |___/_|  \___| .__/|_|  \___/|_|    https://github.com/q-ptr/srcprof
///               |_|    
/// 
/// SPDX-FileCopyrightText: 2023 q-ptr
/// SPDX-License-Identifier: MIT

#include <catch_amalgamated.hpp>
#include <fmt/core.h>


int main(int argc, char *argv[])
{
	int   result = Catch::Session().run(argc, argv);

	if (0 != result)
	{
		fmt::print("Press ENTER key to continue...\n");
		(void) getchar();
	}

	return result;
}


TEST_CASE("test", "[test]")
{
	REQUIRE(true == true);
}
