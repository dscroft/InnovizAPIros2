//Confidential and proprietary to Innoviz Technologies Ltd//

#ifndef __NDARRAY_BITMAPS_H__
#define __NDARRAY_BITMAPS_H__

#include <cstdint>
#include <memory>

namespace ndarray {

	namespace bitmap_helpers {

		class IBitFieldGetter {

		public:

			virtual uint32_t get(int index) = 0;

			virtual ~IBitFieldGetter() {}
		};


		class BitFieldGetter : public IBitFieldGetter {

		private:

			static constexpr int key_offset_factor = 1000;

			static inline int key(int offset, int width);

			static constexpr int ckey(int offset, int width);

			std::unique_ptr<IBitFieldGetter> getter;

		public:
			BitFieldGetter();

			BitFieldGetter(void* data, size_t* strides, size_t ndim, int offset, int width);

			uint32_t get(int index);
		};

	}
}

#endif // __NDARRAY_BITMAPS_H__