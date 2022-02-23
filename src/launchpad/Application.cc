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
     , _lpName("Launchpad Mini")
     , _lpProgram("launchpad")
     , _lpInput(_lpName)
     , _lpOutput(_lpName)
     , _lpParam()
     , _lpDelay()
     , _lpAppType(LaunchpadAppType::kNONE)
     , _lpLaunchpad()
     , _lpLaunchpadApp()
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

bool Application::init()
{
    /* parse command-line */ {
        int index = 0;
        for(auto& argument : _arglist) {
            const std::string argval(arg::value(argument));
            if(index++ == 0) {
                _lpProgram = arg::basename(argument);
            }
            else if(arg::is(argument, "-h") || arg::is(argument, "--help")) {
                if(_lpAppType != LaunchpadAppType::kHELP) {
                    _lpAppType = LaunchpadAppType::kHELP;
                    _lpParam   = argval;
                }
            }
            else if(arg::is(argument, "-l") || arg::is(argument, "--list")) {
                if(_lpAppType != LaunchpadAppType::kHELP) {
                    _lpAppType = LaunchpadAppType::kLIST;
                    _lpParam   = argval;
                }
            }
            else if(arg::is(argument, "--cycle")) {
                if(_lpAppType == LaunchpadAppType::kNONE) {
                    _lpAppType = LaunchpadAppType::kCYCLE;
                    if(argval.size() != 0) {
                        _lpParam = argval;
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
                        _lpParam = argval;
                    }
                    if(_lpParam.empty()) {
                        _lpParam = " Hello World ";
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
                        _lpParam = argval;
                    }
                    if(_lpParam.empty()) {
                        _lpParam = " Hello World ";
                    }
                    if(_lpDelay.empty()) {
                        _lpDelay = "100ms";
                    }
                }
            }
            else if(arg::is(argument, "--game-of-life")) {
                if(_lpAppType == LaunchpadAppType::kNONE) {
                    _lpAppType = LaunchpadAppType::kGAMEOFLIFE;
                    if(argval.size() != 0) {
                        _lpParam = argval;
                    }
                    if(_lpParam.empty()) {
                        _lpParam = "random";
                    }
                    if(_lpDelay.empty()) {
                        _lpDelay = "750ms";
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
    return true;
}

bool Application::loop()
{
    switch(_lpAppType) {
        default:
        case LaunchpadAppType::kHELP:
            {
                _lpLaunchpad    = std::make_unique<Launchpad>(_lpName);
                _lpLaunchpadApp = std::make_unique<launchpad::HelpApp>(_console, *_lpLaunchpad, _lpParam, arg::delay(_lpDelay), _lpProgram, _lpInput, _lpOutput);
            }
            break;
        case LaunchpadAppType::kLIST:
            {
                _lpLaunchpad    = std::make_unique<Launchpad>(_lpName);
                _lpLaunchpadApp = std::make_unique<launchpad::ListApp>(_console, *_lpLaunchpad, _lpParam, arg::delay(_lpDelay));
            }
            break;
        case LaunchpadAppType::kCYCLE:
            {
                _lpLaunchpad    = std::make_unique<Launchpad>(_lpName, _lpInput, _lpOutput);
                _lpLaunchpadApp = std::make_unique<launchpad::CycleApp>(_console, *_lpLaunchpad, _lpParam, arg::delay(_lpDelay));
            }
            break;
        case LaunchpadAppType::kPRINT:
            {
                _lpLaunchpad    = std::make_unique<Launchpad>(_lpName, _lpInput, _lpOutput);
                _lpLaunchpadApp = std::make_unique<launchpad::PrintApp>(_console, *_lpLaunchpad, _lpParam, arg::delay(_lpDelay));
            }
            break;
        case LaunchpadAppType::kSCROLL:
            {
                _lpLaunchpad    = std::make_unique<Launchpad>(_lpName, _lpInput, _lpOutput);
                _lpLaunchpadApp = std::make_unique<launchpad::ScrollApp>(_console, *_lpLaunchpad, _lpParam, arg::delay(_lpDelay));
            }
            break;
        case LaunchpadAppType::kGAMEOFLIFE:
            {
                _lpLaunchpad    = std::make_unique<Launchpad>(_lpName, _lpInput, _lpOutput);
                _lpLaunchpadApp = std::make_unique<launchpad::GameOfLifeApp>(_console, *_lpLaunchpad, _lpParam, arg::delay(_lpDelay));
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
