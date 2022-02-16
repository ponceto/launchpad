/*
 * LaunchpadApp.h - Copyright (c) 2001-2022 - Olivier Poncet
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
#ifndef __LaunchpadApp_h__
#define __LaunchpadApp_h__

#include <novation/Launchpad.h>
#include "ArgList.h"
#include "Console.h"

// ---------------------------------------------------------------------------
// some aliases
// ---------------------------------------------------------------------------

using Launchpad          = novation::Launchpad;
using LaunchpadUniquePtr = std::unique_ptr<Launchpad>;

// ---------------------------------------------------------------------------
// LaunchpadAppType
// ---------------------------------------------------------------------------

enum class LaunchpadAppType
{
    kNONE   = 0,
    kHELP   = 1,
    kLIST   = 2,
    kCYCLE  = 3,
    kPRINT  = 4,
    kSCROLL = 5,
};

// ---------------------------------------------------------------------------
// LaunchpadApp
// ---------------------------------------------------------------------------

class LaunchpadApp
{
public: // public interface
    LaunchpadApp ( const ArgList& arglist
                 , const Console& console
                 , Launchpad&     launchpad
                 , uint32_t       delay );

    virtual ~LaunchpadApp();

    virtual int main() = 0;

protected: // protected data
    const ArgList& _arglist;
    const Console& _console;
    Launchpad&     _launchpad;
    const uint8_t  _black;
    const uint8_t  _red;
    const uint8_t  _green;
    const uint8_t  _yellow;
    const uint32_t _delay;
};

// ---------------------------------------------------------------------------
// LaunchpadListApp
// ---------------------------------------------------------------------------

class LaunchpadListApp
    : public LaunchpadApp
{
public: // public interface
    LaunchpadListApp ( const ArgList& arglist
                     , const Console& console
                     , Launchpad&     launchpad );

    virtual ~LaunchpadListApp();

    virtual int main();
};

// ---------------------------------------------------------------------------
// LaunchpadCycleApp
// ---------------------------------------------------------------------------

class LaunchpadCycleApp
    : public LaunchpadApp
{
public: // public interface
    LaunchpadCycleApp ( const ArgList& arglist
                      , const Console& console
                      , Launchpad&     launchpad
                      , const uint32_t delay );

    virtual ~LaunchpadCycleApp();

    virtual int main();
};

// ---------------------------------------------------------------------------
// LaunchpadPrintApp
// ---------------------------------------------------------------------------

class LaunchpadPrintApp
    : public LaunchpadApp
{
public: // public interface
    LaunchpadPrintApp ( const ArgList&     arglist
                      , const Console&     console
                      , Launchpad&         launchpad
                      , const std::string& string
                      , const uint32_t     delay );

    virtual ~LaunchpadPrintApp();

    virtual int main();

private: // private data
    const std::string _string;
};

// ---------------------------------------------------------------------------
// LaunchpadScrollApp
// ---------------------------------------------------------------------------

class LaunchpadScrollApp
    : public LaunchpadApp
{
public: // public interface
    LaunchpadScrollApp ( const ArgList&     arglist
                       , const Console&     console
                       , Launchpad&         launchpad
                       , const std::string& string
                       , const uint32_t     delay );

    virtual ~LaunchpadScrollApp();

    virtual int main();

private: // private data
    const std::string _string;
};

// ---------------------------------------------------------------------------
// LaunchpadAppUniquePtr
// ---------------------------------------------------------------------------

using LaunchpadAppUniquePtr = std::unique_ptr<LaunchpadApp>;

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------

#endif /* __LaunchpadApp_h__ */
