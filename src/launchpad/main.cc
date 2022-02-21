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
// thr::autojoin
// ---------------------------------------------------------------------------

namespace thr {

class autojoin
{
public:
    autojoin(std::thread& thread)
        : _thread(thread)
    {
    }

   ~autojoin()
    {
        if(_thread.joinable()) {
            _thread.join();
        }
    }

private:
    std::thread& _thread;
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

    static void kill()
    {
        const int rc = ::kill(::getpid(), kSIGTERM);
        if(rc != 0) {
            throw std::runtime_error("kill has failed");
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
// <anonymous>::thread_loop
// ---------------------------------------------------------------------------

namespace {

void thread_loop(const Console* console, Application* application, int* return_value)
{
    int& status(*return_value);

    if(application != nullptr) {
        try {
            status = application->main();
        }
        catch(const std::exception& e) {
            const char* const what(e.what());
            console->errorStream << what << std::endl;
            status = EXIT_FAILURE;
        }
        catch(...) {
            const char* const what("an error has occured");
            console->errorStream << what << std::endl;
            status = EXIT_FAILURE;
        }
    }
    if(application != nullptr) {
        application->stop();
    }
    sig::kill();
}

}

// ---------------------------------------------------------------------------
// <anonymous>::loop
// ---------------------------------------------------------------------------

namespace {

int loop(const ArgList& arglist, const Console& console)
{
    int status = EXIT_SUCCESS;

    try {
        sig::init();
        std::unique_ptr<Application> app(new Application(arglist, console));
        std::thread thread(&thread_loop, &console, app.get(), &status);
        thr::autojoin thread_autojoin(thread);
        /* main loop */ {
            while(app->running()) {
                const int signum = sig::wait();
                if(signum == -1) {
                    app->onTimeout();
                }
                else switch(signum) {
                    case sig::kSIGALRM:
                        app->onSIGALRM();
                        break;
                    case sig::kSIGUSR1:
                        app->onSIGUSR1();
                        break;
                    case sig::kSIGUSR2:
                        app->onSIGUSR2();
                        break;
                    case sig::kSIGPIPE:
                        app->onSIGPIPE();
                        break;
                    case sig::kSIGCHLD:
                        app->onSIGCHLD();
                        break;
                    case sig::kSIGTERM:
                        app->onSIGTERM();
                        break;
                    case sig::kSIGINTR:
                        app->onSIGINTR();
                        break;
                    case sig::kSIGHGUP:
                        app->onSIGHGUP();
                        break;
                    default:
                        break;
                }
            }
        }
    }
    catch(const std::exception& e) {
        const char* const what(e.what());
        console.errorStream << what << std::endl;
        status = EXIT_FAILURE;
    }
    catch(...) {
        const char* const what("an error has occured");
        console.errorStream << what << std::endl;
        status = EXIT_FAILURE;
    }
    return status;
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
