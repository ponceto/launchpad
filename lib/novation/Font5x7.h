/*
 * Font5x7.h - Copyright (c) 2001-2024 - Olivier Poncet
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef __NOVATION_Font5x7_h__
#define __NOVATION_Font5x7_h__

// ---------------------------------------------------------------------------
// novation::Font5x7
// ---------------------------------------------------------------------------

namespace novation {

struct Font5x7
{
    static constexpr int CHAR_WIDTH  = 5;
    static constexpr int CHAR_HEIGHT = 7;
    static constexpr int MAX_CHARS   = 128;
    static constexpr int MAX_BYTES   = 5;

    static const uint8_t data[MAX_CHARS][MAX_BYTES];
};

}

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------

#endif /* __NOVATION_Font5x7_h__ */
