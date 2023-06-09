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
#include <cstdint>


namespace qptr::srcprof::detail
{

struct event_t final
{
	const char          *name_;
	clock::time_point   start_;
	clock::time_point   stop_;
};

}//namespace qptr::srcprof::detail
