// ==================================================================================
// = NAS2D
// = Copyright © 2008 - 2022 Leeor Dicker
// ==================================================================================
// = NAS2D is distributed under the terms of the zlib license. You are free to copy,
// = modify and distribute the software under the terms of the zlib license.
// =
// = Acknowledgement of your use of NAS2D is appriciated but is not required.
// ==================================================================================
#pragma once

#include "Vector.h"

template <typename BaseType>
struct Point {
	BaseType x = 0;
	BaseType y = 0;

	constexpr bool operator==(const Point& point) const {
		return (x == point.x) && (y == point.y);
	}
	constexpr bool operator!=(const Point& point) const {
		return !(*this == point);
	}

	Point& operator+=(const Vector<BaseType>& vector) {
		x += vector.x;
		y += vector.y;
		return *this;
	}

	Point& operator-=(const Vector<BaseType>& vector) {
		x -= vector.x;
		y -= vector.y;
		return *this;
	}

	constexpr Point operator+(const Vector<BaseType>& vector) const {
		return {x + vector.x, y + vector.y};
	}

	constexpr Point operator-(const Vector<BaseType>& vector) const {
		return {x - vector.x, y - vector.y};
	}

	constexpr Vector<BaseType> operator-(const Point& point) const {
		return {x - point.x, y - point.y};
	}

	constexpr Point skewBy(const Vector<BaseType>& other) const {
		return {x * other.x, y * other.y};
	}

	constexpr Point skewInverseBy(const Vector<BaseType>& other) const {
		if (other.x == 0 || other.y == 0) {
			throw std::domain_error("Cannot skewInverseBy a vector with a zero component");
		}
		return {x / other.x, y / other.y};
	}

	template <typename NewBaseType>
	constexpr operator Point<NewBaseType>() const {
		return {
			static_cast<NewBaseType>(x),
			static_cast<NewBaseType>(y)
		};
	}

	template <typename NewBaseType>
	constexpr Point<NewBaseType> to() const {
		return static_cast<Point<NewBaseType>>(*this);
	}
};


template <typename BaseType>
Point(BaseType, BaseType) -> Point<BaseType>;
