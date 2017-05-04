// ---------------------------------------------------------------------------
// AnyRtttl Library - v2.1 - 06/05/2016
// Copyright (C) 2016 Antoine Beauchamp
// The code & updates for the library can be found on http://end2endzone.com
//
// AUTHOR/LICENSE:
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 3.0 of the License, or (at your option) any later version.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License (LGPL-3.0) for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
//
// DISCLAIMER:
//  This software is furnished "as is", without technical support, and with no
//  warranty, express or implied, as to its usefulness for any purpose.
//
// PURPOSE:
//  After publishing my NonBlockingRtttl arduino library, I started using the
//  library in more complex projects which was requiring other libraries.
//  I quickly ran into the hell of library dependencies and library conflicts.
//  I realized that I needed more features that could help me prototype faster.
//
//  Other libraries available which allows you to play a melody in RTTTL format
//  suffer the same issue: they are based on blocking APIs or the RTTTL data is
//  not optimized for space.
//
//  AnyRtttl is different since it packs multiple RTTTL related features in a
//  single library. It supports blocking & non-blocking API which makes it
//  suitable to be used by more advanced algorithm. For instance, when using
//  the non-blocking API, the melody can be stopped when a button is pressed.
//  The library is also compatible with external Tone libraries and supports
//  highly compressed RTTTL binary formats.
//
// USAGE:
//  The library has multiple examples which shows how to use the library with
//  each features. Refers the to examples details. More information is also
//  available on the project's home page: http://end2endzone.com.
//
//  Define ANY_RTTTL_INFO to enable the debugging of the library state
//  on the serial port.
//
//  Use ANY_RTTTL_VERSION to get the current version of the library.
//
// HISTORY:
// 03/19/2016 v1.0 - Initial release of NonBlockingRtttl.
// 05/21/2016 v2.0 - Library converted to AnyRtttl.
// 06/05/2016 v2.1 - Implemented support for RTTTL in Program Memory (PROGMEM).
//
// ---------------------------------------------------------------------------

#include "binrtttl.h"

namespace anyrtttl
{

	static const RTTTL_NOTE_LETTER gNoteLetters[] =   {'c','d','e','f','g','a','b','p'};
	static const uint16_t gNoteLettersCount = sizeof(gNoteLetters)/sizeof(gNoteLetters[0]);

	static const int gNoteOffsets[] = { 1, 3, 5, 6, 8, 10, 12, 0};

	static const RTTTL_DURATION gNoteDurations[] = {1, 2, 4, 8, 16, 32};
	static const uint16_t gNoteDurationsCount = sizeof(gNoteDurations)/sizeof(gNoteDurations[0]);

	static const RTTTL_OCTAVE_VALUE gNoteOctaves[] = {4, 5, 6, 7};
	static const uint16_t gNoteOctavesCount = sizeof(gNoteOctaves)/sizeof(gNoteOctaves[0]);

	static const RTTTL_BPM gNoteBpms[] = {25, 28, 31, 35, 40, 45, 50, 56, 63, 70, 80, 90, 100, 112, 125, 140, 160, 180, 200, 225, 250, 285, 320, 355, 400, 450, 500, 565, 635, 715, 800, 900};
	static const uint16_t gNoteBpmsCount = sizeof(gNoteBpms)/sizeof(gNoteBpms[0]);

	RTTTL_NOTE_LETTER getNoteLetterFromIndex(NOTE_LETTER_INDEX iIndex)
	{
		if (iIndex >= 0 && iIndex < gNoteLettersCount)
		return gNoteLetters[iIndex];
		return -1;
	}

	uint16_t getNoteLettersCount()
	{
		return gNoteLettersCount;
	}

	NOTE_LETTER_INDEX findNoteLetterIndex(RTTTL_NOTE_LETTER n)
	{
		for(NOTE_LETTER_INDEX i=0; i<gNoteLettersCount; i++)
		{
			if (getNoteLetterFromIndex(i) == n)
			{
				return i;
			}
		}
		return -1;
	}

	int getNoteOffsetFromLetterIndex(NOTE_LETTER_INDEX iIndex)
	{
		if (iIndex >= 0 && iIndex < gNoteLettersCount)
		return gNoteOffsets[iIndex];
		return 0;
	}

	int getNoteOffsetFromLetter(RTTTL_NOTE_LETTER n)
	{
		NOTE_LETTER_INDEX index = findNoteLetterIndex(n);
		return getNoteOffsetFromLetterIndex(index);
	}

	RTTTL_DURATION getNoteDurationFromIndex(DURATION_INDEX iIndex)
	{
		if (iIndex >= 0 && iIndex < gNoteDurationsCount)
		return gNoteDurations[iIndex];
		return -1;
	}

	uint16_t getNoteDurationsCount()
	{
		return gNoteDurationsCount;
	}

	DURATION_INDEX findNoteDurationIndex(RTTTL_DURATION n)
	{
		for(DURATION_INDEX i=0; i<gNoteDurationsCount; i++)
		{
			if (getNoteDurationFromIndex(i) == n)
			{
				return i;
			}
		}
		return -1;
	}

	RTTTL_OCTAVE_VALUE getNoteOctaveFromIndex(OCTAVE_INDEX iIndex)
	{
		if (iIndex >= 0 && iIndex < gNoteOctavesCount)
		return gNoteOctaves[iIndex];
		return -1;
	}

	uint16_t getNoteOctavesCount()
	{
		return gNoteOctavesCount;
	}

	OCTAVE_INDEX findNoteOctaveIndex(RTTTL_OCTAVE_VALUE n)
	{
		for(OCTAVE_INDEX i=0; i<gNoteOctavesCount; i++)
		{
			if (getNoteOctaveFromIndex(i) == n)
			{
				return i;
			}
		}
		return -1;
	}

	RTTTL_BPM getBpmFromIndex(BPM_INDEX iIndex)
	{
		if (iIndex >= 0 && iIndex < gNoteBpmsCount)
		return gNoteBpms[iIndex];
		return -1;
	}

	uint16_t getBpmsCount()
	{
		return gNoteBpmsCount;
	}

	BPM_INDEX findBpmIndex(RTTTL_BPM n)
	{
		for(BPM_INDEX i=0; i<gNoteBpmsCount; i++)
		{
			if (getBpmFromIndex(i) == n)
			{
				return i;
			}
		}
		return INVALID_BPM_INDEX;
	}

}; //anyrtttl namespace
