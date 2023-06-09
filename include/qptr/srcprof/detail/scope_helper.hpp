///                                 __ 
///   ___ _ __ ___ _ __  _ __ ___  / _|
///  / __| '__/ __| '_ \| '__/ _ \| |_   srcprof - invasive c++ profiler 
///  \__ \ | | (__| |_) | | | (_) |  _|  --------------------------------
///  |___/_|  \___| .__/|_|  \___/|_|    https://github.com/q-ptr/srcprof
///               |_|    
/// 
/// SPDX-FileCopyrightText: 2023 q-ptr
/// SPDX-License-Identifier: MIT


#include <qptr/srcprof/detail/thread_profiler.hpp>


namespace qptr::srcprof::detail
{

class scope_helper final
{
public:

	inline explicit scope_helper(const char *name)
	{
		thread_profiler_.add(name);
	}

	inline ~scope_helper()
	{
		thread_profiler_.add(nullptr);
	}
};

}//namespace qptr::srcprof::detail
