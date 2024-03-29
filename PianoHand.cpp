// 
// 
// 

#include "PianoHand.h"

PianoHand::PianoHand()
{
	//Wire.begin();                
	//Wire.setClock(400000);      

	//PWMMaker = new PCA9685_ServoEvaluator(128, 324, 526);   // 102, 310, 505);
	//ServoManager = new PCA9685();

	////ServoManager->resetDevices();        // Software resets all PCA9685 devices on Wire line
	//ServoManager->init(B000000);         // Address pins A5-A0 set to B000000
	//ServoManager->setPWMFrequency(50);   // Set frequency to 50Hz
	
}

void PianoHand::Init(uint8_t* pins, uint8_t f1, uint8_t f2, uint8_t f3, uint8_t f4, uint8_t f5)
{
	fingers[0] = f1;
	fingers[1] = f2;
	fingers[2] = f3;
	fingers[3] = f4;
	fingers[4] = f5;

	fingerPins = pins;
}

void PianoHand::SetStepperMotor(uint8_t step, uint8_t dir, uint8_t en, uint8_t ends)
{
	stepPin = step;
	dirPin = dir;
	enPin = en;
	endsPin = ends;

	pinMode(stepPin, OUTPUT);
	pinMode(dirPin, OUTPUT);
	pinMode(enPin, OUTPUT);

	pinMode(endsPin, INPUT_PULLUP);

	digitalWrite(enPin, LOW);
}

void PianoHand::SetActiveAngle(int release, int press)
{
	releaseAngle = release;
	pressAngle = press;
}

int * PianoHand::GetFingerState()
{
	return fingers;
}

uint8_t PianoHand::GetCurrentPos()
{
	updateFingerPosition();
	return fingers[0];
}

void PianoHand::PressKey(uint8_t key, uint8_t value)
{
	updateFingerPosition();

	if (key == RE)
	{
		SkipBeat = value;
		fingerState = PRESSING;
		return;
	}

	uint16_t pwmAmout = PWMMaker->pwmForAngle(pressAngle);
	for (uint8_t i = 0; i < 5; i++)
	{
		if (fingers[i] == key)
		{
			ServoManager->setChannelPWM(fingerPins[i], pwmAmout);

			SkipBeat = value;

			fingerState = PRESSING;
		}
	}
}

void PianoHand::PressKeys(uint8_t * keys, uint8_t value)
{
	updateFingerPosition();

	if (keys[0] == RE)
	{
		SkipBeat = value;
		fingerState = PRESSING;
		return;
	}

	uint16_t pwmAmout = PWMMaker->pwmForAngle(pressAngle);
	for (uint8_t i = 0; i < 5; i++)
	{
		if (fingers[i] == keys[0])
		{
			ServoManager->setChannelPWM(fingerPins[i], pwmAmout);

			SkipBeat = value;

			fingerState = PRESSING;
			delay(50);
			Serial.println(pwmAmout);
			Serial.println("PRESSING");
			Serial.println(pressAngle);
		}
	}
}

void PianoHand::Release()
{
	/*while (1)
	{
		for (uint8_t i = 0; i < 5; i++)
		{
			ServoManager.setChannelPWM(i, PWMMaker.pwmForAngle(30));
		}
		delay(1000);

		for (uint8_t i = 0; i < 5; i++)
		{
			ServoManager.setChannelPWM(i, PWMMaker.pwmForAngle(-50));
		}
		delay(1000);
	}*/
	
	for (uint8_t i = 0; i < 5; i++)
	{
		ServoManager->setChannelPWM(fingerPins[i], PWMMaker->pwmForAngle(releaseAngle));
	}
	
	fingerState = UPPING;
}

void PianoHand::Move(uint8_t note)
{
	float posMM = note * WIDTH_OF_KEY + PLACEMENT_OFFSET;

	long posSteps = posMM * STEPS_PER_MM;

	DesiredStep = posSteps;
	JumpSteps = DesiredStep - CurrentStep;

	if (JumpSteps == 0)
		return;

	int dir = JumpSteps / abs(JumpSteps);

	setDirection(dir);
}

void PianoHand::Stop()
{
	DesiredStep = CurrentStep;
}

void PianoHand::UpdateHandPlacement(uint8_t note)
{
	float posMM = note * WIDTH_OF_KEY - PLACEMENT_OFFSET;

	long posSteps = posMM * STEPS_PER_MM;

	DesiredStep = CurrentStep = posSteps;
}

void PianoHand::setDirection(int8_t dir)
{
	if (IsInvertDirection == true)
		dir = dir * -1;

	DirectionValue = dir;	

	uint8_t state;

	if (dir > 0)
		state = LOW;
	else
		state = HIGH;

	digitalWrite(dirPin, state);
}

void PianoHand::updateFingerPosition()
{
	int fingerID = round((CurrentStep / STEPS_PER_MM) / WIDTH_OF_KEY);

	int jumpNotes = fingerID - fingers[0];

	for (uint8_t i = 0; i < 5; i++)
	{
		fingers[i] += jumpNotes;
	}
}

bool PianoHand::IsEndstopActive()
{
	if (digitalRead(endsPin) == LOW)
		return true;

	return false;
}

bool PianoHand::IsMoving()
{
	if (CurrentStep != DesiredStep)
		return true;

	return false;
}

bool PianoHand::IsPressing()
{
	if (fingerState == PRESSING)
		return true;

	return false;
}

void PianoHand::TestServo()
{
	while (1)
	{
		for (uint8_t i = 0; i < 5; i++)
		{
			ServoManager->setChannelPWM(fingerPins[i], PWMMaker->pwmForAngle(pressAngle));
		}
		delay(1000);

		Serial.println(PWMMaker->pwmForAngle(pressAngle));
		for (uint8_t i = 0; i < 5; i++)
		{
			ServoManager->setChannelPWM(fingerPins[i], PWMMaker->pwmForAngle(releaseAngle));
		}
		delay(1000);
		Serial.println(PWMMaker->pwmForAngle(releaseAngle));
	}
}
