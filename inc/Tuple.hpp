#pragma once

#include "type_traits.hpp"
#include "Sequence.hpp"
#include "Hash.hpp"

namespace bsl
{
	template<class T, class... Rest>
	struct _TupleStorage : _TupleStorage<Rest...>
	{
		T value;

		constexpr _TupleStorage(const T& value, const Rest&... rest) noexcept(is_nothrow_copy_constructible_v<T> && is_nothrow_constructible_v<_TupleStorage<Rest...>, const Rest&...>)
			: _TupleStorage<Rest...>{ rest... }, value{ value }
		{
		}

		constexpr _TupleStorage(const _TupleStorage&) noexcept(is_nothrow_copy_constructible_v<T> && is_nothrow_copy_constructible_v<_TupleStorage<Rest...>>) = default;
		constexpr _TupleStorage(_TupleStorage&&) noexcept(is_nothrow_move_constructible_v<T> && is_nothrow_move_constructible_v<_TupleStorage<Rest...>>) = default;

		virtual constexpr ~_TupleStorage() noexcept(is_nothrow_destructible_v<T> && is_nothrow_destructible_v<_TupleStorage<Rest...>>) = default;

		[[nodiscard]] constexpr _TupleStorage<Rest...>& base() noexcept
		{
			return static_cast<_TupleStorage<Rest...>&>(*this);
		}

		[[nodiscard]] constexpr const _TupleStorage<Rest...>& base() const noexcept
		{
			return static_cast<const _TupleStorage<Rest...>&>(*this);
		}
	};

	template<class T>
	struct _TupleStorage<T>
	{
		T value;

		constexpr _TupleStorage(const T& value) noexcept(is_nothrow_copy_constructible_v<T>)
			: value{ value }
		{
		}

		constexpr _TupleStorage(const _TupleStorage&) noexcept(is_nothrow_copy_constructible_v<T>) = default;
		constexpr _TupleStorage(_TupleStorage&&) noexcept(is_nothrow_move_constructible_v<T>) = default;

		virtual constexpr ~_TupleStorage() noexcept(is_nothrow_destructible_v<T>) = default;
	};

	template<usize I, class... Types>
	[[nodiscard]] constexpr auto& get(_TupleStorage<Types...>& storage) noexcept
	{
		if constexpr (I == 0)
			return storage.value;
		else
			return get<I - 1>(storage.base());
	}

	template<usize I, class... Types>
	[[nodiscard]] constexpr const auto& get(const _TupleStorage<Types...>& storage) noexcept
	{
		if constexpr (I == 0)
			return storage.value;
		else
			return get<I - 1>(storage.base());
	}

	template<class... Types>
	class Tuple
	{
	private:
		using storage_type = typename _TupleStorage<Types...>;

		storage_type m_storage;

		template<usize I, class... Types>
		friend constexpr auto& get(Tuple<Types...>&) noexcept;

		template<usize I, class... Types>
		friend constexpr const auto& get(const Tuple<Types...>&) noexcept;

	public:
		constexpr Tuple(const Types&... args) noexcept(is_nothrow_constructible_v<storage_type, Types...>)
			: m_storage{ args... }
		{
		}

		constexpr Tuple(const Tuple&) noexcept(is_nothrow_copy_constructible_v<storage_type>) = default;
		constexpr Tuple(Tuple&&) noexcept(is_nothrow_move_constructible_v<storage_type>) = default;

		constexpr ~Tuple() noexcept(is_nothrow_destructible_v<storage_type>) = default;

		constexpr Tuple& operator=(const Tuple& other) noexcept(is_nothrow_copy_assignable_v<storage_type>)
		{
			m_storage = other.m_storage;
			return *this;
		}

		constexpr Tuple& operator=(Tuple&& other) noexcept(is_nothrow_move_assignable_v<storage_type>)
		{
			m_storage = move(other.m_storage);
			return *this;
		}

		[[nodiscard]] constexpr usize size() const noexcept
		{
			return sizeof...(Types);
		}
	};

	template<>
	class Tuple<>
	{
	public:
		constexpr Tuple() noexcept = default;
		constexpr Tuple(const Tuple&) noexcept = default;
		constexpr Tuple(Tuple&&) noexcept = default;

		constexpr ~Tuple() noexcept = default;

		constexpr Tuple& operator=(const Tuple& other) noexcept
		{
			return *this;
		}

		constexpr Tuple& operator=(Tuple&& other) noexcept
		{
			return *this;
		}

		[[nodiscard]] constexpr usize size() const noexcept
		{
			return 0;
		}
	};

	template<usize I, class T>
	struct TupleElem;

	template<usize I, class T, class... Rest>
	struct TupleElem<I, Tuple<T, Rest...>> : TupleElem<I - 1, Tuple<T, Rest...>> {};

	template<class T, class... Rest>
	struct TupleElem<0, Tuple<T, Rest...>> : type_identity<T> {};

	template<usize I, class T>
	using TupleElemType = typename TupleElem<I, T>::type;

	template<usize I, class... Types>
	[[nodiscard]] constexpr auto& get(Tuple<Types...>& tup) noexcept
	{
		return get<I>(tup.m_storage);
	}

	template<usize I, class... Types>
	[[nodiscard]] constexpr const auto& get(const Tuple<Types...>& tup) noexcept
	{
		return get<I>(tup.m_storage);
	}

	template<class F, class... Types, usize... Idxs>
	[[nodiscard]] constexpr decltype(auto) _apply_impl(F&& func, Tuple<Types...>& tup, IndexSequence<Idxs...>)
	{
		return foward<F>(func)(get<Idxs>(tup)...);
	}

	template<class F, class... Types>
	[[nodiscard]] constexpr decltype(auto) apply(F&& func, Tuple<Types...>& tup)
	{
		return _apply_impl(forward<F>(func), tup, MakeIndexSequenceType<sizeof...(Types)>{});
	}

	template<class F, class... Types, usize... Idxs>
	[[nodiscard]] constexpr decltype(auto) _apply_impl(F&& func, const Tuple<Types...>& tup, IndexSequence<Idxs...>)
	{
		return foward<F>(func)(get<Idxs>(tup)...);
	}

	template<class F, class... Types>
	[[nodiscard]] constexpr decltype(auto) apply(F&& func, const Tuple<Types...>& tup)
	{
		return _apply_impl(forward<F>(func), tup, MakeIndexSequenceType<sizeof...(Types)>{});
	}

	template<class... ToTypes, class... FromTypes, usize... Idxs>
	[[nodiscard]] constexpr Tuple<ToTypes...> _tuple_cast_impl(const Tuple<FromTypes...>& tup, IndexSequence<Idxs...>) noexcept(noexcept(Tuple<ToTypes...>{ static_cast<const ToTypes&>(get<Idxs>(tup))... }))
	{
		return Tuple<ToTypes...>{ static_cast<const ToTypes&>(get<Idxs>(tup))... };
	}

	template<class... ToTypes, class... FromTypes>
	[[nodiscard]] constexpr Tuple<ToTypes...> tuple_cast(const Tuple<FromTypes...>& tup) noexcept(noexcept(_tuple_cast_impl<ToTypes...>(tup, MakeIndexSequenceType<sizeof...(ToTypes)>{})))
	{
		return _tuple_cast_impl<ToTypes...>(tup, MakeIndexSequenceType<sizeof...(ToTypes)>{});
	}

	template<class... FirstTypes, class... SecondTypes, usize... FirstIdxs, usize... SecondIdxs>
	[[nodiscard]] constexpr Tuple<FirstTypes..., SecondTypes...> _tie_impl(const Tuple<FirstTypes...>& first, const Tuple<SecondTypes...>& second, IndexSequence<FirstIdxs...>, IndexSequence<SecondIdxs...>) noexcept(noexcept(Tuple<FirstTypes..., SecondTypes...>{ get<FirstIdxs>(first)..., get<SecondIdxs>(second)... }))
	{
		return Tuple<FirstTypes..., SecondTypes...>{ get<FirstIdxs>(first)..., get<SecondIdxs>(second)... };
	}

	template<class... FirstTypes, class... SecondTypes>
	[[nodiscard]] constexpr Tuple<FirstTypes..., SecondTypes...> tie(const Tuple<FirstTypes...>& first, const Tuple<SecondTypes...>& second) noexcept(noexcept(_tie_impl(first, second, MakeIndexSequenceType<sizeof...(FirstTypes)>{}, MakeIndexSequenceType<sizeof...(SecondTypes)>{})))
	{
		return _tie_impl(first, second, MakeIndexSequenceType<sizeof...(FirstTypes)>{}, MakeIndexSequenceType<sizeof...(SecondTypes)>{});
	}

	template<usize StartIdx, usize... Idxs, class... Types>
	[[nodiscard]] constexpr auto _slice_impl(const Tuple<Types...>& tup, IndexSequence<Idxs...>) noexcept(is_nothrow_constructible_v<Tuple<Types...>, TupleElemType<StartIdx + Idxs, Tuple<Types...>>...>)
	{
		return Tuple<Types...>{ get<StartIdx + Idxs>(tup)... };
	}

	template<usize StartIdx, usize EndIdx, class... Types>
	[[nodiscard]] constexpr auto slice(const Tuple<Types...>& tup) noexcept(noexcept(_slice_impl<StartIdx>(tup, MakeIndexSequenceType<EndIdx - StartIdx>{})))
	{
		return _slice_impl<StartIdx>(tup, MakeIndexSequenceType<EndIdx - StartIdx>{});
	}

	template<class... Types>
	struct Hash<Tuple<Types...>>
	{
		template<class... Types, usize... Idxs>
		[[nodiscard]] constexpr usize do_hash(const Tuple<Types...>& values, IndexSequence<Idxs...>) noexcept
		{
			return CombinedHash<Types...>{}(get<Idxs>(values)...);
		}

		[[nodiscard]] constexpr usize operator()(const Tuple<Types...>& values) noexcept
		{
			return do_hash(values..., MakeIndexSequenceType<sizeof...(Types)>{});
		}
	};
}
