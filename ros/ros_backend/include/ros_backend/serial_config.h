#include "serial/serial.h"
#include <string>

struct serial_config_t {
	std::string port;
	uint32_t baudrate;
	serial::Timeout timeout;
	serial::bytesize_t bytesize;
	serial::parity_t parity;
	serial::stopbits_t stopbits;
	serial::flowcontrol_t flowcontrol;
};