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


#ifndef ANY_RTTTL_H
#define ANY_RTTTL_H

#define ANY_RTTTL_VERSION 2.0

#include <Arduino.h>
#include "pitches.h"

//#define ANY_RTTTL_DEBUG
//#define ANY_RTTTL_INFO

namespace anyrtttl
{

	/****************************************************************************
	* Custom functions
	****************************************************************************/


	/****************************************************************************
	* Description:
	*   Defines a function pointer to a tone() function
	****************************************************************************/
	typedef void (*ToneFuncPtr)(byte, uint16_t, uint32_t);

	/****************************************************************************
	* Description:
	*   Defines a function pointer to a noTone() function
	****************************************************************************/
	typedef void (*NoToneFuncPtr)(byte);

	/****************************************************************************
	* Description:
	*   Defines a function pointer to a delay() function
	****************************************************************************/
	typedef void (*DelayFuncPtr)(uint32_t);

	/****************************************************************************
	* Description:
	*   Defines a function pointer to a millis() function
	****************************************************************************/
	typedef uint32_t (*MillisFuncPtr)(void);

	/****************************************************************************
	* Description:
	*   Defines the tone() function used by AnyRtttl.
	* Parameters:
	*   iFunc: Pointer to a tone() replacement function.
	****************************************************************************/
	void setToneFunction(ToneFuncPtr iFunc);

	/****************************************************************************
	* Description:
	*   Defines the noTone() function used by AnyRtttl.
	* Parameters:
	*   iFunc: Pointer to a noTone() replacement function.
	****************************************************************************/
	void setNoToneFunction(NoToneFuncPtr iFunc);

	/****************************************************************************
	* Description:
	*   Defines the delay() function used by AnyRtttl.
	* Parameters:
	*   iFunc: Pointer to a delay() replacement function.
	****************************************************************************/
	void setDelayFunction(DelayFuncPtr iFunc);

	/****************************************************************************
	* Description:
	*   Defines the millis() function used by AnyRtttl.
	* Parameters:
	*   iFunc: Pointer to a millis() replacement function.
	****************************************************************************/
	void setMillisFunction(MillisFuncPtr iFunc);




	/****************************************************************************
	* Blocking API
	****************************************************************************/
	namespace blocking
	{

		/****************************************************************************
		* Description:
		*   Plays a native RTTTL melody.
		* Parameters:
		*   iPin:    The pin which is connected to the piezo buffer.
		*   iBuffer: The string buffer of the RTTTL melody.
		****************************************************************************/
		void play(byte iPin, const char * iBuffer);

		/****************************************************************************
		* Description:
		*   Plays a native RTTTL melody which is stored in Program Memory (PROGMEM).
		* Parameters:
		*   iPin:    The pin which is connected to the piezo buffer.
		*   iBuffer: The string buffer of the RTTTL melody.
		****************************************************************************/
		void playProgMem(byte iPin, const char * iBuffer);

		/****************************************************************************
		* Description:
		*   Plays a RTTTL melody which is encoded as 16 bits per notes.
		* Parameters:
		*   iPin:      The pin which is connected to the piezo buffer.
		*   iBuffer:   The binary buffer of the RTTTL melody. See remarks for details.
		*   iNumNotes: The number of notes within the given melody buffer.
		* Remarks:
		*   The first 16 bits of the buffer are reserved for the default section.
		*   See the definition of RTTTL_DEFAULT_VALUE_SECTION union for details.
		*   Each successive notes are encoded as 16 bits per note as defined by
		*   RTTTL_NOTE union.
		****************************************************************************/
		void play16Bits(int iPin, const unsigned char * iBuffer, int iNumNotes);

		/****************************************************************************
		* Description:
		*   Defines a function pointer which is used by the play10Bits() function as
		*   a bit provider function. The signature of a compatible function must be
		*   the following: uint16_t foo(uint8_t iNumBits);
		****************************************************************************/
		typedef uint16_t (*BitReadFuncPtr)(uint8_t);

		/****************************************************************************
		* Description:
		*   Plays a RTTTL melody which is encoded as 10 bits per notes.
		* Parameters:
		*   iPin:      The pin which is connected to the piezo buffer.
		*   iNumNotes: The number of notes within the given melody.
		*   iFuncPtr:  Pointer to a function which is used by play10Bits() as a bit  The binary buffer of the RTTTL melody. See remarks for details.
		* Remarks:
		*   The first 16 bits of the buffer are reserved for the default section.
		*   See the definition of RTTTL_DEFAULT_VALUE_SECTION union for details.
		*   Each successive notes are encoded as 10 bits per note as defined by
		*   RTTTL_NOTE union.
		****************************************************************************/
		void play10Bits(int iPin, int iNumNotes, BitReadFuncPtr iFuncPtr);

	}; //blocking namespace



	/****************************************************************************
	* Non-blocking API
	****************************************************************************/
	namespace nonblocking
	{

		/****************************************************************************
		* Description:
		*   Setups the AnyRtttl library for non-blocking mode and ready to
		*   decode a new RTTTL song.
		* Parameters:
		*   iPin:    The pin which is connected to the piezo buffer.
		*   iBuffer: The string buffer of the RTTTL song.
		****************************************************************************/
		void begin(byte iPin, const char * iBuffer);

		/****************************************************************************
		* Description:
		*   Automatically plays a new note when required.
		*   This function must constantly be called within the loop() function.
		*   Warning: inserting too long delays within the loop function may
		*   disrupt the NON-BLOCKING RTTTL library from playing properly.
		****************************************************************************/
		void play();

		/****************************************************************************
		* Description:
		*   Stops playing the current song.
		****************************************************************************/
		void stop();

		/****************************************************************************
		* Description:
		*   Return true when the library is playing the given RTTTL melody.
		****************************************************************************/
		bool isPlaying();

		/****************************************************************************
		* Description:
		*   Return true when the library is done playing the given RTTTL song.
		****************************************************************************/
		bool done();

	}; //nonblocking namespace

}; //anyrtttl namespace

#endif //ANY_RTTTL_H
