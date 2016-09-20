#include "ros/ros.h"
#include "serial/serial.h"
#include "ros_backend/serial_config.h"

#include <vector>
#include <string>
#include <iostream>
#include <chrono>
#include <signal.h>

serial_config_t serial_config = {
	"", 						// Actual port is set during port enumeration
	115200,						// baudrate
	serial::Timeout::simpleTimeout(1000),			
	serial::eightbits,			// # of data bits
	serial::parity_none,
	serial::stopbits_one,
	serial::flowcontrol_none
};

void sigint_handler(int signal) {
	ros::shutdown();
}

std::string find_device(std::string id) {
	// Search through available ports for desired device
	std::vector<serial::PortInfo> devices_found = serial::list_ports();
	std::vector<serial::PortInfo>::iterator iter;

	std::string port;
	for(iter = devices_found.begin(); iter != devices_found.end(); ++iter) {
		if( (iter -> description).find(id) != std::string::npos ) {
			ROS_INFO("Found device '%s' at port '%s'",(iter->description).c_str(), (iter->port).c_str());
			return (iter->port);
		}
	}
	return "";
}

// #define START 128;
// #define STOP 173;
// #define SAFE 131;

int main(int argc, char **argv) {
	serial_config.port = find_device("Arduino");

	// Open the serial port
	ROS_INFO("Connecting to port '%s'", serial_config.port.c_str());
	serial::Serial sp(
		serial_config.port,
		serial_config.baudrate,
		serial_config.timeout,
		serial_config.bytesize,
		serial_config.parity,
		serial_config.stopbits,
		serial_config.flowcontrol
	);
	if(sp.isOpen()) {
		ROS_INFO("Connecting to port '%s' succeeded.", serial_config.port.c_str());
	}

	ros::init(argc, argv, "ros_backend_node", ros::init_options::NoSigintHandler);
	ros::NodeHandle n;

	signal(SIGINT, sigint_handler);
	// uint8_t START[1] = {128};
	// uint8_t STOP[1] = {173};
	// uint8_t SAFE[1] = {131};
	// uint8_t FULL[1] = {132};

	// std::chrono::time_point<std::chrono::system_clock> start,now;
	// ROS_INFO("Wrote %lu bytes", sp.write(START,1));
	// ROS_INFO("Wrote %lu bytes", sp.write(FULL,1));
	// start = std::chrono::system_clock::now();
	// uint8_t drive_packet[5] = {145, 0, 200, 0, 200};
	// uint8_t main_brush_packet[4] = {144, 127, 0, 0};
	// ROS_INFO("Wrote %lu bytes", sp.write(main_brush_packet,4));
	// //ROS_INFO("Wrote %lu bytes", sp.write(drive_packet, 5));
	// while(true) {
	// 	now = std::chrono::system_clock::now();
	// 	std::chrono::duration<double> elapsed_seconds = now - start;
	// 	if(elapsed_seconds.count() > 5) {
	// 		sp.write(STOP, 1);
	// 		return 0;
	// 	}
	// 	else {
	// 		ROS_INFO("Wrote %lu bytes", sp.write(drive_packet, 5));
	// 	}
	// }



	//ros::init(argc, argv, "ros_backend");
	//ros::NodeHandle n;
	return 0;
}