#pragma once

#include "type_traits.hpp"
#include "Types.hpp"

namespace bsl
{
	template<class T>
	struct Hash;

	template<class T>
	struct _FNVHash
	{
		static constexpr usize FNV_prime = 0x00000100000001B3ULL;
		static constexpr usize FNV_offset_basis = 0xCBF29CE484222325ULL;

		[[nodiscard]] constexpr usize operator()(const T& value) const noexcept
		{
			usize h = FNV_offset_basis;

			const char* data = reinterpret_cast<const char*>(&value);

			for (usize i = 0; i < sizeof(T); ++i)
			{
				h ^= data[i];
				h *= FNV_prime;
			}

			return h;
		}
	};

	template<class T>
	requires is_integral_v<T> || is_floating_point_v<T> || is_same_v<T, bool> || is_pointer_v<T>
	struct Hash<T> : _FNVHash<T>
	{
	};

	template<class T>
	[[nodiscard]] constexpr usize hash_combine(const usize& seed, const T& value) noexcept
	{
		return seed ^ (Hash<T>{}(value) + 0x9E3779B9ULL + (seed << 6) + (seed >> 2));
	}

	template<class... Types>
	struct CombinedHash
	{
		[[nodiscard]] constexpr usize operator()(const Types&... values) const noexcept
		{
			usize h = 0;
			((h = hash_combine<Types>(h, values)), ...);
			return h;
		}
	};
}
