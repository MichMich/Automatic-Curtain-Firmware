#include "Motor.h"
#include "config.h"
#include <TimerOne.h>

/**
 * Constructor
 */
Motor::Motor() {
	pinMode(DIR, OUTPUT);
	pinMode(STEP, OUTPUT);
	pinMode(SLEEP, OUTPUT);
	pinMode(RESET, OUTPUT);
	pinMode(M2, OUTPUT);
	pinMode(M1, OUTPUT);
	pinMode(M0, OUTPUT);
	pinMode(DISABLE, OUTPUT);

	pinMode(ENDSTOP_START, INPUT_PULLUP);
	pinMode(ENDSTOP_END, INPUT_PULLUP);

	digitalWrite(SLEEP, HIGH);
	digitalWrite(RESET, HIGH);
	digitalWrite(DISABLE, HIGH);

	_tick = 0;
	_run = false;
	_calibrating = false;
	_currentStep = 0;
	_totalSteps = 0;
	_targetStep = 0;

	_ticksPerSecond = 1000L / INTERRUPT_INTERVAL * 100;

	setStepSize(STEPSIZE);
}

/**
 * Checks to see if a step is nessecery.
 * Bases on the passed number of ticks.
 */
void Motor::update() {

	if (_tick >= _ticksPerStep) {
		_tick = 0;
		// If we should not run, or we are not allowed to run due to an
		// endstop. We leave the step funtion.
		if (!_run || !allowDirection(_direction)) {
			// Not running or not allowed. Abort step.
			return;
		}
		step();
		updateSpeed();
	} else {
		_tick ++;
	}
}

/**
 * Toggles the running mode.
 * @param direction The direction of the motor.
 */
void Motor::run(Direction direction) {
	if (!_run) {
		setSpeed(DEFAULTSPEED);
	}

	_manualMove = true;

	switch (direction) {
		case Forward:
			_run = true;
			_direction = FORWARD;
		break;
		case Backward:
			_run = true;
			_direction = BACKWARD;
		break;
		case Off:
			_run = false;
		break;

	}
	digitalWrite(DIR, _direction);
	digitalWrite(DISABLE, !_run);
}

/**
 * Stops the motor.
 */
void Motor::stop() {
	run(Off);
}

/**
 * Starts the calibration process.
 */
void Motor::calibrate() {
	_calibrating = true;
	run(Backward);
	Serial.println("Start calibration ... ");
	while (_calibrating) {
		// Wait while calibrating.
	}
	Serial.print("Calibration done! ");
	Serial.print(_totalSteps);
	Serial.println(" steps.");
}

/**
 * Set stepsize for the motor.
 * If you want to set, 1/4 step, give 4 as stepSize.
 * @param stepSize Size of the step: 1/stepSize.
 */
void Motor::setStepSize(int stepSize) {
	if (stepSize >= 32) {
		// Step size: 1/32
		digitalWrite(M0, HIGH);
		digitalWrite(M1, HIGH);
		digitalWrite(M2, HIGH);
		return;
	}
	if (stepSize >= 16) {
		// Step size: 1/16
		digitalWrite(M0, LOW);
		digitalWrite(M1, LOW);
		digitalWrite(M2, HIGH);
		return;
	}
	if (stepSize >= 8) {
		// Step size: 1/8
		digitalWrite(M0, HIGH);
		digitalWrite(M1, HIGH);
		digitalWrite(M2, LOW);
		return;
	}
	if (stepSize >= 4) {
		// Step size: 1/4
		digitalWrite(M0, LOW);
		digitalWrite(M1, HIGH);
		digitalWrite(M2, LOW);
		return;
	}
	if (stepSize >= 2) {
		// Step size: 1/2
		digitalWrite(M0, HIGH);
		digitalWrite(M1, LOW);
		digitalWrite(M2, LOW);
		return;
	}

	// Step size: 1
	digitalWrite(M0, LOW);
	digitalWrite(M1, LOW);
	digitalWrite(M2, LOW);
}

/**
 * Set the speed of the motor.
 * @param speed The amount of steps per millisecond.
 */
void Motor::setSpeed(int speed) {
	_speed = speed;
	_ticksPerStep = _ticksPerSecond / speed;
}

/**
 * Set the desired target position of the motors.
 * This can be a value between 0 and the value of RESOLUTION.
 * @param targetPosition [description]
 */
void Motor::setTargetPosition(int targetPosition) {
	if (targetPosition < 0) targetPosition = 0;
	if (targetPosition > RESOLUTION - 1) targetPosition = RESOLUTION - 1;
	_targetStep = map(targetPosition, 0, RESOLUTION - 1, 0, _totalSteps);
	_startStep = _currentStep;

	bool oldDirection = _direction;

	if(_targetStep > _currentStep) {
		run(Forward);
	} else if (_targetStep < _currentStep) {
		run(Backward);
	} else {
		run(Off);
	}

	if (oldDirection != _direction) {
		setSpeed(MINSPEED);
	}

	_manualMove = false;

	Serial.print("Total steps: ");
	Serial.print(_totalSteps);
	Serial.print(" - Current step: ");
	Serial.print(_currentStep);
	Serial.print(" - Target step: ");
	Serial.println(_targetStep);
}

/**
 * Returns if the motor is currently running.
 * @return If motor is currently running.
 */
bool Motor::isRunning() {
	return _run;
}

// Private Methods

/**
 * Take one step.
 */
void Motor::step() {
	// Take a step.
	digitalWrite(STEP, HIGH);
	digitalWrite(STEP, LOW);

	// Count the step to make sure we know where we are.
	countStep();
}

/**
 * Count a step, and then invoke the checkIfTargetReached() method.
 */
void Motor::countStep() {
	_currentStep += (_direction == FORWARD) ? 1 : -1;

	// If the currentStep exceeds the bounds,
	// Adjust the values to make up for any missed steps.
	if (_currentStep < 0) _currentStep = 0;
	if (_currentStep > _totalSteps) _totalSteps = _currentStep;

	checkIfTargetReached();
}

/**
 * Check if the target is reached. If so, stop the motor.
 */
void Motor::checkIfTargetReached() {
	if (_currentStep == _targetStep && !_calibrating) {
		run(Off);
	}
}

/**
 * Update the speed of the motor to create acceleration and deceleration.
 */
void Motor::updateSpeed() {
	if (_calibrating || _manualMove) {
		setSpeed(DEFAULTSPEED);
		return;
	}

	long progress;
	long progressResolution = 1000;
	long totalStepsInRun = labs(_targetStep - _startStep);
	long stepsToAccelerateDecelerate =  totalStepsInRun / ACCELDECELPORTION;
	long stepsTaken = labs(_currentStep - _startStep);

	bool accelerating = totalStepsInRun - stepsTaken >= stepsToAccelerateDecelerate;

	if (accelerating) {
		progress = progressResolution * stepsTaken / stepsToAccelerateDecelerate ;
	} else {
		progress = progressResolution * (totalStepsInRun - stepsTaken) / stepsToAccelerateDecelerate;
	}

	if (progress > progressResolution) progress = progressResolution;
	int newSpeed = MINSPEED + (MAXSPEED - MINSPEED) * progress / progressResolution;

	if (newSpeed - _speed > MAXACCELERATION) newSpeed = _speed + MAXACCELERATION;

	if (accelerating && newSpeed < _speed) newSpeed = _speed;
	if (!accelerating && newSpeed > _speed) newSpeed = _speed;

	setSpeed(newSpeed);
}

/**
 * Check if the motor is allowed to travel in a direction.
 * @param  direction The direction to check.
 * @return           If the motor is allowed to trvel in that direction.
 */
bool Motor::allowDirection(bool direction) {
	bool directionAllowed = (direction == BACKWARD) ? digitalRead(ENDSTOP_START) : digitalRead(ENDSTOP_END);
	if (_calibrating) {
		if (!directionAllowed) {
			if (!_direction == FORWARD) {
				run(Forward);
				_currentStep = 0;
			} else {
				run(Off);
				_calibrating = false;
				_totalSteps = _currentStep;
			}
		}
	} else {
		if (!directionAllowed) {
			run(Off);
		}
	}

	return directionAllowed;
}
