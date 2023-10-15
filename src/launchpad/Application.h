/*
 * Application.h - Copyright (c) 2001-2023 - Olivier Poncet
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

#include <base/ArgList.h>
#include <base/Console.h>
#include <base/Program.h>
#include "Command.h"

// ---------------------------------------------------------------------------
// some aliases
// ---------------------------------------------------------------------------

using ArgList = base::ArgList;
using Console = base::Console;
using Program = base::Program;

// ---------------------------------------------------------------------------
// Application
// ---------------------------------------------------------------------------

class Application final
    : public Program
{
public: // public interface
    Application ( const ArgList& arglist
                , const Console& console );

    virtual ~Application();

    virtual int  main() override;
    virtual void stop() override;

    virtual void onTIMEOUT() override;
    virtual void onSIGALRM() override;
    virtual void onSIGUSR1() override;
    virtual void onSIGUSR2() override;
    virtual void onSIGPIPE() override;
    virtual void onSIGCHLD() override;
    virtual void onSIGTERM() override;
    virtual void onSIGINTR() override;
    virtual void onSIGHGUP() override;

private: // private interface
    bool init();
    bool loop();

    bool parseOption(const std::string& option);
    bool parseCommand(const std::string& command);
    bool parseArgument(const std::string& argument);

private: // private data
    CommandType        _lpCommandType;
    LaunchpadUniquePtr _lpLaunchpadPtr;
    CommandUniquePtr   _lpCommandPtr;
    ArgList            _lpArguments;
    std::string        _lpName;
    std::string        _lpInput;
    std::string        _lpOutput;
    std::string        _lpProgram;
    std::string        _lpCommand;
    std::string        _lpParam1;
    std::string        _lpParam2;
    std::string        _lpParam3;
    std::string        _lpParam4;
    std::string        _lpDelay;
};

// ---------------------------------------------------------------------------
// ApplicationUniquePtr
// ---------------------------------------------------------------------------

using ApplicationUniquePtr = std::unique_ptr<Application>;

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------

#endif /* __Application_h__ */
