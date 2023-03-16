#pragma once

namespace bsl
{
	namespace op
	{
		template<class T>
		struct PreIncrement
		{
			[[nodiscard]] constexpr T operator()(const T& value) const
			{
				return ++value;
			}
		};

		template<class T>
		struct PostIncrement
		{
			[[nodiscard]] constexpr T operator()(const T& value) const
			{
				return value++;
			}
		};

		template<class T>
		struct PreDecrement
		{
			[[nodiscard]] constexpr T operator()(const T& value) const
			{
				return --value;
			}
		};

		template<class T>
		struct PostDecrement
		{
			[[nodiscard]] constexpr T operator()(const T& value) const
			{
				return value--;
			}
		};

		template<class T>
		struct Add
		{
			[[nodiscard]] constexpr T operator()(const T& lhs, const T& rhs) const
			{
				return lhs + rhs;
			}
		};

		template<class T>
		struct Subtract
		{
			[[nodiscard]] constexpr T operator()(const T& lhs, const T& rhs) const
			{
				return lhs - rhs;
			}
		};

		template<class T>
		struct Multiply
		{
			[[nodiscard]] constexpr T operator()(const T& lhs, const T& rhs) const
			{
				return lhs * rhs;
			}
		};

		template<class T>
		struct Divide
		{
			[[nodiscard]] constexpr T operator()(const T& lhs, const T& rhs) const
			{
				return lhs / rhs;
			}
		};

		template<class T>
		struct Modulo
		{
			[[nodiscard]] constexpr T operator()(const T& lhs, const T& rhs) const
			{
				return lhs % rhs;
			}
		};

		template<class T>
		struct Positive
		{
			[[nodiscard]] constexpr T operator()(const T& value) const
			{
				return +value;
			}
		};

		template<class T>
		struct Negative
		{
			[[nodiscard]] constexpr T operator()(const T& value) const
			{
				return -value;
			}
		};

		template<class T>
		struct Less
		{
			[[nodiscard]] constexpr bool operator()(const T& lhs, const T& rhs) const
			{
				return lhs < rhs;
			}
		};

		template<class T>
		struct LessEqual
		{
			[[nodiscard]] constexpr bool operator()(const T& lhs, const T& rhs) const
			{
				return lhs <= rhs;
			}
		};

		template<class T>
		struct Greater
		{
			[[nodiscard]] constexpr bool operator()(const T& lhs, const T& rhs) const
			{
				return lhs > rhs;
			}
		};

		template<class T>
		struct GreaterEqual
		{
			[[nodiscard]] constexpr bool operator()(const T& lhs, const T& rhs) const
			{
				return lhs >= rhs;
			}
		};

		template<class T>
		struct Equal
		{
			[[nodiscard]] constexpr bool operator()(const T& lhs, const T& rhs) const
			{
				return lhs == rhs;
			}
		};

		template<class T>
		struct NotEqual
		{
			[[nodiscard]] constexpr bool operator()(const T& lhs, const T& rhs) const
			{
				return lhs != rhs;
			}
		};

		template<class T>
		struct LogicalAnd
		{
			[[nodiscard]] constexpr bool operator()(const T& lhs, const T& rhs) const
			{
				return lhs && rhs;
			}
		};

		template<class T>
		struct LogicalOr
		{
			[[nodiscard]] constexpr bool operator()(const T& lhs, const T& rhs) const
			{
				return lhs || rhs;
			}
		};

		template<class T>
		struct LogicalNot
		{
			[[nodiscard]] constexpr bool operator()(const T& value) const
			{
				return !value;
			}
		};

		template<class T>
		struct BitwiseAnd
		{
			[[nodiscard]] constexpr T operator()(const T& lhs, const T& rhs) const
			{
				return lhs & rhs;
			}
		};

		template<class T>
		struct BitwiseOr
		{
			[[nodiscard]] constexpr T operator()(const T& lhs, const T& rhs) const
			{
				return lhs | rhs;
			}
		};

		template<class T>
		struct BitwiseXor
		{
			[[nodiscard]] constexpr T operator()(const T& lhs, const T& rhs) const
			{
				return lhs ^ rhs;
			}
		};

		template<class T>
		struct BitwiseNot
		{
			[[nodiscard]] constexpr T operator()(const T& value) const
			{
				return ~value;
			}
		};
	}
}
