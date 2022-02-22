/*
 * Program.cc - Copyright (c) 2001-2022 - Olivier Poncet
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
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>
#include <iostream>
#include <stdexcept>
#include "Program.h"

// ---------------------------------------------------------------------------
// base::Program
// ---------------------------------------------------------------------------

namespace base {

Program::Program ( const ArgList& arglist
                 , const Console& console )
     : _arglist(arglist)
     , _console(console)
{
}

Program::~Program()
{
}

int Program::main()
{
    return EXIT_SUCCESS;
}

void Program::stop()
{
}

void Program::onTimeout()
{
}

void Program::onSIGALRM()
{
}

void Program::onSIGUSR1()
{
}

void Program::onSIGUSR2()
{
}

void Program::onSIGPIPE()
{
}

void Program::onSIGCHLD()
{
}

void Program::onSIGTERM()
{
}

void Program::onSIGINTR()
{
}

void Program::onSIGHGUP()
{
}

}

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------
