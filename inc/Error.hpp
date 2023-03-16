#pragma once

namespace bsl
{
	struct Error
	{
		virtual ~Error() noexcept = default;

		virtual [[nodiscard]] const char* msg() const noexcept
		{
			return "Error.";
		}
	};

	struct BadAccess : Error
	{
		virtual [[nodiscard]] const char* msg() const noexcept override
		{
			return "Bad access.";
		}
	};

	struct OutOfRange : Error
	{
		virtual [[nodiscard]] const char* msg() const noexcept override
		{
			return "Out of range.";
		}
	};
}
