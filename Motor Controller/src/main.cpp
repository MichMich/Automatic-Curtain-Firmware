#include <Arduino.h>
#include <TimerOne.h>
#include <clickButton.h>
#include <elapsedMillis.h>
#include "Config.h"
#include "Motor.h"
#include "Feedback.h"
#include "SerialBridge.h"


Motor motor = Motor();
ClickButton buttonClose(BUTTON_CLOSE, LOW, CLICKBTN_PULLUP);
ClickButton buttonOpen(BUTTON_OPEN, LOW, CLICKBTN_PULLUP);
Feedback feedback = Feedback(LED, SOUND);
SerialBridge bridge = SerialBridge();
elapsedMillis broadcastTimer;

int lastCurrentPosition = -1;
int lastTargetPosition = -1;

int lastBroadcastedTarget = -1;
int lastBroadcastedState = -1;

/**
 * Dirty Reset Method.
 * Doesn't reset any in- or outputs, but starts from 0.
 */
void reset() {
  asm volatile ("  jmp 0");
}

/**
 * The method that is being called by the interrupt.
 */
void interruptCallback() {
	motor.update();
}

/**
 * What to do when a long press is released.
 */
void releaseCallback() {
	//Stop manual move curtain ...
	motor.run(Off);
	bridge.sendCommand(COMMAND_TARGET, motor.getTargetPosition());
	bridge.sendCommand(COMMAND_STATE, motor.getCurrentPosition());
}

/**
 * What to do when a new target is recieved from the bridge.
 * @param target new target.
 */
void commandReceivedCallback(String command, String payload) {
	if (command == COMMAND_TARGET && !motor.isManualControlled()) {
		motor.setTargetPosition(payload.toInt());
	}
}

/**
 * Panic method.
 * Will be connected to the FAULT sensor.
 */
void panic() {
	while(true) {
		feedback.warning();
	}
}

/**
 * Setup the application.
 */
void setup()
{
	Timer1.initialize(INTERRUPT_INTERVAL);
	Timer1.attachInterrupt(interruptCallback);

	buttonClose.setReleaseCallback(releaseCallback);
	buttonClose.longClickTime = LONGCLICKTIME;
	buttonOpen.setReleaseCallback(releaseCallback);
	buttonOpen.longClickTime = LONGCLICKTIME;

	bridge.start();
	bridge.setCommandReceivedCallback(commandReceivedCallback);

	feedback.starting();
	delay(100);
	motor.calibrate();
	bridge.sendCommand(COMMAND_TOTAL_STEPS, motor.getTotalSteps());
	feedback.ok();
}

/**
 * Loop the application.
 */
void loop()
{
	buttonOpen.Update();
	buttonClose.Update();
	bridge.update();

	int currentPosition = motor.getCurrentPosition();

	if (!motor.isRunning()) {
		if (buttonClose.clicks > 0) {
			//Fully close curtain ...
			motor.setTargetPosition(RESOLUTION);
			switch (buttonClose.clicks) {
				case 1:
					bridge.sendCommand(COMMAND_TARGET, RESOLUTION);
					break;
				case 2:
					bridge.sendCommand(COMMAND_PAIR_TARGET, RESOLUTION);
					break;
				case 3:
					bridge.sendCommand(COMMAND_ALL_TARGET, RESOLUTION);
					break;
			}

		} else if (buttonOpen.clicks > 0) {
			//Fully open curtain ...
			motor.setTargetPosition(0);
			switch (buttonOpen.clicks) {
				case 1:
					bridge.sendCommand(COMMAND_TARGET, 0);
					break;
				case 2:
					bridge.sendCommand(COMMAND_PAIR_TARGET, 0);
					break;
				case 3:
					bridge.sendCommand(COMMAND_ALL_TARGET, 0);
					break;
			}
		}
	} else {
		if (buttonClose.clicks > 0 || buttonOpen.clicks > 0) {
			motor.setTargetPosition(currentPosition);
			bridge.sendCommand(COMMAND_TARGET, currentPosition);
		}
	}

	if (buttonOpen.clicks == -1 && buttonClose.depressed) {
		//Restart!
		bridge.sendCommand(COMMAND_RESET);
		delay(100);
		reset();
	} else {
		if (buttonClose.clicks == -1) {
			//Manual close curtain ...
			motor.run(Forward);
		} else if (buttonOpen.clicks == -1) {
			//Manual open curtain ...
			motor.run(Backward);
		}
	}

	// Send new state to the bridge if cahnged.
	if (currentPosition != lastCurrentPosition) {
		lastCurrentPosition = currentPosition;
		bridge.sendCommand(COMMAND_STATE, currentPosition);
	}

	delay(50);
}
