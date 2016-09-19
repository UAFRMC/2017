#include "ros/ros.h"
#include "serial/serial.h"
#include "ros_backend/serial_config.h"

#include <vector>

serial_config_t serial_config = {
	"/dev/ttyACM0", 			// Actual port is set during port enumeration
	57600,						// baudrate
	serial::Timeout(),			
	serial::eightbits,			// # of data bits
	serial::parity_none,
	serial::stopbits_one,
	serial::flowcontrol_none
};

// void enumerate_ports() {
// 	vector<serial::PortInfo> devices_found = serial::list_ports();

// 	vector<serial::PortInfo>::iterator iter = devices_found.begin();

// 	while( iter != devices_found.end() )
// 	{
// 		serial::PortInfo device = *iter++;

// 		printf( "(%s, %s, %s)\n", device.port.c_str(), device.description.c_str(),
//      device.hardware_id.c_str() );
// 	}
// }


int main(int argc, char **argv) {
	serial::Serial sp(
		serial_config.port,
		serial_config.baudrate,
		serial_config.timeout,
		serial_config.bytesize,
		serial_config.parity,
		serial_config.stopbits,
		serial_config.flowcontrol
	);
}