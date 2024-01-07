#
# acinclude.m4 - Copyright (c) 2001-2024 - Olivier Poncet
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>
#

# ----------------------------------------------------------------------------
# AX_CHECK_C99
# ----------------------------------------------------------------------------

AC_DEFUN([AX_CHECK_C99], [
AC_PROG_CC
])dnl AX_CHECK_C99

# ----------------------------------------------------------------------------
# AX_CHECK_CXX11
# ----------------------------------------------------------------------------

AC_DEFUN([AX_CHECK_CXX11], [
AX_CXX_COMPILE_STDCXX([11], [noext], [mandatory])
])dnl AX_CHECK_CXX11

# ----------------------------------------------------------------------------
# AX_CHECK_CXX14
# ----------------------------------------------------------------------------

AC_DEFUN([AX_CHECK_CXX14], [
AX_CXX_COMPILE_STDCXX([14], [noext], [mandatory])
])dnl AX_CHECK_CXX14

# ----------------------------------------------------------------------------
# AX_CHECK_CXX17
# ----------------------------------------------------------------------------

AC_DEFUN([AX_CHECK_CXX17], [
AX_CXX_COMPILE_STDCXX([17], [noext], [mandatory])
])dnl AX_CHECK_CXX17

# ----------------------------------------------------------------------------
# AX_RECURSIVE_EVAL
# ----------------------------------------------------------------------------

AC_DEFUN([AX_RECURSIVE_EVAL], [
_lcl_receval="$1"
$2=`(test "x$prefix" = xNONE && prefix="$ac_default_prefix"
     test "x$exec_prefix" = xNONE && exec_prefix="${prefix}"
     _lcl_receval_old=''
     while test "[$]_lcl_receval_old" != "[$]_lcl_receval"; do
       _lcl_receval_old="[$]_lcl_receval"
       eval _lcl_receval="\"[$]_lcl_receval\""
     done
     echo "[$]_lcl_receval")`
])dnl AX_RECURSIVE_EVAL

# ----------------------------------------------------------------------------
# AX_CHECK_BYTE_ORDER
# ----------------------------------------------------------------------------

AC_DEFUN([AX_CHECK_BYTE_ORDER], [
AC_C_BIGENDIAN([
AC_DEFINE([MSB_FIRST], [1], [Define to 1 if the host byte-ordering is MSB first.])
], [
AC_DEFINE([LSB_FIRST], [1], [Define to 1 if the host byte-ordering is LSB first.])
], [
AC_MSG_ERROR([unable to determine byte-order])
])
])dnl AX_CHECK_BYTE_ORDER

# ----------------------------------------------------------------------------
# AX_CHECK_HEADERS
# ----------------------------------------------------------------------------

AC_DEFUN([AX_CHECK_HEADERS], [
])dnl AX_CHECK_HEADERS

# ----------------------------------------------------------------------------
# AX_CHECK_PTHREAD
# ----------------------------------------------------------------------------

AC_DEFUN([AX_CHECK_PTHREAD], [
AX_PTHREAD([], [AC_MSG_ERROR([pthread was not found])])
])dnl AX_CHECK_PTHREAD

# ----------------------------------------------------------------------------
# AX_CHECK_RTMIDI
# ----------------------------------------------------------------------------

AC_DEFUN([AX_CHECK_RTMIDI], [
PKG_CHECK_MODULES([rtmidi], [rtmidi >= 3.0.0], [], [AC_MSG_ERROR([rtmidi was not found])])
])dnl AX_CHECK_RTMIDI

# ----------------------------------------------------------------------------
# End-Of-File
# ----------------------------------------------------------------------------
