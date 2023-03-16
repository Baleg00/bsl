#pragma once

namespace bsl
{
	struct InPlace {};

	inline constexpr InPlace in_place = InPlace{};

	template<class T>
	struct InPlaceType {};

	template<size_t I>
	struct InPlaceIndex {};
}
