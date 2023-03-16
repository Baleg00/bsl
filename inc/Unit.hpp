#pragma once

namespace bsl
{
	struct Unit final
	{
		constexpr Unit() noexcept = default;
		constexpr Unit(const Unit&) noexcept = default;
		constexpr Unit(Unit&&) noexcept = default;

		constexpr ~Unit() noexcept = default;
	};

	[[nodiscard]] constexpr bool operator<(const Unit&, const Unit&) noexcept
	{
		return false;
	}

	[[nodiscard]] constexpr bool operator<=(const Unit&, const Unit&) noexcept
	{
		return true;
	}

	[[nodiscard]] constexpr bool operator>(const Unit&, const Unit&) noexcept
	{
		return false;
	}

	[[nodiscard]] constexpr bool operator>=(const Unit&, const Unit&) noexcept
	{
		return true;
	}

	[[nodiscard]] constexpr bool operator==(const Unit&, const Unit&) noexcept
	{
		return true;
	}

	[[nodiscard]] constexpr bool operator!=(const Unit&, const Unit&) noexcept
	{
		return false;
	}

	inline constexpr Unit unit = Unit{};
}
