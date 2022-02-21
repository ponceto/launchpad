/*
 * main.cc - Copyright (c) 2001-2022 - Olivier Poncet
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
#include "Application.h"

// ---------------------------------------------------------------------------
// <anonymous>::run
// ---------------------------------------------------------------------------

namespace {

int run(const ArgList& arglist, const Console& console)
{
    const ApplicationUniquePtr app(new Application(arglist, console));

    return app->main();
}

}

// ---------------------------------------------------------------------------
// <anonymous>::loop
// ---------------------------------------------------------------------------

namespace {

int loop(const ArgList& arglist, const Console& console)
{
    try {
        return run(arglist, console);
    }
    catch(const std::exception& e) {
        const char* const what(e.what());
        console.errorStream << what << std::endl;
    }
    catch(...) {
        const char* const what("an error has occured");
        console.errorStream << what << std::endl;
    }
    return EXIT_FAILURE;
}

}

// ---------------------------------------------------------------------------
// main
// ---------------------------------------------------------------------------

int main(int argc, char* argv[])
{
    const ArgList arglist ( argc
                          , argv );

    const Console console ( std::cin
                          , std::cout
                          , std::cerr );

    return loop(arglist, console);
}

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------
