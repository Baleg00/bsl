#pragma once

#include <new>

#include "type_traits.hpp"
#include "Types.hpp"
#include "Error.hpp"
#include "Unit.hpp"
#include "InPlace.hpp"
#include "Hash.hpp"

namespace bsl
{
	struct BadVariantAccess : Error
	{
		[[nodiscard]] const char* msg() const noexcept override
		{
			return "Bad variant access.";
		}
	};

	template<class... Types>
	struct _VariantStorage
	{
		static inline constexpr usize max_align = max_of_v<usize, alignof(Types)...>;
		static inline constexpr usize max_size = max_of_v<usize, sizeof(Types)...>;

		alignas(max_align) unsigned char data[max_size];
	};

	template<class... Types>
	class Variant;

	template<usize I, class T>
	struct VariantElem;

	template<usize I, class T, class... Rest>
	struct VariantElem<I, Variant<T, Rest...>> : VariantElem<I - 1, Variant<Rest...>> {};

	template<class T, class... Rest>
	struct VariantElem<0, Variant<T, Rest...>> : type_identity<T> {};

	template<usize I, class T>
	using VariantElemType = typename VariantElem<I, T>::type;

	template<class T, class V, usize I = 0>
	struct VariantIndex;

	template<usize I, class T, class First, class... Rest>
	struct VariantIndex<T, Variant<First, Rest...>, I> : conditional_t<is_same_v<T, First>, value_type<usize, I>, VariantIndex<T, Variant<Rest...>, I + 1>> {};

	template<class T, class V>
	inline constexpr usize variant_index = VariantIndex<T, V>::value;

	template<usize I, class T, class... Rest>
	struct _VariantManager : _VariantManager<I + 1, Rest...>
	{
		template<class... Args>
		static constexpr void construct(usize type_index, unsigned char* data, Args&&... args)
		{
			if (type_index == I)
				::new(data) T(forward<Args>(args)...);
			else
				_VariantManager<I + 1, Rest...>::construct(type_index, data, forward<Args>(args)...);
		}

		static constexpr void copy_construct(usize type_index, unsigned char* data, const unsigned char* other)
		{
			if (type_index == I)
				::new(data) T(*reinterpret_cast<const T*>(other));
			else
				_VariantManager<I + 1, Rest...>::copy_construct(type_index, data, other);
		}

		static constexpr void move_construct(usize type_index, unsigned char* data, unsigned char* other)
		{
			if (type_index == I)
				::new(data) T(move(*reinterpret_cast<T*>(other)));
			else
				_VariantManager<I + 1, Rest...>::move_construct(type_index, data, other);
		}

		static constexpr void destruct(usize type_index, unsigned char* data)
		{
			if (type_index == I)
				reinterpret_cast<T*>(data)->~T();
			else
				_VariantManager<I + 1, Rest...>::destruct(type_index, data);
		}
	};

	template<usize I, class T>
	struct _VariantManager<I, T>
	{
		template<class... Args>
		static constexpr void construct(usize type_index, unsigned char* data, Args&&... args)
		{
			if (type_index == I)
				::new(data) T(forward<Args>(args)...);
		}

		static constexpr void copy_construct(usize type_index, unsigned char* data, const unsigned char* other)
		{
			if (type_index == I)
				::new(data) T(*reinterpret_cast<const T*>(other));
		}

		static constexpr void move_construct(usize type_index, unsigned char* data, unsigned char* other)
		{
			if (type_index == I)
				::new(data) T(move(*reinterpret_cast<T*>(other)));
		}

		static constexpr void destruct(usize type_index, unsigned char* data)
		{
			if (type_index == I)
				reinterpret_cast<T*>(data)->~T();
		}
	};

	template<class... Types>
	class Variant
	{
	private:
		using manager_type = _VariantManager<0, Types...>;
		using storage_type = _VariantStorage<Types...>;

		storage_type m_storage;

		usize m_type_index;

		template<usize I, class... Types>
		friend constexpr auto& get(Variant<Types...>&);

		template<usize I, class... Types>
		friend constexpr const auto& get(const Variant<Types...>&);

		template<class T, class... Types>
		friend constexpr T& get(Variant<Types...>&);

		template<class T, class... Types>
		friend constexpr const T& get(const Variant<Types...>&);

	public:
		constexpr Variant() noexcept(is_nothrow_default_constructible_v<VariantElemType<0, Variant>>)
			: m_type_index{ 0 }
		{
			static_assert(is_default_constructible_v<VariantElemType<0, Variant>>, "First type has no default constructor.");

			::new(m_storage.data) VariantElemType<0, Variant>{};
		}

		template<class T, class... Args>
		explicit constexpr Variant(InPlaceType<T>, Args&&... args) noexcept(is_nothrow_constructible_v<T, Args...>)
			: m_type_index{ variant_index<T, Variant> }
		{
			static_assert(is_any_of_v<T, Types...>, "Invalid in place type for variant.");
			static_assert(is_constructible_v<T, Args...>, "In place type has no matching constructor.");
			
			::new(m_storage.data) T{ forward<Args>(args)... };
		}

		template<usize I, class... Args>
		explicit constexpr Variant(InPlaceIndex<I>, Args&&... args) noexcept(is_nothrow_constructible_v<VariantElemType<I, Variant>, Args...>)
			: m_type_index{ I }
		{
			static_assert(I < sizeof...(Types), "Type index is out of range.");

			::new(m_storage.data) VariantElemType<I, Variant>{ forward<Args>(args)... };
		}

		constexpr Variant(const Variant& other)
			: m_type_index{ other.m_type_index }
		{
			manager_type::copy_construct(index(), m_storage.data, other.m_storage.data);
		}

		constexpr Variant(Variant&& other)
			: m_type_index{ other.m_type_index }
		{
			manager_type::move_construct(index(), m_storage.data, other.m_storage.data);
		}

		constexpr ~Variant()
		{
			manager_type::destruct(index(), m_storage.data);
		}

		template<class T, class = enable_if_t<is_any_of_v<T, Types...> && is_copy_constructible_v<T>>>
		constexpr Variant& operator=(const T& value)
		{
			manager_type::destruct(index(), m_storage.data);
			m_type_index = variant_index<T, Variant>;
			::new(m_storage.data) T(value);
			return *this;
		}

		template<class T, class = enable_if_t<is_any_of_v<T, Types...>&& is_move_constructible_v<T>>>
		constexpr Variant& operator=(T&& value)
		{
			manager_type::destruct(index(), m_storage.data);
			m_type_index = variant_index<T, Variant>;
			::new(m_storage.data) T(move(value));
			return *this;
		}

		[[nodiscard]] constexpr usize index() const noexcept
		{
			return m_type_index;
		}
	};

	template<usize I, class... Types>
	[[nodiscard]] constexpr auto& get(Variant<Types...>& var)
	{
		if (I != var.index())
			throw BadVariantAccess{};

		return *reinterpret_cast<VariantElemType<I, Variant<Types...>>*>(var.m_storage.data);
	}

	template<usize I, class... Types>
	[[nodiscard]] constexpr const auto& get(const Variant<Types...>& var)
	{
		if (I != var.index())
			throw BadVariantAccess{};

		return *reinterpret_cast<const VariantElemType<I, Variant<Types...>>*>(var.m_storage.data);
	}

	template<class T, class... Types>
	[[nodiscard]] constexpr T& get(Variant<Types...>& var)
	{
		return get<variant_index<T, Variant<Types...>>>(var);
	}

	template<class T, class... Types>
	[[nodiscard]] constexpr const T& get(const Variant<Types...>& var)
	{
		return get<variant_index<T, Variant<Types...>>>(var);
	}

	template<class... Types>
	struct Hash<Variant<Types...>>
	{
		template<usize I = 0>
		[[nodiscard]] constexpr usize do_hash(const Variant<Types...>& value) noexcept
		{
			if (value.index() == I)
				return Hash<VariantElemType<I, Variant<Types...>>>{}(get<I>(value));

			if constexpr (I + 1 < sizeof...(Types))
				return do_hash<I + 1>(value);
			else
				return 0;
		}

		[[nodiscard]] constexpr usize operator()(const Variant<Types...>& value) noexcept
		{
			return do_hash(value);
		}
	};
}
