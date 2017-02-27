// Ryker Dial
// RMC 2017

// ros_backend_node.cpp
// This node is a driver for sending drive commands to the robot and retrieving sensor
//     data. 

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

// find_device()
// Given an identifier string, searches through an enumerated list of serial
//     ports for a matching device. Returns the first such device found, or
//     the null string if no matches.
std::string find_device(std::string id) {
	// Search through available ports for desired device
	std::vector<serial::PortInfo> devices_found = serial::list_ports();
	std::vector<serial::PortInfo>::iterator iter;

	std::string port;
	for(iter = devices_found.begin(); iter != devices_found.end(); ++iter) {
		if( (iter -> description).find(id) != std::string::npos ) {
			ROS_INFO("Found device '%s' at port '%s'",
				(iter->description).c_str(), 
				(iter->port).c_str()
			);
			return (iter->port);
		}
	}
	return "";
}


int main(int argc, char *argv[]) {
	serial_config.port = find_device("Arduino");
	if(serial_config.port != "") {
		ROS_INFO("Error: Could not find the Arduino");
	}

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
	else {
		ROS_INFO("Error connecting to port.");
	}

	ros::init(argc, argv, "ros_backend_node", ros::init_options::NoSigintHandler);
	ros::NodeHandle n;

	signal(SIGINT, sigint_handler); // Use our own signal handler so that we can
									//     shut down the robot safely.

	return 0;
}