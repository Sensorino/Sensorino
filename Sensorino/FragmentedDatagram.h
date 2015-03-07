template <typename T, int fragMax, int totalMax>
class FragmentedDatagram : public T {
public:
	FragmentedDatagram(RHGenericDriver& driver, uint8_t thisAddress = 0) :
		T(driver, thisAddress) {}

#define FRAG_CONTINUE_FLAG	0x80
	bool sendtoWait(uint8_t *buf, uint8_t len, uint8_t address) {
		uint8_t fragBuf[fragMax];
		uint8_t fragLen;

		while (len) {
			fragLen = len < fragMax - 1 ? len : (fragMax - 1);
			len -= fragLen;

			/* Fragment header */
			memcpy(fragBuf, buf, fragLen);
			fragBuf[fragLen] = msgId++;
			if (len)
				fragBuf[fragLen] |= FRAG_CONTINUE_FLAG;
			else
				fragBuf[fragLen] &= ~FRAG_CONTINUE_FLAG;

			buf += fragLen;

			if (!T::sendtoWait(fragBuf, fragLen + 1, address))
				return 0;
		}

		return 1;
	}

	bool recvfromAck(uint8_t *buf, uint8_t *len, uint8_t *from = NULL,
			uint8_t *to = NULL, uint8_t *id = NULL,
			uint8_t *flags = NULL) {
		uint8_t fragLen;
		uint8_t *fragBuf;
		uint8_t newMsgId, curMsgId;
		uint16_t count;

		*len = 0;
		fragBuf = buf;

		/* TODO:
		 *  # check that from, to, id of each fragment match.
		 *  # use a FRAG_INIT_FLAG too.
		 *  # do reassembly inside .available(), don't block.
		 */

		while (1) {
			if (!T::recvfromAck(fragBuf, &fragLen, from, to, id,
						flags) || fragLen < 2)
				return 0;

			newMsgId = fragBuf[--fragLen];

			/* Check that the Id sequence is continuous */
			if (*len && (newMsgId & ~FRAG_CONTINUE_FLAG) !=
					curMsgId) {
				/* No, it's a start of a new sequence */
				*len = 0;
				for (uint8_t i = 0; i < fragLen; i++)
					buf[i] = fragBuf[i];
				fragBuf = buf;
			}

			*len += fragLen;

			if (!(newMsgId & FRAG_CONTINUE_FLAG))
				return 1;
			if (*len > totalMax - fragMax) /* Out of buffer space */
				return 0;

			fragBuf += fragLen;
			curMsgId = (newMsgId + 1) & ~FRAG_CONTINUE_FLAG;

			for (count = 0xffff; count && !T::available(); count--);
		}
	}

protected:
	uint8_t msgId;
};
