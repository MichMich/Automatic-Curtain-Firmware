#include "Feedback.h"


Feedback::Feedback(uint8_t ledPin, uint8_t speakerPin) {
	pinMode(ledPin, OUTPUT);
	pinMode(speakerPin, OUTPUT);

	_ledPin = ledPin;
	_speakerPin = speakerPin;
}

void Feedback::starting() {
	digitalWrite(_ledPin, HIGH);
	tone(_speakerPin, 1000);
	delay(100);
	digitalWrite(_ledPin, LOW);
	tone(_speakerPin, 2000);
	delay(100);
	digitalWrite(_ledPin, HIGH);
	tone(_speakerPin, 4000);
	delay(50);
	digitalWrite(_ledPin, LOW);
	noTone(_speakerPin);
	delay(50);
	digitalWrite(_ledPin, HIGH);
	tone(_speakerPin, 4000);
	delay(50);
	noTone(_speakerPin);
	digitalWrite(_ledPin, LOW);
}

void Feedback::ok() {
	tone(_speakerPin, 1000);
	delay(50);
	noTone(_speakerPin);
	delay(50);
	tone(_speakerPin, 1000);
	delay(50);
	noTone(_speakerPin);
	delay(50);
	tone(_speakerPin, 4000);
	delay(100);
	noTone(_speakerPin);
}

void Feedback::warning() {
	digitalWrite(_ledPin, HIGH);
	delay(25);
	digitalWrite(_ledPin, LOW);
	tone(_speakerPin, 500);
	delay(475);
	digitalWrite(_ledPin, HIGH);
	delay(25);
	digitalWrite(_ledPin, LOW);
	noTone(_speakerPin);
	delay(475);
}
