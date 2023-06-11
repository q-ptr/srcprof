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


#include <qptr/srcprof/analyzers/basic/output.hpp>
#include <qptr/srcprof/detail/json_writer.hpp>
#include <qptr/srcprof/version.hpp>
#include <qptr/predef/architecture.hpp>
#include <qptr/predef/compiler.hpp>
#include <qptr/predef/configuration.hpp>
#include <qptr/predef/library.hpp>
#include <qptr/predef/os.hpp>
#include <qptr/predef/platform.hpp>
#include <boost/predef.h>
#include <fmt/ostream.h>
#include <fstream>


#define CHECK(op)   if (!(op)) { return false; }


namespace qptr::srcprof::analyzers::basic
{

class json_output : public output
{
public:

	static inline const std::string   NAME              = "basic";
	static const size_t               FLOATING_DECIMALS = 9;

	inline explicit json_output(const std::string_view &output_path) : 
		output_path_(output_path)
	{
	}

	const std::string &get_name() override
	{
		return NAME;
	}

	bool begin() override
	{
		output_stream_.open(output_path_);

		if (!output_stream_)
		{
			return false;
		}

		CHECK(json_writer_.begin())
		CHECK(json_writer_.begin_object(output_stream_))

		CHECK(json_writer_.begin_object(output_stream_, "generator"))
			CHECK(json_writer_.string(output_stream_, VERSION_NAME, "name"))
			CHECK(json_writer_.string(output_stream_, VERSION_DESCRIPTION, "description"))
			CHECK(json_writer_.string(output_stream_, VERSION_URL, "url"))
			CHECK(json_writer_.string(output_stream_, VERSION.to_string(), "version"))
			CHECK(json_writer_.string(output_stream_, get_name(), "analyzer"))
			CHECK(json_writer_.string(output_stream_, get_name(), "output"))
			CHECK(json_writer_.string(output_stream_, QPTR_PREDEF_COMPILER_NAME, "compiler"))
			CHECK(json_writer_.string(output_stream_, QPTR_PREDEF_OS_NAME, "os"))
			CHECK(json_writer_.string(output_stream_, QPTR_PREDEF_PLATFORM_NAME, "platform"))
			CHECK(json_writer_.string(output_stream_, QPTR_PREDEF_ARCHITECTURE_NAME, "architecture"))
			CHECK(json_writer_.string(output_stream_, QPTR_PREDEF_CONFIGURATION_NAME, "configuration"))
		CHECK(json_writer_.end_object(output_stream_))

		return true;
	}

	bool end() override
	{
		if (!output_stream_)
		{
			return false;
		}

		CHECK(json_writer_.end_object(output_stream_))
		CHECK(json_writer_.end())

		output_stream_.close();

		return true;
	}

	bool on_stats(const stats_t &stats) override
	{
		if (!output_stream_)
		{
			return false;
		}

		CHECK(json_writer_.begin_object(output_stream_, "stats"))
			CHECK(json_writer_.integer(output_stream_, stats.total_calls_, "all_calls"))
			CHECK(json_writer_.real(output_stream_, get_elapsed_seconds(stats.duration_total_), "total_duration", 
			                        FLOATING_DECIMALS))
			CHECK(json_writer_.real(output_stream_, percent(stats.duration_profiler_total_, stats.duration_total_), 
			                        "profiler_impact_percent", 2))
		CHECK(json_writer_.end_object(output_stream_))

		return true;
	}

	bool begin_scope_stats(const std::string_view& stats_name) override
	{
		if (!output_stream_)
		{
			return false;
		}

		CHECK(json_writer_.begin_array(output_stream_, stats_name))

		return true;
	}

	bool end_scope_stats() override
	{
		if (!output_stream_)
		{
			return false;
		}

		CHECK(json_writer_.end_array(output_stream_))

		return true;
	}

	bool on_scope_stats(const std::string_view &scope_name, const profile_t &profile) override
	{
		if (!output_stream_)
		{
			return false;
		}

		CHECK(json_writer_.begin_object(output_stream_))
			CHECK(json_writer_.string(output_stream_, scope_name, "name"))
			CHECK(json_writer_.integer(output_stream_, profile.count_, "count"))
			CHECK(json_writer_.real(output_stream_, get_elapsed_seconds(profile.avg_), "avg", 
			                        FLOATING_DECIMALS))
			CHECK(json_writer_.real(output_stream_, get_elapsed_seconds(profile.min_), "min", 
			                        FLOATING_DECIMALS))
			CHECK(json_writer_.real(output_stream_, get_elapsed_seconds(profile.max_), "max", 
			                        FLOATING_DECIMALS))
			CHECK(json_writer_.real(output_stream_, get_elapsed_seconds(profile.total_), "total", 
			                        FLOATING_DECIMALS))
		CHECK(json_writer_.end_object(output_stream_))

		return true;
	}

	bool begin_call_graph() override
	{
		if (!output_stream_)
		{
			return false;
		}

		CHECK(json_writer_.begin_array(output_stream_, "call_graph"))

		return true;
	}

	bool end_call_graph() override
	{
		if (!output_stream_)
		{
			return false;
		}

		CHECK(json_writer_.end_array(output_stream_))

		return true;
	}

	bool begin_call_graph_scope(const scope_t &scope, size_t depth) override
	{
		(void) depth;

		if (!output_stream_)
		{
			return false;
		}

		CHECK(json_writer_.begin_object(output_stream_))
			CHECK(json_writer_.string(output_stream_, scope.name_, "name"))
			CHECK(json_writer_.integer(output_stream_, scope.scope_duration_.count_, "call_count"))
			CHECK(json_writer_.integer(output_stream_, scope.child_scopes_duration_.count_, "child_call_count"))
			CHECK(json_writer_.begin_object(output_stream_, "timings"))
				CHECK(json_writer_.begin_object(output_stream_, "self"))
					CHECK(json_writer_.integer(output_stream_, scope.scope_duration_.count_, "count"))
					CHECK(json_writer_.real(output_stream_, get_elapsed_seconds(scope.scope_duration_.avg_), "avg", 
					                        FLOATING_DECIMALS))
					CHECK(json_writer_.real(output_stream_, get_elapsed_seconds(scope.scope_duration_.min_), "min", 
					                        FLOATING_DECIMALS))
					CHECK(json_writer_.real(output_stream_, get_elapsed_seconds(scope.scope_duration_.max_), "max", 
					                        FLOATING_DECIMALS))
					CHECK(json_writer_.real(output_stream_, get_elapsed_seconds(scope.scope_duration_.total_), "total", 
					                        FLOATING_DECIMALS))
				CHECK(json_writer_.end_object(output_stream_))
				CHECK(json_writer_.begin_object(output_stream_, "children"))
					CHECK(json_writer_.real(output_stream_, get_elapsed_seconds(scope.child_scopes_duration_.avg_), "avg", 
					                        FLOATING_DECIMALS))
					CHECK(json_writer_.real(output_stream_, get_elapsed_seconds(scope.child_scopes_duration_.min_), "min", 
					                        FLOATING_DECIMALS))
					CHECK(json_writer_.real(output_stream_, get_elapsed_seconds(scope.child_scopes_duration_.max_), "max", 
					                        FLOATING_DECIMALS))
					CHECK(json_writer_.real(output_stream_, get_elapsed_seconds(scope.child_scopes_duration_.total_), 
					                        "total", FLOATING_DECIMALS))
				CHECK(json_writer_.end_object(output_stream_))
				CHECK(json_writer_.begin_object(output_stream_, "profiler"))
					CHECK(json_writer_.real(output_stream_, get_elapsed_seconds(scope.profiler_duration_.avg_), "avg", 
					                        FLOATING_DECIMALS))
					CHECK(json_writer_.real(output_stream_, get_elapsed_seconds(scope.profiler_duration_.min_), "min", 
					                        FLOATING_DECIMALS))
					CHECK(json_writer_.real(output_stream_, get_elapsed_seconds(scope.profiler_duration_.max_), "max", 
					                        FLOATING_DECIMALS))
					CHECK(json_writer_.real(output_stream_, get_elapsed_seconds(scope.profiler_duration_.total_), "total", 
					                        FLOATING_DECIMALS))
				CHECK(json_writer_.end_object(output_stream_))
			CHECK(json_writer_.end_object(output_stream_))
			CHECK(json_writer_.begin_array(output_stream_, "call_graph"))

		return true;
	}

	bool end_call_graph_scope(const scope_t &scope, size_t depth) override
	{
		(void) depth;

		if (!output_stream_)
		{
			return false;
		}

			CHECK(json_writer_.end_array(output_stream_))
		CHECK(json_writer_.end_object(output_stream_))

		return true;
	}

private:

	detail::json_writer   json_writer_;
	std::ofstream         output_stream_;
	std::string           output_path_;

	static inline double get_elapsed_seconds(uint64_t nanoseconds)
	{
		return (double)nanoseconds / 1'000'000'000.0;
	}

	static inline double percent(uint64_t part, uint64_t total)
	{
		return 0 == total ? 0.0 : ((double)part * 100.0 / (double)total);
	}
};

}//namespace qptr::srcprof::analyzers::basic
