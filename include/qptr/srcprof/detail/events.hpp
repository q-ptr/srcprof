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


#include <qptr/srcprof/detail/event.hpp>


namespace qptr::srcprof::detail
{

struct events_t final
{
	events_t   *prev_;
	events_t   *next_;
	size_t     total_count_;
	size_t     used_count_;
	event_t    events_[1];

	inline static events_t *create(size_t total_count)
	{
		if (0 == total_count)
		{
			return nullptr;
		}
		else
		{
			auto   events = (events_t *)std::malloc(sizeof(events_t) + sizeof(event_t) * (total_count - 1));

			if (nullptr == events)
			{
				return nullptr;
			}

			events->prev_        = nullptr;
			events->next_        = nullptr;
			events->total_count_ = total_count;
			events->used_count_  = 0;

			return events;
		}
	}

	inline void destroy()
	{
		std::free(this);
	}
};

}//namespace qptr::srcprof::detail
