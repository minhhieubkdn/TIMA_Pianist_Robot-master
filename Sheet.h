// Sheet.h

#ifndef _SHEET_h
#define _SHEET_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "PianoHand.h"
#include "MultiThread.h"
#include "Note.h"
#include <math.h>
#include <EEPROM.h>

#define LEFT	0
#define RIGHT	1

class Sheet
{
public:
	Sheet();

	void ReadNote();
	void SetHandToneFa(PianoHand* hand);
	void SetHandToneSol (PianoHand* hand);
	void SetTempo(int tem);
	void SetSheet(uint8_t handID, uint16_t startAddress, uint8_t notesSize);
	void Execute();

private:
	PianoHand* hand[2];
	uint8_t pressNotes[2][2];
	uint8_t noteValue[2];
	uint8_t handPlacement[2];
	bool isJustRelease[2] = { false, false };
	bool isHandInPosition[2] = { true, true };

	int16_t currentNoteOrder[2] = { 0, 0 };
	int16_t destinationNoteOrder[2] = { 0, 0 };
	uint16_t notesSize[2];
	uint8_t notes[2][200];
	uint16_t interval = 500;
	int tempo = 120;

	MultiThread executeThread;
	MultiThread releaseThread[2];

	void getNotesInEEPROM(uint8_t handId, uint16_t startAdress, uint8_t notesLenght);
	void getNewSection(uint8_t handID);
	void getNextNotes(uint8_t handID);
	void doSomethingIdontKnow(uint8_t handID);
	void convertToNotes(uint8_t handID, String s);
	bool isNoteSign(char c);
	bool isNumber(char c);
	uint8_t getNoteID(String note);
	uint8_t getValueID(String value);
	String getNoteName(uint8_t id);
	

	void checkHandPosition(uint8_t handID);
	void checkTimeForReleaseFinger(uint8_t handID);
	void checkStateForMoveNextPosition(uint8_t handID);
	void checkBeatForPress(uint8_t handID);

	void printNotes(uint8_t handID);
	int16_t log2(int16_t n);
};

#endif