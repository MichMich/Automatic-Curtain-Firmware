#include "SerialBridge.h"
#include "Config.h"

/* Constructor */

/**
 * Constructor for the SerialBridge object.
 */
SerialBridge::SerialBridge() {
	resetState();
}

/* Public Methods */

/**
 * Startup sequence.
 */
void SerialBridge::start() {
	Serial.begin(SERIAL_SPEED);
	resetState();
}

/**
 * Run the update loop. This checks for serial data.
 */
void SerialBridge::update() {
	while (Serial.available()) {
		char b = Serial.read();

		if (b == '\r') {
			continue;
		}

		if (b == '\n' || (_waitingForValue && b =='>')) {
			if (_isCommand) {
				processCommand();
			}
			resetState();
			continue;
		}

		if (!_readyForCommand && !_isCommand) {
			continue;
		}

		if (_readyForCommand && b == '>') {
			_isCommand = true;
			_readyForCommand = false;
			_waitingForCommand = true;
			continue;
		}

		if (_waitingForCommand && b == '=') {
			_waitingForCommand = false;
			_waitingForValue = true;
			continue;
		}

		if (_waitingForCommand) {
			_commandString.concat(b);
			continue;
		}

		if (_waitingForValue) {
			_payloadString.concat(b);
			continue;
		}
	}
}

void SerialBridge::processCommand() {
	_commandReceivedCallback(_commandString, _payloadString);
	// Serial.print("Command: ");
	// Serial.print(_commandString);
	// Serial.print(" - Value: ");
	// Serial.println(_payloadString);
}

void SerialBridge::resetState() {
	_readyForCommand = true;
	_isCommand = false;
	_waitingForCommand = false;
	_waitingForValue = false;
	_commandString = "";
	_payloadString = "";
}

/**
 * Set the callback function for when a new target is received.
 * @param callback Callback function.
 */
void SerialBridge::setCommandReceivedCallback(CommandReceivedCallback callback) {
	_commandReceivedCallback = callback;
}

/**
 * Send a command.
 * @param command The command to send.
 * @param value The value to send.
 */
void SerialBridge::sendCommand(String command, String payload) {
	Serial.print(">");
	Serial.print(command);

	if (payload != "") {
		Serial.print("=");
		Serial.print(payload);
	}

	Serial.println();
}

void SerialBridge::sendCommand(String command, int payload) {
	sendCommand(command, String(payload));
}

void SerialBridge::sendCommand(String command, long payload) {
	sendCommand(command, String(payload));
}

void SerialBridge::sendCommand(String command) {
	sendCommand(command, "");
}
