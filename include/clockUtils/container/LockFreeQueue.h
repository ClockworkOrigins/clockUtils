/*
 * clockUtils
 * Copyright (2016) Michael Baer, Daniel Bonrath, All rights reserved.
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
#include <cassert>

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
		LockFreeQueue() : _readIndex(0), _writeIndex(0), _enqueuing(false), _dequeuing(false), _data() {
		}

		/**
		 * \brief pushes the given value into the queue
		 */
		ClockError push(const T & value) {
			bool enqueueing = false;
			while (!std::atomic_compare_exchange_strong(&_enqueuing, &enqueueing, true)) {
				enqueueing = false;
			}
			assert(!enqueueing);
			assert(_enqueuing);
			uint64_t readIndex = _readIndex;
			uint64_t writeIndex = _writeIndex;
			ClockError err = ClockError::SUCCESS;
			if (writeIndex - readIndex < SIZE || readIndex > writeIndex) {
				_data[writeIndex % SIZE] = value;
				_writeIndex.fetch_add(1);
			} else {
				err = ClockError::NO_SPACE_AVAILABLE;
			}
			_enqueuing.exchange(false);
			return err;
		}

		/**
		 * \brief removes first entry of the queue
		 */
		ClockError pop() {
			bool dequeueing = false;
			while (!std::atomic_compare_exchange_strong(&_dequeuing, &dequeueing, true)) {
				dequeueing = false;
			}
			assert(!dequeueing);
			assert(_dequeuing);
			uint64_t writeIndex = _writeIndex;
			ClockError err = ClockError::SUCCESS;
			if (writeIndex <= _readIndex) {
				err = ClockError::NO_ELEMENT;
			} else {
				_readIndex.fetch_add(1);
			}
			_dequeuing.exchange(false);
			return err;
		}

		/**
		 * \brief returns first entry of the queue, but keeps it in the queue
		 */
		ClockError front(T & value) {
			bool dequeueing = false;
			while (!std::atomic_compare_exchange_strong(&_dequeuing, &dequeueing, true)) {
				dequeueing = false;
			}
			assert(!dequeueing);
			assert(_dequeuing);
			uint64_t writeIndex = _writeIndex;
			uint64_t readIndex = _readIndex;
			ClockError err = ClockError::SUCCESS;
			if (writeIndex > readIndex) {
				value = _data[readIndex % SIZE];
			} else {
				err = ClockError::NO_ELEMENT;
			}
			_dequeuing.exchange(false);
			return err;
		}

		/**
		 * \brief removes first entry of the queue and returns its value
		 */
		ClockError poll(T & value) {
			bool dequeueing = false;
			while (!std::atomic_compare_exchange_strong(&_dequeuing, &dequeueing, true)) {
				dequeueing = false;
			}
			assert(!dequeueing);
			assert(_dequeuing);
			uint64_t writeIndex = _writeIndex;
			uint64_t readIndex = _readIndex;
			ClockError err = ClockError::SUCCESS;
			if (writeIndex > readIndex) {
				value = _data[readIndex % SIZE];
				_readIndex.fetch_add(1);
			} else {
				err = ClockError::NO_ELEMENT;
			}
			_dequeuing.exchange(false);
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
		std::atomic<bool> _enqueuing;
		std::atomic<bool> _dequeuing;

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
