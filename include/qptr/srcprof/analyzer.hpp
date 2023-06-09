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


#include <qptr/srcprof/detail/clock.hpp>


namespace qptr::srcprof
{

class analyzer
{
public:

	virtual ~analyzer() = default;

	virtual const std::string &get_name() = 0;

	virtual bool begin_profiling() = 0;

	virtual bool end_profiling() = 0;

	virtual bool begin_thread_profiling(size_t thread_id) = 0;

	virtual bool end_thread_profiling(size_t thread_id) = 0;

	//start - when the internal profiler code started in the scope
	//stop  - when the internal profiler code finished in the scope
	virtual bool on_profiling(const char *name, 
	                          const detail::clock::time_point &start, const detail::clock::time_point &stop) = 0;

};

}//namespace qptr::srcprof
