#include <stdint.h>
#include <stddef.h>

#define RH_PLATFORM_SIMULATOR 0
#define RH_PLATFORM_ARDUINO 1
#define RH_PLATFORM RH_PLATFORM_ARDUINO

#define RH_NRF24_MAX_MESSAGE_LEN 28
class RHGenericSPI;
class RHGenericDriver {
};

class RH_NRF24 : public RHGenericDriver {
public:
	RH_NRF24(uint8_t chipEnablePin, uint8_t slaveSelectPin);
};

class RHReliableDatagram {
public:
	RHReliableDatagram(RHGenericDriver &driver, uint8_t thisAddress = 0);
	bool init();
	void setThisAddress(uint8_t new_addr);
	bool available();
	bool sendtoWait(uint8_t *buf, uint8_t len, uint8_t address);
	bool recvfromAck(uint8_t *buf, uint8_t *len, uint8_t *from = NULL,
			uint8_t *to = NULL, uint8_t *id = NULL,
			uint8_t *flags = NULL);
protected:
	uint8_t addr;
};
