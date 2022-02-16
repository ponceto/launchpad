/*
 * Console.h - Copyright (c) 2001-2022 - Olivier Poncet
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
#ifndef __Console_h__
#define __Console_h__

// ---------------------------------------------------------------------------
// some aliases
// ---------------------------------------------------------------------------

using IStream = std::istream;
using OStream = std::ostream;

// ---------------------------------------------------------------------------
// Console
// ---------------------------------------------------------------------------

struct Console
{
    Console ( IStream& is
            , OStream& os
            , OStream& es )
        : inputStream(is)
        , printStream(os)
        , errorStream(es)
    {
    }

    IStream& inputStream;
    OStream& printStream;
    OStream& errorStream;
};

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------

#endif /* __Console_h__ */
