#
# Makefile.am - Copyright (c) 2001-2022 - Olivier Poncet
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
# variables
# ----------------------------------------------------------------------------

NOVATION_xCPPFLAGS = -I$(top_srcdir)/lib -I$(top_srcdir)/src
NOVATION_xLDFLAGS = -L$(top_builddir)/lib -L$(top_builddir)/src
NOVATION_xLDADD = $(top_builddir)/lib/novation/libnovation.la

RTMIDI_xCPPFLAGS = $(rtmidi_CFLAGS)
RTMIDI_xLDFLAGS = $(rtmidi_LDFLAGS)
RTMIDI_xLDADD = $(rtmidi_LIBS)

# ----------------------------------------------------------------------------
# bin_PROGRAMS
# ----------------------------------------------------------------------------

bin_PROGRAMS = \
	launchpad \
	$(NULL)

# ----------------------------------------------------------------------------
# launchpad
# ----------------------------------------------------------------------------

launchpad_SOURCES = \
	ArgList.cc \
	ArgList.h \
	Console.cc \
	Console.h \
	Font8x8.cc \
	Font8x8.h \
	LaunchpadApp.cc \
	LaunchpadApp.h \
	Application.cc \
	Application.h \
	main.cc \
	$(NULL)

launchpad_CPPFLAGS = \
	$(NOVATION_xCPPFLAGS) \
	$(RTMIDI_xCPPFLAGS) \
	$(NULL)

launchpad_LDFLAGS = \
	$(NOVATION_xLDFLAGS) \
	$(RTMIDI_xLDFLAGS) \
	$(NULL)

launchpad_LDADD = \
	$(NOVATION_xLDADD) \
	$(RTMIDI_xLDADD) \
	$(NULL)

# ----------------------------------------------------------------------------
# EXTRA_DIST
# ----------------------------------------------------------------------------

EXTRA_DIST = \
	$(NULL)

# ----------------------------------------------------------------------------
# End-Of-File
# ----------------------------------------------------------------------------
