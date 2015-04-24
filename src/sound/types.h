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
 *  Basic sound types.
 */

#ifndef SOUND_TYPES_H
#define SOUND_TYPES_H

#include "src/common/types.h"

namespace Sound {

/** A handle to a sound channel. */
struct ChannelHandle {
	uint16 channel;
	uint32 id;

	ChannelHandle() : channel(0), id(0) { }
};

enum SoundType {
	kSoundTypeUnknown = 0, ///< Unknown/Generic.
	kSoundTypeMusic   = 1, ///< Music.
	kSoundTypeSFX     = 2, ///< Sound effect.
	kSoundTypeVoice   = 3, ///< Voice/Speech.
	kSoundTypeVideo   = 4, ///< Video/Movie.

	kSoundTypeMAX
};

} // End of namespace Sound

#endif // SOUND_TYPES_H
