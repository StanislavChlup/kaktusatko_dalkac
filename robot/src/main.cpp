#include <Arduino.h>
#include "WiFi.h"
#include <iostream>
#include <vector>
#include "RBControl.hpp"
#include "robot.hpp"
#include <string.h>
#include <sstream>

// Dear maintainer:
// When I wrote this code, only I and God
// knew what it was.
// Now, only God knows!
// So if you are done trying to 'optimize'
// this routine (and failed),
// please increment the following counter
// as a warning
// to the next guy:
// total_hours_wasted_here = 9

using namespace rb;

//******************************** constants **************************/
constexpr std::size_t bufferSize = 64;
WiFiUDP udp;
const int trigPin1 = 16;
const int echoPin1 = 14;
const int trigPin2 = 27;
const int echoPin2 = 4;


struct KeyState {
	bool W = false;
	bool S = false;
	bool A = false;
	bool D = false;
};

KeyState keyState;

//********************************* math *****************************/
int lerp(int a, int b, float t) { return a + (b - a) * t; }

int clamp(int value, int min, int max) {
	if(value < min) {
		return min;
	} else if(value > max) {
		return max;
	} else {
		return value;
	}
}

//****************************** colors ******************************/
struct RGB {
	int r = 0;
	int g = 0;
	int b = 0;
	bool detect(int r, int g, int b) {
		bool detected = true;
		if(!(this->r - 25 < r && r < this->r + 25)) {detected = false;}
		if(!(this->g - 15 < g && g < this->g + 30)) {detected = false;}
		if(!(this->b - 15 < b && b < this->b + 30)) {detected = false;}
		return detected;
	}
};

RGB redWanted = {150, 60, 50};
RGB greenWanted = {46, 111, 70};
RGB blueWanted = {39, 89, 115};

int getColor(Robot& robot) {  // returnuje barvu kostky
	int r, g, b = 0;
	robot.color.getRGB(r, g, b);
	if(r > 110 && g < 85 && b < 80) return 0;
	if(greenWanted.detect(r, g, b)) return 1;
	if(blueWanted.detect(r, g, b)) return 2;
	if(r > 120) return 0;
	if(g > 110) return 1;
	if(b > 110) return 2;
	
	return -1;
}


//****************************** client ******************************/

void sendDataToClient(const char* data) {
    udp.beginPacket(udp.remoteIP(), udp.remotePort());
    udp.write(reinterpret_cast<const uint8_t*>(data), strlen(data));
    udp.endPacket();
}
void parseInput(char* buffer, Robot& r);
void readWifiInput(Robot& r) {
	char buffer[bufferSize] = {0};
	memset(buffer, 0, bufferSize);
	udp.parsePacket();
	if(udp.read(buffer, bufferSize) > 0) {
		parseInput(buffer, r);
	}
}

//****************************** utils ******************************/


std::string doubleToString(double value) {
	std::stringstream stream;
	stream << value;
	return stream.str();
}


//****************************** main ******************************/
void sendDistance(Robot& r){
	double dis = r.us.getDistance(trigPin1, echoPin1);
	std::cout << "distance" << dis << std::endl;
	sendDataToClient(('u' + doubleToString(dis)).c_str());
}

void sendColor(Robot& r){
	int red, green, blue;
	r.color.getRGB(red, green, blue);
	std::string data = 'c' 
	+ doubleToString(red) + ','
	+ doubleToString(green) + ','
	+ doubleToString(blue) + ','
	+ doubleToString(getColor(r));
	sendDataToClient(data.c_str());
}

void parseInput(char* buffer, Robot& r) {
	rb::Manager& man = rb::Manager::get();
	if(buffer[0] == 'u') {
		// valid packet input
		keyState.W = buffer[1] == '1';
		keyState.S = buffer[2] == '1';
		keyState.A = buffer[3] == '1';
		keyState.D = buffer[4] == '1';
		if(buffer[5] == '1') {
			man.servoBus().set(0, 140_deg);
		}
		if(buffer[6] == '1') {
			man.servoBus().set(0, 0_deg);
		}
		if(buffer[7] == '1') {
			sendDistance(r);
		}
		if(buffer[8] == '1') {
			sendColor(r);
		}
	}
}




void loop(Robot& r);







//***************************** setup *****************************/


void setupSensors(Robot& rob) {
	pinMode(trigPin1, OUTPUT);
	pinMode(echoPin1, INPUT);
	pinMode(trigPin2, OUTPUT);
	pinMode(echoPin2, INPUT);

	rob.color.init(23, 22, 5);
}

void setupServo() {
	rb::Manager& man = rb::Manager::get();
	man.initSmartServoBus(1, GPIO_NUM_17);
}

void setup() {
	//^ init wifi
	WiFi.softAP("kaktusatko", "dumbass0");
	printf("%s\n", WiFi.softAPIP().toString().c_str());
	udp.begin(80);

	//^ setup buttons
	pinMode(35, INPUT);
	pinMode(32, INPUT);

	

	//^ init motors
	Robot r(180, 33);
	
	//^ sensors & servos
	setupSensors(r);
	setupServo();

	rb::Manager& man = rb::Manager::get();
	man.initSmartServoBus(1, GPIO_NUM_17);
	std::cout << man.servoBus().pos(0).deg() << std::endl;
	man.servoBus().set(0, 0_deg);
	while(true){
		loop(r);
		

		delay(10);
	}
}

//***************************** loop *****************************/
int leftSpeed = 0;
int rightSpeed = 0;
void loop(Robot& r) {
	//^ wifi input
	readWifiInput(r);

	//! end of listening for inputs
	int wantLeftSpeed = 0;
	int wantRightSpeed = 0;

	if(keyState.W) {
		wantLeftSpeed += 85;
		wantRightSpeed += 85;
	}
	if(keyState.S) {
		wantLeftSpeed -= 85;
		wantRightSpeed -= 85;
	}
	if(keyState.A) {
		wantLeftSpeed -= 50;
		wantRightSpeed += 50;
	}
	if(keyState.D) {
		wantLeftSpeed += 50;
		wantRightSpeed -= 50;
	}

	leftSpeed = lerp(leftSpeed, wantLeftSpeed, 0.05);
	rightSpeed = lerp(rightSpeed, wantRightSpeed, 0.05);

	r.setSpeeds(clamp(leftSpeed, -100, 100), clamp(rightSpeed, -100, 100));
}





void loop() {}