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


#include <qptr/srcprof/detail/events.hpp>
#include <thread>


namespace qptr::srcprof::detail
{

struct thread_entries_t final
{
	thread_entries_t   *prev_;
	thread_entries_t   *next_;
	events_t           *events_head_;
	events_t           *events_tail_;
	std::thread::id    thread_id_;

	inline static thread_entries_t *create(size_t count)
	{
		auto   thread_entries = new (std::nothrow) thread_entries_t();

		if (nullptr == thread_entries)
		{
			return nullptr;
		}

		auto   events = events_t::create(count);

		if (nullptr == events)
		{
			thread_entries->destroy();
			thread_entries = nullptr;

			return nullptr;
		}

		thread_entries->prev_        = nullptr;
		thread_entries->next_        = nullptr;
		thread_entries->events_head_ = events;
		thread_entries->events_tail_ = events;
		thread_entries->thread_id_   = std::this_thread::get_id();

		return thread_entries;
	}

	inline void destroy()
	{
		auto       events      = events_head_;
		events_t   *tmp_events = nullptr;

		while (nullptr != events)
		{
			tmp_events = events;
			events     = events->next_;
			tmp_events->destroy();
		}

		delete this;
	}

	inline event_t *get_next_event(size_t count)
	{
		if (events_tail_->used_count_ == events_tail_->total_count_)
		{
			auto   events = events_t::create(count);

			if (nullptr == events)
			{
				return nullptr;
			}

			events_tail_->next_  = events;
			events->prev_        = events_tail_;
			events_tail_         = events;
		}

		return &events_tail_->events_[events_tail_->used_count_++];
	}
};

}//namespace qptr::srcprof::detail
