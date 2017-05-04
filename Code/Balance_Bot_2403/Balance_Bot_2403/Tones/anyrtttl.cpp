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

#include "arduino.h"
#include "anyrtttl.h"
#include "binrtttl.h"

/*********************************************************
* RTTTL Library data
*********************************************************/

namespace anyrtttl
{

	const uint16_t notes[] = { NOTE_SILENT,
		NOTE_C4, NOTE_CS4, NOTE_D4, NOTE_DS4, NOTE_E4, NOTE_F4, NOTE_FS4, NOTE_G4, NOTE_GS4, NOTE_A4, NOTE_AS4, NOTE_B4,
		NOTE_C5, NOTE_CS5, NOTE_D5, NOTE_DS5, NOTE_E5, NOTE_F5, NOTE_FS5, NOTE_G5, NOTE_GS5, NOTE_A5, NOTE_AS5, NOTE_B5,
		NOTE_C6, NOTE_CS6, NOTE_D6, NOTE_DS6, NOTE_E6, NOTE_F6, NOTE_FS6, NOTE_G6, NOTE_GS6, NOTE_A6, NOTE_AS6, NOTE_B6,
		NOTE_C7, NOTE_CS7, NOTE_D7, NOTE_DS7, NOTE_E7, NOTE_F7, NOTE_FS7, NOTE_G7, NOTE_GS7, NOTE_A7, NOTE_AS7, NOTE_B7
	};

	#define isdigit(n) (n >= '0' && n <= '9')
	typedef uint16_t TONE_DURATION;
	static const byte NOTES_PER_OCTAVE = 12;

	const char * buffer = "";
	int bufferIndex = -32760;
	byte default_dur = 4;
	byte default_oct = 5;
	RTTTL_BPM bpm = 63;
	RTTTL_DURATION wholenote;
	byte pin = -1;
	unsigned long delayToNextNote = 0; //milliseconds before playing the next note
	bool playing = false;
	TONE_DURATION duration;
	byte noteOffset;
	RTTTL_OCTAVE_VALUE scale;
	int tmpNumber;

	/**
	* Description:
	*   Class to encapsulate a string defined in program memory (PROGMEM)
	*/
	class ProgramMemoryString
	{
		public:
		ProgramMemoryString(const char * iBuffer) {
			mOffset = 0;
			mBuffer = iBuffer;
			findLength();
		}
		
		char operator[] (int16_t iOffset) const {
			if (iOffset > mLength)
			return '\0'; //out of bounds
			return pgm_read_byte_near(mBuffer + iOffset);
		}

		void reset() { mOffset = 0; }
		void setOffset(int16_t iOffset) { mOffset = iOffset; }
		void operator ++ (int) { mOffset++; }
		void operator -- (int) { mOffset--; }
		void operator += (int16_t iOffset) { mOffset += iOffset; }
		int16_t getLength() const { return mLength; }
		
		//implicit conversion to char
		operator char() const { return (*this)[mOffset]; }

		private:
		void findLength() {
			mLength = 0;
			char c = pgm_read_byte_near(mBuffer + mLength);
			while(c != '\0') {
				mLength++;
				c = pgm_read_byte_near(mBuffer + mLength);
			}
		}
		
		//attributes
		private:
		int16_t mOffset;
		int16_t mLength;
		const char * mBuffer;
	};

	const char * readNumber(const char * iBuffer, int & oValue)
	{
		oValue = 0;
		while(isdigit(*iBuffer))
		{
			oValue = (oValue * 10) + (*iBuffer++ - '0');
		}
		return iBuffer;
	}

	void readNumber(ProgramMemoryString & iString, int & oValue)
	{
		oValue = 0;
		while(isdigit(iString))
		{
			oValue = (oValue * 10) + (iString - '0');
			iString++;
		}
	}

	/****************************************************************************
	* Custom functions
	****************************************************************************/

	ToneFuncPtr _tone = &tone;
	NoToneFuncPtr _noTone = &noTone;
	DelayFuncPtr _delay = &delay;
	MillisFuncPtr _millis = &millis;

	void setToneFunction(ToneFuncPtr iFunc) {
		_tone = iFunc;
	}

	void setNoToneFunction(NoToneFuncPtr iFunc) {
		_noTone = iFunc;
	}

	void setDelayFunction(DelayFuncPtr iFunc) {
		_delay = iFunc;
	}

	void setMillisFunction(MillisFuncPtr iFunc) {
		_millis = iFunc;
	}


	/****************************************************************************
	* Blocking API
	****************************************************************************/
	namespace blocking
	{

		void play(byte iPin, const char * iBuffer) {
			// Absolutely no error checking in here

			default_dur = 4;
			default_oct = 6;
			bpm = 63;

			// format: d=N,o=N,b=NNN:
			// find the start (skip name, etc)

			while(*iBuffer != ':') iBuffer++; // ignore name
			iBuffer++;                        // skip ':'

			// get default duration
			if(*iBuffer == 'd')
			{
				iBuffer++; iBuffer++;           // skip "d="
				iBuffer = readNumber(iBuffer, tmpNumber);
				if(tmpNumber > 0)
				default_dur = tmpNumber;
				iBuffer++;                      // skip comma
			}

			#ifdef ANY_RTTTL_INFO
			Serial.print("ddur: "); Serial.println(default_dur, 10);
			#endif

			// get default octave
			if(*iBuffer == 'o')
			{
				iBuffer++; iBuffer++;           // skip "o="
				iBuffer = readNumber(iBuffer, tmpNumber);
				if(tmpNumber >= 3 && tmpNumber <= 7)
				default_oct = tmpNumber;
				iBuffer++;                      // skip comma
			}

			#ifdef ANY_RTTTL_INFO
			Serial.print("doct: "); Serial.println(default_oct, 10);
			#endif

			// get BPM
			if(*iBuffer == 'b')
			{
				iBuffer++; iBuffer++;         // skip "b="
				iBuffer = readNumber(iBuffer, tmpNumber);
				bpm = tmpNumber;
				iBuffer++;                    // skip colon
			}

			#ifdef ANY_RTTTL_INFO
			Serial.print("bpm: "); Serial.println(bpm, 10);
			#endif

			// BPM usually expresses the number of quarter notes per minute
			wholenote = (60 * 1000L / bpm) * 4;  // this is the time for whole noteOffset (in milliseconds)

			#ifdef ANY_RTTTL_INFO
			Serial.print("wn: "); Serial.println(wholenote, 10);
			#endif

			// now begin note loop
			while(*iBuffer)
			{
				// first, get note duration, if available
				iBuffer = readNumber(iBuffer, tmpNumber);
				
				if(tmpNumber)
				duration = wholenote / tmpNumber;
				else
				duration = wholenote / default_dur;  // we will need to check if we are a dotted noteOffset after

				// now get the note
				noteOffset = getNoteOffsetFromLetter(*iBuffer);
				iBuffer++;

				// now, get optional '#' sharp
				if(*iBuffer == '#')
				{
					noteOffset++;
					iBuffer++;
				}

				// now, get optional '.' dotted note
				if(*iBuffer == '.')
				{
					duration += duration/2;
					iBuffer++;
				}
				
				// now, get scale
				if(isdigit(*iBuffer))
				{
					scale = *iBuffer - '0';
					iBuffer++;
				}
				else
				{
					scale = default_oct;
				}

				if(*iBuffer == ',')
				iBuffer++;       // skip comma for next note (or we may be at the end)

				// now play the note
				if(noteOffset)
				{
					uint16_t frequency = notes[(scale - 4) * NOTES_PER_OCTAVE + noteOffset];

					#ifdef ANY_RTTTL_INFO
					Serial.print("Playing: ");
					Serial.print(scale, 10); Serial.print(' ');
					Serial.print(noteOffset, 10); Serial.print(" (");
					Serial.print(frequency, 10);
					Serial.print(") ");
					Serial.println(duration, 10);
					#endif

					_tone(iPin, frequency, duration);
					_delay(duration+1);
					_noTone(iPin);
				}
				else
				{
					#ifdef ANY_RTTTL_INFO
					Serial.print("Pausing: ");
					Serial.println(duration, 10);
					#endif
					_delay(duration);
				}
			}
		}


		void playProgMem(byte iPin, const char * iProgMemBuffer) {
			// Absolutely no error checking in here

			default_dur = 4;
			default_oct = 6;
			bpm = 63;

			//create a class which encapsulate the string defined in PROGMEM
			ProgramMemoryString buffer(iProgMemBuffer);

			// format: d=N,o=N,b=NNN:
			// find the start (skip name, etc)

			while(buffer != ':') buffer++;          // ignore name
			buffer++;                               // skip ':'

			// get default duration
			if(buffer == 'd')
			{
				buffer++; buffer++;                   // skip "d="
				readNumber(buffer, tmpNumber);
				if(tmpNumber > 0)
				default_dur = tmpNumber;
				buffer++;                             // skip comma
			}

			#ifdef ANY_RTTTL_INFO
			Serial.print("ddur: "); Serial.println(default_dur, 10);
			#endif

			// get default octave
			if(buffer == 'o')
			{
				buffer++; buffer++;                   // skip "o="
				readNumber(buffer, tmpNumber);
				if(tmpNumber >= 3 && tmpNumber <= 7)
				default_oct = tmpNumber;
				buffer++;                             // skip comma
			}

			#ifdef ANY_RTTTL_INFO
			Serial.print("doct: "); Serial.println(default_oct, 10);
			#endif

			// get BPM
			if(buffer == 'b')
			{
				buffer++; buffer++;                   // skip "b="
				readNumber(buffer, tmpNumber);
				bpm = tmpNumber;
				buffer++;                             // skip colon
			}

			#ifdef ANY_RTTTL_INFO
			Serial.print("bpm: "); Serial.println(bpm, 10);
			#endif

			// BPM usually expresses the number of quarter notes per minute
			wholenote = (60 * 1000L / bpm) * 4;  // this is the time for whole noteOffset (in milliseconds)

			#ifdef ANY_RTTTL_INFO
			Serial.print("wn: "); Serial.println(wholenote, 10);
			#endif

			// now begin note loop
			while(buffer)
			{
				// first, get note duration, if available
				readNumber(buffer, tmpNumber);
				
				if(tmpNumber)
				duration = wholenote / tmpNumber;
				else
				duration = wholenote / default_dur;  // we will need to check if we are a dotted noteOffset after

				// now get the note
				noteOffset = getNoteOffsetFromLetter(buffer);
				buffer++;

				// now, get optional '#' sharp
				if(buffer == '#')
				{
					noteOffset++;
					buffer++;
				}

				// now, get optional '.' dotted note
				if(buffer == '.')
				{
					duration += duration/2;
					buffer++;
				}
				
				// now, get scale
				if(isdigit(buffer))
				{
					scale = buffer - '0';
					buffer++;
				}
				else
				{
					scale = default_oct;
				}

				if(buffer == ',')
				buffer++;       // skip comma for next note (or we may be at the end)

				// now play the note
				if(noteOffset)
				{
					uint16_t frequency = notes[(scale - 4) * NOTES_PER_OCTAVE + noteOffset];

					#ifdef ANY_RTTTL_INFO
					Serial.print("Playing: ");
					Serial.print(scale, 10); Serial.print(' ');
					Serial.print(noteOffset, 10); Serial.print(" (");
					Serial.print(frequency, 10);
					Serial.print(") ");
					Serial.println(duration, 10);
					#endif

					_tone(iPin, frequency, duration);
					_delay(duration+1);
					_noTone(iPin);
				}
				else
				{
					#ifdef ANY_RTTTL_INFO
					Serial.print("Pausing: ");
					Serial.println(duration, 10);
					#endif
					_delay(duration);
				}
			}
		}


		void play16Bits(int iPin, const unsigned char * iBuffer, int iNumNotes) {
			// Absolutely no error checking in here

			RTTTL_DEFAULT_VALUE_SECTION * defaultSection = (RTTTL_DEFAULT_VALUE_SECTION *)iBuffer;
			RTTTL_NOTE * notesBuffer = (RTTTL_NOTE *)iBuffer;

			bpm = defaultSection->bpm;

			#ifdef ANY_RTTTL_DEBUG
			Serial.print("numNotes=");
			Serial.println(iNumNotes);
			// format: d=N,o=N,b=NNN:
			Serial.print("d=");
			Serial.print(getNoteDurationFromIndex(defaultSection->durationIdx));
			Serial.print(",o=");
			Serial.print(getNoteOctaveFromIndex(defaultSection->octaveIdx));
			Serial.print(",b=");
			Serial.println(bpm);
			#endif
			
			// BPM usually expresses the number of quarter notes per minute
			wholenote = (60 * 1000L / bpm) * 4;  // this is the time for whole noteOffset (in milliseconds)

			// now begin note loop
			for(int i=0; i<iNumNotes; i++) {
				const RTTTL_NOTE & n = notesBuffer[i+1]; //offset by 16 bits for RTTTL_DEFAULT_VALUE_SECTION

				// first, get note duration, if available
				duration = wholenote / getNoteDurationFromIndex(n.durationIdx);

				// now get the note
				//noteOffset = noteOffsets[n.noteIdx];
				noteOffset = getNoteOffsetFromLetterIndex(n.noteIdx);

				// now, get optional '#' sharp
				if(n.pound)
				{
					noteOffset++;
				}

				// now, get optional '.' dotted note
				if(n.dotted)
				{
					duration += duration/2;
				}

				// now, get scale
				scale = getNoteOctaveFromIndex(n.octaveIdx);

				if(noteOffset)
				{
					#ifdef ANY_RTTTL_DEBUG
					Serial.print(getNoteDurationFromIndex(n.durationIdx));
					static const char noteCharacterValues[] =   {'c','d','e','f','g','a','b','p'};
					Serial.print(noteCharacterValues[n.noteIdx]);
					Serial.print( (n.pound ? "#" : "") );
					Serial.print( (n.dotted ? "." : "") );
					Serial.println(getNoteOctaveFromIndex(n.octaveIdx));
					#endif
					
					uint16_t frequency = notes[(scale - 4) * NOTES_PER_OCTAVE + noteOffset];

					_tone(iPin, frequency, duration);
					_delay(duration+1);
					_noTone(iPin);
				}
				else
				{
					#ifdef ANY_RTTTL_DEBUG
					Serial.print(getNoteDurationFromIndex(n.durationIdx));
					static const char noteCharacterValues[] =   {'c','d','e','f','g','a','b','p'};
					Serial.print(noteCharacterValues[n.noteIdx]);
					Serial.print( (n.pound ? "#" : "") );
					Serial.print( (n.dotted ? "." : "") );
					Serial.println();
					#endif

					_delay(duration);
				}
			}
		}

		void play10Bits(int iPin, int iNumNotes, BitReadFuncPtr iFuncPtr) {
			// Absolutely no error checking in here

			//read default section
			RTTTL_DEFAULT_VALUE_SECTION defaultSection;
			defaultSection.raw = iFuncPtr(16);

			bpm = defaultSection.bpm;

			#ifdef ANY_RTTTL_DEBUG
			Serial.print("numNotes=");
			Serial.println(iNumNotes);
			// format: d=N,o=N,b=NNN:
			Serial.print("d=");
			Serial.print(getNoteDurationFromIndex(defaultSection.durationIdx));
			Serial.print(",o=");
			Serial.print(getNoteOctaveFromIndex(defaultSection.octaveIdx));
			Serial.print(",b=");
			Serial.println(bpm);
			#endif
			
			// BPM usually expresses the number of quarter notes per minute
			wholenote = (60 * 1000L / bpm) * 4;  // this is the time for whole noteOffset (in milliseconds)

			// now begin note loop
			for(int i=0; i<iNumNotes; i++) {
				RTTTL_NOTE n;
				n.raw = iFuncPtr(10);

				// first, get note duration, if available
				duration = wholenote / getNoteDurationFromIndex(n.durationIdx);

				// now get the note
				noteOffset = getNoteOffsetFromLetterIndex(n.noteIdx);

				// now, get optional '#' sharp
				if(n.pound)
				{
					noteOffset++;
				}

				// now, get optional '.' dotted note
				if(n.dotted)
				{
					duration += duration/2;
				}

				// now, get scale
				scale = getNoteOctaveFromIndex(n.octaveIdx);

				if(noteOffset)
				{
					#ifdef ANY_RTTTL_DEBUG
					Serial.print(getNoteDurationFromIndex(n.durationIdx));
					static const char noteCharacterValues[] =   {'c','d','e','f','g','a','b','p'};
					Serial.print(noteCharacterValues[n.noteIdx]);
					Serial.print( (n.pound ? "#" : "") );
					Serial.print( (n.dotted ? "." : "") );
					Serial.println(getNoteOctaveFromIndex(n.octaveIdx));
					#endif
					
					uint16_t frequency = notes[(scale - 4) * 12 + noteOffset];
					_tone(iPin, frequency, duration);
					_delay(duration+1);
					_noTone(iPin);
				}
				else
				{
					#ifdef ANY_RTTTL_DEBUG
					Serial.print(getNoteDurationFromIndex(n.durationIdx));
					static const char noteCharacterValues[] =   {'c','d','e','f','g','a','b','p'};
					Serial.print(noteCharacterValues[n.noteIdx]);
					Serial.print( (n.pound ? "#" : "") );
					Serial.print( (n.dotted ? "." : "") );
					Serial.println();
					#endif

					_delay(duration);
				}
			}
		}



	}; //blocking namespace


	/****************************************************************************
	* Non-blocking API
	****************************************************************************/
	namespace nonblocking
	{


		//pre-declaration
		void nextnote();

		void begin(byte iPin, const char * iBuffer)
		{
			#ifdef ANY_RTTTL_DEBUG
			Serial.print("playing: ");
			Serial.println(buffer);
			#endif
			
			//init values
			pin = iPin;
			buffer = iBuffer;
			bufferIndex = 0;
			default_dur = 4;
			default_oct = 6;
			bpm=63;
			playing = true;
			delayToNextNote = 0;
			#ifdef ANY_RTTTL_DEBUG
			Serial.print("delayToNextNote=");
			Serial.println(delayToNextNote);
			#endif
			
			//stop current note
			noTone(pin);

			// format: d=N,o=N,b=NNN:
			// find the start (skip name, etc)

			//read buffer until first note
			while(*buffer != ':') buffer++;     // ignore name
			buffer++;                           // skip ':'

			// get default duration
			if(*buffer == 'd')
			{
				buffer++; buffer++;               // skip "d="
				buffer = readNumber(buffer, tmpNumber);
				if(tmpNumber > 0)
				default_dur = tmpNumber;
				buffer++;                         // skip comma
			}

			#ifdef ANY_RTTTL_INFO
			Serial.print("ddur: "); Serial.println(default_dur, 10);
			#endif
			
			// get default octave
			if(*buffer == 'o')
			{
				buffer++; buffer++;               // skip "o="
				buffer = readNumber(buffer, tmpNumber);
				if(tmpNumber >= 3 && tmpNumber <= 7)
				default_oct = tmpNumber;
				buffer++;                         // skip comma
			}

			#ifdef ANY_RTTTL_INFO
			Serial.print("doct: "); Serial.println(default_oct, 10);
			#endif
			
			// get BPM
			if(*buffer == 'b')
			{
				buffer++; buffer++;              // skip "b="
				buffer = readNumber(buffer, tmpNumber);
				bpm = tmpNumber;
				buffer++;                   // skip colon
			}

			#ifdef ANY_RTTTL_INFO
			Serial.print("bpm: "); Serial.println(bpm, 10);
			#endif

			// BPM usually expresses the number of quarter notes per minute
			wholenote = (60 * 1000L / bpm) * 4;  // this is the time for whole noteOffset (in milliseconds)

			#ifdef ANY_RTTTL_INFO
			Serial.print("wn: "); Serial.println(wholenote, 10);
			#endif
		}

		void nextnote()
		{
			//stop current note
			_noTone(pin);

			// first, get note duration, if available
			buffer = readNumber(buffer, tmpNumber);
			
			if(tmpNumber)
			duration = wholenote / tmpNumber;
			else
			duration = wholenote / default_dur;  // we will need to check if we are a dotted noteOffset after

			// now get the note
			noteOffset = getNoteOffsetFromLetter(*buffer);
			buffer++;

			// now, get optional '#' sharp
			if(*buffer == '#')
			{
				noteOffset++;
				buffer++;
			}

			// now, get optional '.' dotted note
			if(*buffer == '.')
			{
				duration += duration/2;
				buffer++;
			}

			// now, get scale
			if(isdigit(*buffer))
			{
				scale = *buffer - '0';
				buffer++;
			}
			else
			{
				scale = default_oct;
			}

			if(*buffer == ',')
			buffer++;       // skip comma for next note (or we may be at the end)

			// now play the note
			if(noteOffset)
			{
				#ifdef ANY_RTTTL_INFO
				Serial.print("Playing: ");
				Serial.print(scale, 10); Serial.print(' ');
				Serial.print(noteOffset, 10); Serial.print(" (");
				Serial.print(notes[(scale - 4) * NOTES_PER_OCTAVE + noteOffset], 10);
				Serial.print(") ");
				Serial.println(duration, 10);
				#endif
				
				uint16_t frequency = notes[(scale - 4) * NOTES_PER_OCTAVE + noteOffset];
				_tone(pin, frequency, duration);
				
				delayToNextNote = _millis() + (duration+1);
			}
			else
			{
				#ifdef ANY_RTTTL_INFO
				Serial.print("Pausing: ");
				Serial.println(duration, 10);
				#endif
				
				delayToNextNote = _millis() + (duration);
			}
		}

		void play()
		{
			//if done playing the song, return
			if (!playing)
			{
				#ifdef ANY_RTTTL_DEBUG
				Serial.println("done playing...");
				#endif
				
				return;
			}
			
			//are we still playing a note ?
			unsigned long m = _millis();
			if (m < delayToNextNote)
			{
				#ifdef ANY_RTTTL_DEBUG
				Serial.println("still playing a note...");
				#endif
				
				//wait until the note is completed
				return;
			}

			//ready to play the next note
			if (*buffer == '\0')
			{
				//no more notes. Reached the end of the last note

				#ifdef ANY_RTTTL_DEBUG
				Serial.println("end of note...");
				#endif
				
				playing = false;

				//stop current note (if any)
				_noTone(pin);

				return; //end of the song
			}
			else
			{
				//more notes to play...

				#ifdef ANY_RTTTL_DEBUG
				Serial.println("next note...");
				#endif
				
				nextnote();
			}
		}

		void stop()
		{
			if (playing)
			{
				//increase song buffer until the end
				while (*buffer != '\0')
				{
					buffer++;
				}
			}

			playing = false;

			//stop current note (if any)
			_noTone(pin);
		}

		bool done()
		{
			return !playing;
		}

		bool isPlaying()
		{
			return playing;
		}

	}; //nonblocking namespace

}; //anyrtttl namespace
