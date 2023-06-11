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


#include <fmt/ostream.h>
#include <fstream>
#include <stack>


namespace qptr::srcprof::detail
{

class json_writer final
{
public:

	inline void clear()
	{
		while (!containers_.empty())
		{
			containers_.pop();
		}
	}

	inline bool begin()
	{
		clear();
		containers_.emplace(container_t::type_t::array);

		return true;
	}

	inline bool end()
	{
		if (containers_.empty() || !containers_.top().is_array())
		{
			return false;
		}
		containers_.pop();

		return true;
	}

	template <typename OUTPUT>
	inline bool begin_object(OUTPUT &output)
	{
		if (!add_child(output, false))
		{
			return false;
		}

		fmt::print(output, "{:\t>{}}{{", "", containers_.size() - 1);
		containers_.push(container_t(container_t::type_t::object));

		return true;
	}

	template <typename OUTPUT>
	inline bool begin_object(OUTPUT &output, const std::string_view &name)
	{
		if (!add_child(output, true))
		{
			return false;
		}

		fmt::print(output, "{:\t>{}}\"{}\":\n", "", containers_.size() - 1, name);
		fmt::print(output, "{:\t>{}}{{", "", containers_.size() - 1);
		containers_.push(container_t(container_t::type_t::object));

		return true;
	}

	template <typename OUTPUT>
	inline bool end_object(OUTPUT &output)
	{
		if (containers_.empty() || container_t::type_t::object != containers_.top().type_)
		{
			return false;
		}

		fmt::print(output, "\n{:\t>{}}}}", "", containers_.size() - 2);
		containers_.pop();

		return true;
	}

	template <typename OUTPUT>
	inline bool begin_array(OUTPUT &output)
	{
		if (!add_child(output, false))
		{
			return false;
		}

		fmt::print(output, "{:\t>{}}[[", "", containers_.size());
		containers_.push(container_t(container_t::type_t::array));

		return true;
	}

	template <typename OUTPUT>
	inline bool begin_array(OUTPUT &output, const std::string_view &name)
	{
		if (!add_child(output, true))
		{
			return false;
		}

		fmt::print(output, "{:\t>{}}\"{}\":\n", "", containers_.size() - 1, name);
		fmt::print(output, "{:\t>{}}[", "", containers_.size() - 1);
		containers_.push(container_t(container_t::type_t::array));

		return true;
	}

	template <typename OUTPUT>
	inline bool end_array(OUTPUT &output)
	{
		if (containers_.empty() || container_t::type_t::array != containers_.top().type_)
		{
			return false;
		}

		fmt::print(output, "\n{:\t>{}}]", "", containers_.size() - 2);
		containers_.pop();

		return true;
	}

	template <typename OUTPUT>
	inline bool boolean(OUTPUT &output, bool val)
	{
		if (!add_child(output, false))
		{
			return false;
		}

		fmt::print(output, "{:\t>{}}{}", "", containers_.size() - 1, val ? "true" : "false");

		return true;
	}

	template <typename OUTPUT>
	inline bool boolean(OUTPUT &output, bool val, const std::string_view &name)
	{
		if (!add_child(output, true))
		{
			return false;
		}

		fmt::print(output, "{:\t>{}}\"{}\": {}", "", containers_.size() - 1, name, val ? "true" : "false");

		return true;
	}
	

	template <typename OUTPUT, typename T, typename = std::enable_if_t<std::is_integral<T>::value>>
	inline bool integer(OUTPUT &output, T val)
	{
		if (!add_child(output, false))
		{
			return false;
		}

		fmt::print(output, "{:\t>{}}{}", "", containers_.size() - 1, val);

		return true;
	}

	template <typename OUTPUT, typename T, typename = std::enable_if_t<std::is_integral<T>::value>>
	inline bool integer(OUTPUT &output, T val, const std::string_view &name)
	{
		if (!add_child(output, true))
		{
			return false;
		}

		fmt::print(output, "{:\t>{}}\"{}\": {}", "", containers_.size() - 1, name, val);

		return true;
	}

	template <typename OUTPUT, typename T, typename = std::enable_if_t<std::is_floating_point<T>::value>>
	inline bool real(OUTPUT &output, T val, size_t decimals = 9)
	{
		if (!add_child(output, false))
		{
			return false;
		}

		fmt::print(output, "{:\t>{}}{:.{}f}", "", containers_.size() - 1, val, decimals);

		return true;
	}

	template <typename OUTPUT, typename T, typename = std::enable_if_t<std::is_floating_point<T>::value>>
	inline bool real(OUTPUT &output, T val, const std::string_view &name, size_t decimals = 9)
	{
		if (!add_child(output, true))
		{
			return false;
		}

		fmt::print(output, "{:\t>{}}\"{}\": {:.{}f}", "", containers_.size() - 1, name, val, decimals);

		return true;
	}

	template <typename OUTPUT>
	inline bool string(OUTPUT &output, const std::string_view &val)
	{
		if (!add_child(output, false))
		{
			return false;
		}

		fmt::print(output, "{:\t>{}}\"{}\"", "", containers_.size() - 1, val);

		return true;
	}

	template <typename OUTPUT>
	inline bool string(OUTPUT &output, const std::string_view &val, const std::string_view &name)
	{
		if (!add_child(output, true))
		{
			return false;
		}

		fmt::print(output, "{:\t>{}}\"{}\": \"{}\"", "", containers_.size() - 1, name, val);

		return true;
	}

private:

	struct container_t
	{
		enum class type_t
		{
			object,
			array,
		};

		type_t   type_     = type_t::array;
		size_t   children_ = 0;

		using stack = std::stack<container_t>;

		explicit container_t(type_t type) :
			type_(type)
		{
		}

		bool is_array() const
		{
			return (type_t::array == type_);
		}

		bool is_object() const
		{
			return (type_t::object == type_);
		}
	};

	container_t::stack   containers_;

	template <typename OUTPUT>
	inline bool add_child(OUTPUT &output, bool has_name)
	{
		if (containers_.empty())
		{
			return false;
		}
		else
		if (1 == containers_.size())
		{
			if (has_name)
			{
				return false;
			}
		}

		if ((containers_.top().is_object() && !has_name) || (containers_.top().is_array() && has_name))
		{
			return false;
		}

		if (1 < containers_.size())
		{
			if (0 < containers_.top().children_)
			{
				fmt::print(output, ",\n");
			}
			else
			if (1 < containers_.size())
			{
				fmt::print(output, "\n");
			}
		}

		containers_.top().children_++;

		return true;
	}
};

}//namespace qptr::srcprof::detail
