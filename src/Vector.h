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

#include <stdexcept>


template <typename BaseType>
struct Vector {
	BaseType x = 0;
	BaseType y = 0;

	constexpr bool operator==(const Vector& vector) const {
		return (x == vector.x) && (y == vector.y);
	}
	constexpr bool operator!=(const Vector& vector) const {
		return !(*this == vector);
	}

	Vector& operator+=(const Vector& vector) {
		x += vector.x;
		y += vector.y;
		return *this;
	}
	Vector& operator-=(const Vector& vector) {
		x -= vector.x;
		y -= vector.y;
		return *this;
	}

	constexpr Vector operator+(const Vector& vector) const {
		return {
			x + vector.x,
			y + vector.y
		};
	}
	constexpr Vector operator-(const Vector& vector) const {
		return {
			x - vector.x,
			y - vector.y
		};
	}

	Vector& operator*=(BaseType scalar) {
		x *= scalar;
		y *= scalar;
		return *this;
	}
	Vector& operator/=(BaseType scalar) {
		if (scalar == 0) {
			throw std::domain_error("Cannot divide vector by 0");
		}
		x /= scalar;
		y /= scalar;
		return *this;
	}

	constexpr Vector operator*(BaseType scalar) const {
		return {x * scalar, y * scalar};
	}
	constexpr Vector operator/(BaseType scalar) const {
		if (scalar == 0) {
			throw std::domain_error("Cannot divide vector by 0");
		}
		return {x / scalar, y / scalar};
	}

	constexpr Vector skewBy(const Vector& other) const {
		return {x * other.x, y * other.y};
	}

	constexpr Vector skewInverseBy(const Vector& other) const {
		if (other.x == 0 || other.y == 0) {
			throw std::domain_error("Cannot skewInverseBy a vector with a zero component");
		}
		return {x / other.x, y / other.y};
	}

	constexpr BaseType lengthSquared() const {
		return (x * x) + (y * y);
	}

	constexpr BaseType dotProduct(const Vector& other) const {
		return (x * other.x) + (y * other.y);
	}

	// Reflect the x-coordinate (flip across Y-axis)
	constexpr Vector reflectX() const {
		return {-x, y};
	}

	// Reflect the y-coordinate (flip across X-axis)
	constexpr Vector reflectY() const {
		return {x, -y};
	}

	template <typename NewBaseType>
	constexpr operator Vector<NewBaseType>() const {
		return {
			static_cast<NewBaseType>(x),
			static_cast<NewBaseType>(y)
		};
	}

	template <typename NewBaseType>
	constexpr Vector<NewBaseType> to() const {
		return static_cast<Vector<NewBaseType>>(*this);
	}
};


template <typename BaseType>
Vector(BaseType, BaseType) -> Vector<BaseType>;
