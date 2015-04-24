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

#ifndef COMMON_MUTEX_H
#define COMMON_MUTEX_H

#include <boost/thread/recursive_mutex.hpp>
#include <boost/thread/condition_variable.hpp>

#include "common/types.h"

namespace Common {

/** A mutex. */
class Mutex {
public:
	Mutex();
	~Mutex();

	void lock();
	void unlock();

private:
	boost::recursive_mutex _mutex;

	friend class Condition;
};

/** Convenience class that locks a mutex on creation and unlocks it on destruction. */
class StackLock {
public:
	StackLock(Mutex &mutex);
	~StackLock();

private:
	Mutex *_mutex;
};

/** A condition. */
class Condition {
public:
	Condition();
	Condition(Mutex &mutex);
	~Condition();

	/** Wait for this condition to be signaled or the timeout to expire.
	 *
	 *  @param  timeout Time to wait for a signal in ms.
	 *  @return true If the timeout expired without a signal occurring, false otherwise.
	 */
	bool wait(uint32 timeout = 0);
	void signal();

private:
	bool _ownMutex;

	Mutex *_mutex;

	boost::condition_variable_any _condition;
};

} // End of namespace Common

#endif // COMMON_MUTEX_H
