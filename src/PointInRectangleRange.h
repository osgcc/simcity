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

#include "VectorSizeRange.h"
#include "Rectangle.h"
#include "Vector.h"
#include "Point.h"


template <typename BaseType>
class PointInRectangleRange {
public:
	class Iterator {
	public:
		Iterator(const Rectangle<BaseType>& rect, Vector<BaseType> initial = Vector<BaseType>{0, 0}) :
			mIterator(rect.size(), initial),
			mStartPoint(rect.startPoint())
		{}
		Iterator(const Iterator& other) = default;
		Iterator& operator=(const Iterator& other) = default;

		Iterator& operator++() {
			++mIterator;
			return *this;
		}

		Iterator& operator--() {
			--mIterator;
			return *this;
		}

		bool operator==(const Iterator& other) const {
			return **this == *other;
		}

		bool operator!=(const Iterator& other) const {
			return !(*this == other);
		}

		Point<BaseType> operator*() const {
			return mStartPoint + *mIterator;
		}

	private:
		typename VectorSizeRange<BaseType>::Iterator mIterator;
		const Point<BaseType> mStartPoint;
	};


	PointInRectangleRange(Rectangle<BaseType> rect) :
		mRect(rect)
	{}

	Iterator begin() const {
		return Iterator{mRect};
	}

	Iterator end() const {
		return Iterator{mRect, Vector<BaseType>{0, mRect.height}};
	}

private:
	const Rectangle<BaseType> mRect;
};
