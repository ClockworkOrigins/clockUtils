/*
 * clockUtils
 * Copyright (2015) Michael Baer, Daniel Bonrath, All rights reserved.
 *
 * This file is part of clockUtils; clockUtils is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

/**
 * \addtogroup container
 * @{
 */

#ifndef __CLOCKUTILS_CONTAINER_LOCKFREEQUEUE_H__
#define __CLOCKUTILS_CONTAINER_LOCKFREEQUEUE_H__

#include <array>
#include <atomic>

#include "clockUtils/errors.h"

#include "clockUtils/container/containerParameters.h"

namespace clockUtils {
namespace container {

	/**
	 * class LockFreeQueue
	 *
	 * T defines the data type being contained in the queue
	 * size defines the maximum amount of entries this queue has space for
	 */
	template<typename T, size_t SIZE>
	class LockFreeQueue {
	public:
		/**
		 * \brief default constructor
		 */
		LockFreeQueue() : _readIndex(0), _writeIndex(0), _data() {
		}

		/**
		 * \brief pushes the given value into the queue
		 */
		ClockError push(const T & value) {
			uint64_t writeIndex = _writeIndex.fetch_add(1);
			uint64_t readIndex = _readIndex;
			ClockError err = ClockError::SUCCESS;
			if (writeIndex - readIndex < SIZE) {
				_data[writeIndex % SIZE] = value;
			} else {
				_writeIndex.fetch_sub(1);
				err = ClockError::NO_SPACE_AVAILABLE;
			}
			return err;
		}

		/**
		 * \brief removes first entry of the queue
		 */
		ClockError pop() {
			uint64_t writeIndex = _writeIndex;
			uint64_t readIndex = _readIndex.fetch_add(1);
			ClockError err = ClockError::SUCCESS;
			if (writeIndex <= readIndex) {
				_readIndex.fetch_sub(1);
				err = ClockError::NO_ELEMENT;
			}
			return err;
		}

		/**
		 * \brief returns first entry of the queue
		 */
		ClockError front(T & value) {
			uint64_t writeIndex = _writeIndex;
			uint64_t readIndex = _readIndex;
			ClockError err = ClockError::SUCCESS;
			if (writeIndex > readIndex) {
				value = _data[readIndex % SIZE];
			} else {
				err = ClockError::NO_ELEMENT;
			}
			return err;
		}

		/**
		 * \brief removes first entry of the queue and returns its value
		 */
		ClockError poll(T & value) {
			uint64_t writeIndex = _writeIndex;
			uint64_t readIndex = _readIndex.fetch_add(1);
			ClockError err = ClockError::SUCCESS;
			if (writeIndex > readIndex) {
				value = _data[readIndex % SIZE];
			} else {
				_readIndex.fetch_sub(1);
				err = ClockError::NO_ELEMENT;
			}
			return err;
		}

		/**
		 * \brief returns true if the queue is empty, otherwise false
		 */
		inline bool empty() const {
			uint64_t readIdx = _readIndex;
			uint64_t writeIdx = _writeIndex;
			return readIdx == writeIdx;
		}

		/**
		 * \brief returns size of the queue
		 */
		inline size_t size() const {
			uint64_t readIdx = _readIndex;
			uint64_t writeIdx = _writeIndex;
			return size_t(writeIdx - readIdx);
		}

		/**
		 * \brief removes all elements in the queue
		 */
		void clear() {
			_readIndex.store(_writeIndex);
		}

	private:
		std::atomic<uint64_t> _readIndex;
		std::atomic<uint64_t> _writeIndex;

		std::array<T, SIZE> _data;

		/**
		 * \brief forbidden
		 */
		LockFreeQueue(const LockFreeQueue &) = delete;
	};

} /* namespace container */
} /* namespace clockUtils */

#endif /* __CLOCKUTILS_CONTAINER_LOCKFREEQUEUE_H__ */

/**
 * @}
 */
