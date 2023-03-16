#pragma once

namespace bsl
{
	template<class T>
	struct type_identity
	{
		using type = typename T;
	};

	template<class T>
	using type_identity_t = typename type_identity<T>::type;

	template<class T, class... Rest>
	struct first_type : type_identity<T> {};

	template<class... T>
	using first_type_t = typename first_type<T...>::type;

	template<class T, T val>
	struct value_type
	{
		using type = typename T;

		static inline constexpr T value = val;

		[[nodiscard]] constexpr operator T() const noexcept
		{
			return value;
		}

		[[nodiscard]] constexpr T operator()() const noexcept
		{
			return value;
		}
	};

	template<bool val>
	struct bool_type : value_type<bool, val> {};

	struct true_type : bool_type<true> {};

	struct false_type : bool_type<false> {};

	template<class T>
	struct always_true : true_type {};

	template<class T>
	inline constexpr bool always_true_v = always_true<T>::value;

	template<class T>
	struct always_false : false_type {};

	template<class T>
	inline constexpr bool always_false_v = always_false<T>::value;

	template<class... B>
	struct conjunction : bool_type<(B::value &&...)> {};

	template<class... B>
	inline constexpr bool conjunction_v = conjunction<B...>::value;

	template<class... B>
	struct disjunction : bool_type<(B::value || ...)> {};

	template<class... B>
	inline constexpr bool disjunction_v = disjunction<B...>::value;

	template<class B>
	struct negation : bool_type<!B::value> {};

	template<class B>
	inline constexpr bool negation_v = negation<B>::value;

	template<bool B, class T, class F>
	struct conditional {};

	template<class T, class F>
	struct conditional<true, T, F> : type_identity<T> {};

	template<class T, class F>
	struct conditional<false, T, F> : type_identity<F> {};

	template<bool B, class T, class F>
	using conditional_t = typename conditional<B, T, F>::type;

	template<class U, class V>
	struct is_same : false_type {};

	template<class T>
	struct is_same<T, T> : true_type {};

	template<class U, class V>
	inline constexpr bool is_same_v = is_same<U, V>::value;

	template<class U, class V>
	concept same_as = is_same_v<U, V>;

	template<class U, class... V>
	struct is_any_of : disjunction<is_same<U, V>...> {};

	template<class U, class... V>
	inline constexpr bool is_any_of_v = is_any_of<U, V...>::value;

	template<class U, class... V>
	concept any_of = is_any_of_v<U, V...>;

	template<class U, class... V>
	struct is_all_same : conjunction<is_same<U, V>...> {};

	template<class U, class... V>
	inline constexpr bool is_all_same_v = is_all_same<U, V...>::value;

	template<class T, T First, T... Rest>
	struct max_of : conditional_t<(First > max_of<T, Rest...>::value), value_type<T, First>, value_type<T, max_of<T, Rest...>::value>> {};

	template<class T, T First>
	struct max_of<T, First> : value_type<T, First> {};

	template<class T, T... Values>
	inline constexpr T max_of_v = max_of<T, Values...>::value;

	template<class T, T First, T... Rest>
	struct min_of : conditional_t<(First < min_of<T, Rest...>::value), value_type<T, First>, value_type<T, min_of<T, Rest...>::value>> {};

	template<class T, T First>
	struct min_of<T, First> : value_type<T, First> {};

	template<class T, T... Values>
	inline constexpr T min_of_v = min_of<T, Values...>::value;

	template<bool B, class T = void>
	struct enable_if {};

	template<class T>
	struct enable_if<true, T> : type_identity<T> {};

	template<bool B, class T = void>
	using enable_if_t = typename enable_if<B, T>::type;

	using void_t = decltype(void());

	template<class T>
	struct is_void : is_same<T, void_t> {};

	template<class T>
	inline constexpr bool is_void_v = is_void<T>::value;

	using nullptr_t = decltype(nullptr);

	template<class T>
	struct is_nullptr : is_same<T, nullptr_t> {};

	template<class T>
	inline constexpr bool is_nullptr_v = is_nullptr<T>::value;

	template<class T>
	struct is_integral : is_any_of<T, char, unsigned char, short, unsigned short, int, unsigned int, long, unsigned long, long long, unsigned long long> {};

	template<class T>
	inline constexpr bool is_integral_v = is_integral<T>::value;

	template<class T>
	concept integral = is_integral_v<T>;

	template<class T>
	struct is_floating_point : is_any_of<T, float, double, long double> {};

	template<class T>
	inline constexpr bool is_floating_point_v = is_floating_point<T>::value;

	template<class T>
	concept floating_point = is_floating_point_v<T>;

	template<class T>
	struct is_signed: is_any_of<T, char, short, int, long, long long, float, double, long double> {};

	template<class T>
	inline constexpr bool is_signed_v = is_signed<T>::value;

	template<class T>
	struct is_unsigned : is_any_of<T, unsigned char, unsigned short, unsigned int, unsigned long, unsigned long long> {};

	template<class T>
	inline constexpr bool is_unsigned_v = is_unsigned<T>::value;

	template<class T>
	struct is_arithmetic : disjunction<is_integral<T>, is_floating_point<T>> {};

	template<class T>
	inline constexpr bool is_arithmetic_v = is_arithmetic<T>::value;

	template<class T>
	concept arithmetic = is_arithmetic_v<T>;

	template<class T>
	struct make_signed {};

	template<>
	struct make_signed<char> : type_identity<char> {};

	template<>
	struct make_signed<unsigned char> : type_identity<char> {};

	template<>
	struct make_signed<short> : type_identity<short> {};

	template<>
	struct make_signed<unsigned short> : type_identity<short> {};

	template<>
	struct make_signed<int> : type_identity<int> {};

	template<>
	struct make_signed<unsigned int> : type_identity<int> {};

	template<>
	struct make_signed<long> : type_identity<long> {};

	template<>
	struct make_signed<unsigned long> : type_identity<long> {};

	template<>
	struct make_signed<long long> : type_identity<long long> {};

	template<>
	struct make_signed<unsigned long long> : type_identity<long long> {};

	template<>
	struct make_signed<float> : type_identity<float> {};

	template<>
	struct make_signed<double> : type_identity<double> {};

	template<>
	struct make_signed<long double> : type_identity<long double> {};

	template<class T>
	using make_signed_t = typename make_signed<T>::type;

	template<class T>
	struct make_unsigned {};

	template<>
	struct make_unsigned<char> : type_identity<unsigned char> {};

	template<>
	struct make_unsigned<unsigned char> : type_identity<unsigned char> {};

	template<>
	struct make_unsigned<short> : type_identity<unsigned short> {};

	template<>
	struct make_unsigned<unsigned short> : type_identity<unsigned short> {};

	template<>
	struct make_unsigned<int> : type_identity<unsigned int> {};

	template<>
	struct make_unsigned<unsigned int> : type_identity<unsigned int> {};

	template<>
	struct make_unsigned<long> : type_identity<unsigned long> {};

	template<>
	struct make_unsigned<unsigned long> : type_identity<unsigned long> {};

	template<>
	struct make_unsigned<long long> : type_identity<unsigned long long> {};

	template<>
	struct make_unsigned<unsigned long long> : type_identity<unsigned long long> {};

	template<class T>
	using make_unsigned_t = typename make_unsigned<T>::type;

	template<class T>
	struct is_pointer : false_type {};

	template<class T>
	struct is_pointer<T*> : true_type {};

	template<class T>
	inline constexpr bool is_pointer_v = is_pointer<T>::value;

	template<class T>
	concept pointer = is_pointer_v<T>;

	template<class T>
	struct add_pointer : type_identity<T*> {};

	template<class T>
	using add_pointer_t = typename add_pointer<T>::type;

	template<class T>
	struct remove_pointer : type_identity<T> {};

	template<class T>
	struct remove_pointer<T*> : type_identity<T> {};

	template<class T>
	using remove_pointer_t = typename remove_pointer<T>::type;

	template<class T>
	struct remove_all_pointers : type_identity<T> {};

	template<class T>
	struct remove_all_pointers<T*> : remove_all_pointers<T> {};

	template<class T>
	using remove_all_pointers_t = typename remove_all_pointers<T>::type;

	template<class T, size_t D = 0>
	struct degree : value_type<size_t, D> {};

	template<class T, size_t D>
	struct degree<T*, D> : degree<T, D + 1> {};

	template<class T>
	inline constexpr size_t degree_v = degree<T>::value;

	template<class T>
	struct is_lvalue_reference : false_type {};

	template<class T>
	struct is_lvalue_reference<T&> : true_type {};

	template<class T>
	inline constexpr bool is_lvalue_reference_v = is_lvalue_reference<T>::value;

	template<class T>
	concept lvalue_reference = is_lvalue_reference_v<T>;

	template<class T>
	struct add_lvalue_reference : type_identity<T&> {};

	template<class T>
	using add_lvalue_reference_t = typename add_lvalue_reference<T>::type;

	template<class T>
	struct remove_lvalue_reference : type_identity<T> {};

	template<class T>
	struct remove_lvalue_reference<T&> : type_identity<T> {};

	template<class T>
	using remove_lvalue_reference_t = typename remove_lvalue_reference<T>::type;

	template<class T>
	struct is_rvalue_reference : false_type {};

	template<class T>
	struct is_rvalue_reference<T&&> : true_type {};

	template<class T>
	inline constexpr bool is_rvalue_reference_v = is_rvalue_reference<T>::value;

	template<class T>
	concept rvalue_reference = is_rvalue_reference_v<T>;

	template<class T>
	struct add_rvalue_reference : type_identity<T&&> {};

	template<class T>
	using add_rvalue_reference_t = typename add_rvalue_reference<T>::type;

	template<class T>
	struct remove_rvalue_reference : type_identity<T> {};

	template<class T>
	struct remove_rvalue_reference<T&&> : type_identity<T> {};

	template<class T>
	using remove_rvalue_reference_t = typename remove_rvalue_reference<T>::type;

	template<class T>
	struct is_reference : disjunction<is_lvalue_reference<T>, is_rvalue_reference<T>> {};

	template<class T>
	inline constexpr bool is_reference_v = is_reference<T>::value;

	template<class T>
	concept reference = is_reference_v<T>;

	template<class T>
	struct remove_reference : type_identity<remove_lvalue_reference_t<remove_rvalue_reference_t<T>>> {};

	template<class T>
	using remove_reference_t = typename remove_reference<T>::type;

	template<class T>
	struct is_const : false_type {};

	template<class T>
	struct is_const<const T> : true_type {};

	template<class T>
	inline constexpr bool is_const_v = is_const<T>::value;

	template<class T>
	struct add_const : type_identity<const T> {};

	template<class T>
	using add_const_t = typename add_const<T>::type;

	template<class T>
	struct remove_const : type_identity<T> {};

	template<class T>
	struct remove_const<const T> : type_identity<T> {};

	template<class T>
	using remove_const_t = typename remove_const<T>::type;

	template<class T>
	struct is_volatile : false_type {};

	template<class T>
	struct is_volatile<volatile T> : true_type {};

	template<class T>
	inline constexpr bool is_volatile_v = is_volatile<T>::value;

	template<class T>
	struct add_volatile : type_identity<volatile T> {};

	template<class T>
	using add_volatile_t = typename add_volatile<T>::type;

	template<class T>
	struct remove_volatile : type_identity<T> {};

	template<class T>
	struct remove_volatile<volatile T> : type_identity<T> {};

	template<class T>
	using remove_volatile_t = typename remove_volatile<T>::type;

	template<class T>
	struct add_const_volatile : type_identity<const volatile T> {};

	template<class T>
	using add_const_volatile_t = typename add_const_volatile<T>::type;

	template<class T>
	struct remove_const_volatile : type_identity<remove_const_t<remove_volatile_t<T>>> {};

	template<class T>
	using remove_const_volatile_t = typename remove_const_volatile<T>::type;

	template<class T>
	struct is_array : false_type {};

	template<class T>
	struct is_array<T[]> : true_type {};

	template<class T, size_t S>
	struct is_array<T[S]> : true_type {};

	template<class T>
	struct is_bounded_array : false_type {};

	template<class T, size_t S>
	struct is_bounded_array<T[S]> : true_type {};

	template<class T>
	inline constexpr bool is_bounded_array_v = is_bounded_array<T>::value;

	template<class T>
	struct is_unbounded_array : false_type {};

	template<class T>
	struct is_unbounded_array<T[]> : true_type {};

	template<class T, size_t S>
	struct is_unbounded_array<T[S]> : false_type {};

	template<class T>
	inline constexpr bool is_unbounded_array_v = is_unbounded_array<T>::value;

	template<class T>
	struct rank : value_type<size_t, 0> {};

	template<class T>
	struct rank<T[]> : value_type<size_t, rank<T>::value + 1> {};

	template<class T, size_t S>
	struct rank<T[S]> : value_type<size_t, rank<T>::value + 1> {};

	template<class T>
	inline constexpr size_t rank_v = rank<T>::value;

	template<class T, size_t S = 0>
	struct add_rank : type_identity<T[S]> {};

	template<class T>
	struct add_rank<T, 0> : type_identity<T[]> {};

	template<class T, size_t S>
	using add_rank_t = typename add_rank<T, S>::type;

	template<class T>
	struct remove_rank : type_identity<T> {};

	template<class T>
	struct remove_rank<T[]> : type_identity<T> {};

	template<class T, size_t S>
	struct remove_rank<T[S]> : type_identity<T> {};

	template<class T>
	using remove_rank_t = typename remove_rank<T>::type;

	template<class T>
	struct remove_all_ranks : type_identity<T> {};

	template<class T>
	struct remove_all_ranks<T[]> : remove_all_ranks<T> {};

	template<class T, size_t S>
	struct remove_all_ranks<T[S]> : remove_all_ranks<T> {};

	template<class T>
	using remove_all_ranks_t = typename remove_all_ranks<T>::type;

	template<class T, size_t N = 0>
	struct extent : value_type<size_t, 0> {};

	template<class T>
	struct extent<T[], 0> : value_type<size_t, 0> {};

	template<class T, size_t N>
	struct extent<T[], N> : extent<T, N - 1> {};

	template<class T, size_t S, size_t N>
	struct extent<T[S], N> : extent<T, N - 1> {};

	template<class T, size_t S>
	struct extent<T[S], 0> : value_type<size_t, S> {};

	template<class T>
	inline constexpr size_t extent_v = extent<T>::value;

	template<class T>
	struct bare : type_identity<T> {};

	template<class T>
	struct bare<const T> : bare<T> {};

	template<class T>
	struct bare<volatile T> : bare<T> {};

	template<class T>
	struct bare<const volatile T> : bare<T> {};

	template<class T>
	struct bare<T*> : bare<T> {};

	template<class T>
	struct bare<const T*> : bare<T> {};

	template<class T>
	struct bare<volatile T*> : bare<T> {};

	template<class T>
	struct bare<const volatile T*> : bare<T> {};

	template<class T>
	struct bare<T&> : bare<T> {};

	template<class T>
	struct bare<const T&> : bare<T> {};

	template<class T>
	struct bare<volatile T&> : bare<T> {};

	template<class T>
	struct bare<const volatile T&> : bare<T> {};

	template<class T>
	struct bare<T&&> : bare<T> {};

	template<class T>
	struct bare<const T&&> : bare<T> {};

	template<class T>
	struct bare<volatile T&&> : bare<T> {};

	template<class T>
	struct bare<const volatile T&&> : bare<T> {};

	template<class T>
	struct bare<T[]> : bare<T> {};

	template<class T>
	struct bare<const T[]> : bare<T> {};

	template<class T>
	struct bare<volatile T[]> : bare<T> {};

	template<class T>
	struct bare<const volatile T[]> : bare<T> {};

	template<class T, size_t S>
	struct bare<T[S]> : bare<T> {};

	template<class T, size_t S>
	struct bare<const T[S]> : bare<T> {};

	template<class T, size_t S>
	struct bare<volatile T[S]> : bare<T> {};

	template<class T, size_t S>
	struct bare<const volatile T[S]> : bare<T> {};

	template<class T>
	using bare_t = typename bare<T>::type;

	template<class T>
	[[nodiscard]] constexpr add_rvalue_reference_t<T> declval() noexcept
	{
		static_assert(always_false_v<T>, "bad declval call");
	}

	template<class T>
	[[nodiscard]] constexpr T&& forward(remove_reference_t<T>& arg) noexcept
	{
		return static_cast<T&&>(arg);
	}

	template<class T>
	[[nodiscard]] constexpr T&& forward(remove_reference_t<T>&& arg) noexcept {
		static_assert(!is_lvalue_reference_v<T>, "bad forward call");
		return static_cast<T&&>(arg);
	}

	template<class T>
	[[nodiscard]] constexpr remove_reference_t<T>&& move(T&& arg) noexcept
	{
		return static_cast<remove_reference_t<T>&&>(arg);
	}

	template<class T>
	struct is_member_pointer : false_type {};

	template<class T, class C>
	struct is_member_pointer<T C::*> : true_type {};

	template<class T>
	inline constexpr bool is_member_pointer_v = is_member_pointer<T>::value;

	template<class T>
	struct is_member_function_pointer : false_type {};

	template<class R, class C, class... Args>
	struct is_member_function_pointer<R(C::*)(Args...)> : true_type {};

	template<class R, class C, class... Args>
	struct is_member_function_pointer<R(C::*)(Args...)const> : true_type {};

	template<class R, class C, class... Args>
	struct is_member_function_pointer<R(C::*)(Args...)noexcept> : true_type {};

	template<class R, class C, class... Args>
	struct is_member_function_pointer<R(C::*)(Args...)const noexcept> : true_type {};

	template<class R, class C, class... Args>
	struct is_member_function_pointer<R(C::*)(Args...)&> : true_type {};

	template<class R, class C, class... Args>
	struct is_member_function_pointer<R(C::*)(Args...)const&> : true_type {};

	template<class R, class C, class... Args>
	struct is_member_function_pointer<R(C::*)(Args...)&noexcept> : true_type {};

	template<class R, class C, class... Args>
	struct is_member_function_pointer<R(C::*)(Args...)const& noexcept> : true_type {};

	template<class R, class C, class... Args>
	struct is_member_function_pointer<R(C::*)(Args...)&&> : true_type {};

	template<class R, class C, class... Args>
	struct is_member_function_pointer<R(C::*)(Args...)const&&> : true_type {};

	template<class R, class C, class... Args>
	struct is_member_function_pointer<R(C::*)(Args...)&& noexcept> : true_type {};

	template<class R, class C, class... Args>
	struct is_member_function_pointer<R(C::*)(Args...)const&& noexcept> : true_type {};

	template<class T>
	inline constexpr bool is_member_function_pointer_v = is_member_function_pointer<T>::value;

	template<class T>
	struct is_function_pointer : is_member_function_pointer<T> {};

	template<class R, class... Args>
	struct is_function_pointer<R(*)(Args...)> : true_type {};

	template<class R, class... Args>
	struct is_function_pointer<R(*)(Args...)noexcept> : true_type {};

	template<class T>
	inline constexpr bool is_function_pointer_v = is_function_pointer<T>::value;

	template<class T>
	struct is_function : is_function_pointer<T> {};

	template<class R, class... Args>
	struct is_function<R(Args...)> : true_type {};

	template<class R, class... Args>
	struct is_function<R(Args...)noexcept> : true_type {};

	template<class T>
	inline constexpr bool is_function_v = is_function<T>::value;

	template<class T, class... Args>
	true_type _is_constructible_test(decltype(T(declval<Args>()...))*)
	{
		return true_type{};
	}

	template<class T, class... Args>
	false_type _is_constructible_test(...)
	{
		return false_type{};
	}

	template<class T, class... Args>
	struct is_constructible : decltype(_is_constructible_test<T, Args...>(nullptr)) {};

	template<class T, class... Args>
	inline constexpr bool is_constructible_v = is_constructible<T, Args...>::value;

	template<class T, class... Args>
	struct is_nothrow_constructible : bool_type<noexcept(T(declval<Args>()...))> {};

	template<class T, class... Args>
	inline constexpr bool is_nothrow_constructible_v = is_nothrow_constructible<T, Args...>::value;

	template<class T>
	struct is_default_constructible : is_constructible<T> {};

	template<class T>
	inline constexpr bool is_default_constructible_v = is_default_constructible<T>::value;

	template<class T>
	struct is_nothrow_default_constructible : is_nothrow_constructible<T> {};

	template<class T>
	inline constexpr bool is_nothrow_default_constructible_v = is_nothrow_default_constructible<T>::value;

	template<class T>
	struct is_copy_constructible : is_constructible<T, const T&> {};

	template<class T>
	inline constexpr bool is_copy_constructible_v = is_copy_constructible<T>::value;

	template<class T>
	struct is_nothrow_copy_constructible : is_nothrow_constructible<T, const T&> {};

	template<class T>
	inline constexpr bool is_nothrow_copy_constructible_v = is_nothrow_copy_constructible<T>::value;

	template<class T>
	struct is_move_constructible : is_constructible<T, T&&> {};

	template<class T>
	inline constexpr bool is_move_constructible_v = is_move_constructible<T>::value;

	template<class T>
	struct is_nothrow_move_constructible : is_nothrow_constructible<T, T&&> {};

	template<class T>
	inline constexpr bool is_nothrow_move_constructible_v = is_nothrow_move_constructible<T>::value;

	template<class T>
	struct is_nothrow_destructible : bool_type<noexcept(declval<T>().~T())> {};

	template<class T>
	inline constexpr bool is_nothrow_destructible_v = is_nothrow_destructible<T>::value;

	template<class U, class V>
	true_type _is_assignable_test(decltype(declval<U>() = declval<V>())*)
	{
		return true_type{};
	}

	template<class U, class V>
	false_type _is_assignable_test(...)
	{
		return false_type{};
	}

	template<class U, class V>
	struct is_assignable : decltype(_is_assignable_test<U, V>(nullptr)) {};

	template<class U, class V>
	inline constexpr bool is_assignable_v = is_assignable<U, V>::value;

	template<class U, class V>
	struct is_nothrow_assignable : bool_type<noexcept(declval<U>() = declval<V>())> {};

	template<class U, class V>
	inline constexpr bool is_nothrow_assignable_v = is_nothrow_assignable<U, V>::value;

	template<class T>
	struct is_copy_assignable : is_assignable<T&, const T&> {};

	template<class T>
	inline constexpr bool is_copy_assignable_v = is_copy_assignable<T>::value;

	template<class T>
	struct is_nothrow_copy_assignable : bool_type<noexcept(declval<T&>() = declval<const T&>())> {};

	template<class T>
	inline constexpr bool is_nothrow_copy_assignable_v = is_nothrow_copy_assignable<T>::value;

	template<class T>
	struct is_move_assignable : is_assignable<T&, T&&> {};

	template<class T>
	inline constexpr bool is_move_assignable_v = is_move_assignable<T>::value;

	template<class T>
	struct is_nothrow_move_assignable : bool_type<noexcept(declval<T&>() = declval<T&&>())> {};

	template<class T>
	inline constexpr bool is_nothrow_move_assignable_v = is_nothrow_move_assignable<T>::value;

	template<class From, class To>
	true_type _is_explicitly_convertible_test(decltype(static_cast<To>(declval<From>()))*)
	{
		return true_type{};
	}

	template<class From, class To>
	false_type _is_explicitly_convertible_test(...)
	{
		return false_type{};
	}

	template<class From, class To>
	struct is_explicitly_convertible : decltype(_is_explicitly_convertible_test<From, To>(nullptr)) {};

	template<class From, class To>
	inline constexpr bool is_explicitly_convertible_v = is_explicitly_convertible<From, To>::value;

	template<class From, class To>
	struct is_nothrow_explicitly_convertible : bool_type<noexcept(static_cast<To>(declval<From>()))> {};

	template<class From, class To>
	inline constexpr bool is_nothrow_explicitly_convertible_v = is_nothrow_explicitly_convertible<From, To>::value;

	template<class To>
	true_type _is_implicitly_convertible_test(To)
	{
		return true_type{};
	}

	template<class To>
	false_type _is_implicitly_convertible_test(...)
	{
		return false_type{};
	}

	template<class From, class To>
	struct is_implicitly_convertible : decltype(_is_implicitly_convertible_test<To>(declval<From>())) {};

	template<class From, class To>
	inline constexpr bool is_implicitly_convertible_v = is_implicitly_convertible<From, To>::value;

	template<class From, class To>
	struct is_convertible : disjunction<is_explicitly_convertible<From, To>, is_implicitly_convertible<From, To>> {};

	template<class From, class To>
	inline constexpr bool is_convertible_v = is_convertible<From, To>::value;
}
