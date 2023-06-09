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


#include <qptr/srcprof/detail/thread_entries.hpp>
#include <qptr/srcprof/detail/global_profiler.hpp>
#include <thread>
#include <cstdint>


namespace qptr::srcprof::detail
{

class thread_profiler final
{
public:

	inline ~thread_profiler()
	{
		if (nullptr != thread_entries_)
		{
			global_profiler_.end_thread(thread_entries_);
			thread_entries_ = nullptr;
		}
	}

	inline bool add(const char *scope_name)
	{
		auto   start = clock::now();

		if (!global_profiler_.is_enabled())
		{
			return false;
		}

		if (nullptr == thread_entries_)
		{
			if (nullptr == (thread_entries_ = thread_entries_t::create(global_profiler_.get_events_pre_alloc_count())))
			{
				return false;
			}
			global_profiler_.begin_thread();
		}

		auto   event = thread_entries_->get_next_event(global_profiler_.get_events_pre_alloc_count());

		if (nullptr == event)
		{
			return false;
		}

		event->name_  = scope_name;
		event->start_ = start;
		event->stop_  = clock::now();

		return true;
	}

private:

	thread_entries_t   *thread_entries_ = nullptr;

};

static inline thread_local  thread_profiler   thread_profiler_;

}//namespace qptr::srcprof::detail
