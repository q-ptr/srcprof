///                                 __ 
///   ___ _ __ ___ _ __  _ __ ___  / _|
///  / __| '__/ __| '_ \| '__/ _ \| |_   srcprof - invasive c++ profiler 
///  \__ \ | | (__| |_) | | | (_) |  _|  --------------------------------
///  |___/_|  \___| .__/|_|  \___/|_|    https://github.com/q-ptr/srcprof
///               |_|    
/// 
/// SPDX-FileCopyrightText: 2023 q-ptr
/// SPDX-License-Identifier: MIT

#pragma once


#include <qptr/srcprof/detail/global_profiler.hpp>
#include <qptr/srcprof/detail/scope_helper.hpp>
#include <qptr/srcprof/analyzer.hpp>
#include <memory>


#ifdef QPTR_SRCPROF_ENABLED
	#define QPTR_SRCPROF_SCOPE(name)      qptr::srcprof::detail::scope_helper   __qpr_srcprof_scope__(name "");
	#define QPTR_SRCPROF()                qptr::srcprof::detail::scope_helper   __qpr_srcprof_scope__(__FUNCTION__);
#else
	#define QPTR_SRCPROF_SCOPE(name)
	#define QPTR_SRCPROF()
#endif


namespace qptr::srcprof
{

class profile final
{
public:

	static bool is_enabled()
	{
		return detail::global_profiler_.is_enabled();
	}

	static void enable(std::shared_ptr<analyzer> analyzer)
	{
		return detail::global_profiler_.enable(analyzer);
	}

private:

	profile() = default;

	~profile() = default;
};

}//namespace qptr::srcprof

