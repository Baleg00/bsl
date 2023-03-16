#pragma once

#include <string>

#include "Types.hpp"
#include "type_traits.hpp"

namespace bsl::mem
{
	using namespace bsl;

	template<class T>
	T* copy(T* dest, const T* src, usize count)
	{
		return reinterpret_cast<T*>(::std::memcpy(dest, src, static_cast<::std::size_t>(count) * sizeof(T)));
	}

	template<class T>
	T* move(T* dest, const T* src, usize count)
	{
		return reinterpret_cast<T*>(::std::memmove(dest, src, static_cast<::std::size_t>(count) * sizeof(T)));
	}

	template<class T>
	T* clear(T* dest, T value, usize count)
	{
		for (usize i = 0; i < count; ++i)
			copy(dest + i, &value, 1);

		return dest;
	}

	template<>
	byte* clear<byte>(byte* dest, byte value, usize count)
	{
		return reinterpret_cast<byte*>(::std::memset(dest, static_cast<int>(value), static_cast<::std::size_t>(count)));
	}
}
