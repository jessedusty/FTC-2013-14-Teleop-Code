#pragma config(Hubs,  S1, HTServo,  HTMotor,  HTMotor,  HTMotor)
#pragma config(Sensor, S1,     ,               sensorI2CMuxController)
#pragma config(Motor,  mtr_S1_C2_1,     motorRightF,   tmotorTetrix, openLoop, reversed)
#pragma config(Motor,  mtr_S1_C2_2,     motorRightR,   tmotorTetrix, openLoop, reversed)
#pragma config(Motor,  mtr_S1_C3_1,     motorLeftF,    tmotorTetrix, openLoop)
#pragma config(Motor,  mtr_S1_C3_2,     motorLeftR,    tmotorTetrix, openLoop)
#pragma config(Motor,  mtr_S1_C4_1,     liftMotor,     tmotorTetrix, openLoop)
#pragma config(Motor,  mtr_S1_C4_2,     flagMotor,     tmotorTetrix, openLoop)
#pragma config(Servo,  srvo_S1_C1_1,    trayTiltMotor,        tServoStandard)
#pragma config(Servo,  srvo_S1_C1_2,    upperLiftMotor,       tServoContinuousRotation)
#pragma config(Servo,  srvo_S1_C1_3,    servo3,               tServoNone)
#pragma config(Servo,  srvo_S1_C1_4,    servo4,               tServoNone)
#pragma config(Servo,  srvo_S1_C1_5,    servo5,               tServoNone)
#pragma config(Servo,  srvo_S1_C1_6,    servo6,               tServoNone)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

// THIS IS THE MAIN DRIVE CODE - FOR USE IN TELEOP, THIS FILE HAS BEEN CREATED ON Oct 27th, 2013


#include "JoystickDriver.c" // ABSOLUTLY CRUTIAL FOR DRIVING WITH A JOYSTICK

// VD - varible declarations
int rev = 1; // 1 = not reversed | -1 means reversed
bool normalctl = true; // normal joystick configuration if true, reverse joystick configuration if false
int power = 1; // the number that joystick is divided by to give
int forwardbackwardthresh = 10; // this is the threshold for the left joystick in leftmode
int leftrightthresh = 10; // this is the threshhold for the right joystick in left mode
bool leftmode = true; //leftmode is when up/down is on left and left/right is on right
bool rightmode = false; // rightmode the reverse of leftmode
bool isRunning = true; // specifies wether the robot is not in E-STOP mode - will stop all robot actions if set to false
int quadrent = 0;
bool driverslew = true;

bool firstController = false; // designates wether the drive mode is controller 1 or controller 2 for single control operation

bool heighthold = false;
// values set throughout the program

float leftstickval = 0;
float rightstickval = 0;
float newrightstickval = 0;


// global speeds of drive motors
float leftmotorval = 0;
float rightmotorval = 0;

// target positions for extremities, lengths in inches, angles in degrees
int currentstage = -2;
// end of "global" varible declaration

// make sure that the values going to the motor arn't less than -100 or greater than 100
float makesureitsinlimits(float input) {
	if (input > 100) input = 100;
	if (input < -100) input = -100;
	return input;
}

// driving joystick (the one that actually moves the robot's position)

void joyval_joystick1st() { // standard mode on left joystick

	leftstickval = pow(((float)joystick.joy1_y1 / (float)64 * (float)5), 2) ;
	if (joystick.joy1_y1 < 0) leftstickval = leftstickval * -1.0;

	rightstickval = pow(((float)joystick.joy1_x1 / (float)64 * (float)5), 2) ;
	if (joystick.joy1_x1 < 0) rightstickval = rightstickval * -1.0;

}

void joyval_joystick1rv() { // reverse mode on left joystick
	leftstickval = pow(((float)joystick.joy1_y2 / (float)64 * (float)5), 2) ;
	if (joystick.joy1_y2 < 0) leftstickval = leftstickval * -1.0;

	rightstickval = pow(((float)joystick.joy1_x1 / (float)64 * (float)5), 2) ;
	if (joystick.joy1_x1 < 0) rightstickval = rightstickval * -1.0;
}

void joyval_joystick2st() { // standard mode on right joystick
	leftstickval = pow(((float)joystick.joy2_y1 / (float)64 * (float)5), 2) ;
	if (joystick.joy2_y1 < 0) leftstickval = leftstickval * -1.0;

	rightstickval = pow(((float)joystick.joy2_x2 / (float)64 * (float)5), 2) ;
	if (joystick.joy2_x2 < 0) rightstickval = rightstickval * -1.0;
}

void joyval_joystick2rv() { // reverse mode on right joystick
	leftstickval = pow(((float)joystick.joy2_y2 / (float)64 * (float)5), 2) ;
	if (joystick.joy2_y2 < 0) leftstickval = leftstickval * -1.0;

	rightstickval = pow(((float)joystick.joy2_x1 / (float)64 * (float)5), 2) ;
	if (joystick.joy2_x1 < 0) rightstickval = rightstickval * -1.0;
}

//discover the values for use in the driving routine (using above methods)

void joyval_correct() {
	if ((normalctl) & (rev == 1)) {
		joyval_joystick1st();
		} else if ((normalctl) & (rev == -1)) {
		joyval_joystick1rv();
		} else if ((!normalctl) & (rev == 1)) {
		joyval_joystick2st();
		} else if ((!normalctl) & (rev == -1)) {
		joyval_joystick2rv();
		} else {
		PlayImmediateTone(500, 1);
	}
	leftstickval = leftstickval * rev;
	if (abs(leftstickval) < forwardbackwardthresh) leftstickval = 0;
	rightstickval = rightstickval * -1.0;
	if (abs(rightstickval) < leftrightthresh) rightstickval = 0;
}

float whichismax(float left, float right) {
	float returnval;
	if (left > right) returnval = left;
	if (right > left) returnval = right;
	if (right == left) returnval = (left + right) / 2;
	return returnval;
}

//MTR - move the motors - moves motors in normal scope
void movethemotors() {
	motor[motorLeftF] = leftmotorval;
	motor[motorLeftR] = leftmotorval;
	motor[motorRightF] = rightmotorval;
	motor[motorRightR] = rightmotorval;
}

//DJ - where the main driving happens (it calls the other stuff above)
void leftJoystickDrive() {
	if (joy1Btn(9)) { while (joy1Btn(9)) {PlayTone(4000, 1);} rev *= -1;}
	joyval_correct(); //put the correct joystick values in their varibles
	if (leftstickval > 0.0) {
		if (rightstickval > 0.0) {
			quadrent = 1;
			leftmotorval = leftstickval - rightstickval;
			rightmotorval = whichismax(leftstickval, rightstickval);
			} else {
			quadrent = 2;
			leftmotorval = whichismax(leftstickval, (rightstickval * -1.0));
			rightmotorval = leftstickval + rightstickval;
		}
		} else {
		if (rightstickval > 0.0) {
			quadrent = 3;
			leftmotorval = (whichismax((leftstickval * -1.0), rightstickval)) * -1.0;
			rightmotorval = leftstickval + rightstickval;
			} else {
			quadrent = 4;
			leftmotorval = leftstickval - rightstickval;
			rightmotorval = (whichismax((leftstickval * -1.0), (rightstickval * -1.0))) * -1.0;
		}
	}

	movethemotors();
}

// power control (used for slowing down driving motors to get greater accuracy)

void powercontrol () {
	if (normalctl) {
		if (joy1Btn(1)) power = 1;
		if (joy1Btn(2)) power = 2;
		if (joy1Btn(3)) power = 3;
		if (joy1Btn(4)) power = 4;
		} else {
		if (joy2Btn(1)) power = 1;
		if (joy2Btn(2)) power = 2;
		if (joy2Btn(3)) power = 3;
		if (joy2Btn(4)) power = 4;
	}
}

float leftWheelType, rightWheelType;

void moveMechaiumWheels(float x, float y)
{
	float xIn = x * 100;
	float yIn = y * 100;
	// Negate y for the joystick.
	//yIn = -yIn;


	leftWheelType = xIn + yIn;
	rightWheelType = -xIn + yIn;

	//leftWheelType = normalizeFloats(leftWheelType);
	//rightWheelType = normalizeFloats(rightWheelType);

	motor[motorLeftF] = leftWheelType;
	motor[motorRightF] = rightWheelType;
	motor[motorLeftR] = rightWheelType;
	motor[motorRightR] = leftWheelType;

}



void panning_joystick () {
	moveMechaiumWheels((float)joystick.joy1_x2 / 128, (float)joystick.joy1_y2 / 128);
}


bool withinThreshhold(int number) {
	return (abs(number) < 15);
}

bool joy1_used () {
	return (!withinThreshhold(joystick.joy1_x1)) || (!withinThreshhold(joystick.joy1_y1));
}

bool joy2_used () {
	return (!withinThreshhold(joystick.joy1_x2)) || (!withinThreshhold(joystick.joy1_y2));
}

void stopTheRobot() {
	motor[motorLeftF] = 0;
	motor[motorLeftR] = 0;
	motor[motorRightF] = 0;
	motor[motorRightR] = 0;
}

void base_movement () {
	if (joy1_used() & !joy2_used()) {
		leftJoystickDrive();
	} else if (joy2_used()) {
		panning_joystick();
	} else {
		stopTheRobot();
	}
}

int lastTiltMotorPos = 90;

void driverJoystick() {
	// lift motor
	if (joy1Btn(4) && !joy1Btn(2)) {
		motor[liftMotor] = -100;
	} else if (!joy1Btn(4) && joy1Btn(2)) {
		motor[liftMotor] = 50;
	} else if (joy1Btn(10)) {
		motor[liftMotor] = 100;
	} else if (!joy2Btn(4) && !joy2Btn(2)){
		motor[liftMotor] = 0;
	}

	// upper lift motor

	if (joy1Btn(5) && !joy1Btn(7)) {
		servo[upperLiftMotor] = 0;
	} else if (!joy1Btn(5) && joy1Btn(7)) {
		servo[upperLiftMotor] = 180;
	} else if (!joy2Btn(5) && !joy2Btn(7)) {
		servo[upperLiftMotor] = 122;
	}

	if (joy1Btn(6) && !joy1Btn(8)) {
		lastTiltMotorPos += 10;
	} else if (!joy1Btn(6) && joy1Btn(8)) {
		lastTiltMotorPos -= 10;
	}
	if (lastTiltMotorPos > 189) lastTiltMotorPos = 189;
	if (lastTiltMotorPos < 110) lastTiltMotorPos = 110;

	servo[trayTiltMotor] = lastTiltMotorPos;

	// flag motor

	if (joy1Btn(1) && !joy1Btn(3)) {
		motor[flagMotor] = -100;
	} else if (!joy1Btn(1) && joy1Btn(3)) {
		motor[flagMotor] = 100;
	} else if(!joy2Btn(1) && !joy2Btn(3)) {
		motor[flagMotor] = 0;
	}

}

void accessJoystick() {
	// lift motor
		if (joy2Btn(4) && !joy2Btn(2)) {
		motor[liftMotor] = -100;
	} else if (!joy2Btn(4) && joy2Btn(2)) {
		motor[liftMotor] = 50;
	} else if (joy2Btn(10)) {
		motor[liftMotor] = 100;
	}
	// upper lift motor

	if (joy2Btn(5) && !joy2Btn(7)) {
		servo[upperLiftMotor] = 0;
	} else if (!joy2Btn(5) && joy2Btn(7)) {
		servo[upperLiftMotor] = 180;
	}

	if (joy2Btn(6) && !joy2Btn(8)) {
		lastTiltMotorPos += 10;
	} else if (!joy2Btn(6) && joy2Btn(8)) {
		lastTiltMotorPos -= 10;
	}
	if (lastTiltMotorPos > 189) lastTiltMotorPos = 189;
	if (lastTiltMotorPos < 110) lastTiltMotorPos = 110;

	//servo[trayTiltMotor] = lastTiltMotorPos;

	// flag motor

	if (joy2Btn(1) && !joy2Btn(3)) {
		motor[flagMotor] = -100;
	} else if (!joy2Btn(1) && joy2Btn(3)) {
		motor[flagMotor] = 100;
	}
}


void appendages() {

	driverJoystick();
	accessJoystick();

}

// BC - battery check
void batterycheck () {
	if (externalBattery == -1) PlayImmediateTone(4000, 1);
}

void runLoopPause() {
	while (time1[T1] < 50) {
		wait1Msec(1);
	}
	time1[T1] = 0;
}

// TMT this is the main thread code
task main() {

	//waitForStart();
	isRunning = true; // sets isRunning to true, just in case it gets set to false

	while (isRunning) {
		getJoystickSettings(joystick);

		//if (joy1Btn(2)) {
		//	if (!firstController) PlayImmediateTone(4000, 1);
		//	firstController = true;
		//	} else {
		//	if (firstController) PlayImmediateTone(2000, 1);
		//	firstController = false;
		//}
		base_movement();

		batterycheck();

		appendages();
		//powercontrol();
		runLoopPause();

	}
}
