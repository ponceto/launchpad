/*
 * Application.cc - Copyright (c) 2001-2022 - Olivier Poncet
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
#include <cerrno>
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
// <anonymous>::arg
// ---------------------------------------------------------------------------

namespace {

struct arg
{
    static const char* basename(const std::string& argument)
    {
        const char* str = argument.c_str();
        const char* sep = ::strrchr(str, '/');

        if(sep != nullptr) {
            return sep + 1;
        }
        return str;
    }

    static bool is(const std::string& argument, const std::string& expected)
    {
        if(argument == expected) {
            return true;
        }
        else {
            const char* const str = argument.c_str();
            const char* const equ = ::strchr(str, '=');
            if(equ != nullptr) {
                const size_t pos = 0;
                const size_t len = (equ - str);
                if(argument.compare(pos, len, expected) == 0) {
                    return true;
                }
            }
        }
        return false;
    }

    static const char* value(const std::string& argument)
    {
        const char* str = argument.c_str();
        const char* equ = ::strchr(str, '=');

        if(equ != nullptr) {
            return equ + 1;
        }
        return "";
    }

    static uint32_t delay(const std::string& argument)
    {
        const char* string = argument.c_str();
        const char* endptr = nullptr;
        const long  value  = ::strtol(string, const_cast<char**>(&endptr), 10);

        if((endptr != nullptr) && (*endptr != '\0')) {
            if(::strcmp(endptr, "ms") == 0) {
                return value;
            }
            if(::strcmp(endptr, "s") == 0) {
                return value * 1000;
            }
            if(::strcmp(endptr, "m") == 0) {
                return value * 1000 * 60;
            }
            throw std::runtime_error("invalid delay");
        }
        if(value <= 0) {
            throw std::runtime_error("invalid delay");
        }
        return value;
    }
};

}

// ---------------------------------------------------------------------------
// Application
// ---------------------------------------------------------------------------

Application::Application ( const ArgList& arglist
                         , const Console& console )
     : Program(arglist, console)
     , _lpName("Launchpad Mini")
     , _lpInput(_lpName)
     , _lpOutput(_lpName)
     , _lpString(" Hello World ")
     , _lpDelay()
     , _lpAppType(LaunchpadAppType::kNONE)
     , _lpLaunchpad()
     , _lpLaunchpadApp()
     , _lpShutdown()
{
}

Application::~Application()
{
}

int Application::main()
{
    /* parse command-line */ {
        int index = 0;
        for(auto& argument : _arglist) {
            const std::string argval(arg::value(argument));
            if(index++ == 0) {
                continue;
            }
            else if(arg::is(argument, "-h") || arg::is(argument, "--help")) {
                if(_lpAppType != LaunchpadAppType::kHELP) {
                    _lpAppType = LaunchpadAppType::kHELP;
                }
            }
            else if(arg::is(argument, "-l") || arg::is(argument, "--list")) {
                if(_lpAppType != LaunchpadAppType::kHELP) {
                    _lpAppType = LaunchpadAppType::kLIST;
                }
            }
            else if(arg::is(argument, "--cycle")) {
                if(_lpAppType == LaunchpadAppType::kNONE) {
                    _lpAppType = LaunchpadAppType::kCYCLE;
                    if(argval.size() != 0) {
                        _lpString = argval;
                    }
                    if(_lpDelay.empty()) {
                        _lpDelay = "500ms";
                    }
                }
            }
            else if(arg::is(argument, "--print")) {
                if(_lpAppType == LaunchpadAppType::kNONE) {
                    _lpAppType = LaunchpadAppType::kPRINT;
                    if(argval.size() != 0) {
                        _lpString = argval;
                    }
                    if(_lpDelay.empty()) {
                        _lpDelay = "250ms";
                    }
                }
            }
            else if(arg::is(argument, "--scroll")) {
                if(_lpAppType == LaunchpadAppType::kNONE) {
                    _lpAppType = LaunchpadAppType::kSCROLL;
                    if(argval.size() != 0) {
                        _lpString = argval;
                    }
                    if(_lpDelay.empty()) {
                        _lpDelay = "100ms";
                    }
                }
            }
            else if(arg::is(argument, "--delay")) {
                _lpDelay  = argval;
            }
            else if(arg::is(argument, "--midi")) {
                _lpInput  = argval;
                _lpOutput = argval;
            }
            else if(arg::is(argument, "--midi-input")) {
                _lpInput  = argval;
            }
            else if(arg::is(argument, "--midi-output")) {
                _lpOutput = argval;
            }
            else {
                throw std::runtime_error(std::string("invalid argument") + ' ' + argument);
            }
        }
    }
    return loop();
}

int Application::loop()
{
    switch(_lpAppType) {
        case LaunchpadAppType::kHELP:
            break;
        case LaunchpadAppType::kLIST:
            {
                _lpLaunchpad.reset(new Launchpad(_lpName));
                _lpLaunchpadApp.reset(new LaunchpadListApp(_arglist, _console, *_lpLaunchpad));
            }
            break;
        case LaunchpadAppType::kCYCLE:
            {
                _lpLaunchpad.reset(new Launchpad(_lpName, _lpInput, _lpOutput));
                _lpLaunchpadApp.reset(new LaunchpadCycleApp(_arglist, _console, *_lpLaunchpad, arg::delay(_lpDelay)));
            }
            break;
        case LaunchpadAppType::kPRINT:
            {
                _lpLaunchpad.reset(new Launchpad(_lpName, _lpInput, _lpOutput));
                _lpLaunchpadApp.reset(new LaunchpadPrintApp(_arglist, _console, *_lpLaunchpad, _lpString, arg::delay(_lpDelay)));
            }
            break;
        case LaunchpadAppType::kSCROLL:
            {
                _lpLaunchpad.reset(new Launchpad(_lpName, _lpInput, _lpOutput));
                _lpLaunchpadApp.reset(new LaunchpadScrollApp(_arglist, _console, *_lpLaunchpad, _lpString, arg::delay(_lpDelay)));
            }
            break;
        default:
            break;
    }
    if(_lpLaunchpadApp) {
        _lpLaunchpadApp->main();
    }
    else {
        return help();
    }
    return EXIT_SUCCESS;
}

int Application::help()
{
    const std::string program(arg::basename(_arglist.at(0)));

    if(_console.printStream.good()) {
        _console.printStream << "Usage: " << program << ' ' << "[OPTIONS]"                  << std::endl;
        _console.printStream << ""                                                          << std::endl;
        _console.printStream << "    -h, --help                  display this help"         << std::endl;
        _console.printStream << "    -l, --list                  list available MIDI ports" << std::endl;
        _console.printStream << ""                                                          << std::endl;
        _console.printStream << "    --cycle                     cycle colors"              << std::endl;
        _console.printStream << "    --print={text}              print a text"              << std::endl;
        _console.printStream << "    --scroll={text}             scroll a text"             << std::endl;
        _console.printStream << ""                                                          << std::endl;
        _console.printStream << "    --delay={value[ms|s|m]}     delay (ms by default)"     << std::endl;
        _console.printStream << ""                                                          << std::endl;
        _console.printStream << "    --midi={port}               MIDI input/output"         << std::endl;
        _console.printStream << "    --midi-input={port}         MIDI input"                << std::endl;
        _console.printStream << "    --midi-output={port}        MIDI output"               << std::endl;
        _console.printStream << ""                                                          << std::endl;
        _console.printStream << "MIDI input:"                                               << std::endl;
        _console.printStream << ""                                                          << std::endl;
        _console.printStream << "  - " << _lpInput                                          << std::endl;
        _console.printStream << ""                                                          << std::endl;
        _console.printStream << "MIDI output:"                                              << std::endl;
        _console.printStream << ""                                                          << std::endl;
        _console.printStream << "  - " << _lpOutput                                         << std::endl;
        _console.printStream << ""                                                          << std::endl;
    }
    return EXIT_SUCCESS;
}

void Application::stop()
{
    if(_lpLaunchpadApp) {
        if(_lpLaunchpadApp->running()) {
            _lpLaunchpadApp->shutdown();
        }
    }
    _lpShutdown = true;
}

bool Application::running()
{
    if(_lpLaunchpadApp) {
        if(_lpLaunchpadApp->terminated()) {
            _lpShutdown = true;
        }
    }
    return _lpShutdown == false;
}

bool Application::terminated()
{
    if(_lpLaunchpadApp) {
        if(_lpLaunchpadApp->terminated()) {
            _lpShutdown = true;
        }
    }
    return _lpShutdown != false;
}

void Application::onTimeout()
{
}

void Application::onSIGALRM()
{
}

void Application::onSIGUSR1()
{
}

void Application::onSIGUSR2()
{
}

void Application::onSIGPIPE()
{
}

void Application::onSIGCHLD()
{
}

void Application::onSIGTERM()
{
    stop();
}

void Application::onSIGINTR()
{
    stop();
}

void Application::onSIGHGUP()
{
    stop();
}

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------
