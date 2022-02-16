/*
 * Font8x8.cc - Copyright (c) 2001-2022 - Olivier Poncet
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
#include "Font8x8.h"

// ---------------------------------------------------------------------------
// Font8x8
// ---------------------------------------------------------------------------

const uint8_t Font8x8::data[MAX_CHARS][MAX_BYTES] = {
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, // NUL
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, // SOH
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, // STX
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, // ETX
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, // EOT
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, // ENQ
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, // ACK
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, // BEL
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, // BS
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, // HT
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, // LF
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, // VT
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, // FF
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, // CR
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, // SO
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, // SI
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, // DLE
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, // DC1
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, // DC2
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, // DC3
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, // DC4
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, // NAK
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, // SYN
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, // ETB
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, // CAN
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, // EM
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, // SUB
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, // ESC
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, // FS
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, // GS
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, // RS
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, // US
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, // SPC
    { 0x18, 0x3c, 0x3c, 0x18, 0x18, 0x00, 0x18, 0x00 }, // (!)
    { 0x36, 0x36, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, // (")
    { 0x36, 0x36, 0x7f, 0x36, 0x7f, 0x36, 0x36, 0x00 }, // (#)
    { 0x0c, 0x3e, 0x03, 0x1e, 0x30, 0x1f, 0x0c, 0x00 }, // ($)
    { 0x00, 0x63, 0x33, 0x18, 0x0c, 0x66, 0x63, 0x00 }, // (%)
    { 0x1c, 0x36, 0x1c, 0x6e, 0x3b, 0x33, 0x6e, 0x00 }, // (&)
    { 0x06, 0x06, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00 }, // (')
    { 0x18, 0x0c, 0x06, 0x06, 0x06, 0x0c, 0x18, 0x00 }, // (()
    { 0x06, 0x0c, 0x18, 0x18, 0x18, 0x0c, 0x06, 0x00 }, // ())
    { 0x00, 0x66, 0x3c, 0xff, 0x3c, 0x66, 0x00, 0x00 }, // (*)
    { 0x00, 0x0c, 0x0c, 0x3f, 0x0c, 0x0c, 0x00, 0x00 }, // (+)
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x0c, 0x06 }, // (,)
    { 0x00, 0x00, 0x00, 0x3f, 0x00, 0x00, 0x00, 0x00 }, // (-)
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x0c, 0x00 }, // (.)
    { 0x60, 0x30, 0x18, 0x0c, 0x06, 0x03, 0x01, 0x00 }, // (/)
    { 0x3e, 0x63, 0x73, 0x7b, 0x6f, 0x67, 0x3e, 0x00 }, // (0)
    { 0x0c, 0x0e, 0x0c, 0x0c, 0x0c, 0x0c, 0x3f, 0x00 }, // (1)
    { 0x1e, 0x33, 0x30, 0x1c, 0x06, 0x33, 0x3f, 0x00 }, // (2)
    { 0x1e, 0x33, 0x30, 0x1c, 0x30, 0x33, 0x1e, 0x00 }, // (3)
    { 0x38, 0x3c, 0x36, 0x33, 0x7f, 0x30, 0x78, 0x00 }, // (4)
    { 0x3f, 0x03, 0x1f, 0x30, 0x30, 0x33, 0x1e, 0x00 }, // (5)
    { 0x1c, 0x06, 0x03, 0x1f, 0x33, 0x33, 0x1e, 0x00 }, // (6)
    { 0x3f, 0x33, 0x30, 0x18, 0x0c, 0x0c, 0x0c, 0x00 }, // (7)
    { 0x1e, 0x33, 0x33, 0x1e, 0x33, 0x33, 0x1e, 0x00 }, // (8)
    { 0x1e, 0x33, 0x33, 0x3e, 0x30, 0x18, 0x0e, 0x00 }, // (9)
    { 0x00, 0x0c, 0x0c, 0x00, 0x00, 0x0c, 0x0c, 0x00 }, // (:)
    { 0x00, 0x0c, 0x0c, 0x00, 0x00, 0x0c, 0x0c, 0x06 }, // (;)
    { 0x18, 0x0c, 0x06, 0x03, 0x06, 0x0c, 0x18, 0x00 }, // (<)
    { 0x00, 0x00, 0x3f, 0x00, 0x00, 0x3f, 0x00, 0x00 }, // (=)
    { 0x06, 0x0c, 0x18, 0x30, 0x18, 0x0c, 0x06, 0x00 }, // (>)
    { 0x1e, 0x33, 0x30, 0x18, 0x0c, 0x00, 0x0c, 0x00 }, // (?)
    { 0x3e, 0x63, 0x7b, 0x7b, 0x7b, 0x03, 0x1e, 0x00 }, // (@)
    { 0x0c, 0x1e, 0x33, 0x33, 0x3f, 0x33, 0x33, 0x00 }, // (A)
    { 0x3f, 0x66, 0x66, 0x3e, 0x66, 0x66, 0x3f, 0x00 }, // (B)
    { 0x3c, 0x66, 0x03, 0x03, 0x03, 0x66, 0x3c, 0x00 }, // (C)
    { 0x1f, 0x36, 0x66, 0x66, 0x66, 0x36, 0x1f, 0x00 }, // (D)
    { 0x7f, 0x46, 0x16, 0x1e, 0x16, 0x46, 0x7f, 0x00 }, // (E)
    { 0x7f, 0x46, 0x16, 0x1e, 0x16, 0x06, 0x0f, 0x00 }, // (F)
    { 0x3c, 0x66, 0x03, 0x03, 0x73, 0x66, 0x7c, 0x00 }, // (G)
    { 0x33, 0x33, 0x33, 0x3f, 0x33, 0x33, 0x33, 0x00 }, // (H)
    { 0x1e, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x1e, 0x00 }, // (I)
    { 0x78, 0x30, 0x30, 0x30, 0x33, 0x33, 0x1e, 0x00 }, // (J)
    { 0x67, 0x66, 0x36, 0x1e, 0x36, 0x66, 0x67, 0x00 }, // (K)
    { 0x0f, 0x06, 0x06, 0x06, 0x46, 0x66, 0x7f, 0x00 }, // (L)
    { 0x63, 0x77, 0x7f, 0x7f, 0x6b, 0x63, 0x63, 0x00 }, // (M)
    { 0x63, 0x67, 0x6f, 0x7b, 0x73, 0x63, 0x63, 0x00 }, // (N)
    { 0x1c, 0x36, 0x63, 0x63, 0x63, 0x36, 0x1c, 0x00 }, // (O)
    { 0x3f, 0x66, 0x66, 0x3e, 0x06, 0x06, 0x0f, 0x00 }, // (P)
    { 0x1e, 0x33, 0x33, 0x33, 0x3b, 0x1e, 0x38, 0x00 }, // (Q)
    { 0x3f, 0x66, 0x66, 0x3e, 0x36, 0x66, 0x67, 0x00 }, // (R)
    { 0x1e, 0x33, 0x07, 0x0e, 0x38, 0x33, 0x1e, 0x00 }, // (S)
    { 0x3f, 0x2d, 0x0c, 0x0c, 0x0c, 0x0c, 0x1e, 0x00 }, // (T)
    { 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x3f, 0x00 }, // (U)
    { 0x33, 0x33, 0x33, 0x33, 0x33, 0x1e, 0x0c, 0x00 }, // (V)
    { 0x63, 0x63, 0x63, 0x6b, 0x7f, 0x77, 0x63, 0x00 }, // (W)
    { 0x63, 0x63, 0x36, 0x1c, 0x1c, 0x36, 0x63, 0x00 }, // (X)
    { 0x33, 0x33, 0x33, 0x1e, 0x0c, 0x0c, 0x1e, 0x00 }, // (Y)
    { 0x7f, 0x63, 0x31, 0x18, 0x4c, 0x66, 0x7f, 0x00 }, // (Z)
    { 0x1e, 0x06, 0x06, 0x06, 0x06, 0x06, 0x1e, 0x00 }, // ([)
    { 0x03, 0x06, 0x0c, 0x18, 0x30, 0x60, 0x40, 0x00 }, // (\)
    { 0x1e, 0x18, 0x18, 0x18, 0x18, 0x18, 0x1e, 0x00 }, // (])
    { 0x08, 0x1c, 0x36, 0x63, 0x00, 0x00, 0x00, 0x00 }, // (^)
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff }, // (_)
    { 0x0c, 0x0c, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00 }, // (`)
    { 0x00, 0x00, 0x1e, 0x30, 0x3e, 0x33, 0x6e, 0x00 }, // (a)
    { 0x07, 0x06, 0x06, 0x3e, 0x66, 0x66, 0x3b, 0x00 }, // (b)
    { 0x00, 0x00, 0x1e, 0x33, 0x03, 0x33, 0x1e, 0x00 }, // (c)
    { 0x38, 0x30, 0x30, 0x3e, 0x33, 0x33, 0x6e, 0x00 }, // (d)
    { 0x00, 0x00, 0x1e, 0x33, 0x3f, 0x03, 0x1e, 0x00 }, // (e)
    { 0x1c, 0x36, 0x06, 0x0f, 0x06, 0x06, 0x0f, 0x00 }, // (f)
    { 0x00, 0x00, 0x6e, 0x33, 0x33, 0x3e, 0x30, 0x1f }, // (g)
    { 0x07, 0x06, 0x36, 0x6e, 0x66, 0x66, 0x67, 0x00 }, // (h)
    { 0x0c, 0x00, 0x0e, 0x0c, 0x0c, 0x0c, 0x1e, 0x00 }, // (i)
    { 0x30, 0x00, 0x30, 0x30, 0x30, 0x33, 0x33, 0x1e }, // (j)
    { 0x07, 0x06, 0x66, 0x36, 0x1e, 0x36, 0x67, 0x00 }, // (k)
    { 0x0e, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x1e, 0x00 }, // (l)
    { 0x00, 0x00, 0x33, 0x7f, 0x7f, 0x6b, 0x63, 0x00 }, // (m)
    { 0x00, 0x00, 0x1f, 0x33, 0x33, 0x33, 0x33, 0x00 }, // (n)
    { 0x00, 0x00, 0x1e, 0x33, 0x33, 0x33, 0x1e, 0x00 }, // (o)
    { 0x00, 0x00, 0x3b, 0x66, 0x66, 0x3e, 0x06, 0x0f }, // (p)
    { 0x00, 0x00, 0x6e, 0x33, 0x33, 0x3e, 0x30, 0x78 }, // (q)
    { 0x00, 0x00, 0x3b, 0x6e, 0x66, 0x06, 0x0f, 0x00 }, // (r)
    { 0x00, 0x00, 0x3e, 0x03, 0x1e, 0x30, 0x1f, 0x00 }, // (s)
    { 0x08, 0x0c, 0x3e, 0x0c, 0x0c, 0x2c, 0x18, 0x00 }, // (t)
    { 0x00, 0x00, 0x33, 0x33, 0x33, 0x33, 0x6e, 0x00 }, // (u)
    { 0x00, 0x00, 0x33, 0x33, 0x33, 0x1e, 0x0c, 0x00 }, // (v)
    { 0x00, 0x00, 0x63, 0x6b, 0x7f, 0x7f, 0x36, 0x00 }, // (w)
    { 0x00, 0x00, 0x63, 0x36, 0x1c, 0x36, 0x63, 0x00 }, // (x)
    { 0x00, 0x00, 0x33, 0x33, 0x33, 0x3e, 0x30, 0x1f }, // (y)
    { 0x00, 0x00, 0x3f, 0x19, 0x0c, 0x26, 0x3f, 0x00 }, // (z)
    { 0x38, 0x0c, 0x0c, 0x07, 0x0c, 0x0c, 0x38, 0x00 }, // ({)
    { 0x18, 0x18, 0x18, 0x00, 0x18, 0x18, 0x18, 0x00 }, // (|)
    { 0x07, 0x0c, 0x0c, 0x38, 0x0c, 0x0c, 0x07, 0x00 }, // (})
    { 0x6e, 0x3b, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, // (~)
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }  // DEL
};

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------
