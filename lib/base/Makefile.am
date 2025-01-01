#
# Makefile.am - Copyright (c) 2001-2025 - Olivier Poncet
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
# globals
# ----------------------------------------------------------------------------

BASE_xCPPFLAGS = -I$(top_srcdir)/lib -I$(top_srcdir)/src
BASE_xLDFLAGS = -L$(top_builddir)/lib -L$(top_builddir)/src
BASE_xLIBADD = -lpthread

# ----------------------------------------------------------------------------
# noinst_LTLIBRARIES
# ----------------------------------------------------------------------------

noinst_LTLIBRARIES = \
	libbase.la \
	$(NULL)

# ----------------------------------------------------------------------------
# libbase
# ----------------------------------------------------------------------------

libbase_la_SOURCES = \
	ArgList.cc \
	ArgList.h \
	Console.cc \
	Console.h \
	Program.cc \
	Program.h \
	$(NULL)

libbase_la_CPPFLAGS = \
	$(BASE_xCPPFLAGS) \
	$(NULL)

libbase_la_LDFLAGS = \
	$(BASE_xLDFLAGS) \
	$(NULL)

libbase_la_LIBADD = \
	$(BASE_xLIBADD) \
	$(NULL)

# ----------------------------------------------------------------------------
# End-Of-File
# ----------------------------------------------------------------------------
