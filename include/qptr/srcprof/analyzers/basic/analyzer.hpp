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


#include <qptr/srcprof/analyzers/basic/output.hpp>
#include <qptr/srcprof/analyzer.hpp>
#include <ankerl/unordered_dense.h>
#include <vector>
#include <algorithm>
#include <memory>


namespace qptr::srcprof::analyzers::basic
{

class analyzer final : public srcprof::analyzer
{
public:

	static inline const std::string   NAME = "basic";

	inline explicit analyzer(std::shared_ptr<output> output) : 
		output_(output)
	{
	}

	inline ~analyzer()
	{
		cleanup_call_graph(&root_scope_);
	}

	const std::string &get_name() override
	{
		return NAME;
	}

	inline bool begin_profiling() override
	{
		current_scope_ = &root_scope_;

		return true;
	}

	inline bool end_profiling() override
	{
		scope_stats_t::map      scopes_stats_map;
		scope_stats_t::vector   scopes_stats_vector;
		output::stats_t         stats;

		process_tracing_events(&root_scope_, scopes_stats_map, stats);
		scope_stats_t::finalize(scopes_stats_map, scopes_stats_vector);

		output_->begin();

		if (!output_->on_stats(stats))
		{
			return false;
		}

		std::sort(scopes_stats_vector.begin(), scopes_stats_vector.end(), scope_stats_t::sort_desc_by_total());
		if (!write_scope_stats("scopes_by_total_duration", scopes_stats_vector))
		{
			return false;
		}

		std::sort(scopes_stats_vector.begin(), scopes_stats_vector.end(), scope_stats_t::sort_desc_by_avg());
		if (!write_scope_stats("scopes_by_avg_duration", scopes_stats_vector))
		{
			return false;
		}

		std::sort(scopes_stats_vector.begin(), scopes_stats_vector.end(), scope_stats_t::sort_desc_by_max());
		if (!write_scope_stats("scopes_by_max_duration", scopes_stats_vector))
		{
			return false;
		}

		if (!write_call_graph())
		{
			return false;
		}

		output_->end();

		return true;
	}

	inline bool begin_thread_profiling(size_t thread_id) override
	{
		auto   now = detail::clock::now();

		while (current_scope_ != &root_scope_)
		{
			leave_scope(now, now);
			current_scope_ = current_scope_->parent_;
		}

		return true;
	}

	inline bool end_thread_profiling(size_t thread_id) override
	{
		while (current_scope_ != &root_scope_)
		{
			current_scope_ = current_scope_->parent_;
		}

		return true;
	}

	inline bool on_profiling(const char *name, 
	                         const detail::clock::time_point &start, const detail::clock::time_point &end) override
	{
		if (nullptr != name)
		{
			(void) enter_scope(name, start, end);
		}
		else
		{
			(void) leave_scope(start, end);
		}

		return true;
	}

private:

	struct scope_t final
	{
		detail::clock::time_point   begin_;
		detail::clock::rep          profiler_duration_     = 0;
		detail::clock::rep          child_scopes_duration_ = 0;
		scope_t                     *parent_               = nullptr;
		scope_t                     *prev_                 = nullptr;
		scope_t                     *next_                 = nullptr;
		scope_t                     *children_head_        = nullptr;
		scope_t                     *children_tail_        = nullptr;
		output::scope_t             scope_;
	};

	struct scope_stats_t
	{
		const char          *name_ = nullptr;
		output::profile_t   profile_;

		using map    = ankerl::unordered_dense::map<std::string_view, scope_stats_t>;
		using vector = std::vector<scope_stats_t>;

		struct sort_desc_by_total
		{
			constexpr bool operator()(const scope_stats_t &l, const scope_stats_t &r) const noexcept
			{
				return l.profile_.total_ > r.profile_.total_;
			}
		};

		struct sort_desc_by_max
		{
			constexpr bool operator()(const scope_stats_t &l, const scope_stats_t &r) const noexcept
			{
				return l.profile_.max_ > r.profile_.max_;
			}
		};

		struct sort_desc_by_avg
		{
			constexpr bool operator()(const scope_stats_t &l, const scope_stats_t &r) const noexcept
			{
				return l.profile_.avg_ > r.profile_.avg_;
			}
		};

		static inline void finalize(map &map, vector &vector)
		{
			for (auto & [key, value] : map)
			{
				value.profile_.end();
				vector.push_back(value);
			}
		}
	};

	std::shared_ptr<output>   output_;
	scope_t                   root_scope_;
	scope_t                   *current_scope_ = &root_scope_;

	inline bool enter_scope(const char *name, 
	                        const detail::clock::time_point &start, const detail::clock::time_point &stop)
	{
		if (nullptr == current_scope_)
		{
			return false;
		}

		auto   scope = current_scope_->children_head_;

		while (nullptr != scope)
		{
			if (name == scope->scope_.name_)
			{
				break;
			}
			else
			{
				scope = scope->next_;
			}
		}

		if (nullptr == scope)
		{
			if (nullptr == (scope = new (std::nothrow) scope_t()))
			{
				return false;
			}

			scope->scope_.name_ = name;
			scope->parent_      = current_scope_;

			scope->scope_.profiler_duration_.begin();
			scope->scope_.scope_duration_.begin();
			scope->scope_.child_scopes_duration_.begin();

			if (nullptr == current_scope_->children_tail_)
			{
				current_scope_->children_head_ = scope;
				current_scope_->children_tail_ = scope;
			}
			else
			{
				current_scope_->children_tail_->next_ = scope;
				scope->prev_                          = current_scope_->children_tail_;
				current_scope_->children_tail_        = scope;
			}
		}

		scope->profiler_duration_ = detail::clock::duration(stop - start).count();
		scope->begin_             = start;

		current_scope_ = scope;

		return true;
	}

	inline bool leave_scope(const detail::clock::time_point &start, const detail::clock::time_point &stop)
	{
		if (nullptr == current_scope_)
		{
			return false;
		}

		current_scope_->profiler_duration_ += detail::clock::duration(stop - start).count();
		current_scope_->scope_.profiler_duration_.update(current_scope_->profiler_duration_);

		auto   total_duration = detail::clock::duration(stop - current_scope_->begin_).count();
		auto   scope_duration = total_duration - 
		                        current_scope_->profiler_duration_ - current_scope_->child_scopes_duration_;

		current_scope_->scope_.scope_duration_.update(scope_duration);

		if (nullptr != current_scope_->parent_)
		{
			current_scope_->parent_->scope_.child_scopes_duration_.update(scope_duration);
		}
		 
		current_scope_ = current_scope_->parent_;

		return true;
	}

	inline void process_tracing_events(scope_t *scope, scope_stats_t::map &scopes_stats_map, output::stats_t &stats)
	{
		auto   child = scope->children_head_;

		while (nullptr != child)
		{
			child->scope_.child_scopes_duration_.end();
			child->scope_.scope_duration_.end();
			child->scope_.profiler_duration_.end();

			stats.total_calls_ += child->scope_.scope_duration_.count_;
			stats.duration_total_ += child->scope_.scope_duration_.total_;
			stats.duration_profiler_total_ += child->scope_.profiler_duration_.total_;

			std::string_view   name(child->scope_.name_, std::strlen(child->scope_.name_));

			auto   iter = scopes_stats_map.find(name);

			if (scopes_stats_map.end() != iter)
			{
				iter->second.profile_.update(child->scope_.scope_duration_.min_, child->scope_.scope_duration_.max_, 
				                             child->scope_.scope_duration_.total_, child->scope_.scope_duration_.count_);
			}
			else
			{
				scopes_stats_map[name].name_ = name.data();
				scopes_stats_map[name].profile_.begin();
				scopes_stats_map[name].profile_.update(child->scope_.scope_duration_.min_, 
				                                       child->scope_.scope_duration_.max_, 
				                                       child->scope_.scope_duration_.total_, 
				                                       child->scope_.scope_duration_.count_);
			}

			process_tracing_events(child, scopes_stats_map, stats);

			child = child->next_;
		}
	}

	inline bool write_scope_stats(const std::string_view &stats_name, const scope_stats_t::vector &scopes_stats_vector)
	{
		if (!output_->begin_scope_stats(stats_name))
		{
			return false;
		}
		for (const auto &value : scopes_stats_vector)
		{
			if (!output_->on_scope_stats(value.name_, value.profile_))
			{
				return false;
			}
		}
		if (!output_->end_scope_stats())
		{
			return false;
		}

		return true;
	}

	inline bool write_call_graph()
	{
		if (!output_->begin_call_graph())
		{
			return false;
		}
		if (!write_call_graph(&root_scope_))
		{
			return false;
		}
		if (!output_->end_call_graph())
		{
			return false;
		}

		return true;
	}

	inline bool write_call_graph(const scope_t *scope, size_t depth = 0)
	{
		auto   child = scope->children_head_;

		while (nullptr != child)
		{
			if (!output_->begin_call_graph_scope(child->scope_, depth))
			{
				return false;
			}

			if (!write_call_graph(child, depth + 1))
			{
				return false;
			}

			if (!output_->end_call_graph_scope(child->scope_, depth))
			{
				return false;
			}

			child = child->next_;
		}

		return true;
	}

	inline void cleanup_call_graph(scope_t *scope)
	{
		auto   child = scope->children_head_;

		while (nullptr != child)
		{
			cleanup_call_graph(child);

			auto   tmp = child;

			child = child->next_;

			delete tmp;
		}
		scope->children_head_ = nullptr;
		scope->children_tail_ = nullptr;
	}
};

}//namespace qptr::srcprof::analyzers::basic
