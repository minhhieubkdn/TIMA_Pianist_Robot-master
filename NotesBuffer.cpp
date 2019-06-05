// 
// 
// 

#include "NotesBuffer.h"

NotesBuffer::NotesBuffer()
{
	bufferSize = 5;
	notes = new Note[5];
}

NotesBuffer::NotesBuffer(uint8_t _bufferSize)
{
	bufferSize = _bufferSize;
	notes = new Note[bufferSize];
}

void NotesBuffer::GetNoteBuffer(uint16_t EEPROM_Address)
{
	Note _note;
	for (uint8_t i = 0; i < bufferSize; i++)
	{
		EEPROM.get(EEPROM_Address + i * sizeOfNote, _note);
		this->notes[i] = _note;
	}
}

uint8_t NotesBuffer::CaculateBestPosition()
{
	uint8_t highestNote = notes[0].id;
	uint8_t lowestNote = notes[0].id;
	uint8_t maxNumOfNoteCanPressInPossition = 0;
	uint8_t curentPosMaxNotes = 0;
	for (uint8_t i = 1; i < bufferSize; i++)
	{
		if (highestNote < notes[i].id)
		{
			highestNote = notes[i].id;
		}
		if (lowestNote > notes[i].id)
		{
			lowestNote = notes[i].id;
		}
	}
	for (uint8_t firstKeyPos = lowestNote; firstKeyPos < highestNote - 5; firstKeyPos++)
	{
		curentPosMaxNotes = 0;
		for (uint8_t noteIndex = 0; noteIndex < bufferSize; noteIndex++)
		{
			if (notes[noteIndex].id == (firstKeyPos + noteIndex))
			{
				curentPosMaxNotes++;
			}
		}
		if (maxNumOfNoteCanPressInPossition < curentPosMaxNotes)
		{
			maxNumOfNoteCanPressInPossition = curentPosMaxNotes;
			bestPosition = firstKeyPos + 2;
		}
	}
}
