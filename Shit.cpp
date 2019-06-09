// 
// 
// 

#include "Shit.h"

Shit::Shit()
{
	executeThread = MultiThread();
	releaseThread[LEFT] = MultiThread();
	releaseThread[RIGHT] = MultiThread();
	lNoteBuffer = new NotesBuffer(4);
	lNextNoteBuffer = new NotesBuffer(4);
	rNoteBuffer = new NotesBuffer(4);
	rNoteBuffer = new NotesBuffer(4);
}

void Shit::SetHandToneFa(PianoHand * left)
{
	hand[LEFT] = left;
}

void Shit::SetHandToneSol(PianoHand * right)
{
	hand[RIGHT] = right;
}

void Shit::GetNotesFromEEPROM(uint16_t lStart, uint8_t lSize, uint16_t rStart, uint8_t rSize)
{
	currentAddress[LEFT] = lStart;
	currentAddress[RIGHT] = rStart;
	lNoteBuffer->GetNoteBuffer(lStart);
	rNoteBuffer->GetNoteBuffer(rStart);
	lNextNoteBuffer->GetNoteBuffer(currentAddress[LEFT] + lNextNoteBuffer->bufferSize * 2);
	rNextNoteBuffer->GetNoteBuffer(currentAddress[RIGHT] + rNextNoteBuffer->bufferSize * 2);
	sheetSize[LEFT] = lSize;
	sheetSize[RIGHT] = rSize;
}

void Shit::SetTempo(int tem)
{
	this->tempo = tem;
	interval = (60000) / (tempo * 4);
}

void Shit::Execute()
{
	checkHandPosition(LEFT);
	checkHandPosition(RIGHT);
	if (isHandInPosition[LEFT])
	{
		checkTimeForReleaseFinger(LEFT);
		if (isJustRelease[LEFT])
		{

		}
	}
	RUN_EVERY(executeThread, interval);
	checkBeatForPress(RIGHT);
	checkBeatForPress(LEFT);
}

void Shit::getSheetfromEEPROM(uint8_t handID, uint16_t startAddress, uint8_t sheetLenght)
{
	Note _note;
	if (handID == LEFT)
	{
		for (uint8_t i = 0; i < sheetLenght; i++)
		{
			EEPROM.get(i * 2 + startAddress, _note);
			delay(5);
			//leftSheet[i] = _note;
		}
	}
	else if (handID == RIGHT)
	{
		for (uint8_t i = 0; i < sheetLenght; i++)
		{
			EEPROM.get(i * 2 + startAddress, _note);
			delay(5);
			//rightSheet[i] = _note;
		}
	}
}

uint16_t Shit::getTimeToNextNote(Note _currentNote, Note _nextNote)
{
	uint8_t notesNum = abs(_nextNote.id - _currentNote.id);
	uint16_t time = notesNum * 80 * WIDTH_OF_KEY * STEPS_PER_MM / 1000;
	return time;
}

void Shit::getNewSection(uint8_t handID)
{
	uint8_t pos = hand[handID]->GetCurrentPos();
	if (pos >= currentNote[handID].id && pos <= (currentNote[handID].id + 5))
	{
		
	}
	else
	{

		hand[handID]->Move(desiredHandPlacement[handID]);
	}
}

void Shit::getNextBuffer(uint8_t handID)
{
	if (handID == LEFT)
	{
		for (uint8_t i = 0; i < lNoteBuffer->bufferSize; i++)
		{
			lNoteBuffer->notes[i] = lNextNoteBuffer->notes[i];
		}
		lNextNoteBuffer->currentAddress += lNextNoteBuffer->bufferSize * 2;
		lNextNoteBuffer->GetNoteBuffer(lNextNoteBuffer->currentAddress);
	}
	else if (handID == RIGHT)
	{
		for (uint8_t i = 0; i < rNoteBuffer->bufferSize; i++)
		{
			rNoteBuffer->notes[i] = rNextNoteBuffer->notes[i];
		}
		rNextNoteBuffer->currentAddress += rNextNoteBuffer->bufferSize * 2;
		rNextNoteBuffer->GetNoteBuffer(rNextNoteBuffer->currentAddress);
	}
}

void Shit::getNextNote(uint8_t handID)
{
	if (handID == LEFT)
	{
		lNoteBuffer->currentNoteIndex++;
		if (lNoteBuffer->currentNoteIndex == 5)
		{
			getNextBuffer(handID);
			lNoteBuffer->currentNoteIndex = 0;
		}
		currentNote[LEFT] = nextNote[LEFT];
		nextNote[LEFT] = lNoteBuffer->GetNoteAt(lNoteBuffer->currentNoteIndex);
	}
	else if (handID == RIGHT)
	{
		rNoteBuffer->currentNoteIndex++;
		if (rNoteBuffer->currentNoteIndex == 5)
		{
			getNextBuffer(handID);
			rNoteBuffer->currentNoteIndex = 0;
		}
		currentNote[LEFT] = nextNote[LEFT];
		nextNote[LEFT] = rNoteBuffer->GetNoteAt(rNoteBuffer->currentNoteIndex);
	}
}

void Shit::checkHandPosition(uint8_t handID)
{
	if (hand[handID]->GetCurrentPos() != desiredHandPlacement[handID])
	{
		hand[handID]->Move(desiredHandPlacement[handID]);
		isHandInPosition[handID] = false;
	}
	else
	{
		isHandInPosition[handID] = true;
	}
}

void Shit::checkTimeForReleaseFinger(uint8_t handID)
{
	if (releaseThread[handID].isSchedule(hand[handID]->PressTime) && hand[handID]->IsPressing())
	{
		hand[handID]->Release();
		isJustRelease[handID] = true;
	}
}

void Shit::checkStateForMoveNextPosition(uint8_t handID)
{
	if (isJustRelease[handID] == true)
	{
		getNextNote(handID); 
	}
}

void Shit::checkBeatForPress(uint8_t handID)
{
	hand[handID]->SkipBeat--;

	if (!hand[handID]->IsMoving() && hand[handID]->SkipBeat <= 0)
	{
		uint16_t moveTime = getTimeToNextNote(currentNote[handID], nextNote[handID]);
		hand[handID]->PressKey(currentNote[handID].id, currentNote[handID].value - 70);
		hand[handID]->PressTime = interval * hand[handID]->SkipBeat - UP_FINGER_TIME - moveTime;
		releaseThread[handID].Counter = millis();
	}
}
