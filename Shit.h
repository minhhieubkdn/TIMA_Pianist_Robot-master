// Shit.h

#ifndef _SHIT_h
#define _SHIT_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif
// Actually is Sheet :))))
#include "PianoHand.h"
#include "MultiThread.h"
#include "NotesBuffer.h"
#include <math.h>

#define LEFT	0
#define RIGHT	1

class Shit {
public:
	Shit();
	void SetHandToneFa(PianoHand* left);
	void SetHandToneSol(PianoHand* right);
	void GetNotesFromEEPROM(uint16_t lStart, uint8_t lSize, uint16_t rStart, uint8_t rSize);
	void SetTempo(int tem);
	void Execute();
private:
	PianoHand * hand[2];
	NotesBuffer *lNoteBuffer;
	NotesBuffer *rNoteBuffer;
	NotesBuffer *lNextNoteBuffer;
	NotesBuffer *rNextNoteBuffer;
	Note currentNote[2];
	Note nextNote[2];
	uint8_t sheetSize[2];

	uint16_t startAddress[2];
	uint16_t currentAddress[2];
	uint8_t desiredHandPlacement[2];

	bool isJustRelease[2] = { false, false };
	bool isHandInPosition[2] = { true, true };

	uint16_t notesLenght[2];

	uint16_t interval = 500;
	int tempo = 120;

	MultiThread executeThread;
	MultiThread releaseThread[2];

	void getSheetfromEEPROM(uint8_t handID, uint16_t startAddress, uint8_t sheetLenght);
	uint16_t getTimeToNextNote(Note _currentNote, Note _nextNote);
	void getNewSection(uint8_t handID);
	void getNextBuffer(uint8_t handID);
	void getNextNote(uint8_t handID);
	void checkHandPosition(uint8_t handID);
	void checkTimeForReleaseFinger(uint8_t handID);
	void checkStateForMoveNextPosition(uint8_t handID);
	void checkBeatForPress(uint8_t handID);
};

#endif

