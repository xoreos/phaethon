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
 *  Threading helpers.
 */

#include "src/common/thread.h"

namespace Common {

Thread::Thread() : _shouldQuit(false) {
}

Thread::~Thread() {
	try {
		destroyThread();
	} catch (...) {
	}
}

void Thread::createThread() {
	// Make sure the boost::thread is a not-a-thread (not already running)
	assert(_thread.get_id() == boost::thread::id());

	_shouldQuit = false;

	// Create the thread using the ThreadHelper and give it this instance to work on
	_thread = boost::thread(ThreadHelper(), this);
}

void Thread::destroyThread() {
	_shouldQuit = true;

	_thread.join();
}

bool Thread::shouldQuitThread() const {
	return _shouldQuit;
}

} // End of namespace Common
