#pragma once

#include <cstdint>
#include <compare>
#include <stdexcept>
#include <karkinolution/core/global_epsilon.hpp>

/*
 * Copyright 2026 Koöperative Rüpestrën
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 */

class NormalizedValue;
class SignedNormalizedValue;


template <typename Derived, typename T>
class LimitedValue {
private:

    T _value;
    T _max;
    T _min;


protected:

    Derived& derived() {
        return static_cast<Derived&>(*this);
    }

    const Derived& derived() const {
        return static_cast<const Derived&>(*this);
    }


public:

    LimitedValue(T value, T max, T min = T(0.0f))
    : _value(value), _max(max), _min(min)
{
    if (_min > Approx<float>(_max)) {
        throw std::invalid_argument("Invalid LimitedValue range");
    }

    clamp();
}

    template<typename OtherDerived, typename U>
    void validate_compatibility(
        const LimitedValue<OtherDerived, U>& other
    ) const
    {
        if (_min != other.min() || _max != other.max()) {
            throw std::invalid_argument(
                "LimitedValue incompatible ranges"
            );
        }
    }

    operator T() const {
    return value();
}

    T value() const {
        return _value;
    }

    T max() const {
        return _max;
    }

    T min() const {
        return _min;
    }


    void clamp() {
        if (_value < _min) {
            _value = _min;
        } else if (_value > _max) {
            _value = _max;
        }
    }


    template<typename U>
    Derived& set(U new_value)
    {
        _value = static_cast<T>(new_value);
        clamp();
        return derived();
    }


    template<typename U>
    Derived& operator+=(U other)
    {
        _value += static_cast<T>(other);
        clamp();
        return derived();
    }


    template<typename OtherDerived, typename U>
    Derived operator+(
        const LimitedValue<OtherDerived, U>& other
    ) const
    {
        validate_compatibility(other);

        Derived result = derived();
        result += other.value();
        result.clamp();

        return result;
    }


    template<typename U>
    Derived operator+(U other) const
    {
        Derived result = derived();
        result += other;
        result.clamp();

        return result;
    }


    template<typename U>
    Derived& operator-=(U other)
    {
        _value -= static_cast<T>(other);
        clamp();
        return derived();
    }


    template<typename OtherDerived, typename U>
    Derived operator-(
        const LimitedValue<OtherDerived, U>& other
    ) const
    {
        validate_compatibility(other);

        Derived result = derived();
        result -= other.value();
        result.clamp();

        return result;
    }


    template<typename U>
    Derived operator-(U other) const
    {
        Derived result = derived();
        result -= other;
        result.clamp();

        return result;
    }


    template<typename U>
    Derived& operator*=(U other)
    {
        _value *= static_cast<T>(other);
        clamp();
        return derived();
    }

    template <typename U>
    Derived operator*(U other)
    {
        Derived result = derived();
        result*=other;
        result.clamp();
        return result;
    }


    template<typename U>
    Derived& operator/=(U other)
    {
        _value /= static_cast<T>(other);
        clamp();
        return derived();
    }

    template <typename U>
    Derived operator/(U other)
    {
        Derived result = derived();
        result /= other;
        result.clamp();
        return result;
    }

    auto operator<=>(const LimitedValue&) const = default;


    void zero()
    {
        _value = _min;
        clamp();
    }


    void full()
    {
        _value = _max;
        clamp();
    }


    NormalizedValue ratio() const;

    SignedNormalizedValue ratio_min() const;
};



class NormalizedValue
    : public LimitedValue<NormalizedValue, float>
{
public:

    NormalizedValue(float value)
        : LimitedValue(value, 1.0f, 0.0f)
    {}

    NormalizedValue()
        : LimitedValue(0.0f, 1.0f, 0.0f)
    {}
};



class SignedNormalizedValue
    : public LimitedValue<SignedNormalizedValue, float>
{
public:

    SignedNormalizedValue(float value)
        : LimitedValue(value, 1.0f, -1.0f)
    {}

    SignedNormalizedValue()
        : LimitedValue(0.0f, 1.0f, -1.0f)
    {}
};



template <typename Derived, typename T>
NormalizedValue LimitedValue<Derived, T>::ratio() const
{
    return NormalizedValue(
        static_cast<float>(_value - _min) /
        static_cast<float>(_max - _min)
    );
}



template <typename Derived, typename T>
SignedNormalizedValue LimitedValue<Derived, T>::ratio_min() const
{
    return SignedNormalizedValue(
        static_cast<float>(_value - _min) /
        static_cast<float>(_max - _min) * 2.0f - 1.0f
    );
}



template<typename T>
class IntegerLimited {

private:

    T _value;
    T _max;


public:

    template <typename U, typename Z>
    IntegerLimited(U value, Z max)
        : _value(static_cast<T>(value)),
          _max(static_cast<T>(max))
    {
        if (_value > _max) {
            throw std::invalid_argument(
                "Invalid IntegerLimited"
            );
        }
    }


    template <typename U>
    IntegerLimited(U max)
        : _value(T(0)),
          _max(static_cast<T>(max))
    {}


    auto operator<=>(const IntegerLimited&) const = default;


    void pass()
    {
        if (_value < _max)
            ++_value;
        clamp();
    }


    NormalizedValue ratio() const
    {
        if (_max == 0) {
            return NormalizedValue(0.0f);
        }

        return NormalizedValue(
            static_cast<float>(_value) /
            static_cast<float>(_max)
        );
    }


    T value() const
    {
        return _value;
    }


    T max() const
    {
        return _max;
    }


    void clamp()
    {
        if (_value < 0)
            _value = 0;
        else if (_value > _max)
            _value = _max;
    }


    void full()
    {
        _value = _max;
        clamp();
    }


    void zero()
    {
        _value = 0;
        clamp();
    }
};



class Energy
    : public LimitedValue<Energy, float>
{
public:
    using LimitedValue<Energy, float>::LimitedValue;
};



class Life
    : public LimitedValue<Life, float>
{
public:
    using LimitedValue<Life, float>::LimitedValue;
};



class Fertility
    : public IntegerLimited<uint8_t>
{
public:

    using IntegerLimited<uint8_t>::IntegerLimited;


    bool reproductive_capability() const
    {
        return value() > 0;
    }
};



class Age
    : public IntegerLimited<uint16_t>
{
public:

    using IntegerLimited<uint16_t>::IntegerLimited;
};