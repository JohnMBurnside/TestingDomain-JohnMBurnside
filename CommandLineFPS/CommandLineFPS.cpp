#pragma once
#include <iostream>
#include <Windows.h>
#ifndef _CHRONO_
#define _CHRONO_
#include <yvals_core.h>
#if _STL_COMPILER_PREPROCESSOR
#include <limits>
#include <ratio>
#include <time.h>
#include <utility>
#include <xtimec.h>

#pragma pack(push, _CRT_PACKING)
#pragma warning(push, _STL_WARNING_LEVEL)
#pragma warning(disable : _STL_DISABLED_WARNINGS)
_STL_DISABLE_CLANG_WARNINGS
#pragma push_macro("new")
#undef new

_STD_BEGIN
namespace chrono {
    // STRUCT TEMPLATE treat_as_floating_point
    template <class _Rep>
    struct treat_as_floating_point : is_floating_point<_Rep> { // tests for floating-point type
    };

    template <class _Rep>
    _INLINE_VAR constexpr bool treat_as_floating_point_v = treat_as_floating_point<_Rep>::value;

    // STRUCT TEMPLATE duration_values
    template <class _Rep>
    struct duration_values { // gets arithmetic properties of a type
        _NODISCARD static constexpr _Rep zero() noexcept {
            // get zero value
            return _Rep(0);
        }

        _NODISCARD static constexpr _Rep(min)() noexcept {
            // get smallest value
            return numeric_limits<_Rep>::lowest();
        }

        _NODISCARD static constexpr _Rep(max)() noexcept {
            // get largest value
            return (numeric_limits<_Rep>::max)();
        }
    };

    // CLASS TEMPLATE duration
    template <class _Rep, class _Period = ratio<1>>
    class duration;

    // VARIABLE TEMPLATE _Is_duration_v
    template <class _Ty>
    _INLINE_VAR constexpr bool _Is_duration_v = _Is_specialization_v<_Ty, duration>;

    template <class _To, class _Rep, class _Period, enable_if_t<_Is_duration_v<_To>, int> = 0>
    constexpr _To duration_cast(const duration<_Rep, _Period>&) noexcept(
        is_arithmetic_v<_Rep>&& is_arithmetic_v<typename _To::rep>); // strengthened

    template <class _Rep, class _Period>
    class duration { // represents a time duration
    public:
        using rep = _Rep;
        using period = typename _Period::type;

        static_assert(!_Is_duration_v<_Rep>, "duration can't have duration as first template argument");
        static_assert(_Is_ratio_v<_Period>, "period not an instance of std::ratio");
        static_assert(0 < _Period::num, "period negative or zero");

        constexpr duration() = default;

        template <class _Rep2,
            enable_if_t<is_convertible_v<const _Rep2&,
            _Rep> && (treat_as_floating_point_v<_Rep> || !treat_as_floating_point_v<_Rep2>),
            int> = 0>
            constexpr explicit duration(const _Rep2& _Val) noexcept(
                is_arithmetic_v<_Rep>&& is_arithmetic_v<_Rep2>) // strengthened
            : _MyRep(static_cast<_Rep>(_Val)) {}

        template <class _Rep2, class _Period2,
            enable_if_t<
            treat_as_floating_point_v<
            _Rep> || (_Ratio_divide_sfinae<_Period2, _Period>::den == 1 && !treat_as_floating_point_v<_Rep2>),
            int> = 0>
            constexpr duration(const duration<_Rep2, _Period2>& _Dur) noexcept(
                is_arithmetic_v<_Rep>&& is_arithmetic_v<_Rep2>) // strengthened
            : _MyRep(chrono::duration_cast<duration>(_Dur).count()) {}

        _NODISCARD constexpr _Rep count() const noexcept(is_arithmetic_v<_Rep>) /* strengthened */ {
            return _MyRep;
        }

        _NODISCARD constexpr common_type_t<duration> operator+() const
            noexcept(is_arithmetic_v<_Rep>) /* strengthened */ {
            return common_type_t<duration>(*this);
        }

        _NODISCARD constexpr common_type_t<duration> operator-() const
            noexcept(is_arithmetic_v<_Rep>) /* strengthened */ {
            return common_type_t<duration>(-_MyRep);
        }

        _CONSTEXPR17 duration& operator++() noexcept(is_arithmetic_v<_Rep>) /* strengthened */ {
            ++_MyRep;
            return *this;
        }

        _CONSTEXPR17 duration operator++(int) noexcept(is_arithmetic_v<_Rep>) /* strengthened */ {
            return duration(_MyRep++);
        }

        _CONSTEXPR17 duration& operator--() noexcept(is_arithmetic_v<_Rep>) /* strengthened */ {
            --_MyRep;
            return *this;
        }

        _CONSTEXPR17 duration operator--(int) noexcept(is_arithmetic_v<_Rep>) /* strengthened */ {
            return duration(_MyRep--);
        }

        _CONSTEXPR17 duration& operator+=(const duration& _Right) noexcept(is_arithmetic_v<_Rep>) /* strengthened */ {
            _MyRep += _Right._MyRep;
            return *this;
        }

        _CONSTEXPR17 duration& operator-=(const duration& _Right) noexcept(is_arithmetic_v<_Rep>) /* strengthened */ {
            _MyRep -= _Right._MyRep;
            return *this;
        }

        _CONSTEXPR17 duration& operator*=(const _Rep& _Right) noexcept(is_arithmetic_v<_Rep>) /* strengthened */ {
            _MyRep *= _Right;
            return *this;
        }

        _CONSTEXPR17 duration& operator/=(const _Rep& _Right) noexcept(is_arithmetic_v<_Rep>) /* strengthened */ {
            _MyRep /= _Right;
            return *this;
        }

        _CONSTEXPR17 duration& operator%=(const _Rep& _Right) noexcept(is_arithmetic_v<_Rep>) /* strengthened */ {
            _MyRep %= _Right;
            return *this;
        }

        _CONSTEXPR17 duration& operator%=(const duration& _Right) noexcept(is_arithmetic_v<_Rep>) /* strengthened */ {
            _MyRep %= _Right.count();
            return *this;
        }

        _NODISCARD static constexpr duration zero() noexcept {
            // get zero value
            return duration(duration_values<_Rep>::zero());
        }

        _NODISCARD static constexpr duration(min)() noexcept {
            // get minimum value
            return duration((duration_values<_Rep>::min)());
        }

        _NODISCARD static constexpr duration(max)() noexcept {
            // get maximum value
            return duration((duration_values<_Rep>::max)());
        }

    private:
        _Rep _MyRep; // the stored rep
    };

    template <class _Clock, class _Duration = typename _Clock::duration>
    class time_point { // represents a point in time
    public:
        using clock = _Clock;
        using duration = _Duration;
        using rep = typename _Duration::rep;
        using period = typename _Duration::period;

        static_assert(_Is_duration_v<_Duration>, "duration must be an instance of std::duration");

        constexpr time_point() = default;

        constexpr explicit time_point(const _Duration& _Other) noexcept(is_arithmetic_v<rep>) // strengthened
            : _MyDur(_Other) {}

        template <class _Duration2, enable_if_t<is_convertible_v<_Duration2, _Duration>, int> = 0>
        constexpr time_point(const time_point<_Clock, _Duration2>& _Tp) noexcept(
            is_arithmetic_v<rep>&& is_arithmetic_v<typename _Duration2::rep>) // strengthened
            : _MyDur(_Tp.time_since_epoch()) {}

        _NODISCARD constexpr _Duration time_since_epoch() const noexcept(is_arithmetic_v<rep>) /* strengthened */ {
            return _MyDur;
        }

        _CONSTEXPR17 time_point& operator+=(const _Duration& _Dur) noexcept(is_arithmetic_v<rep>) /* strengthened */ {
            _MyDur += _Dur;
            return *this;
        }

        _CONSTEXPR17 time_point& operator-=(const _Duration& _Dur) noexcept(is_arithmetic_v<rep>) /* strengthened */ {
            _MyDur -= _Dur;
            return *this;
        }

        _NODISCARD static constexpr time_point(min)() noexcept {
            return time_point((_Duration::min)());
        }

        _NODISCARD static constexpr time_point(max)() noexcept {
            return time_point((_Duration::max)());
        }

    private:
        _Duration _MyDur{ duration::zero() }; // duration since the epoch
    };
} // namespace chrono

// STRUCT TEMPLATE _Lcm (LEAST COMMON MULTIPLE)
template <intmax_t _Ax, intmax_t _Bx>
struct _Lcm
    : integral_constant<intmax_t, (_Ax / _Gcd<_Ax, _Bx>::value)* _Bx> { // compute least common multiple of _Ax and _Bx
};

// STRUCT TEMPLATE common_type SPECIALIZATIONS
template <class _Rep1, class _Period1, class _Rep2, class _Period2>
struct common_type<chrono::duration<_Rep1, _Period1>,
    chrono::duration<_Rep2, _Period2>> { // common type of two durations
    using type = chrono::duration<common_type_t<_Rep1, _Rep2>,
        ratio<_Gcd<_Period1::num, _Period2::num>::value, _Lcm<_Period1::den, _Period2::den>::value>>;
};

template <class _Clock, class _Duration1, class _Duration2>
struct common_type<chrono::time_point<_Clock, _Duration1>,
    chrono::time_point<_Clock, _Duration2>> { // common type of two time points
    using type = chrono::time_point<_Clock, common_type_t<_Duration1, _Duration2>>;
};

namespace chrono {
    // duration ARITHMETIC
    template <class _Rep1, class _Period1, class _Rep2, class _Period2>
    _NODISCARD constexpr common_type_t<duration<_Rep1, _Period1>, duration<_Rep2, _Period2>>
        operator+(const duration<_Rep1, _Period1>& _Left, const duration<_Rep2, _Period2>& _Right) noexcept(
            is_arithmetic_v<_Rep1>&& is_arithmetic_v<_Rep2>) /* strengthened */ {
        using _CD = common_type_t<duration<_Rep1, _Period1>, duration<_Rep2, _Period2>>;
        return _CD(_CD(_Left).count() + _CD(_Right).count());
    }

    template <class _Rep1, class _Period1, class _Rep2, class _Period2>
    _NODISCARD constexpr common_type_t<duration<_Rep1, _Period1>, duration<_Rep2, _Period2>>
        operator-(const duration<_Rep1, _Period1>& _Left, const duration<_Rep2, _Period2>& _Right) noexcept(
            is_arithmetic_v<_Rep1>&& is_arithmetic_v<_Rep2>) /* strengthened */ {
        using _CD = common_type_t<duration<_Rep1, _Period1>, duration<_Rep2, _Period2>>;
        return _CD(_CD(_Left).count() - _CD(_Right).count());
    }

    template <class _Rep1, class _Period1, class _Rep2,
        enable_if_t<is_convertible_v<const _Rep2&, common_type_t<_Rep1, _Rep2>>, int> = 0>
        _NODISCARD constexpr duration<common_type_t<_Rep1, _Rep2>, _Period1> operator*(
            const duration<_Rep1, _Period1>& _Left,
            const _Rep2& _Right) noexcept(is_arithmetic_v<_Rep1>&& is_arithmetic_v<_Rep2>) /* strengthened */ {
        using _CR = common_type_t<_Rep1, _Rep2>;
        using _CD = duration<_CR, _Period1>;
        return _CD(_CD(_Left).count() * _Right);
    }

    template <class _Rep1, class _Rep2, class _Period2,
        enable_if_t<is_convertible_v<const _Rep1&, common_type_t<_Rep1, _Rep2>>, int> = 0>
        _NODISCARD constexpr duration<common_type_t<_Rep1, _Rep2>, _Period2>
        operator*(const _Rep1& _Left, const duration<_Rep2, _Period2>& _Right) noexcept(
            is_arithmetic_v<_Rep1>&& is_arithmetic_v<_Rep2>) /* strengthened */ {
        return _Right * _Left;
    }

    template <class _CR, class _Period1, class _Rep2, bool = is_convertible_v<const _Rep2&, _CR>>
    struct _Duration_div_mod1 { // return type for duration / rep and duration % rep
        using type = duration<_CR, _Period1>;
    };

    template <class _CR, class _Period1, class _Rep2>
    struct _Duration_div_mod1<_CR, _Period1, _Rep2, false> { // no return type
    };

    template <class _Rep1, class _Period1, class _Rep2, bool = _Is_duration_v<_Rep2>>
    struct _Duration_div_mod { // no return type
    };

    template <class _Rep1, class _Period1, class _Rep2>
    struct _Duration_div_mod<_Rep1, _Period1, _Rep2, false>
        : _Duration_div_mod1<common_type_t<_Rep1, _Rep2>, _Period1, _Rep2> {
        // return type for duration / rep and duration % rep
    };

    template <class _Rep1, class _Period1, class _Rep2>
    _NODISCARD constexpr typename _Duration_div_mod<_Rep1, _Period1, _Rep2>::type operator/(
        const duration<_Rep1, _Period1>& _Left,
        const _Rep2& _Right) noexcept(is_arithmetic_v<_Rep1>&& is_arithmetic_v<_Rep2>) /* strengthened */ {
        using _CR = common_type_t<_Rep1, _Rep2>;
        using _CD = duration<_CR, _Period1>;
        return _CD(_CD(_Left).count() / _Right);
    }

    template <class _Rep1, class _Period1, class _Rep2, class _Period2>
    _NODISCARD constexpr common_type_t<_Rep1, _Rep2>
        operator/(const duration<_Rep1, _Period1>& _Left, const duration<_Rep2, _Period2>& _Right) noexcept(
            is_arithmetic_v<_Rep1>&& is_arithmetic_v<_Rep2>) /* strengthened */ {
        using _CD = common_type_t<duration<_Rep1, _Period1>, duration<_Rep2, _Period2>>;
        return _CD(_Left).count() / _CD(_Right).count();
    }

    template <class _Rep1, class _Period1, class _Rep2>
    _NODISCARD constexpr typename _Duration_div_mod<_Rep1, _Period1, _Rep2>::type operator%(
        const duration<_Rep1, _Period1>& _Left,
        const _Rep2& _Right) noexcept(is_arithmetic_v<_Rep1>&& is_arithmetic_v<_Rep2>) /* strengthened */ {
        using _CR = common_type_t<_Rep1, _Rep2>;
        using _CD = duration<_CR, _Period1>;
        return _CD(_CD(_Left).count() % _Right);
    }

    template <class _Rep1, class _Period1, class _Rep2, class _Period2>
    _NODISCARD constexpr common_type_t<duration<_Rep1, _Period1>, duration<_Rep2, _Period2>>
        operator%(const duration<_Rep1, _Period1>& _Left, const duration<_Rep2, _Period2>& _Right) noexcept(
            is_arithmetic_v<_Rep1>&& is_arithmetic_v<_Rep2>) /* strengthened */ {
        using _CD = common_type_t<duration<_Rep1, _Period1>, duration<_Rep2, _Period2>>;
        return _CD(_CD(_Left).count() % _CD(_Right).count());
    }

    // duration COMPARISONS
    template <class _Rep1, class _Period1, class _Rep2, class _Period2>
    _NODISCARD constexpr bool
        operator==(const duration<_Rep1, _Period1>& _Left, const duration<_Rep2, _Period2>& _Right) noexcept(
            is_arithmetic_v<_Rep1>&& is_arithmetic_v<_Rep2>) /* strengthened */ {
        using _CT = common_type_t<duration<_Rep1, _Period1>, duration<_Rep2, _Period2>>;
        return _CT(_Left).count() == _CT(_Right).count();
    }

    template <class _Rep1, class _Period1, class _Rep2, class _Period2>
    _NODISCARD constexpr bool
        operator!=(const duration<_Rep1, _Period1>& _Left, const duration<_Rep2, _Period2>& _Right) noexcept(
            is_arithmetic_v<_Rep1>&& is_arithmetic_v<_Rep2>) /* strengthened */ {
        return !(_Left == _Right);
    }

    template <class _Rep1, class _Period1, class _Rep2, class _Period2>
    _NODISCARD constexpr bool
        operator<(const duration<_Rep1, _Period1>& _Left, const duration<_Rep2, _Period2>& _Right) noexcept(
            is_arithmetic_v<_Rep1>&& is_arithmetic_v<_Rep2>) /* strengthened */ {
        using _CT = common_type_t<duration<_Rep1, _Period1>, duration<_Rep2, _Period2>>;
        return _CT(_Left).count() < _CT(_Right).count();
    }

    template <class _Rep1, class _Period1, class _Rep2, class _Period2>
    _NODISCARD constexpr bool
        operator<=(const duration<_Rep1, _Period1>& _Left, const duration<_Rep2, _Period2>& _Right) noexcept(
            is_arithmetic_v<_Rep1>&& is_arithmetic_v<_Rep2>) /* strengthened */ {
        return !(_Right < _Left);
    }

    template <class _Rep1, class _Period1, class _Rep2, class _Period2>
    _NODISCARD constexpr bool
        operator>(const duration<_Rep1, _Period1>& _Left, const duration<_Rep2, _Period2>& _Right) noexcept(
            is_arithmetic_v<_Rep1>&& is_arithmetic_v<_Rep2>) /* strengthened */ {
        return _Right < _Left;
    }

    template <class _Rep1, class _Period1, class _Rep2, class _Period2>
    _NODISCARD constexpr bool
        operator>=(const duration<_Rep1, _Period1>& _Left, const duration<_Rep2, _Period2>& _Right) noexcept(
            is_arithmetic_v<_Rep1>&& is_arithmetic_v<_Rep2>) /* strengthened */ {
        return !(_Left < _Right);
    }

    // FUNCTION TEMPLATE duration_cast
    template <class _To, class _Rep, class _Period, enable_if_t<_Is_duration_v<_To>, int> _Enabled>
    _NODISCARD constexpr _To duration_cast(const duration<_Rep, _Period>& _Dur) noexcept(
        is_arithmetic_v<_Rep>&& is_arithmetic_v<typename _To::rep>) /* strengthened */ {
        // convert duration to another duration; truncate
        using _CF = ratio_divide<_Period, typename _To::period>;

        using _ToRep = typename _To::rep;
        using _CR = common_type_t<_ToRep, _Rep, intmax_t>;

        constexpr bool _Num_is_one = _CF::num == 1;
        constexpr bool _Den_is_one = _CF::den == 1;

        if (_Den_is_one) {
            if (_Num_is_one) {
                return static_cast<_To>(static_cast<_ToRep>(_Dur.count()));
            }
            else {
                return static_cast<_To>(
                    static_cast<_ToRep>(static_cast<_CR>(_Dur.count()) * static_cast<_CR>(_CF::num)));
            }
        }
        else {
            if (_Num_is_one) {
                return static_cast<_To>(
                    static_cast<_ToRep>(static_cast<_CR>(_Dur.count()) / static_cast<_CR>(_CF::den)));
            }
            else {
                return static_cast<_To>(static_cast<_ToRep>(
                    static_cast<_CR>(_Dur.count()) * static_cast<_CR>(_CF::num) / static_cast<_CR>(_CF::den)));
            }
        }
    }

    // FUNCTION TEMPLATE floor
    template <class _To, class _Rep, class _Period, enable_if_t<_Is_duration_v<_To>, int> = 0>
    _NODISCARD constexpr _To floor(const duration<_Rep, _Period>& _Dur) noexcept(
        is_arithmetic_v<_Rep>&& is_arithmetic_v<typename _To::rep>) /* strengthened */ {
        // convert duration to another duration; round towards negative infinity
        // i.e. the greatest integral result such that the result <= _Dur
        const _To _Casted{ chrono::duration_cast<_To>(_Dur) };
        if (_Casted > _Dur) {
            return _To{ _Casted.count() - static_cast<typename _To::rep>(1) };
        }

        return _Casted;
    }

    // FUNCTION TEMPLATE ceil
    template <class _To, class _Rep, class _Period, enable_if_t<_Is_duration_v<_To>, int> = 0>
    _NODISCARD constexpr _To ceil(const duration<_Rep, _Period>& _Dur) noexcept(
        is_arithmetic_v<_Rep>&& is_arithmetic_v<typename _To::rep>) /* strengthened */ {
        // convert duration to another duration; round towards positive infinity
        // i.e. the least integral result such that _Dur <= the result
        const _To _Casted{ chrono::duration_cast<_To>(_Dur) };
        if (_Casted < _Dur) {
            return _To{ _Casted.count() + static_cast<typename _To::rep>(1) };
        }

        return _Casted;
    }

    // FUNCTION TEMPLATE round
    template <class _Rep>
    constexpr bool _Is_even(_Rep _Val) noexcept(is_arithmetic_v<_Rep>) /* strengthened */ {
        // Tests whether _Val is even
        return _Val % 2 == 0;
    }

    template <class _To, class _Rep, class _Period,
        enable_if_t<_Is_duration_v<_To> && !treat_as_floating_point_v<typename _To::rep>, int> = 0>
        _NODISCARD constexpr _To round(const duration<_Rep, _Period>& _Dur) noexcept(
            is_arithmetic_v<_Rep>&& is_arithmetic_v<typename _To::rep>) /* strengthened */ {
        // convert duration to another duration, round to nearest, ties to even
        const _To _Floored{ chrono::floor<_To>(_Dur) };
        const _To _Ceiled{ _Floored + _To{1} };
        const auto _Floor_adjustment = _Dur - _Floored;
        const auto _Ceil_adjustment = _Ceiled - _Dur;
        if (_Floor_adjustment < _Ceil_adjustment
            || (_Floor_adjustment == _Ceil_adjustment && _Is_even(_Floored.count()))) {
            return _Floored;
        }

        return _Ceiled;
    }

    // FUNCTION TEMPLATE abs
    template <class _Rep, class _Period, enable_if_t<numeric_limits<_Rep>::is_signed, int> = 0>
    _NODISCARD constexpr duration<_Rep, _Period> abs(const duration<_Rep, _Period> _Dur) noexcept(
        is_arithmetic_v<_Rep>) /* strengthened */ {
        // create a duration with count() the absolute value of _Dur.count()
        return _Dur < duration<_Rep, _Period>::zero() ? duration<_Rep, _Period>::zero() - _Dur : _Dur;
    }

    // duration TYPES
    using nanoseconds = duration<long long, nano>;
    using microseconds = duration<long long, micro>;
    using milliseconds = duration<long long, milli>;
    using seconds = duration<long long>;
    using minutes = duration<int, ratio<60>>;
    using hours = duration<int, ratio<3600>>;

    // time_point ARITHMETIC
    template <class _Clock, class _Duration, class _Rep, class _Period>
    _NODISCARD constexpr time_point<_Clock, common_type_t<_Duration, duration<_Rep, _Period>>>
        operator+(const time_point<_Clock, _Duration>& _Left, const duration<_Rep, _Period>& _Right) noexcept(
            is_arithmetic_v<typename _Duration::rep>&& is_arithmetic_v<_Rep>) /* strengthened */ {
        using _RT = time_point<_Clock, common_type_t<_Duration, duration<_Rep, _Period>>>;
        return _RT(_Left.time_since_epoch() + _Right);
    }

    template <class _Rep, class _Period, class _Clock, class _Duration>
    _NODISCARD constexpr time_point<_Clock, common_type_t<duration<_Rep, _Period>, _Duration>>
        operator+(const duration<_Rep, _Period>& _Left, const time_point<_Clock, _Duration>& _Right) noexcept(
            is_arithmetic_v<_Rep>&& is_arithmetic_v<typename _Duration::rep>) /* strengthened */ {
        return _Right + _Left;
    }

    template <class _Clock, class _Duration, class _Rep, class _Period>
    _NODISCARD constexpr time_point<_Clock, common_type_t<_Duration, duration<_Rep, _Period>>>
        operator-(const time_point<_Clock, _Duration>& _Left, const duration<_Rep, _Period>& _Right) noexcept(
            is_arithmetic_v<typename _Duration::rep>&& is_arithmetic_v<_Rep>) /* strengthened */ {
        using _RT = time_point<_Clock, common_type_t<_Duration, duration<_Rep, _Period>>>;
        return _RT(_Left.time_since_epoch() - _Right);
    }

    template <class _Clock, class _Duration1, class _Duration2>
    _NODISCARD constexpr common_type_t<_Duration1, _Duration2>
        operator-(const time_point<_Clock, _Duration1>& _Left, const time_point<_Clock, _Duration2>& _Right) noexcept(
            is_arithmetic_v<typename _Duration1::rep>&& is_arithmetic_v<typename _Duration2::rep>) /* strengthened */ {
        return _Left.time_since_epoch() - _Right.time_since_epoch();
    }

    // time_point COMPARISONS
    template <class _Clock, class _Duration1, class _Duration2>
    _NODISCARD constexpr bool
        operator==(const time_point<_Clock, _Duration1>& _Left, const time_point<_Clock, _Duration2>& _Right) noexcept(
            is_arithmetic_v<typename _Duration1::rep>&& is_arithmetic_v<typename _Duration2::rep>) /* strengthened */ {
        return _Left.time_since_epoch() == _Right.time_since_epoch();
    }

    template <class _Clock, class _Duration1, class _Duration2>
    _NODISCARD constexpr bool
        operator!=(const time_point<_Clock, _Duration1>& _Left, const time_point<_Clock, _Duration2>& _Right) noexcept(
            is_arithmetic_v<typename _Duration1::rep>&& is_arithmetic_v<typename _Duration2::rep>) /* strengthened */ {
        return !(_Left == _Right);
    }

    template <class _Clock, class _Duration1, class _Duration2>
    _NODISCARD constexpr bool
        operator<(const time_point<_Clock, _Duration1>& _Left, const time_point<_Clock, _Duration2>& _Right) noexcept(
            is_arithmetic_v<typename _Duration1::rep>&& is_arithmetic_v<typename _Duration2::rep>) /* strengthened */ {
        return _Left.time_since_epoch() < _Right.time_since_epoch();
    }

    template <class _Clock, class _Duration1, class _Duration2>
    _NODISCARD constexpr bool
        operator<=(const time_point<_Clock, _Duration1>& _Left, const time_point<_Clock, _Duration2>& _Right) noexcept(
            is_arithmetic_v<typename _Duration1::rep>&& is_arithmetic_v<typename _Duration2::rep>) /* strengthened */ {
        return !(_Right < _Left);
    }

    template <class _Clock, class _Duration1, class _Duration2>
    _NODISCARD constexpr bool
        operator>(const time_point<_Clock, _Duration1>& _Left, const time_point<_Clock, _Duration2>& _Right) noexcept(
            is_arithmetic_v<typename _Duration1::rep>&& is_arithmetic_v<typename _Duration2::rep>) /* strengthened */ {
        return _Right < _Left;
    }

    template <class _Clock, class _Duration1, class _Duration2>
    _NODISCARD constexpr bool
        operator>=(const time_point<_Clock, _Duration1>& _Left, const time_point<_Clock, _Duration2>& _Right) noexcept(
            is_arithmetic_v<typename _Duration1::rep>&& is_arithmetic_v<typename _Duration2::rep>) /* strengthened */ {
        return !(_Left < _Right);
    }

    // FUNCTION TEMPLATE time_point_cast
    template <class _To, class _Clock, class _Duration, enable_if_t<_Is_duration_v<_To>, int> = 0>
    _NODISCARD constexpr time_point<_Clock, _To> time_point_cast(const time_point<_Clock, _Duration>& _Time) noexcept(
        is_arithmetic_v<typename _Duration::rep>&& is_arithmetic_v<typename _To::rep>) /* strengthened */ {
        // change the duration type of a time_point; truncate
        return time_point<_Clock, _To>(chrono::duration_cast<_To>(_Time.time_since_epoch()));
    }

    // FUNCTION TEMPLATE floor (for time_point instances)
    template <class _To, class _Clock, class _Duration, enable_if_t<_Is_duration_v<_To>, int> = 0>
    _NODISCARD constexpr time_point<_Clock, _To> floor(const time_point<_Clock, _Duration>& _Time) noexcept(
        is_arithmetic_v<typename _Duration::rep>&& is_arithmetic_v<typename _To::rep>) /* strengthened */ {
        // change the duration type of a time_point; round towards negative infinity
        return time_point<_Clock, _To>(chrono::floor<_To>(_Time.time_since_epoch()));
    }

    // FUNCTION TEMPLATE ceil (for time_point instances)
    template <class _To, class _Clock, class _Duration, enable_if_t<_Is_duration_v<_To>, int> = 0>
    _NODISCARD constexpr time_point<_Clock, _To> ceil(const time_point<_Clock, _Duration>& _Time) noexcept(
        is_arithmetic_v<typename _Duration::rep>&& is_arithmetic_v<typename _To::rep>) /* strengthened */ {
        // change the duration type of a time_point; round towards positive infinity
        return time_point<_Clock, _To>(chrono::ceil<_To>(_Time.time_since_epoch()));
    }

    // FUNCTION TEMPLATE round (for time_point instances)
    template <class _To, class _Clock, class _Duration,
        enable_if_t<_Is_duration_v<_To> && !treat_as_floating_point_v<typename _To::rep>, int> = 0>
        _NODISCARD constexpr time_point<_Clock, _To> round(const time_point<_Clock, _Duration>& _Time) noexcept(
            is_arithmetic_v<typename _Duration::rep>&& is_arithmetic_v<typename _To::rep>) /* strengthened */ {
        // change the duration type of a time_point; round to nearest, ties to even
        return time_point<_Clock, _To>(chrono::round<_To>(_Time.time_since_epoch()));
    }

    // CLOCKS
    struct system_clock { // wraps GetSystemTimePreciseAsFileTime/GetSystemTimeAsFileTime
        using rep = long long;

        using period = ratio_multiply<ratio<_XTIME_NSECS_PER_TICK, 1>, nano>;

        using duration = chrono::duration<rep, period>;
        using time_point = chrono::time_point<system_clock>;
        static constexpr bool is_steady = false;

        _NODISCARD static time_point now() noexcept { // get current time
            return time_point(duration(_Xtime_get_ticks()));
        }

        _NODISCARD static __time64_t to_time_t(const time_point& _Time) noexcept { // convert to __time64_t
            return static_cast<__time64_t>(_Time.time_since_epoch().count() / _XTIME_TICKS_PER_TIME_T);
        }

        _NODISCARD static time_point from_time_t(__time64_t _Tm) noexcept { // convert from __time64_t
            return time_point(duration(_Tm * _XTIME_TICKS_PER_TIME_T));
        }
    };

    struct steady_clock { // wraps QueryPerformanceCounter
        using rep = long long;
        using period = nano;
        using duration = nanoseconds;
        using time_point = chrono::time_point<steady_clock>;
        static constexpr bool is_steady = true;

        _NODISCARD static time_point now() noexcept { // get current time
            const long long _Freq = _Query_perf_frequency(); // doesn't change after system boot
            const long long _Ctr = _Query_perf_counter();
            static_assert(period::num == 1, "This assumes period::num == 1.");
            const long long _Whole = (_Ctr / _Freq) * period::den;
            const long long _Part = (_Ctr % _Freq) * period::den / _Freq;
            return time_point(duration(_Whole + _Part));
        }
    };

    using high_resolution_clock = steady_clock;
} // namespace chrono

// HELPERS
template <class _Rep, class _Period>
_NODISCARD bool _To_xtime_10_day_clamped(_CSTD xtime& _Xt, const chrono::duration<_Rep, _Period>& _Rel_time) noexcept(
    is_arithmetic_v<_Rep>) {
    // Convert duration to xtime, maximum 10 days from now, returns whether clamping occurred.
    // If clamped, timeouts will be transformed into spurious non-timeout wakes, due to ABI restrictions where
    // the other side of the DLL boundary overflows int32_t milliseconds.
    // Every function calling this one is TRANSITION, ABI
    constexpr chrono::nanoseconds _Ten_days{ chrono::hours{24} *10 };
    constexpr chrono::duration<double> _Ten_days_d{ _Ten_days };
    chrono::nanoseconds _T0 = chrono::system_clock::now().time_since_epoch();
    const bool _Clamped = _Ten_days_d < _Rel_time;
    if (_Clamped) {
        _T0 += _Ten_days;
    }
    else {
        _T0 += chrono::duration_cast<chrono::nanoseconds>(_Rel_time);
    }

    const auto _Whole_seconds = chrono::duration_cast<chrono::seconds>(_T0);
    _Xt.sec = _Whole_seconds.count();
    _T0 -= _Whole_seconds;
    _Xt.nsec = static_cast<long>(_T0.count());
    return _Clamped;
}

// duration LITERALS
inline namespace literals {
    inline namespace chrono_literals {
        _NODISCARD constexpr chrono::hours operator"" h(unsigned long long _Val) noexcept /* strengthened */ {
            return chrono::hours(_Val);
        }

        _NODISCARD constexpr chrono::duration<double, ratio<3600>> operator"" h(
            long double _Val) noexcept /* strengthened */ {
            return chrono::duration<double, ratio<3600>>(_Val);
        }

        _NODISCARD constexpr chrono::minutes(operator"" min)(unsigned long long _Val) noexcept /* strengthened */ {
            return chrono::minutes(_Val);
        }

        _NODISCARD constexpr chrono::duration<double, ratio<60>>(operator"" min)(
            long double _Val) noexcept /* strengthened */ {
            return chrono::duration<double, ratio<60>>(_Val);
        }

        _NODISCARD constexpr chrono::seconds operator"" s(unsigned long long _Val) noexcept /* strengthened */ {
            return chrono::seconds(_Val);
        }

        _NODISCARD constexpr chrono::duration<double> operator"" s(long double _Val) noexcept /* strengthened */ {
            return chrono::duration<double>(_Val);
        }

        _NODISCARD constexpr chrono::milliseconds operator"" ms(unsigned long long _Val) noexcept /* strengthened */ {
            return chrono::milliseconds(_Val);
        }

        _NODISCARD constexpr chrono::duration<double, milli> operator"" ms(
            long double _Val) noexcept /* strengthened */ {
            return chrono::duration<double, milli>(_Val);
        }

        _NODISCARD constexpr chrono::microseconds operator"" us(unsigned long long _Val) noexcept /* strengthened */ {
            return chrono::microseconds(_Val);
        }

        _NODISCARD constexpr chrono::duration<double, micro> operator"" us(
            long double _Val) noexcept /* strengthened */ {
            return chrono::duration<double, micro>(_Val);
        }

        _NODISCARD constexpr chrono::nanoseconds operator"" ns(unsigned long long _Val) noexcept /* strengthened */ {
            return chrono::nanoseconds(_Val);
        }

        _NODISCARD constexpr chrono::duration<double, nano> operator"" ns(
            long double _Val) noexcept /* strengthened */ {
            return chrono::duration<double, nano>(_Val);
        }
    } // namespace chrono_literals
} // namespace literals

namespace chrono {
    using namespace literals::chrono_literals;
} // namespace chrono

_STD_END
#pragma pop_macro("new")
_STL_RESTORE_CLANG_WARNINGS
#pragma warning(pop)
#pragma pack(pop)
#endif // _STL_COMPILER_PREPROCESSOR
#endif

// Screen Variables
int screenWidth = 120;
int screenHeight = 40;
// Player Variables
float playerXPos = 8.0f;
float playerYPos = 8.0f;
float playerA = 0.0f;
float fieldOfView = 3.14159 / 4.0;
float depth = 16.0f;
// Map Variables
int mapWidth = 16;
int mapHeight = 16;
int main()
{
    wchar_t *screen = new wchar_t[screenWidth * screenHeight];
    HANDLE consoleHandle = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
    SetConsoleActiveScreenBuffer(consoleHandle);
    DWORD bytesWritten = 0;
    std::wstring map;
    map += L"################";
    map += L"#..............#";
    map += L"#..............#";
    map += L"#..............#";
    map += L"#..............#";
    map += L"#..............#";
    map += L"#..............#";
    map += L"#..............#";
    map += L"#..............#";
    map += L"#..............#";
    map += L"#..............#";
    map += L"#..............#";
    map += L"#..............#";
    map += L"#..............#";
    map += L"#..............#";
    map += L"################";
    while(1)
    {
        if (GetAsyncKeyState((unsigned short)'A') & 0x80000)
            playerA -= (0.1f);
        if (GetAsyncKeyState((unsigned short)'D') & 0x80000)
            playerA += (0.1f);
        for (int x = 0; x < screenWidth; x++)
        {
            float rayAngle = (playerA - fieldOfView / 2.0f) + ((float)x / (float)screenWidth) * fieldOfView;
            float distanceToWall = 0.0f;
            bool hitWall = false;
            float eyeX = sinf(rayAngle);
            float eyeY = cosf(rayAngle);
            while (!hitWall && distanceToWall < depth)
            {
                distanceToWall += 0.1f;
                int testX = (int)(playerXPos + eyeX * distanceToWall);
                int testY = (int)(playerYPos + eyeY * distanceToWall);
                if (testX < 0 || testX >= mapWidth || testY < 0 || testY >= mapHeight)
                {
                    hitWall = true;
                    distanceToWall = depth;
                }
                else
                    if (map[testY * mapWidth + testX] == '#')
                        hitWall = true;
            }
            int ceiling = (float)(screenHeight / 2.0) - screenHeight / ((float)distanceToWall);
            int floor = screenHeight - ceiling;
            for (int y = 0; y < screenHeight; y++)
            {
                if (y < ceiling)
                    screen[y * screenWidth + x] = ' ';
                else if(y > ceiling && y <= floor)
                    screen[y * screenWidth + x] = '#';
                else
                    screen[y * screenWidth + x] = ' ';
            }
        }
        screen[screenWidth * screenHeight - 1] = '\0';
        WriteConsoleOutputCharacter(consoleHandle, screen, screenWidth * screenHeight, { 0, 0 }, &bytesWritten);
    }
    return 0;
}
