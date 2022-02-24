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
    static constexpr long US_SCALE = 1L;
    static constexpr long MS_SCALE = 1000L;
    static constexpr long SS_SCALE = 1000L * 1000L;
    static constexpr long MN_SCALE = 1000L * 1000L * 60L;

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

    static std::string to_lower(const std::string& argument)
    {
        std::string string(argument);
        for(auto& character : string) {
            character = ::tolower(character);
        }
        return string;
    }

    static std::string to_upper(const std::string& argument)
    {
        std::string string(argument);
        for(auto& character : string) {
            character = ::toupper(character);
        }
        return string;
    }

    static uint64_t delay(const std::string& argument)
    {
        const char* string = argument.c_str();
        const char* endptr = nullptr;
        const long  value  = ::strtol(string, const_cast<char**>(&endptr), 10);

        auto check = [&](const long delay) -> uint64_t
        {
            constexpr long min_delay = 0UL;
            constexpr long max_delay = 1000000UL;
            if((delay < min_delay) || (delay > max_delay)) {
                char error[256];
                const int rc = ::snprintf(error, sizeof(error), "invalid delay <%s>: it must be expressed in [us, ms, s, m] and must be [%ldms <= delay <= %ldms]", string, (min_delay / 1000), (max_delay / 1000));
                if((rc < 0) || (rc == sizeof(error))) {
                    throw std::runtime_error("invalid delay");
                }
                throw std::runtime_error(error);
            }
            return delay;
        };

        if((endptr != nullptr) && (*endptr != '\0')) {
            if(::strcmp(endptr, "us") == 0) {
                return check(value * US_SCALE);
            }
            if(::strcmp(endptr, "ms") == 0) {
                return check(value * MS_SCALE);
            }
            if(::strcmp(endptr, "s") == 0) {
                return check(value * SS_SCALE);
            }
            if(::strcmp(endptr, "m") == 0) {
                return check(value * MN_SCALE);
            }
            return check(-1);
        }
        return check(value * MS_SCALE);
    }
};

}

// ---------------------------------------------------------------------------
// Application
// ---------------------------------------------------------------------------

Application::Application ( const ArgList& arglist
                         , const Console& console )
     : Program(arglist, console)
     , _lpAppType(LaunchpadAppType::kNONE)
     , _lpLaunchpad()
     , _lpLaunchpadApp()
     , _lpName("Launchpad Mini")
     , _lpInput(_lpName)
     , _lpOutput(_lpName)
     , _lpProgram("launchpad")
     , _lpCommand()
     , _lpParam1()
     , _lpParam2()
     , _lpParam3()
     , _lpParam4()
     , _lpDelay()
{
}

Application::~Application()
{
}

int Application::main()
{
    if(init() == false) {
        return EXIT_FAILURE;
    }
    if(loop() == false) {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

bool Application::parseOption(const std::string& option)
{
    const std::string value(arg::value(option));

    if(arg::is(option, "-h") || arg::is(option, "--help")) {
        if(_lpAppType != LaunchpadAppType::kHELP) {
            _lpAppType = LaunchpadAppType::kHELP;
        }
        return true;
    }
    else if(arg::is(option, "-l") || arg::is(option, "--list")) {
        if(_lpAppType != LaunchpadAppType::kHELP) {
            _lpAppType = LaunchpadAppType::kLIST;
        }
        return true;
    }
    else if(arg::is(option, "--delay")) {
        _lpDelay  = value;
        return true;
    }
    else if(arg::is(option, "--midi")) {
        _lpInput  = value;
        _lpOutput = value;
        return true;
    }
    else if(arg::is(option, "--midi-input")) {
        _lpInput  = value;
        return true;
    }
    else if(arg::is(option, "--midi-output")) {
        _lpOutput = value;
        return true;
    }
    return false;
}

bool Application::parseCommand(const std::string& command)
{
    auto setCommand = [&](const LaunchpadAppType appType) -> bool
    {
        _lpCommand = command;
        if(_lpAppType == LaunchpadAppType::kNONE) {
            _lpAppType = appType;
        }
        return true;
    };

    if(_lpCommand.empty()) {
        if(command.compare("help") == 0) {
            return setCommand(LaunchpadAppType::kHELP);
        }
        if(command.compare("list") == 0) {
            return setCommand(LaunchpadAppType::kLIST);
        }
        if(command.compare("cycle") == 0) {
            return setCommand(LaunchpadAppType::kCYCLE);
        }
        if(command.compare("print") == 0) {
            return setCommand(LaunchpadAppType::kPRINT);
        }
        if(command.compare("scroll") == 0) {
            return setCommand(LaunchpadAppType::kSCROLL);
        }
        if(command.compare("gameoflife") == 0) {
            return setCommand(LaunchpadAppType::kGAMEOFLIFE);
        }
    }
    return false;
}

bool Application::parseArgument(const std::string& argument)
{
    if(_lpCommand.empty()) {
        return false;
    }
    else if(_lpParam1.empty()) {
        _lpParam1 = argument;
        return true;
    }
    else if(_lpParam2.empty()) {
        _lpParam2 = argument;
        return true;
    }
    else if(_lpParam3.empty()) {
        _lpParam3 = argument;
        return true;
    }
    else if(_lpParam4.empty()) {
        _lpParam4 = argument;
        return true;
    }
    return false;
}

bool Application::init()
{
    int index = 0;
    for(auto& argument : _arglist) {
        if(index++ == 0) {
            _lpProgram = arg::basename(argument);
        }
        else if(parseOption(argument) != false) {
            continue;
        }
        else if(parseCommand(argument) != false) {
            continue;
        }
        else if(parseArgument(argument) != false) {
            continue;
        }
        else {
            throw std::runtime_error(std::string("invalid argument") + ' ' + argument);
        }
    }
    return true;
}

bool Application::loop()
{
    switch(_lpAppType) {
        default:
        case LaunchpadAppType::kHELP:
            {
                _lpLaunchpad    = std::make_unique<Launchpad>(_lpName);
                _lpLaunchpadApp = std::make_unique<launchpad::HelpApp>(_console, *_lpLaunchpad, _lpParam1, _lpParam2, _lpParam3, _lpParam4, arg::delay(_lpDelay), _lpProgram, _lpInput, _lpOutput);
            }
            break;
        case LaunchpadAppType::kLIST:
            {
                _lpLaunchpad    = std::make_unique<Launchpad>(_lpName);
                _lpLaunchpadApp = std::make_unique<launchpad::ListApp>(_console, *_lpLaunchpad, _lpParam1, _lpParam2, _lpParam3, _lpParam4, arg::delay(_lpDelay));
            }
            break;
        case LaunchpadAppType::kCYCLE:
            {
                _lpLaunchpad    = std::make_unique<Launchpad>(_lpName, _lpInput, _lpOutput);
                _lpLaunchpadApp = std::make_unique<launchpad::CycleApp>(_console, *_lpLaunchpad, _lpParam1, _lpParam2, _lpParam3, _lpParam4, arg::delay(_lpDelay));
            }
            break;
        case LaunchpadAppType::kPRINT:
            {
                _lpLaunchpad    = std::make_unique<Launchpad>(_lpName, _lpInput, _lpOutput);
                _lpLaunchpadApp = std::make_unique<launchpad::PrintApp>(_console, *_lpLaunchpad, _lpParam1, _lpParam2, _lpParam3, _lpParam4, arg::delay(_lpDelay));
            }
            break;
        case LaunchpadAppType::kSCROLL:
            {
                _lpLaunchpad    = std::make_unique<Launchpad>(_lpName, _lpInput, _lpOutput);
                _lpLaunchpadApp = std::make_unique<launchpad::ScrollApp>(_console, *_lpLaunchpad, _lpParam1, _lpParam2, _lpParam3, _lpParam4, arg::delay(_lpDelay));
            }
            break;
        case LaunchpadAppType::kGAMEOFLIFE:
            {
                _lpLaunchpad    = std::make_unique<Launchpad>(_lpName, _lpInput, _lpOutput);
                _lpLaunchpadApp = std::make_unique<launchpad::GameOfLifeApp>(_console, *_lpLaunchpad, _lpParam1, _lpParam2, _lpParam3, _lpParam4, arg::delay(_lpDelay));
            }
            break;
    }
    if(_lpLaunchpadApp) {
        _lpLaunchpadApp->main();
    }
    return true;
}

void Application::stop()
{
    if(_lpLaunchpadApp) {
        _lpLaunchpadApp->shutdown();
    }
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
