#include "robot.hpp"

#include <Arduino.h>

#include <chrono>
#include <iostream>
#include <thread>

#include "RBControl.hpp"

void msdelay(int ms) { std::this_thread::sleep_for(std::chrono::milliseconds(ms)); }

Robot::Robot(double w, double r) : baseWidth(w), wheelRadius(r) { man.install(rb::ManagerInstallFlags::MAN_DISABLE_MOTOR_FAILSAFE); };

void Robot::drive(int dis, int speed, rb::MotorId id, std::function<void(rb::Encoder&)> callback) {
	const float mmtoenc = 1000 / (2 * PI * wheelRadius);
	man.motor(id).drive(dis * mmtoenc, speed, callback);
}


int Robot::getEncoder(rb::MotorId id) {
	switch(id){
		case MotorL: return man.motor(MotorL).encoder()->value();
		case MotorR: return -man.motor(MotorR).encoder()->value();
		default:
			std::cout << "motor not found" << std::endl;
			return 0;
	}
}

void const Robot::go2(int dis, int speed) {
	int startL = getEncoder(MotorL);
	int startR = getEncoder(MotorR);
	int leftEnc = 0;
	int rightEnc = 0;
	int timeout = 0;
	while(abs(dis * mmToEnc) > abs(leftEnc)) {
		leftEnc = getEncoder(MotorL) - startL;
		rightEnc = getEncoder(MotorR) - startR;
		int diff = (rightEnc - leftEnc)/4;
		setSpeeds(speed + diff, speed - diff);
		
		timeout++;
		if(timeout > dis) break;
		msdelay(10);
	}
	setSpeeds(0);
}


void const Robot::go3(int dis, int startSpeed, int endSpeed) {
	int startL = getEncoder(MotorL);
	int startR = getEncoder(MotorR);
	int leftEnc = 0;
	int rightEnc = 0;
	int speed = startSpeed;
	int timeout = 0;
	while(abs(dis * mmToEnc) > abs(leftEnc)) {
		leftEnc = getEncoder(MotorL) - startL;
		rightEnc = getEncoder(MotorR) - startR;
		int diff = (rightEnc - leftEnc)/4;
		speed = lerp(startSpeed, endSpeed, abs(leftEnc) / (dis * mmToEnc));
		setSpeeds(speed + diff, speed - diff);
		
		timeout++;
		if(timeout > dis) break;
		msdelay(10);
	}
	setSpeeds(0);
}


void const Robot::turn(double angle, int speed, int turnRadius) {
	angle = angle * PI / 180;
	int startL = getEncoder(MotorL);
	int startR = getEncoder(MotorR);
	double disl = (turnRadius + baseWidth / 2.) * angle;
	double disr = (turnRadius - baseWidth / 2.) * angle;
	double aspect = disl / disr;
	int leftEnc = 0;
	int rightEnc = 0;


	int timeout = 0;
	while(true){
		leftEnc = getEncoder(MotorL) - startL;
		rightEnc = getEncoder(MotorR) - startR;
		
		if(angle>0){
			double diff = turnRadius < baseWidth / 2. ? 0 : (rightEnc * aspect - leftEnc) / 4;
			setSpeeds(speed + diff, speed/aspect - diff);
			if(abs(disl * mmToEnc) <= abs(leftEnc)) break;
		}else{
			double diff = turnRadius < baseWidth / 2. ? 0 : (rightEnc - leftEnc * aspect) / 4;
			setSpeeds(speed/aspect + diff, speed - diff);
			if(abs(disl * mmToEnc) <= abs(rightEnc)) break;
		}

		timeout++;
		//if(timeout > disl) break;
		msdelay(10);
	}
	setSpeeds(0);
	
}

void const Robot::setSpeeds(int speedL, int speedR) {
	man.motor(MotorL).power(clampSpeed(speedL * leftFix));
	man.motor(MotorR).power(clampSpeed(-speedR));
}

void const Robot::setSpeeds(int speed) {
	man.motor(MotorL).power(clampSpeed(speed * leftFix));
	man.motor(MotorR).power(clampSpeed(-speed));
}

void const Robot::Klepeta::close() {
	rb::Manager& man = rb::Manager::get();
	man.servoBus().set(0, rb::Angle::deg(20));
}
void const Robot::Klepeta::open() {
	rb::Manager& man = rb::Manager::get();
	man.servoBus().set(0, rb::Angle::deg(100));
}

constexpr double SOUND_SPEED = 0.34;  // mm / uS
double const Robot::US::getDistance(int trig, int echo) {
	digitalWrite(trig, LOW);  // reset
	delayMicroseconds(2);
	digitalWrite(trig, HIGH);  // send pulse
	delayMicroseconds(10);
	digitalWrite(trig, LOW);
	int duration = pulseIn(echo, HIGH);  // get duration
	double distance = duration * SOUND_SPEED / 2;
	return distance;
}

void const Robot::srovnat(int speed, int timeout){
	int startL = getEncoder(MotorL);
	int startR = getEncoder(MotorR);
	int leftEnc = 0;
	int rightEnc = 0;
	int time = 0;
	while(digitalRead(32) == 1) {
		leftEnc = getEncoder(MotorL) - startL;
		rightEnc = getEncoder(MotorR) - startR;
		int diff = (rightEnc - leftEnc)/4;
		setSpeeds(-speed + diff, -speed - diff);
		time++;
		if(time > timeout*100) break;
		msdelay(10);
	}

	setSpeeds(0);
}

//* color sensor
#include <Wire.h>  // must be manually included before #include "Adafruit_TCS34725.h"

#include "Adafruit_TCS34725.h"

Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_1X);

void Robot::Color::init(int sda, int scl, int led) {
	TCS_SDA_pin = sda;
	TCS_SCL_pin = scl;
	TCS_led_pin = led;
	pinMode(TCS_SDA_pin, PULLUP);
	pinMode(TCS_SCL_pin, PULLUP);
	pinMode(TCS_led_pin, GPIO_MODE_OUTPUT);
	Wire1.begin(TCS_SDA_pin, TCS_SCL_pin, 100000);
	if(!tcs.begin(TCS34725_ADDRESS, &Wire1)) {
		std::cout << "Can not connect to the RGB sensor" << std::endl;
	}
	digitalWrite(TCS_led_pin, 1);
}

// inicializace RGB
void const Robot::Color::getRGB(int& r, int& g, int& b) {
	Wire1.begin(TCS_SDA_pin, TCS_SCL_pin, 100000);
	digitalWrite(TCS_led_pin, 1);
	if(!tcs.begin(TCS34725_ADDRESS, &Wire1)) {
		std::cout << "Can not connect to the RGB sensor" << std::endl;
		r = g = b = -1;
		return;
	}
	// precteni a poslani RGB
	float rr, gg, bb = 0;
	tcs.getRGB(&rr, &gg, &bb);
	r = rr;
	g = gg;
	b = bb;
}