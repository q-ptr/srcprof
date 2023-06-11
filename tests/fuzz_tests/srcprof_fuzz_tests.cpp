///                                 __ 
///   ___ _ __ ___ _ __  _ __ ___  / _|
///  / __| '__/ __| '_ \| '__/ _ \| |_   srcprof - invasive cpu profiler 
///  \__ \ | | (__| |_) | | | (_) |  _|  --------------------------------
///  |___/_|  \___| .__/|_|  \___/|_|    https://github.com/q-ptr/srcprof
///               |_|    
/// 
/// SPDX-FileCopyrightText: 2023 q-ptr
/// SPDX-License-Identifier: MIT

#include <cstdint>


extern "C" int LLVMFuzzerInitialize(int *argc, char ***argv)
{
	(void) argc;
	(void) argv;

	return 0;
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t data_size)
{
	(void) data;
	(void) data_size;

	return 0;
}
