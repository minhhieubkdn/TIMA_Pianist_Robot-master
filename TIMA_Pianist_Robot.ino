#include <EEPROM.h>
#include "Note.h"
#include <Wire.h>
#include "PCA9685.h"
#include "Sheet.h"
#include "PianoConstant.h"
#include "PianoHand.h"
#include "VirtualTimer.h"
#include "fastio.h"

#define X_STEP			2
#define Y_STEP			3
#define Z_STEP			4

#define X_DIR			5
#define Y_DIR			6
#define Z_DIR			7

#define EN				8

#define X_ENDSTOP		9
#define Y_ENDSTOP		10
#define Z_ENDSTOP		11

#define LEFT_PRESS_ANGLE -80
#define LEFT_RELEASE_ANGLE 10

#define RIGHT_PRESS_ANGLE 70
#define RIGHT_RELEASE_ANGLE -30

#define TEMPO 120

#define LEFT_SHEET /*"C4-2 G4 C4 G4 G3 D4 G4 D4 A3 E4 A4 E4 E3 B3 E4 B3 F3 C4 F4 C4 C3 G3 C4 G3 F3 C4-2 F4 C4 G3 D4 G4 D4 C4-2 G4 C4 G4 G3 D4 G4 D4 A3 E4 A4 E4 E3 B3 E4 B3 F3 C4 F4 C4 C3 G3 C4 G3 F3 C4 F4 C4 G3 D4"*/"G4-2 D4 C4 G4 C4 G4 G3 D4 G4 D4 A3-2 E4 A4 E4 E3 B3 E4 B3 F3 C4 F4 C4 C3 G3 C4 G3 F3 C4 F4 C4 G3 D4 G4 B4 D5 RE-6 C4-2 G4 C4 G4"// "G3-2 D4 G4 D4 A3 E4 A4 E4 E3 B3 E4 B3 F3 C4 F4 C4 C3 G3 C4 G3 G3 D4 G4 B4 D5 RE-6 C4-2 G4 C4 G4 G3 D4 G4 D4 A3 E4 A4 E4 E3 B3 E4-2 B3 F3 C4 F4 C4 C3 G3 C4 G3 G3 D4 G4 B4 D5 RE-2 RE-4 C4 G4 C4 G4 G3 D4 G4 D4 F3 C4 F4 G4 A4-8"//*/
#define RIGHT_SHEET /*"E6-8 D6 C6 B5 A5 G5 A5 B5 C6-2 C6 C6 C6 B5-4 G5 RE-2 A5-2 B5 C6 G5-4 E5 RE-2 A5-2 B5 C6 G5-4 E5 RE-2 A5-2 B5 C6 D6-8 C6-2 C6 C6 C6 B5-4 G5 RE-2 A5 B5 C6 G5-4 E5 RE-2 A5-2 B5 C6 G5-4 E5 RE-2 B5-4 C6-2 D6-8"*/"RE-2 RE C6-2 D6 E6 E6-1 E6 E6-2 G6 G6-4 D6 RE-2 C6 C6 E6 E6-4 B5 RE-2 A5 A5 F6-2 E6 F6-1 E6 C6-2 D6-8 RE-4 C6 D6 E6 E6-1 E6 E6-2 G6 G6-4 D6-4 RE-2 C6-2 C6-2 E6-2 E6-4 B5-4 RE-2 A5-2 A5-2 F6-2 E6-2" //"F6-1 E6 C6-2 D6-8 RE-4 C6-4 C6-8 B5-2 C6-2 D6-2 C6-2 C6-8"//*/

#define LEFT_START_ADDRESS 0
#define RIGHT_START_ADDRESS 255

#define LEFT_SHEET_LENGHT 75
#define RIGHT_SHEET_LENGHT 75


//#define SAVE_SHEET_TO_EEPROM

#ifndef SAVE_SHEET_TO_EEPROM

PianoHand* LeftHand;
PianoHand* RightHand;

Sheet PianoSheet;

uint8_t RightPins[5] = { 4, 3, 2, 1, 0 };
uint8_t LeftPins[5] = { 9, 8, 7, 6, 5 };

PCA9685 servoManager = PCA9685(Wire, PCA9685_PhaseBalancer_Weaved);
PCA9685_ServoEvaluator pwmMaker(128, 324, 526);

#else

SheetSaver leftSheetSaver;
SheetSaver rightSheetSaver;
Note * lNotes;
Note * rNotes;

#endif

void setup()
{
#ifndef SAVE_SHEET_TO_EEPROM
	Serial.begin(9600);
	EEPROM.begin();
	InitVirtualTimer();

	LeftHand = new PianoHand();	
	RightHand = new PianoHand();

	InitServos();
	InitHands();
	InitPianoSheet();

	Home();

	//LaunchFingers();
#else
	Serial.begin(9600);

	//clearEEPROM();

	InitLeftSheetSaver();
	InitRightSheetSaver();

#endif
}

void loop()
{	
#ifndef SAVE_SHEET_TO_EEPROM
	PianoSheet.Execute();
#endif
}

#ifndef SAVE_SHEET_TO_EEPROM
void InitVirtualTimer() 
{
	VirtualTimer.Init();
	VirtualTimer.Add(ToggleStepPinX, 40);
	VirtualTimer.Add(ToggleStepPinZ, 40);
	VirtualTimer.Run();
}

void InitServos()
{
	Wire.begin();
	Wire.setClock(400000);
	servoManager.resetDevices();
	servoManager.init(B000000);
	servoManager.setPWMFrequency(50);
	RightHand->PWMMaker = &pwmMaker;
	LeftHand->PWMMaker = &pwmMaker;
	RightHand->ServoManager = &servoManager;
	LeftHand->ServoManager = &servoManager;
}

void InitHands()
{
	RightHand->Init(RightPins, C5, D5, E5, F5, G5);
	LeftHand->Init(LeftPins, C3, D3, E3, F3, G3);

	LeftHand->SetStepperMotor(X_STEP, X_DIR, EN, X_ENDSTOP);
	RightHand->SetStepperMotor(Z_STEP, Z_DIR, EN, Z_ENDSTOP);

	LeftHand->SetActiveAngle(LEFT_RELEASE_ANGLE, LEFT_PRESS_ANGLE);
	RightHand->SetActiveAngle(RIGHT_RELEASE_ANGLE, RIGHT_PRESS_ANGLE);
}

void InitPianoSheet()
{
	PianoSheet.SetTempo(TEMPO);
	PianoSheet.SetSheet(LEFT, LEFT_START_ADDRESS, LEFT_SHEET_LENGHT);
	PianoSheet.SetSheet(RIGHT, RIGHT_START_ADDRESS, RIGHT_SHEET_LENGHT);

	PianoSheet.SetHandToneFa(LeftHand);
	PianoSheet.SetHandToneSol(RightHand);
	
	RightHand->Release();
	LeftHand->Release();

	delay(UP_FINGER_TIME);
}

void LaunchFingers()
{
	RightHand->Move(C6);
	WaitR();
	LeftHand->Move(C3);
	WaitL();
	RightHand->PressKey(G6, 4);
	delay(300);
	RightHand->Release();
	delay(100);
	RightHand->PressKey(C6, 4);
	delay(300);
	RightHand->Release();
	delay(100);
	RightHand->PressKey(D6, 4);
	delay(300);
	RightHand->Release();
	delay(100);
	RightHand->PressKey(D6, 4);
	LeftHand->PressKey(C3, 4);
	delay(300);
	RightHand->Release();
	delay(100);
	LeftHand->PressKey(E3, 4);
	delay(300);
	LeftHand->Release();
	delay(100);
	LeftHand->PressKey(E3, 4);
	delay(300);
	LeftHand->Release();
	delay(100);
}

void WaitR()
{
	while (RightHand->IsMoving())
	{
		delay(1);
	};
}

void WaitL()
{
	while (LeftHand->IsMoving())
	{
		delay(1);
	};
}

void Home()
{
	LeftHand->UpdateHandPlacement(C8);

	LeftHand->Move(A0);

	while (!LeftHand->IsEndstopActive())
	{

	}

	LeftHand->Stop();

	LeftHand->UpdateHandPlacement(A0);

	//----------------------------------------

	RightHand->UpdateHandPlacement(A0);
	RightHand->Move(F7);

	/*while (1)
	{
		Serial.println(RightHand->IsEndstopActive());
		Serial.println(LeftHand->IsEndstopActive());
		delay(100);
	}*/

	while (!RightHand->IsEndstopActive())
	{
		
	}

	RightHand->Stop();

	RightHand->UpdateHandPlacement(F7);

}

void ToggleStepPinX()
{
	if (LeftHand->CurrentStep == LeftHand->DesiredStep)
	{
		return;
	}

	TOGGLE(X_STEP);

	LeftHand->CurrentStep += LeftHand->DirectionValue;
}

void ToggleStepPinZ()
{
	if (RightHand->CurrentStep == RightHand->DesiredStep)
	{
		return;
	}

	TOGGLE(Z_STEP);

	RightHand->CurrentStep += RightHand->DirectionValue;
}

#else
void clearEEPROM()
{
	for (uint16_t i = 0; i < EEPROM.length(); i++)
	{
		EEPROM.write(i, 0);
		delay(6);
	}
}

void InitLeftSheetSaver()
{
	String leftSheet = LEFT_SHEET;

	leftSheetSaver = SheetSaver(124);
	leftSheetSaver.Init();
	leftSheetSaver.SetNotesPointer(lNotes);
	leftSheetSaver.ConvertToNotes(leftSheet);
	leftSheetSaver.SaveToEEPROM();
	Serial.println("lsheet saved:");
	leftSheetSaver.checkNotesData();
	Serial.println("\nleft sheet lenght: ");
	Serial.println(leftSheetSaver.lenght());
	Serial.print("last ad: ");
	Serial.println(leftSheetSaver.getLastAddress());
	leftSheetSaver.checkDataInEEPROM();
}

void InitRightSheetSaver()
{
	String rightSheet = RIGHT_SHEET;

	rightSheetSaver = SheetSaver(361);
	rightSheetSaver.Init();
	rightSheetSaver.SetNotesPointer(rNotes);
	rightSheetSaver.ConvertToNotes(rightSheet);
	rightSheetSaver.SaveToEEPROM();
	Serial.println("rSheet saved:");
	rightSheetSaver.checkNotesData();
	Serial.print("\nright sheet lenght: ");
	Serial.println(rightSheetSaver.lenght());
	Serial.print("last ad: ");
	Serial.println(rightSheetSaver.getLastAddress());
	rightSheetSaver.checkDataInEEPROM();
}

#endif