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
#include <csignal>
#include <memory>
#include <string>
#include <vector>
#include <iostream>
#include <stdexcept>
#include <thread>
#include <unistd.h>
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
// <anonymous>::sig
// ---------------------------------------------------------------------------

namespace {

struct sig
{
    using native_sigset    = sigset_t;
    using native_sigaction = struct sigaction;
    using native_timespec  = struct timespec;

    static constexpr int kSIGALRM = SIGALRM;
    static constexpr int kSIGUSR1 = SIGUSR1;
    static constexpr int kSIGUSR2 = SIGUSR2;
    static constexpr int kSIGPIPE = SIGPIPE;
    static constexpr int kSIGCHLD = SIGCHLD;
    static constexpr int kSIGTERM = SIGTERM;
    static constexpr int kSIGINTR = SIGINT;
    static constexpr int kSIGHGUP = SIGHUP;

    static void default_handler(int signum)
    {
        static_cast<void>(signum);
    }

    static void emptyset(native_sigset& signal_set)
    {
        const int rc = ::sigemptyset(&signal_set);
        if(rc != 0) {
            throw std::runtime_error("sigemptyset has failed");
        }
    }

    static void fillset(native_sigset& signal_set)
    {
        const int rc = ::sigfillset(&signal_set);
        if(rc != 0) {
            throw std::runtime_error("sigfillset has failed");
        }
    }

    static void addset(native_sigset& signal_set, const int signum)
    {
        const int rc = ::sigaddset(&signal_set, signum);
        if(rc != 0) {
            throw std::runtime_error("sigaddset has failed");
        }
    }

    static void delset(native_sigset& signal_set, const int signum)
    {
        const int rc = ::sigdelset(&signal_set, signum);
        if(rc != 0) {
            throw std::runtime_error("sigdelset has failed");
        }
    }

    static void procmask(const int how, const native_sigset& signal_set)
    {
       const int rc = ::pthread_sigmask(how, &signal_set, nullptr);
        if(rc != 0) {
            throw std::runtime_error("sigprocmask has failed");
        }
    }

    static int timedwait(const native_sigset& signal_set, const native_timespec& timeout)
    {
        const int rc = ::sigtimedwait(&signal_set, nullptr, &timeout);
        if((rc == -1) && (errno != EAGAIN)) {
            throw std::runtime_error("sigtimedwait has failed");
        }
        return rc;
    }

    static void action(const int signum, const native_sigaction& act)
    {
        const int rc = ::sigaction(signum, &act, nullptr);
        if(rc != 0) {
            throw std::runtime_error("sigaction has failed");
        }
    }

    static void init()
    {
        native_sigset    signal_set;
        native_sigaction signal_action;

        /* initialize signal_set */ {
            sig::emptyset(signal_set);
            sig::addset(signal_set, kSIGALRM);
            sig::addset(signal_set, kSIGUSR1);
            sig::addset(signal_set, kSIGUSR2);
            sig::addset(signal_set, kSIGPIPE);
            sig::addset(signal_set, kSIGCHLD);
            sig::addset(signal_set, kSIGTERM);
            sig::addset(signal_set, kSIGINTR);
            sig::addset(signal_set, kSIGHGUP);
        }
        /* initialize signal_action */ {
            signal_action.sa_handler = &sig::default_handler;
            signal_action.sa_flags   = 0;
            signal_action.sa_mask    = signal_set;
        }
        /* install signal handlers */ {
            sig::action(kSIGALRM, signal_action);
            sig::action(kSIGUSR1, signal_action);
            sig::action(kSIGUSR2, signal_action);
            sig::action(kSIGPIPE, signal_action);
            sig::action(kSIGCHLD, signal_action);
            sig::action(kSIGTERM, signal_action);
            sig::action(kSIGINTR, signal_action);
            sig::action(kSIGHGUP, signal_action);
        }
        /* mask signals */ {
            sig::procmask(SIG_BLOCK, signal_set);
        }
    }

    static int wait()
    {
        native_sigset   signal_set;
        native_timespec timeout;

        /* initialize signal_set */ {
            sig::emptyset(signal_set);
            sig::addset(signal_set, kSIGALRM);
            sig::addset(signal_set, kSIGUSR1);
            sig::addset(signal_set, kSIGUSR2);
            sig::addset(signal_set, kSIGPIPE);
            sig::addset(signal_set, kSIGCHLD);
            sig::addset(signal_set, kSIGTERM);
            sig::addset(signal_set, kSIGINTR);
            sig::addset(signal_set, kSIGHGUP);
        }
        /* initialize timespec */ {
            timeout.tv_sec  = 1L;
            timeout.tv_nsec = 0L;
        }
        return sig::timedwait(signal_set, timeout);
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
    return init();
}

int Application::init()
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
        return loop();
    }
    return help();
}

int Application::loop()
{
    std::thread thread;

    /* initialize signals */ {
        sig::init();
    }
    /* start thread */ {
        std::thread(&Application::run, this).swap(thread);
    }
    /* main loop */ {
        while(running()) {
            const int signum = sig::wait();
            if(signum == -1) {
                onTimeout();
            }
            else switch(signum) {
                case sig::kSIGALRM:
                    onSigALRM();
                    break;
                case sig::kSIGUSR1:
                    onSigUSR1();
                    break;
                case sig::kSIGUSR2:
                    onSigUSR2();
                    break;
                case sig::kSIGPIPE:
                    onSigPIPE();
                    break;
                case sig::kSIGCHLD:
                    onSigCHLD();
                    break;
                case sig::kSIGTERM:
                    onSigTERM();
                    break;
                case sig::kSIGINTR:
                    onSigINTR();
                    break;
                case sig::kSIGHGUP:
                    onSigHGUP();
                    break;
                default:
                    break;
            }
        }
    }
    /* join thread */ {
        thread.join();
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

void Application::run()
{
    if(_lpLaunchpadApp) {
        try {
            _lpLaunchpadApp->main();
            _lpLaunchpadApp->shutdown();
        }
        catch(...) {
            _lpLaunchpadApp->shutdown();
        }
    }
    static_cast<void>(::kill(::getpid(), SIGTERM));
}

void Application::stop()
{
    if(_lpLaunchpadApp->running()) {
        _lpLaunchpadApp->shutdown();
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

void Application::onSigALRM()
{
}

void Application::onSigUSR1()
{
}

void Application::onSigUSR2()
{
}

void Application::onSigPIPE()
{
}

void Application::onSigCHLD()
{
}

void Application::onSigTERM()
{
    stop();
}

void Application::onSigINTR()
{
    stop();
}

void Application::onSigHGUP()
{
    stop();
}

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------
