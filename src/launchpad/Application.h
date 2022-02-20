/*
 * Application.h - Copyright (c) 2001-2022 - Olivier Poncet
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
#ifndef __Application_h__
#define __Application_h__

#include "ArgList.h"
#include "Console.h"
#include "LaunchpadApp.h"

// ---------------------------------------------------------------------------
// Application
// ---------------------------------------------------------------------------

class Application
{
public: // public interface
    Application ( const ArgList&
                , const Console& );

    virtual ~Application();

    virtual int main();

protected: // protected interface
    virtual int init();
    virtual int loop();
    virtual int help();

    virtual void run();

    virtual void onTimeout();
    virtual void onSigALRM();
    virtual void onSigUSR1();
    virtual void onSigUSR2();
    virtual void onSigPIPE();
    virtual void onSigCHLD();
    virtual void onSigTERM();
    virtual void onSigINTR();
    virtual void onSigHGUP();

protected: // protected data
    const ArgList&        _arglist;
    const Console&        _console;
    std::string           _lpName;
    std::string           _lpInput;
    std::string           _lpOutput;
    std::string           _lpString;
    std::string           _lpDelay;
    LaunchpadAppType      _lpAppType;
    LaunchpadUniquePtr    _lpLaunchpad;
    LaunchpadAppUniquePtr _lpLaunchpadApp;
    bool                  _lpShouldExit;

private: // disable copy and assignment
    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;
};

// ---------------------------------------------------------------------------
// ApplicationUniquePtr
// ---------------------------------------------------------------------------

using ApplicationUniquePtr = std::unique_ptr<Application>;

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------

#endif /* __Application_h__ */
