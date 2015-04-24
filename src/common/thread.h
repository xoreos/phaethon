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

#ifndef COMMON_THREAD_H
#define COMMON_THREAD_H

/* WORKAROUND: boost/atomic.hpp typedefs atomic<intptr_t> and atomic<uintptr_t>,
 *             but doesn't include stdint.h on purpose when compiling with gcc,
 *             only unistd.h. Unfortunately, unistd.h in glibc 2.19 only
 *             defines intptr_t and not uintptr_t, breaking compilation.
 *
 *             To fix this issue, we manually include both unistd.h and stdint.h
 *             (in that order), if they're available.
 */
#ifdef HAVE_CONFIG_H
	#include "config.h"
#endif // HAVE_CONFIG_H
#ifdef HAVE_UNISTD_H
	#include <unistd.h>
#endif // HAVE_UNISTD_H
#ifdef HAVE_STDINT_H
	#include <stdint.h>
#endif // HAVE_STDINT_H

#include <boost/atomic.hpp>
#include <boost/thread/thread.hpp>

#include "src/common/noncopyable.h"

namespace Common {

/** A class that creates its own thread. */
class Thread : NonCopyable {
public:
	Thread();
	virtual ~Thread();

protected:
	/** Create and run the thread. */
	void createThread();
	/** Request the thread to quit and wait for it to finish. */
	void destroyThread();

	/** The thread method should call this periodically to query if it should quit. */
	bool shouldQuitThread() const;

private:
	struct ThreadHelper {
		void operator()(Thread *thread) {
			thread->threadMethod();
		}
	};

	boost::thread _thread;
	boost::atomic<bool> _shouldQuit;

	virtual void threadMethod() = 0;

	friend struct ThreadHelper;
};

} // End of namespace Common

#endif // COMMON_THREAD_H
