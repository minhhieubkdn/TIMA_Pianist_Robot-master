// 
// 
// 

#include "Shit.h"

Shit::Shit()
{
	executeThread = MultiThread();
}

void Shit::SetHandToneFa(PianoHand * left)
{
	hand[LEFT] = left;
}

void Shit::SetHandToneSol(PianoHand * right)
{
	hand[RIGHT] = right;
}

void Shit::GetNotesFromEEPROM()
{

}

void Shit::SetTempo(int tem)
{
	this->tempo = tem;
	interval = (60000) / (tempo * 4);
}

void Shit::Execute()
{

	RUN_EVERY(executeThread, interval);
	Serial.print("P:");
	Serial.println(hand[RIGHT]->GetCurrentPos());
	checkBeatForPress(RIGHT);
	checkBeatForPress(LEFT);
}

void Shit::getNewSection(uint8_t handID)
{

}

void Shit::getNextBuffer(uint8_t handID)
{
	//notesBuffer[handID] = NotesBuffer(4);
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

}

void Shit::checkStateForMoveNextPosition(uint8_t handID)
{

}

void Shit::checkBeatForPress(uint8_t handID)
{
	hand[handID]->SkipBeat--;

	if (!hand[handID]->IsMoving() && hand[handID]->SkipBeat <= 0)
	{
		hand[handID]->PressKeys(pressNotes[handID], currentNote[handID].value - 70);
		hand[handID]->PressTime = interval * hand[handID]->SkipBeat - UP_FINGER_TIME;
		releaseThread[handID].Counter = millis();
	}
}
