#pragma once

#include "Types.hpp"
#include "Error.hpp"
#include "Maybe.hpp"

namespace bsl
{
	template<class T>
	class ArrayIter
	{
	private:
		T *m_begin, *m_end, *m_current;

		template<class T, usize S>
		friend class Array;

		constexpr ArrayIter(T* begin, T* end, T* current) noexcept
			: m_begin{ begin }, m_end{ end }, m_current{ current }
		{
		}

	public:
		[[nodiscard]] constexpr Maybe<ArrayIter> next() noexcept
		{
			if (m_current + 1 == m_end)
				return {};

			return ArrayIter{ m_begin, m_end, m_current + 1 };
		}

		[[nodiscard]] constexpr Maybe<ArrayIter> prev() noexcept
		{
			if (m_current == m_begin)
				return {};

			return ArrayIter{ m_begin, m_end, m_current - 1 };
		}
	};

	template<class T, usize S>
	class Array
	{
	private:
		T m_data[S];

	public:
		[[nodiscard]] constexpr T& at(usize i) noexcept(false)
		{
			if (i >= size())
				throw OutOfRange{};

			return m_data[i];
		}

		[[nodiscard]] constexpr const T& at(usize i) const noexcept(false)
		{
			if (i >= size())
				throw OutOfRange{};

			return m_data[i];
		}

		[[nodiscard]] constexpr T& operator[](usize i) noexcept(false)
		{
			return at(i);
		}

		[[nodiscard]] constexpr const T& operator[](usize i) const noexcept(false)
		{
			return at(i);
		}

		[[nodiscard]] constexpr usize size() const noexcept
		{
			return S;
		}

		[[nodiscard]] constexpr usize capacity() const noexcept
		{
			return S;
		}

		[[nodiscard]] constexpr bool empty() const noexcept
		{
			return false;
		}
	};
}
