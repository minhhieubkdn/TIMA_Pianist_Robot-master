// NotesBuffer.h

#ifndef _NOTESBUFFER_h
#define _NOTESBUFFER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "Note.h"
#include "PianoConstant.h"
class NotesBuffer
{
public:
	const uint8_t sizeOfNote = 2;
	NotesBuffer();
	NotesBuffer(uint8_t _bufferSize);
	void GetNoteBuffer(uint16_t EEPROM_Address);
	
	uint8_t CaculateBestPosition();
private:
	Note * notes;
	uint8_t bufferSize;
	uint8_t bestPosition = 0;
};

#endif

