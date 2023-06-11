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


#include <semver.hpp>
#include <string>


namespace qptr::srcprof
{

static inline const std::string           VERSION_NAME          = "q-ptr.srcprof";
static inline const std::string           VERSION_DESCRIPTION   = "invasive cpu profiler";
static inline const std::string           VERSION_URL           = "https://github.com/q-ptr/srcprof";

static inline constexpr uint8_t           VERSION_MAJOR         = 0;
static inline constexpr uint8_t           VERSION_MINOR         = 0;
static inline constexpr uint8_t           VERSION_PATCH         = 1;
static inline constexpr auto              VERSION_PREREL_TYPE   = semver::prerelease::alpha;
static inline constexpr uint8_t           VERSION_PREREL_NUMBER = 1;

static inline constexpr semver::version   VERSION               = semver::version 
{
	VERSION_MAJOR, 
	VERSION_MINOR, 
	VERSION_PATCH, 
	VERSION_PREREL_TYPE, 
	VERSION_PREREL_NUMBER
};

}//namespace qptr::srcprof
