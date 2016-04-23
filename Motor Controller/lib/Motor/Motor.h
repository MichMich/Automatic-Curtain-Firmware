enum Direction {
	Forward,
	Backward,
	Off
};

class Motor
{
  	public:
	    Motor();
		void update();
		void run(Direction direction);
		void stop();
		void calibrate();
		void setStepSize(int stepSize);
		void setSpeed(int speed);
		void setTargetPosition(int targetPosition);
		bool isRunning();

  	private:
		volatile int _tick;
		volatile int _speed;
		volatile bool _run;
	    volatile bool _direction;
		volatile bool _calibrating;
		volatile bool _manualMove;
		volatile long _currentStep;
		volatile long _totalSteps;
		volatile long _targetStep;
		volatile long _ticksPerStep;
		volatile long _ticksPerSecond;
		volatile long _startStep;

		void step();
		void countStep();
		void checkIfTargetReached();
		void updateSpeed();
		bool allowDirection(bool direction);
};
