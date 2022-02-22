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

#include <base/ArgList.h>
#include <base/Console.h>
#include <novation/Launchpad.h>

// ---------------------------------------------------------------------------
// some aliases
// ---------------------------------------------------------------------------

using Launchpad          = novation::Launchpad;
using LaunchpadUniquePtr = std::unique_ptr<Launchpad>;
using ArgList            = base::ArgList;
using Console            = base::Console;

// ---------------------------------------------------------------------------
// LaunchpadAppType
// ---------------------------------------------------------------------------

enum class LaunchpadAppType
{
    kNONE       = 0,
    kHELP       = 1,
    kLIST       = 2,
    kCYCLE      = 3,
    kPRINT      = 4,
    kSCROLL     = 5,
    kGAMEOFLIFE = 6,
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
                 , uint64_t       delay );

    virtual ~LaunchpadApp();

    virtual void main() = 0;

    void shutdown()
    {
        _shutdown = true;
    }

protected: // protected data
    const ArgList& _arglist;
    const Console& _console;
    Launchpad&     _launchpad;
    const uint64_t _delay;
    const uint8_t  _black;
    const uint8_t  _red;
    const uint8_t  _green;
    const uint8_t  _yellow;
    bool           _shutdown;
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

    virtual void main();
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
                      , const uint64_t delay );

    virtual ~LaunchpadCycleApp();

    virtual void main();
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
                      , const uint64_t     delay );

    virtual ~LaunchpadPrintApp();

    virtual void main();

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
                       , const uint64_t     delay );

    virtual ~LaunchpadScrollApp();

    virtual void main();

private: // private data
    const std::string _string;
};

// ---------------------------------------------------------------------------
// LaunchpadGameOfLifeApp
// ---------------------------------------------------------------------------

class LaunchpadGameOfLifeApp
    : public LaunchpadApp
{
public: // public interface
    LaunchpadGameOfLifeApp ( const ArgList&     arglist
                           , const Console&     console
                           , Launchpad&         launchpad
                           , const std::string& string
                           , const uint64_t     delay );

    virtual ~LaunchpadGameOfLifeApp();

    virtual void main();

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
