#pragma once

#include "type_traits.hpp"
#include "Error.hpp"
#include "InPlace.hpp"
#include "Maybe.hpp"

namespace bsl
{
	struct BadResultAccess : BadAccess
	{
		[[nodiscard]] const char* msg() const noexcept override
		{
			return "Bad result access.";
		}
	};

	struct BadResultMove : Error
	{
		[[nodiscard]] const char* msg() const noexcept override
		{
			return "Bad result move.";
		}
	};

	using ResultOk = struct {};

	inline constexpr ResultOk result_ok = ResultOk{};

	using ResultError = struct {};

	inline constexpr ResultError result_error = ResultError{};

	template<class R, class E>
	class Result
	{
	private:
		union
		{
			R m_result;
			E m_error;
			struct {} m_dummy;
		};

		bool m_is_ok;
		bool m_is_unwrapped;

	public:
		using OkType = typename R;
		using ErrorType = typename E;

		constexpr Result(OkType&& res) noexcept(is_nothrow_move_constructible_v<OkType>)
			: m_result{ move(res) }, m_is_ok{ true }, m_is_unwrapped{ false }
		{
		}

		constexpr Result(ResultOk, OkType&& res) noexcept(is_nothrow_move_constructible_v<OkType>)
			: m_result{ move(res) }, m_is_ok{ true }, m_is_unwrapped{ false }
		{
		}

		constexpr Result(ErrorType&& err) noexcept(is_nothrow_move_constructible_v<ErrorType>)
			: m_error{ move(err) }, m_is_ok{ false }, m_is_unwrapped{ false }
		{
		}

		constexpr Result(ResultError, ErrorType&& err) noexcept(is_nothrow_move_constructible_v<ErrorType>)
			: m_error{ move(err) }, m_is_ok{ false }, m_is_unwrapped{ false }
		{
		}

		template<class... Args>
		constexpr Result(InPlace, ResultOk, Args&&... args) noexcept(noexcept(OkType(forward<Args>(args)...)))
			: m_result{ forward<Args>(args)... }, m_is_ok{ true }, m_is_unwrapped{ false }
		{
		}

		template<class... Args>
		constexpr Result(InPlace, ResultError, Args&&... args) noexcept(noexcept(ErrorType(forward<Args>(args)...)))
			: m_error{ forward<Args>(args)... }, m_is_ok{ false }, m_is_unwrapped{ false }
		{
		}

		Result(const Result&) = delete;

		constexpr Result(Result&& other) noexcept(is_nothrow_move_constructible_v<OkType> && is_nothrow_move_constructible_v<ErrorType>)
			: m_dummy{}, m_is_ok{ other.m_is_ok }, m_is_unwrapped{ false }
		{
			if (other.m_is_unwrapped)
				throw BadResultMove{};

			if (is_ok())
				m_result = move(other.m_result);
			else
				m_error = move(other.m_error);
		}

		constexpr ~Result() noexcept(is_nothrow_destructible_v<OkType> && is_nothrow_destructible_v<ErrorType>)
		{
			if (!m_is_unwrapped)
				if (is_ok())
					m_result.~OkType();
				else
					m_error.~ErrorType();
		}

		[[nodiscard]] constexpr bool is_ok() const noexcept
		{
			return m_is_ok;
		}

		[[nodiscard]] constexpr bool is_error() const noexcept
		{
			return !m_is_ok;
		}

		[[nodiscard]] constexpr Maybe<OkType> to_ok() noexcept
		{
			if (is_error() || m_is_unwrapped)
				return {};

			m_is_unwrapped = true;

			return move(m_result);
		}

		[[nodiscard]] constexpr Maybe<ErrorType> to_error() noexcept
		{
			if (is_ok() || m_is_unwrapped)
				return {};

			m_is_unwrapped = true;

			return move(m_error);
		}

		[[nodiscard]] constexpr OkType expect() noexcept(false)
		{
			if (m_is_unwrapped)
				throw BadResultAccess{};

			if (is_error())
				throw BadResultAccess{};

			m_is_unwrapped = true;

			return move(m_result);
		}

		[[nodiscard]] constexpr OkType expect_or(OkType&& value) noexcept
		{
			if (m_is_unwrapped)
				throw BadResultAccess{};

			if (is_error())
				return move(value);

			m_is_unwrapped = true;

			return move(m_result);
		}

		template<class = enable_if_t<is_default_constructible_v<OkType>>>
		[[nodiscard]] constexpr OkType expect_or_default() noexcept(is_nothrow_default_constructible_v<OkType>)
		{
			if (m_is_unwrapped)
				throw BadResultAccess{};

			if (is_error())
				return OkType{};

			m_is_unwrapped = true;

			return move(m_result);
		}

		constexpr void expect_and_discard() noexcept(false)
		{
			static_cast<void>(expect());
		}

		[[nodiscard]] constexpr ErrorType expect_error() noexcept(false)
		{
			if (m_is_unwrapped)
				throw BadResultAccess{};

			if (is_ok())
				throw BadResultAccess{};

			m_is_unwrapped = true;

			return move(m_error);
		}

		constexpr void discard() noexcept
		{
		}

		[[nodiscard]] constexpr operator bool() const noexcept
		{
			return is_ok();
		}

		[[nodiscard]] constexpr OkType operator*() noexcept(false)
		{
			return expect();
		}
	};

	template<class R, class E>
	struct Hash<Result<R, E>>
	{
	};
}
