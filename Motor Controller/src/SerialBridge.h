#include "Arduino.h"

class SerialBridge {

	typedef void (*CommandReceivedCallback)(String command, String payload);

	public:
		SerialBridge();
		void start();
		void update();
		void setCommandReceivedCallback(CommandReceivedCallback callback);
		void sendCommand(String command, String payload);
		void sendCommand(String command, int payload);
		void sendCommand(String command, long payload);
		void sendCommand(String command);

	private:

		CommandReceivedCallback _commandReceivedCallback;

		void processCommand();
		void resetState();

		bool _readyForCommand;
		bool _isCommand;
		bool _waitingForCommand;
		bool _waitingForValue;
		String _commandString;
		String _payloadString;
};
