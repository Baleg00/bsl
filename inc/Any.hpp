#pragma once

#include <new>

#include "Error.hpp"
#include "Types.hpp"
#include "type_traits.hpp"
#include "TypeInfo.hpp"

namespace bsl
{
	struct _AnyUtil
	{
		typedef void* (*CopyConstructor)(const void*);
		typedef void* (*MoveConstructor)(void*);
		typedef void(*Destructor)(void*);

		typedef void* (*CopyAssignment)(void*, const void*);
		typedef void* (*MoveAssignment)(void*, void*);

		CopyConstructor m_cctor;
		MoveConstructor m_mctor;
		Destructor m_dtor;

		CopyAssignment m_cas;
		MoveAssignment m_mas;

		template<class T>
		[[nodiscard]] static constexpr _AnyUtil make() noexcept
		{
			return _AnyUtil{ &copy_constructor_impl<T>, &move_constructor_impl<T>, &destructor_impl<T>, &copy_assignment_impl<T>, &move_assignment_impl<T> };
		}

		template<class T>
		static void* copy_constructor_impl(const void* other)
		{
			return ::new T{ *reinterpret_cast<const T*>(other) };
		}

		template<>
		static void* copy_constructor_impl<void>(const void*)
		{
			return nullptr;
		}

		template<class T>
		static void* move_constructor_impl(void* other)
		{
			return ::new T{ move(*reinterpret_cast<T*>(other)) };
		}

		template<>
		static void* move_constructor_impl<void>(void*)
		{
			return nullptr;
		}

		template<class T>
		static void destructor_impl(void* obj)
		{
			::delete reinterpret_cast<T*>(obj);
		}

		template<>
		static void destructor_impl<void>(void*)
		{
		}

		template<class T>
		static void* copy_assignment_impl(void* lhs, const void* rhs)
		{
			return &(*reinterpret_cast<T*>(lhs) = *reinterpret_cast<const T*>(rhs));
		}

		template<>
		static void* copy_assignment_impl<void>(void*, const void*)
		{
			return nullptr;
		}

		template<class T>
		static void* move_assignment_impl(void* lhs, void* rhs)
		{
			return &(*reinterpret_cast<T*>(lhs) = move(*reinterpret_cast<T*>(rhs)));
		}

		template<>
		static void* move_assignment_impl<void>(void*, void*)
		{
			return nullptr;
		}
	};

	struct BadAnyCast : Error
	{
		[[nodiscard]] const char* msg() const noexcept override
		{
			return "Bad any cast.";
		}
	};

	class Any
	{
	private:
		_AnyUtil m_util;

		TypeId m_id;
		void* m_data;

		template<class T>
		friend constexpr T any_cast(Any&) noexcept(false);

		template<class T>
		friend constexpr T any_cast(const Any&) noexcept(false);

		template<class T>
		friend constexpr T* any_cast(Any*) noexcept(false);
		
		template<class T>
		friend constexpr const T* any_cast(const Any*) noexcept(false);

		constexpr void reset()
		{
			if (m_data != nullptr)
				m_util.m_dtor(m_data);
			
			m_util = _AnyUtil::make<void>();
			m_id = TypeInfo<void>::id();
		}

	public:
		constexpr Any() noexcept
			: m_util{ _AnyUtil::make<void>() }, m_id{ TypeInfo<void>::id() }, m_data{ nullptr }
		{
		}

		template<class T>
		constexpr Any(const T& value) noexcept(is_nothrow_copy_constructible_v<T>)
			: m_util{ _AnyUtil::make<T>() }, m_id{ TypeInfo<T>::id() }, m_data{ new T{ value } }
		{
		}

		template<class T>
		constexpr Any(T&& value) noexcept(is_nothrow_move_constructible_v<T>)
			: m_util{ _AnyUtil::make<T>() }, m_id{ TypeInfo<T>::id() }, m_data{ new T{ move(value) } }
		{
		}

		template<class T, class... Args>
		explicit constexpr Any(InPlaceType<T>, Args&&... args) noexcept(is_nothrow_constructible_v<T, Args...>)
			: m_util{ _AnyUtil::make<T>() }, m_id{ TypeInfo<T>::id() }, m_data{ new T{ forward<Args>(args)... } }
		{
		}

		constexpr Any(const Any& other) noexcept(false)
			: m_util{ other.m_util }, m_id{ other.m_id }, m_data{ m_util.m_cctor(other.m_data) }
		{
		}

		constexpr Any(Any&& other) noexcept(false)
			: m_util{ move(other.m_util) }, m_id{ move(other.m_id) }, m_data{ m_util.m_mctor(other.m_data) }
		{
		}

		constexpr ~Any()
		{
			reset();
		}

		constexpr Any& operator=(const Any& rhs) noexcept(false)
		{
			if (m_id == rhs.m_id)
				m_data = m_util.m_cas(m_data, rhs.m_data);
			else
			{
				reset();

				m_util = rhs.m_util;
				m_id = rhs.m_id;
				m_data = m_util.m_cctor(rhs.m_data);
			}

			return *this;
		}

		constexpr Any& operator=(Any&& rhs) noexcept(false)
		{
			if (m_id == rhs.m_id)
				m_data = m_util.m_mas(m_data, rhs.m_data);
			else
			{
				reset();

				m_util = rhs.m_util;
				m_id = rhs.m_id;
				m_data = m_util.m_mctor(rhs.m_data);
			}

			return *this;
		}

		[[nodiscard]] constexpr bool has_value() const noexcept
		{
			return m_id != TypeInfo<void>::id();
		}

		[[nodiscard]] constexpr TypeId type_id() const noexcept
		{
			return m_id;
		}
	};

	template<class T>
	[[nodiscard]] constexpr T* any_cast(Any* any) noexcept(false)
	{
		if (any->type_id() != TypeInfo<T>::id())
			throw BadAnyCast{};

		return reinterpret_cast<T*>(any->m_data);
	}

	template<class T>
	[[nodiscard]] constexpr const T* any_cast(const Any* any) noexcept(false)
	{
		if (any->type_id() != TypeInfo<T>::id())
			throw BadAnyCast{};

		return reinterpret_cast<const T*>(any->m_data);
	}

	template<class T>
	[[nodiscard]] constexpr T any_cast(Any& any) noexcept(false)
	{
		return *any_cast<T>(&any);
	}

	template<class T>
	[[nodiscard]] constexpr T any_cast(const Any& any) noexcept(false)
	{
		return *any_cast<T>(&any);
	}
}
