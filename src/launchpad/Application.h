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

#include <base/Program.h>
#include "LaunchpadApp.h"

// ---------------------------------------------------------------------------
// Application
// ---------------------------------------------------------------------------

class Application final
    : public Program
{
public: // public interface
    Application ( const ArgList&
                , const Console& );

    virtual ~Application();

    virtual int  main() override;
    virtual void stop() override;

    virtual void onTimeout() override;
    virtual void onSigALRM() override;
    virtual void onSigUSR1() override;
    virtual void onSigUSR2() override;
    virtual void onSigPIPE() override;
    virtual void onSigCHLD() override;
    virtual void onSigTERM() override;
    virtual void onSigINTR() override;
    virtual void onSigHGUP() override;

protected: // protected interface
    virtual int init();
    virtual int loop();
    virtual int help();

    virtual void run();
    virtual bool running();
    virtual bool terminated();

protected: // protected data
    std::string           _lpName;
    std::string           _lpInput;
    std::string           _lpOutput;
    std::string           _lpString;
    std::string           _lpDelay;
    LaunchpadAppType      _lpAppType;
    LaunchpadUniquePtr    _lpLaunchpad;
    LaunchpadAppUniquePtr _lpLaunchpadApp;
    bool                  _lpShutdown;
};

// ---------------------------------------------------------------------------
// ApplicationUniquePtr
// ---------------------------------------------------------------------------

using ApplicationUniquePtr = std::unique_ptr<Application>;

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------

#endif /* __Application_h__ */
