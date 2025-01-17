#pragma once
#include "RBControl.hpp"

constexpr rb::MotorId MotorL = rb::MotorId::M1;
constexpr rb::MotorId MotorR = rb::MotorId::M4;

/**
* @brief lerp
*
* @param a - float start
* @param b - float end
* @param t - float factor
*/
inline float lerp(float a, float b, float t) { return a + t * (b - a); }

inline int clampSpeed(int speed) { return speed < -100 ? -100 : speed > 100 ? 100 : speed; }
class Robot {
public:
	Robot(double w, double h);

	int getEncoder(rb::MotorId id);
	void const go2(int dis, int speed);
	void const go3(int dis, int startSpeed, int endSpeed);
	void const turn(double angle, int speed, int turnRadius = 0);
	void const setSpeeds(int speedL, int speedR);
	void const setSpeeds(int speed);
	void const srovnat(int speed = 40, int timeout = 2);
	void const setup();

public:
	class US {
	public:
		double const getDistance(int trig, int echo);
	};
	US us;
	class Color {
	public:
		int TCS_SDA_pin;
		int TCS_SCL_pin;
		int TCS_led_pin;
		void init(int sda, int scl, int led);
		void const getRGB(int& r, int& g, int& b);
	};
	Color color;
	class Klepeta {
	public:
		void const close();
		void const open();
	};
	Klepeta klepeta;

private:
	void drive(int dis, int speed, rb::MotorId id, std::function<void(rb::Encoder&)> callback = nullptr);

public:
	const double baseWidth;
	const double wheelRadius;
	const double mmToEnc = 1000. / (2. * 3.1415926535 * wheelRadius);;
	double motorSpeedDiff = 1;
	
	const double leftFix = 1;
	rb::Manager& man = rb::Manager::get();
};