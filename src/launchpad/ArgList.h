/*
 * ArgList.h - Copyright (c) 2001-2022 - Olivier Poncet
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
#ifndef __ArgList_h__
#define __ArgList_h__

// ---------------------------------------------------------------------------
// ArgList
// ---------------------------------------------------------------------------

struct ArgList
{
    ArgList ( int   argc
            , char* argv[] )
        : arguments(argv, argv + argc)
    {
    }

    auto begin() const -> auto
    {
        return arguments.begin();
    }

    auto end() const -> auto
    {
        return arguments.end();
    }

    auto at(unsigned int index) const -> auto
    {
        return arguments.at(index);
    }

    std::vector<std::string> arguments;
};

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------

#endif /* __ArgList_h__ */
