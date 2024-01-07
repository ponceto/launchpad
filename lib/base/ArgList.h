/*
 * ArgList.h - Copyright (c) 2001-2024 - Olivier Poncet
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
#ifndef __BASE_ArgList_h__
#define __BASE_ArgList_h__

// ---------------------------------------------------------------------------
// base::ArgList
// ---------------------------------------------------------------------------

namespace base {

class ArgList
{
public: // public interface
    ArgList()
        : _arglist()
    {
    }

    ArgList ( int   argc
            , char* argv[] )
        : _arglist(argv, argv + argc)
    {
    }

    auto begin() const -> auto
    {
        return _arglist.begin();
    }

    auto end() const -> auto
    {
        return _arglist.end();
    }

    auto count() const -> auto
    {
        return _arglist.size();
    }

    auto at(unsigned int index) const -> auto
    {
        return _arglist.at(index);
    }

    auto add(const std::string& argument)
    {
        _arglist.push_back(argument);
    }

private: // private data
    std::vector<std::string> _arglist;
};

}

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------

#endif /* __BASE_ArgList_h__ */
