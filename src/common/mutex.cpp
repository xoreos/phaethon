/* Phaethon - A FLOSS resource explorer for BioWare's Aurora engine games
 *
 * Phaethon is the legal property of its developers, whose names
 * can be found in the AUTHORS file distributed with this source
 * distribution.
 *
 * Phaethon is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * Phaethon is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Phaethon. If not, see <http://www.gnu.org/licenses/>.
 */

/** @file
 *  Thread mutex classes.
 */

#include <boost/chrono/duration.hpp>
#include <boost/thread/lock_types.hpp>
#include <boost/thread/condition_variable.hpp>

#include "common/mutex.h"

namespace Common {

Mutex::Mutex() {
}

Mutex::~Mutex() {
}

void Mutex::lock() {
	_mutex.lock();
}

void Mutex::unlock() {
	_mutex.unlock();
}


StackLock::StackLock(Mutex &mutex) : _mutex(&mutex) {
	_mutex->lock();
}

StackLock::~StackLock() {
	_mutex->unlock();
}


Condition::Condition() : _ownMutex(true), _mutex(new Mutex) {
}

Condition::Condition(Mutex &mutex) : _ownMutex(false), _mutex(&mutex) {
}

Condition::~Condition() {
	if (_ownMutex)
		delete _mutex;
}

bool Condition::wait(uint32 timeout) {
	boost::unique_lock<boost::recursive_mutex> lock(_mutex->_mutex);
	boost::chrono::milliseconds t(timeout);

	return _condition.wait_for(lock, t) == boost::cv_status::timeout;
}

void Condition::signal() {
	_condition.notify_one();
}

} // End of namespace Common
