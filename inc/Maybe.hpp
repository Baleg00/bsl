#pragma once

#include <new>

#include "type_traits.hpp"
#include "Error.hpp"
#include "InPlace.hpp"
#include "Nothing.hpp"
#include "Hash.hpp"

namespace bsl
{
	struct BadMaybeAccess : Error
	{
		[[nodiscard]] const char* msg() const noexcept override
		{
			return "Bad maybe access.";
		}
	};

	template<class T>
	class Maybe
	{
	private:
		union
		{
			T m_value;
			struct {} m_dummy;
		};

		bool m_has_value;

	public:
		using value_type = typename T;
		using reference_type = typename value_type&;
		using const_reference_type = typename const value_type&;
		using pointer_type = typename value_type*;
		using const_pointer_type = typename const value_type*;

		constexpr Maybe() noexcept
			: m_dummy{}, m_has_value{ false }
		{
		}

		constexpr Maybe(const_reference_type value) noexcept(is_nothrow_copy_constructible_v<value_type>)
			: m_value{ value }, m_has_value{ true }
		{
		}

		constexpr Maybe(value_type&& value) noexcept(is_nothrow_move_constructible_v<value_type>)
			: m_value{ move(value) }, m_has_value{ true }
		{
		}

		template<class... Args>
		explicit constexpr Maybe(InPlace, Args&&... args) noexcept(noexcept(value_type(forward<Args>(args)...)))
			: m_value{ forward<Args>(args)... }, m_has_value{ true }
		{
		}

		constexpr Maybe(Nothing) noexcept
			: Maybe{}
		{
		}

		template<class U, class = enable_if_t<is_constructible_v<value_type, U>>>
		explicit constexpr Maybe(const Maybe<U>& other) noexcept(is_nothrow_constructible_v<value_type, U>)
			: m_dummy{}, m_has_value{ other.has_value() }
		{
			if (m_has_value)
				::new(&m_value) value_type{ other.value() };
		}

		constexpr ~Maybe() noexcept(is_nothrow_destructible_v<value_type>)
		{
			if (has_value())
				m_value.~value_type();
		}

		constexpr Maybe& operator=(const Maybe& other) noexcept(is_nothrow_copy_assignable_v<value_type>&& is_nothrow_destructible_v<value_type>)
		{
			if (other.has_value())
				m_value = other.m_value;
			else if (has_value())
				m_value.~T();

			m_has_value = other.m_has_value;

			return *this;
		}

		constexpr Maybe& operator=(Maybe&& other) noexcept(is_nothrow_move_assignable_v<value_type>&& is_nothrow_destructible_v<value_type>)
		{
			if (other.has_value())
				m_value = move(other.m_value);
			else if (has_value())
				m_value.~value_type();

			m_has_value = other.m_has_value;

			return *this;
		}

		constexpr Maybe& operator=(const_reference_type value) noexcept(is_nothrow_copy_assignable_v<value_type>)
		{
			m_value = value;
			m_has_value = true;
			return *this;
		}

		constexpr Maybe& operator=(value_type&& value) noexcept(is_nothrow_move_assignable_v<value_type>)
		{
			m_value = move(value);
			m_has_value = true;
			return *this;
		}

		constexpr Maybe& operator=(Nothing) noexcept(is_nothrow_destructible_v<value_type>)
		{
			reset();
			return *this;
		}

		constexpr void reset() noexcept(is_nothrow_destructible_v<value_type>)
		{
			if (has_value())
				m_value.~value_type();

			m_has_value = false;
		}

		[[nodiscard]] constexpr bool has_value() const noexcept
		{
			return m_has_value;
		}

		[[nodiscard]] constexpr operator bool() const noexcept
		{
			return m_has_value;
		}

		[[nodiscard]] constexpr reference_type value() noexcept(false)
		{
			if (!has_value())
				throw BadMaybeAccess{};

			return m_value;
		}

		[[nodiscard]] constexpr const_reference_type value() const noexcept(false)
		{
			if (!has_value())
				throw BadMaybeAccess{};

			return m_value;
		}

		[[nodiscard]] constexpr reference_type value_or(reference_type value) noexcept
		{
			if (!has_value())
				return value;

			return m_value;
		}

		[[nodiscard]] constexpr const_reference_type value_or(const_reference_type value) const noexcept
		{
			if (!has_value())
				return value;

			return m_value;
		}

		[[nodiscard]] constexpr value_type&& unwrap() noexcept(false)
		{
			if (!has_value())
				throw BadMaybeAccess{};

			m_has_value = false;

			return move(m_value);
		}

		[[nodiscard]] constexpr value_type&& unwrap_or(value_type&& value) noexcept
		{
			if (!has_value())
				return move(value);

			m_has_value = false;

			return move(m_value);
		}

		[[nodiscard]] constexpr reference_type operator*() noexcept(false)
		{
			return value();
		}

		[[nodiscard]] constexpr const_reference_type operator*() const noexcept(false)
		{
			return value();
		}

		[[nodiscard]] constexpr pointer_type operator->() noexcept(false)
		{
			return &value();
		}

		[[nodiscard]] constexpr const_pointer_type operator->() const noexcept(false)
		{
			return &value();
		}
	};

	template<class T>
	struct Hash<Maybe<T>>
	{
		[[nodiscard]] constexpr usize operator()(const Maybe<T>& value) noexcept
		{
			if (value.has_value())
				return Hash<T>{}(value.value());

			return 0;
		}
	};
}
