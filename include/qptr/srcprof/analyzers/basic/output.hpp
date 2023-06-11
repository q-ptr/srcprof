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


#include <qptr/srcprof/detail/clock.hpp>


namespace qptr::srcprof::analyzers::basic
{

class output
{
public:

	struct profile_t final
	{
		uint64_t     count_ = 0;
		uint64_t     avg_   = 0;
		uint64_t     min_   = 0;
		uint64_t     max_   = 0;
		uint64_t     total_ = 0;

		inline void begin()
		{
			count_ = 0;
			avg_   = 0;
			min_   = 0;
			max_   = 0;
			total_ = 0;
		}

		inline void end()
		{
			if (0 == count_)
			{
				avg_ = 0;
			}
			else
			{
				avg_ = total_ / count_;
			}
			
		}

		inline void update(uint64_t duration)
		{
			if (0 == count_ || duration < min_)
			{
				min_ = duration;
			}
			if (0 == count_ || duration > max_)
			{
				max_ = duration;
			}
			total_ += duration;
			count_++;
		}

		inline void update(uint64_t min_duration, uint64_t max_duration, uint64_t total_duration, uint64_t count)
		{
			if (0 == count_ || min_duration < min_)
			{
				min_ = min_duration;
			}
			if (0 == count_ || max_duration > max_)
			{
				max_ = max_duration;
			}
			total_ += total_duration;
			count_ += count;
		}
	};

	struct scope_t final
	{
		const char   *name_ = nullptr;
		profile_t    scope_duration_;
		profile_t    child_scopes_duration_;
		profile_t    profiler_duration_;
	};

	struct stats_t final
	{
		uint64_t   total_calls_             = 0;
		uint64_t   duration_total_          = 0;
		uint64_t   duration_profiler_total_ = 0;
	};

	virtual ~output() = default;

	virtual const std::string &get_name() = 0;

	virtual bool begin() = 0;

	virtual bool end() = 0;

	virtual bool on_stats(const stats_t &stats) = 0;

	virtual bool begin_scope_stats(const std::string_view &stats_name) = 0;

	virtual bool end_scope_stats() = 0;

	virtual bool on_scope_stats(const std::string_view &scope_name, const profile_t &profile) = 0;

	virtual bool begin_call_graph() = 0;

	virtual bool end_call_graph() = 0;

	virtual bool begin_call_graph_scope(const scope_t &scope, size_t depth) = 0;

	virtual bool end_call_graph_scope(const scope_t &scope, size_t depth) = 0;

};

}//namespace qptr::srcprof::analyzers::basic
