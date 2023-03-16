#pragma once

#include "Types.hpp"
#include "type_traits.hpp"
#include "Memory.hpp"

namespace bsl::math
{
	using namespace bsl;

	template<class T, usize R, usize C>
	class Matrix
	{
	private:
		static_assert(is_arithmetic_v<T> && R > 0 && C > 0);

		T m_data[R][C];

	public:
		constexpr Matrix() noexcept
			: m_data{ 0 }
		{
		}

		constexpr Matrix(const T data[R][C]) noexcept
			: Matrix{}
		{
			mem::copy<T>(m_data, data, R * C);
		}

		constexpr Matrix(const Matrix& other) noexcept
			: Matrix{}
		{
			mem::copy<T>(m_data, other.m_data, R * C);
		}

		constexpr Matrix(Matrix&& other) noexcept
			: Matrix{}
		{
			mem::move<T>(m_data, other.m_data, R * C);
		}

		[[nodiscard]] constexpr usize rows() const noexcept
		{
			return R;
		}

		[[nodiscard]] constexpr usize cols() const noexcept
		{
			return C;
		}

		[[nodiscard]] constexpr T& at(usize r, usize c)
		{
			return m_data[r][c];
		}

		[[nodiscard]] constexpr const T& at(usize r, usize c) const
		{
			return m_data[r][c];
		}

		[[nodiscard]] constexpr Matrix operator+() const noexcept
		{
			return *this;
		}


		[[nodiscard]] constexpr Matrix operator-() const noexcept
		{
			Matrix result{};

			for (usize r = 0; r < result.rows(); ++r)
				for (usize c = 0; c < result.cols(); ++c)
					result.at(r, c) = -at(r, c);

			return *this;
		}

		[[nodiscard]] constexpr Matrix operator+(const Matrix& rhs) const noexcept
		{
			Matrix result{};

			for (usize r = 0; r < result.rows(); ++r)
				for (usize c = 0; c < result.cols(); ++c)
					result.at(r, c) = at(r, c) + rhs.at(r, c);

			return result;
		}

		[[nodiscard]] constexpr Matrix operator-(const Matrix& rhs) const noexcept
		{
			Matrix result{};

			for (usize r = 0; r < result.rows(); ++r)
				for (usize c = 0; c < result.cols(); ++c)
					result.at(r, c) = at(r, c) - rhs.at(r, c);

			return result;
		}

		template<usize _C>
		[[nodiscard]] constexpr Matrix<T, R, _C> operator*(const Matrix<T, C, _C>& rhs) const noexcept
		{
			Matrix<T, R, _C> result{};

			for (usize r = 0; r < result.rows(); ++r)
				for (usize c = 0; c < result.cols(); ++c)
					for (usize i = 0; i < cols(); ++i)
						result.at(r, c) += at(r, i) * rhs.at(i, c);

			return result;
		}

		[[nodiscard]] constexpr Matrix operator*(const T& rhs) const noexcept
		{
			Matrix result{};

			for (usize r = 0; r < result.rows(); ++r)
				for (usize c = 0; c < result.cols(); ++c)
					result.at(r, c) = at(r, c) * rhs;

			return result;
		}

		[[nodiscard]] constexpr Matrix operator/(const T& rhs) const noexcept
		{
			Matrix result{};

			for (usize r = 0; r < result.rows(); ++r)
				for (usize c = 0; c < result.cols(); ++c)
					result.at(r, c) = at(r, c) / rhs;

			return result;
		}

		[[nodiscard]] constexpr Matrix<T, C, R> transpose() const noexcept
		{
			Matrix<T, C, R> result{};

			for (usize r = 0; r < rows(); ++r)
				for (usize c = 0; c < cols(); ++c)
					result.at(c, r) = at(r, c);

			return result;
		}

		template<class = enable_if_t<(R > 1 && C > 1)>>
		[[nodiscard]] constexpr Matrix<T, R - 1, C - 1> submatrix(usize r, usize c) const noexcept
		{
			Matrix<T, R - 1, C - 1> result{};

			for (usize i = 0, di = 0; i < rows(); ++i)
			{
				if (i == r)
				{
					++di;
					continue;
				}

				for (usize j = 0, dj = 0; j < cols(); ++j)
				{
					if (j == c)
					{
						++dj;
						continue;
					}

					result.at(i - di, j - dj) = at(i, j);
				}
			}

			return result;
		}
	};

	typedef Matrix<i32, 2, 2> Mat2i;
	typedef Matrix<i32, 3, 3> Mat3i;
	typedef Matrix<i32, 4, 4> Mat4i;

	typedef Matrix<i64, 2, 2> Mat2l;
	typedef Matrix<i64, 3, 3> Mat3l;
	typedef Matrix<i64, 4, 4> Mat4l;

	typedef Matrix<f32, 2, 2> Mat2f;
	typedef Matrix<f32, 3, 3> Mat3f;
	typedef Matrix<f32, 4, 4> Mat4f;

	typedef Matrix<f64, 2, 2> Mat2d;
	typedef Matrix<f64, 3, 3> Mat3d;
	typedef Matrix<f64, 4, 4> Mat4d;
}
