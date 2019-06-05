// Note.h

#ifndef _NOTE_h
#define _NOTE_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include <EEPROM.h>
#include "PianoConstant.h"

struct Note {
	uint8_t id;
	uint8_t value;
};
//On ATmega168 & ATmega8 EEPROM.lenght() = 512 bytes
class SheetSaver
{
public:
	SheetSaver();
	SheetSaver(uint16_t startAddress);
	void Init();
	uint16_t lenght();
	uint16_t getLastAddress();
	void SetNotesPointer(Note * notes);
	void AddNote(uint8_t noteID, uint8_t value);
	void AddNote(String noteID, uint8_t value);
	void AddNote(Note note);
	Note GetNoteAt(uint16_t address);
	uint8_t GetNoteID(uint16_t address);
	uint8_t GetNoteValue(String keys);
	void ConvertToNotes(String sheet);
	void SaveToEEPROM();
	uint8_t getNoteID(String note);
	void checkNotesData();
	void checkDataInEEPROM();
private:
	Note * notes;
	uint16_t notesLenght;
	uint16_t startAddress;
	uint16_t lastAddress;
	uint8_t sizeOfNote;
};

#endif

