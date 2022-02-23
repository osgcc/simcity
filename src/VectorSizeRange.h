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
class VectorSizeRange {
public:
	class Iterator {
	public:
		Iterator(const Vector<BaseType>& size, Vector<BaseType> initial = Vector<BaseType>{0, 0}) :
			mCurrent(initial),
			mSize(size)
		{}
		Iterator(const Iterator& other) = default;
		Iterator& operator=(const Iterator& other) = default;

		Iterator& operator++() {
			++mCurrent.x;
			if (mCurrent.x >= mSize.x) {
				mCurrent.x = 0;
				++mCurrent.y;
			}
			return *this;
		}

		Iterator& operator--() {
			if (mCurrent.x <= 0) {
				mCurrent.x = mSize.x;
				--mCurrent.y;
			}
			--mCurrent.x;
			return *this;
		}

		bool operator==(const Iterator& other) const {
			return mCurrent == other.mCurrent;
		}

		bool operator!=(const Iterator& other) const {
			return !(*this == other);
		}

		Vector<BaseType> operator*() const {
			return mCurrent;
		}

	private:
		Vector<BaseType> mCurrent;
		const Vector<BaseType> mSize;
	};


	VectorSizeRange(Vector<BaseType> size) :
		mSize(size)
	{}

	Iterator begin() const {
		return Iterator{mSize};
	}

	Iterator end() const {
		return Iterator{mSize, Vector<BaseType>{0, mSize.y}};
	}

private:
	const Vector<BaseType> mSize;
};
