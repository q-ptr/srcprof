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


#include <qptr/srcprof/detail/thread_entries.hpp>
#include <qptr/srcprof/analyzer.hpp>
#include <atomic>
#include <mutex>
#include <cstdint>


namespace qptr::srcprof::detail
{

class global_profiler final
{
public:

	static const size_t   DEFAULT_EVENTS_PRE_ALLOC_COUNT = 256;
	static const bool     DEFAULT_ENABLED                = false;

	size_t get_events_pre_alloc_count() const
	{
		return events_pre_alloc_count_.load();
	}

	void set_events_pre_alloc_count(size_t events_pre_alloc_count)
	{
		events_pre_alloc_count_.store(events_pre_alloc_count);
	}

	inline bool is_enabled() const
	{
		return (nullptr != analyzer_.get());
	}

	inline void enable(std::shared_ptr<analyzer> analyzer)
	{
		analyzer_  = analyzer;
	}

	inline bool begin_thread()
	{
		threads_.fetch_add(1);

		return true;
	}

	inline bool end_thread(thread_entries_t *thread_entries)
	{
		std::scoped_lock   lock(mutex_);

		if (nullptr == tail_)
		{
			head_ = thread_entries;
			tail_ = thread_entries;
		}
		else
		{
			tail_->next_          = thread_entries;
			thread_entries->prev_ = tail_;
			tail_                 = thread_entries;
		}

		if (1 == threads_.fetch_sub(1))
		{
			(void) finalize();
		}

		return true;
	}

private:

	thread_entries_t             *head_                  = nullptr;
	thread_entries_t             *tail_                  = nullptr;
	std::mutex                   mutex_; 
	std::shared_ptr<analyzer>    analyzer_;
	std::atomic<size_t>          events_pre_alloc_count_ = DEFAULT_EVENTS_PRE_ALLOC_COUNT;
	std::atomic<size_t>          threads_                = 0;

	inline bool finalize()
	{
		std::hash<std::thread::id>   hasher;
		auto                         stopped      = !analyzer_;
		auto                         thread_entry = head_;
		size_t                       thread_id    = 0;

		if (!stopped)
		{
			stopped = !analyzer_->begin_profiling();
		}

		while (nullptr != thread_entry)
		{
			if (!stopped)
			{
				thread_id = hasher(thread_entry->thread_id_);
				stopped   = !analyzer_->begin_thread_profiling(thread_id);
			}

			auto   events = thread_entry->events_head_;

			while (nullptr != events)
			{
				for (size_t i = 0; i < events->used_count_ && !stopped; i++)
				{
					stopped = !analyzer_->on_profiling(events->events_[i].name_, 
					                                   events->events_[i].start_, 
					                                   events->events_[i].stop_);
				}

				auto   tmp_events = events;

				events = events->next_;

				tmp_events->destroy();
			}

			if (!stopped)
			{
				stopped = !analyzer_->end_thread_profiling(thread_id);
			}

			thread_entry = thread_entry->next_;
		}

		if (!stopped)
		{
			stopped = !analyzer_->end_profiling();
		}

		head_ = tail_ = nullptr;

		return !stopped;
	}
};

static inline global_profiler   global_profiler_;

}//namespace qptr::srcprof::detail
