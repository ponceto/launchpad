/*
 * Program.h - Copyright (c) 2001-2022 - Olivier Poncet
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
#ifndef __Program_h__
#define __Program_h__

#include <base/ArgList.h>
#include <base/Console.h>

// ---------------------------------------------------------------------------
// Program
// ---------------------------------------------------------------------------

class Program
{
public: // public interface
    Program ( const ArgList&
            , const Console& );

    virtual ~Program();

    virtual int  main();
    virtual void stop();

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
    const ArgList& _arglist;
    const Console& _console;
};

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------

#endif /* __Program_h__ */
