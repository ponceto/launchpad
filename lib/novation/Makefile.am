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

NOVATION_xCPPFLAGS = -I$(top_srcdir)/lib -I$(top_srcdir)/src
NOVATION_xLDFLAGS = -L$(top_builddir)/lib -L$(top_builddir)/src
NOVATION_xLIBADD = -lpthread

RTMIDI_xCPPFLAGS = $(rtmidi_CFLAGS)
RTMIDI_xLDFLAGS = $(rtmidi_LDFLAGS)
RTMIDI_xLIBADD = $(rtmidi_LIBS)

# ----------------------------------------------------------------------------
# noinst_LTLIBRARIES
# ----------------------------------------------------------------------------

noinst_LTLIBRARIES = \
	libnovation.la \
	$(NULL)

# ----------------------------------------------------------------------------
# libnovation
# ----------------------------------------------------------------------------

libnovation_la_SOURCES = \
	Launchpad.cc \
	Launchpad.h \
	Midi.cc \
	Midi.h \
	Font5x7.cc \
	Font5x7.h \
	Font8x8.cc \
	Font8x8.h \
	$(NULL)

libnovation_la_CPPFLAGS = \
	$(NOVATION_xCPPFLAGS) \
	$(RTMIDI_xCPPFLAGS) \
	$(NULL)

libnovation_la_LDFLAGS = \
	$(NOVATION_xLDFLAGS) \
	$(RTMIDI_xLDFLAGS) \
	$(NULL)

libnovation_la_LIBADD = \
	$(NOVATION_xLIBADD) \
	$(RTMIDI_xLIBADD) \
	$(NULL)

# ----------------------------------------------------------------------------
# End-Of-File
# ----------------------------------------------------------------------------
