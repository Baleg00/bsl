#pragma once

#include "type_traits.hpp"

namespace bsl
{
	template<class T, T... Values>
	struct Sequence
	{
		static inline constexpr size_t size = sizeof...(Values);
	};

	template<integral T, T... Ints>
	struct IntegerSequence : Sequence<T, Ints...> {};

	template<size_t... Idxs>
	struct IndexSequence : IntegerSequence<size_t, Idxs...> {};

	template<size_t N, size_t... Rest>
	struct MakeIndexSequence : MakeIndexSequence<N - 1, N - 1, Rest...> {};

	template<size_t... Rest>
	struct MakeIndexSequence<0, Rest...> : type_identity<IndexSequence<Rest...>> {};

	template<size_t N>
	using MakeIndexSequenceType = typename MakeIndexSequence<N>::type;
}
