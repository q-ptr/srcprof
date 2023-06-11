///                                 __ 
///   ___ _ __ ___ _ __  _ __ ___  / _|
///  / __| '__/ __| '_ \| '__/ _ \| |_   srcprof - invasive cpu profiler 
///  \__ \ | | (__| |_) | | | (_) |  _|  --------------------------------
///  |___/_|  \___| .__/|_|  \___/|_|    https://github.com/q-ptr/srcprof
///               |_|    
/// 
/// SPDX-FileCopyrightText: 2023 q-ptr
/// SPDX-License-Identifier: MIT

#pragma once


#include <cstdint>
#include <chrono>


namespace qptr::srcprof::detail
{

using clock = std::conditional_t<std::chrono::high_resolution_clock::is_steady, 
                                 std::chrono::high_resolution_clock, 
                                 std::chrono::steady_clock>;

}//namespace qptr::srcprof::detail
