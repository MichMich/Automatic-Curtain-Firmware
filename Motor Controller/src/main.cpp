#include "Arduino.h"
#include <Motor.h>
#include <TimerOne.h>
#include "clickButton.h"
#include "Feedback.h"
#include "config.h"


Motor motor = Motor();
ClickButton buttonClose(BUTTON_CLOSE, LOW, CLICKBTN_PULLUP);
ClickButton buttonOpen(BUTTON_OPEN, LOW, CLICKBTN_PULLUP);
Feedback feedback = Feedback(LED, SOUND);

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
	Serial.println("Stop manual move curtain ...");
	motor.run(Off);
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
	Serial.begin(9600);

	Timer1.initialize(INTERRUPT_INTERVAL);
	Timer1.attachInterrupt(interruptCallback);

	buttonClose.setReleaseCallback(releaseCallback);
	buttonClose.longClickTime = LONGCLICKTIME;
	buttonOpen.setReleaseCallback(releaseCallback);
	buttonOpen.longClickTime = LONGCLICKTIME;

	pinMode(SOUND, OUTPUT);

	feedback.starting();
	delay(100);
	motor.calibrate();
	feedback.ok();
}

/**
 * Loop the application.
 */
void loop()
{
	buttonOpen.Update();
	buttonClose.Update();

	if (Serial.available()) {
		int newTarget = Serial.parseInt();
		if (newTarget != 0) {
			motor.setTargetPosition(newTarget);
		}
	}

	if (!motor.isRunning()) {
		if (buttonClose.clicks == 1) {
			Serial.println("Fully close curtain ...");
			motor.setTargetPosition(RESOLUTION);
		} else if (buttonOpen.clicks == 1) {
			Serial.println("Fully open curtain ...");
			motor.setTargetPosition(0);
		}
	} else {
		if (buttonClose.clicks > 0 || buttonOpen.clicks > 0) {
			motor.stop();
		}
	}

	if (buttonOpen.clicks == -1 && buttonClose.depressed) {
		Serial.println("Restart!");
		delay(100);
		reset();
	} else {
		if (buttonClose.clicks == -1) {
			Serial.println("Manual close curtain ...");
			motor.run(Forward);
		} else if (buttonOpen.clicks == -1) {
			Serial.println("Manual open curtain ...");
			motor.run(Backward);
		}
	}

	delay(10);
}
