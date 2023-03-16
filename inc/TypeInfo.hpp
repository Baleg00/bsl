#pragma once

#include "Types.hpp"
#include "Hash.hpp"

namespace bsl
{
	class TypeId
	{
	private:
		friend TypeId _next_type_id() noexcept;
		friend struct Hash<TypeId>;

		usize m_id;

		constexpr TypeId(usize id) noexcept
			: m_id{ id }
		{
		}

	public:
		TypeId() = delete;

		constexpr TypeId(const TypeId&) noexcept = default;
		constexpr TypeId(TypeId&&) noexcept = default;

		constexpr ~TypeId() noexcept = default;

		constexpr TypeId& operator=(const TypeId&) noexcept = default;
		constexpr TypeId& operator=(TypeId&&) noexcept = default;

		[[nodiscard]] constexpr bool operator==(const TypeId& rhs) const noexcept
		{
			return m_id == rhs.m_id;
		}

		[[nodiscard]] constexpr bool operator!=(const TypeId& rhs) const noexcept
		{
			return m_id != rhs.m_id;
		}
	};

	[[nodiscard]] static TypeId _next_type_id() noexcept
	{
		static usize id = 0;
		return TypeId{ id++ };
	}

	template<class T>
	struct TypeInfo
	{
		[[nodiscard]] static constexpr TypeId id() noexcept
		{
			static const TypeId type_id = _next_type_id();
			return type_id;
		}
	};

	template<>
	struct Hash<TypeId>
	{
		[[nodiscard]] constexpr usize operator()(const TypeId& value) const noexcept
		{
			return Hash<usize>{}(value.m_id);
		}
	};

	template<class T>
	struct Hash<TypeInfo<T>>
	{
		[[nodiscard]] constexpr usize operator()(const TypeInfo<T>& value) const noexcept
		{
			return Hash<TypeId>{}(value.id());
		}
	};
}
