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

LAUNCHPAD_xCPPFLAGS = -I$(top_srcdir)/lib -I$(top_srcdir)/src
LAUNCHPAD_xLDFLAGS = -L$(top_builddir)/lib -L$(top_builddir)/src
LAUNCHPAD_xLDADD = 

NOVATION_xCPPFLAGS = 
NOVATION_xLDFLAGS = 
NOVATION_xLDADD = $(top_builddir)/lib/novation/libnovation.la

RTMIDI_xCPPFLAGS = $(rtmidi_CFLAGS)
RTMIDI_xLDFLAGS = $(rtmidi_LDFLAGS)
RTMIDI_xLDADD = $(rtmidi_LIBS)

BASE_xCPPFLAGS = 
BASE_xLDFLAGS = 
BASE_xLDADD = $(top_builddir)/lib/base/libbase.la

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
	Font8x8.cc \
	Font8x8.h \
	LaunchpadApp.cc \
	LaunchpadApp.h \
	Application.cc \
	Application.h \
	main.cc \
	$(NULL)

launchpad_CPPFLAGS = \
	$(LAUNCHPAD_xCPPFLAGS) \
	$(NOVATION_xCPPFLAGS) \
	$(RTMIDI_xCPPFLAGS) \
	$(BASE_xCPPFLAGS) \
	$(NULL)

launchpad_LDFLAGS = \
	$(LAUNCHPAD_xLDFLAGS) \
	$(NOVATION_xLDFLAGS) \
	$(RTMIDI_xLDFLAGS) \
	$(BASE_xLDFLAGS) \
	$(NULL)

launchpad_LDADD = \
	$(LAUNCHPAD_xLDADD) \
	$(NOVATION_xLDADD) \
	$(RTMIDI_xLDADD) \
	$(BASE_xLDADD) \
	$(NULL)

# ----------------------------------------------------------------------------
# EXTRA_DIST
# ----------------------------------------------------------------------------

EXTRA_DIST = \
	$(NULL)

# ----------------------------------------------------------------------------
# End-Of-File
# ----------------------------------------------------------------------------
