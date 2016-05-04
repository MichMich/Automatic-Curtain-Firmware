#include "Arduino.h"

class Feedback
{
  	public:
		Feedback(uint8_t ledPin, uint8_t speakerPin);
		void starting();
		void ok();
		void warning();

	private:
		int8_t _ledPin;
		int8_t _speakerPin;
};
