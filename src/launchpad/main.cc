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
#include <unistd.h>
#include <memory>
#include <string>
#include <vector>
#include <iostream>
#include <stdexcept>
#include <atomic>
#include <thread>
#include <mutex>
#include <future>
#include "Application.h"

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

    static int init()
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
        return 0;
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
// <anonymous>::AutoJoin
// ---------------------------------------------------------------------------

namespace {

class AutoJoin
{
public: // public interface
    AutoJoin(std::thread& thread)
        : _thread(thread)
    {
    }

   ~AutoJoin()
    {
        if(_thread.joinable()) {
            _thread.join();
        }
    }

private: // private data
   std::thread& _thread;
};

}

// ---------------------------------------------------------------------------
// <anonymous>::Main
// ---------------------------------------------------------------------------

namespace {

class Main
{
public: // public interface
    Main(const ArgList& arglist, const Console& console)
        : _siginit(sig::init())
        , _arglist(arglist)
        , _console(console)
        , _application()
        , _thread()
        , _promise()
        , _future(_promise.get_future())
        , _status(EXIT_SUCCESS)
        , _shutdown(false)
    {
    }

   ~Main()
    {
    }

    int main()
    {
        const AutoJoin autojoin(_thread);
        try {
            createApplication();
            thrdStart();
            mainLoop();
            thrdEnd();
        }
        catch(const std::exception& e) {
            failure(e.what());
        }
        catch(...) {
            failure("an error has occured");
        }
        return _status;
    }

protected: // protected interface
    void mainLoop()
    {
        try {
            while(_shutdown == false) {
                const int signum = sig::wait();
                if(signum == -1) {
                    _application->onTimeout();
                }
                else switch(signum) {
                    case sig::kSIGALRM:
                        _application->onSIGALRM();
                        break;
                    case sig::kSIGUSR1:
                        _application->onSIGUSR1();
                        break;
                    case sig::kSIGUSR2:
                        _application->onSIGUSR2();
                        break;
                    case sig::kSIGPIPE:
                        _application->onSIGPIPE();
                        break;
                    case sig::kSIGCHLD:
                        _application->onSIGCHLD();
                        break;
                    case sig::kSIGTERM:
                        _application->onSIGTERM();
                        break;
                    case sig::kSIGINTR:
                        _application->onSIGINTR();
                        break;
                    case sig::kSIGHGUP:
                        _application->onSIGHGUP();
                        break;
                    default:
                        break;
                }
            }
        }
        catch(const std::exception& e) {
            failure(e.what());
        }
        catch(...) {
            failure("an error has occured");
        }
    }

    void createApplication()
    {
        _application = std::make_unique<Application>(_arglist, _console);
    }

    void thrdStart()
    {
        std::thread(&Main::thrdLoop, this).swap(_thread);
    }

    void thrdLoop()
    {
        try {
            _promise.set_value(_application->main());
        }
        catch(const std::exception& e) {
            _promise.set_value(EXIT_FAILURE);
            errorln(e.what());
        }
        catch(...) {
            _promise.set_value(EXIT_FAILURE);
            errorln("an error has occured");
        }
        shutdown();
    }

    void thrdEnd()
    {
        const int status = _future.get();
        if((status != EXIT_SUCCESS) && (_status == EXIT_SUCCESS)) {
            _status = status;
        }
    }

    void shutdown()
    {
        try {
            if(_shutdown == false) {
                _shutdown = true;
                sig::kill();
            }
        }
        catch(const std::exception& e) {
            errorln(e.what());
        }
        catch(...) {
            errorln("an error has occured");
        }
    }

    void failure(const char* message = nullptr)
    {
        if(_status == EXIT_SUCCESS) {
            _status = EXIT_FAILURE;
        }
        errorln(message);
    }

    void println(const char* message)
    {
        if((message != nullptr) && (_console.printStream.good())) {
            _console.printStream << message << std::endl;
        }
    }

    void errorln(const char* message)
    {
        if((message != nullptr) && (_console.errorStream.good())) {
            _console.errorStream << message << std::endl;
        }
    }

private: // private data
    const int            _siginit;
    const ArgList&       _arglist;
    const Console&       _console;
    ApplicationUniquePtr _application;
    std::thread          _thread;
    std::promise<int>    _promise;
    std::future<int>     _future;
    std::atomic_int      _status;
    std::atomic_bool     _shutdown;
};

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

    return Main(arglist, console).main();
}

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------
