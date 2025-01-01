/*
 * Program.h - Copyright (c) 2001-2025 - Olivier Poncet
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
#ifndef __BASE_Program_h__
#define __BASE_Program_h__

#include <base/ArgList.h>
#include <base/Console.h>

// ---------------------------------------------------------------------------
// base::Program
// ---------------------------------------------------------------------------

namespace base {

class Program
{
public: // public interface
    Program ( const ArgList&
            , const Console& );

    virtual ~Program();

    virtual int  main();
    virtual void stop();

    virtual void onTIMEOUT();
    virtual void onSIGALRM();
    virtual void onSIGUSR1();
    virtual void onSIGUSR2();
    virtual void onSIGPIPE();
    virtual void onSIGCHLD();
    virtual void onSIGTERM();
    virtual void onSIGINTR();
    virtual void onSIGHGUP();

protected: // protected data
    const ArgList& _arglist;
    const Console& _console;
};

}

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------

#endif /* __BASE_Program_h__ */
