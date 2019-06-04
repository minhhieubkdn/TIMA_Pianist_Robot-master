// 
// 
// 

#include "Note.h"

SheetSaver::SheetSaver()
{
	EEPROM.begin();
	startAddress = lastAddress = 0;
	sizeOfNote = sizeof(Note);
	notesLenght = 0;
}

SheetSaver::SheetSaver(uint16_t startAddress)
{
	EEPROM.begin();
	this->startAddress = startAddress;
	lastAddress = startAddress;
	sizeOfNote = sizeof(Note);
	notesLenght = 0;
}

void SheetSaver::Init()
{
	for (uint16_t index = startAddress; index < startAddress + 255; index += sizeOfNote)
	{
		Note _note;
		EEPROM.get(index, _note);
		if (_note.value > 0)
		{
			lastAddress += sizeOfNote;
		}
	}
}

void SheetSaver::SetNotesPointer(Note * notes)
{
	this->notes = notes;
}

void SheetSaver::AddNote(uint8_t noteID, uint8_t value)
{
	Note _note;
	_note.id = noteID;
	_note.value = value;
	this->notesLenght += 1;
	this->notes[notesLenght] = _note;
}

void SheetSaver::AddNote(String noteID, uint8_t value)
{
	uint8_t order;
	uint8_t tone;
	uint8_t id;
	Note note;

	if (noteID[0] == 'C')
		order = 0;
	if (noteID[0] == 'D')
		order = 1;
	if (noteID[0] == 'E')
		order = 2;
	if (noteID[0] == 'F')
		order = 3;
	if (noteID[0] == 'G')
		order = 4;
	if (noteID[0] == 'A')
		order = 5;
	if (noteID[0] == 'B')
		order = 6;

	tone = noteID[1] - '0';

	if (noteID == "RE")
		id = RE;
	else
		id = tone * 7 + order - 5 + 1;
	note.id = id;
	note.value = value;

	this->notesLenght += 1;
	this->notes[notesLenght] = note;
}

void SheetSaver::AddNote(Note note)
{
	this->notesLenght += 1;
	this->notes[notesLenght] = note;
}

Note SheetSaver::GetNoteAt(uint16_t address)
{
	Note _note;
	EEPROM.get(address, _note);
	return _note;
}

uint8_t SheetSaver::GetNoteID(uint16_t address)
{
	Note _note;
	EEPROM.get(address, _note);
	return _note.id;
}

void SheetSaver::ConvertToNotes(String sheet)
{
	int idOrder = 0;
	int spaceOrder1 = 0;
	int spaceOrder2 = 1;
	uint16_t noteOrder = 0;
	int8_t noteValue = 1;

	while (spaceOrder2 != sheet.length())
	{
		spaceOrder2 = sheet.indexOf(' ', spaceOrder1 + 1);

		if (spaceOrder2 < 0)
			spaceOrder2 = sheet.length();

		String keyS = sheet.substring(spaceOrder1, spaceOrder2);

		Serial.print(keyS);
		Serial.print(" ");

		spaceOrder1 = spaceOrder2 + 1;

		int8_t barOrder = keyS.indexOf('-');

		if (barOrder < 0)
		{
			barOrder = keyS.length();
		}
		else
		{
			String ValueID = keyS.substring(barOrder + 1);
			noteValue = ValueID.toInt() + 70;
		}

		String noteS = keyS.substring(0, barOrder);
		uint8_t noteID = getNoteID(noteS);

		notes[noteOrder].id = noteID;
		notes[noteOrder].value = noteValue;

		noteOrder++;
		notesLenght++;
	}
}

void SheetSaver::SaveToEEPROM(Note * notes)
{
	for (int i = 0; i < notesLenght; i++)
	{
		EEPROM.put(sizeOfNote * i + startAddress, notes[i]);
		delay(10);
	}
}

uint8_t SheetSaver::getNoteID(String note)
{
	uint8_t order;
	uint8_t tone;

	if (note[0] == 'C')
		order = 0;
	if (note[0] == 'D')
		order = 1;
	if (note[0] == 'E')
		order = 2;
	if (note[0] == 'F')
		order = 3;
	if (note[0] == 'G')
		order = 4;
	if (note[0] == 'A')
		order = 5;
	if (note[0] == 'B')
		order = 6;

	tone = note[1] - '0';

	if (note == "RE")
		return RE;
	else
		return tone * 7 + order - 5 + 1;
}

void SheetSaver::checkNotesData()
{
	
}

void SheetSaver::checkDataInEEPROM()
{
	Note note;
	for (int i = startAddress; i < notesLenght; i++)
	{
		EEPROM.get(i*sizeOfNote + startAddress, note);
		delay(5);
		Serial.print(note.id);
		Serial.print('-');
		Serial.print(note.value);
		Serial.print(' ');
	}
}
