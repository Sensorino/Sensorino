/** NRF24 control library.
 * This library implements the functionalities of the NRF24L01+ chip without
 * (almost) any further abstraction.
 *
 * Author: Dario Salvi (dariosalvi78 at gmail dot com)
 *
 * Based on the work of Mike McCauley (mikem at airspayce dot com)
 * Original code Id: NRF24.h,v 1.1 2012/12/12 11:17:16 mikem Exp mikem
 * Available at http://www.airspayce.com/mikem/arduino/NRF24/
 *
 * Differences with the original library:
 * - uses java doc documentation style
 * - adds reliability: almost every command is checked after being executed
 * - supports all pipes (not only 0 and 1)
 * - adds several functionalities that were missing in the original version
 * Still unsupported:
 * - acks with payload
 *
 * Licensed under the GPL license http://www.gnu.org/copyleft/gpl.html
 */

#include <NRF24.h>
#include <SPI.h>

NRF24::NRF24(uint8_t chipEnablePin, uint8_t chipSelectPin) {
    _crc = NRF24_EN_CRC; // Default: 1 byte CRC enabled
    _chipEnablePin = chipEnablePin;
    _chipSelectPin = chipSelectPin;
}

boolean NRF24::init() {
    // Initialise the slave select pin
    pinMode(_chipEnablePin, OUTPUT);
    digitalWrite(_chipEnablePin, LOW);
    pinMode(_chipSelectPin, OUTPUT);
    digitalWrite(_chipSelectPin, HIGH);

    // Added code to initilize the SPI interface and wait 100 ms
    // to allow NRF24 device to "settle".  100 ms may be overkill.
    pinMode(SCK, OUTPUT);
    pinMode(MOSI, OUTPUT);
    // Wait for NRF24 POR (up to 100msec)
    delay(100);

    // start the SPI library:
    // Note the NRF24 wants mode 0, MSB first and default to 1 Mbps
    SPI.begin();
    SPI.setDataMode(SPI_MODE0);
    SPI.setBitOrder(MSBFIRST);
//    SPI.setClockDivider(SPI_2XCLOCK_MASK); // 1 MHz SPI clock
    SPI.setClockDivider(SPI_CLOCK_DIV2); // 8MHz SPI clock

    // Clear interrupts
    spiWriteRegister(NRF24_REG_07_STATUS, NRF24_RX_DR | NRF24_TX_DS | NRF24_MAX_RT);

    //Enables dynamic payloads and dyanamic acks
    spiWriteRegister(NRF24_REG_1D_FEATURE, NRF24_EN_DPL | NRF24_EN_DYN_ACK);

    //Make sure we are powered down
    powerDown();

    // Flush FIFOs
    flushTx();
    flushRx();

    return true;
}

// Low level commands for interfacing with the device
uint8_t NRF24::spiCommand(uint8_t command)
{
    digitalWrite(_chipSelectPin, LOW);
    uint8_t status = SPI.transfer(command);
    digitalWrite(_chipSelectPin, HIGH);
    return status;
}

// Read and write commands
uint8_t NRF24::spiRead(uint8_t command)
{
    digitalWrite(_chipSelectPin, LOW);
    SPI.transfer(command); // Send the address, discard status
    uint8_t val = SPI.transfer(0); // The MOSI value is ignored, value is read
    digitalWrite(_chipSelectPin, HIGH);
    return val;
}

uint8_t NRF24::spiWrite(uint8_t command, uint8_t val)
{
    digitalWrite(_chipSelectPin, LOW);
    uint8_t status = SPI.transfer(command);
    SPI.transfer(val); // New register value follows
    digitalWrite(_chipSelectPin, HIGH);
    return status;
}

void NRF24::spiBurstRead(uint8_t command, uint8_t* dest, uint8_t len)
{
    digitalWrite(_chipSelectPin, LOW);
    SPI.transfer(command); // Send the start address, discard status
    while (len--){
        *dest++ = SPI.transfer(0); // The MOSI value is ignored, value is read
    }
    digitalWrite(_chipSelectPin, HIGH);
    // 300 microsecs for 32 octet payload
}

uint8_t NRF24::spiBurstWrite(uint8_t command, uint8_t* src, uint8_t len)
{
    digitalWrite(_chipSelectPin, LOW);
    uint8_t status = SPI.transfer(command);
    while (len--)
	SPI.transfer(*src++);
    digitalWrite(_chipSelectPin, HIGH);
    return status;
}

// Use the register commands to read and write the registers
uint8_t NRF24::spiReadRegister(uint8_t reg)
{
    return spiRead((reg & NRF24_REGISTER_MASK) | NRF24_COMMAND_R_REGISTER);
}

uint8_t NRF24::spiWriteRegister(uint8_t reg, uint8_t val)
{
    return spiWrite((reg & NRF24_REGISTER_MASK) | NRF24_COMMAND_W_REGISTER, val);
}

void NRF24::spiBurstReadRegister(uint8_t reg, uint8_t* dest, uint8_t len)
{
    return spiBurstRead((reg & NRF24_REGISTER_MASK) | NRF24_COMMAND_R_REGISTER, dest, len);
}

uint8_t NRF24::spiBurstWriteRegister(uint8_t reg, uint8_t* src, uint8_t len)
{
    return spiBurstWrite((reg & NRF24_REGISTER_MASK) | NRF24_COMMAND_W_REGISTER, src, len);
}

uint8_t NRF24::statusRead()
{
    return spiReadRegister(NRF24_REG_07_STATUS);
//    return spiCommand(NRF24_COMMAND_NOP); // Side effect is to read status
}

uint8_t NRF24::flushTx()
{
    return spiCommand(NRF24_COMMAND_FLUSH_TX);
}

uint8_t NRF24::flushRx() {
    return spiCommand(NRF24_COMMAND_FLUSH_RX);
}

boolean NRF24::setChannel(uint8_t channel) {
    spiWriteRegister(NRF24_REG_05_RF_CH, channel & NRF24_RF_CH);
    uint8_t actch = getChannel();
    return (actch == channel);
}

uint8_t NRF24::getChannel(){
    uint8_t reg = spiReadRegister(NRF24_REG_05_RF_CH);
    return reg;
}

boolean NRF24::isPoweredUp(){
    return ((spiReadRegister(NRF24_REG_00_CONFIG) & NRF24_PWR_UP) !=0);
}

boolean NRF24::getRPD(){
    uint8_t rpd = spiReadRegister(NRF24_REG_09_RPD);
    return (rpd>0);
}

NRF24::NRF24CRC NRF24::getCRC() {
    uint8_t reg = spiReadRegister(0);
    if((reg & NRF24_EN_CRC) == 0)
        return NRF24CRCNO;
    else{
        if((reg & NRF24_CRCO) == 0)
            return NRF24CRC1Byte;
        else return NRF24CRC2Bytes;
    }
}

boolean NRF24::setCRC(NRF24CRC crc) {
    uint8_t reg = spiReadRegister(0);
    if(crc == NRF24CRCNO){
        _crc = 0;
        reg = reg & ~NRF24_EN_CRC;
    }
    else if(crc == NRF24CRC1Byte){
        _crc = NRF24_EN_CRC;
        reg = reg | NRF24_EN_CRC & ~NRF24_CRCO;
    }
    else{
        _crc = NRF24_EN_CRC | NRF24_CRCO;
        reg = reg | NRF24_EN_CRC | NRF24_CRCO;
    }
    spiWriteRegister(NRF24_REG_00_CONFIG, reg);
    NRF24CRC actcrc = getCRC();
    if(actcrc == crc) return true;
    else return false;
}

boolean NRF24::setTXRetries(uint8_t delay, uint8_t count)
{
    spiWriteRegister(NRF24_REG_04_SETUP_RETR, ((delay << 4) & NRF24_ARD) | (count & NRF24_ARC));
    return true;
}

boolean NRF24::setAddressSize(NRF24AddressSize size){
    if((size != NRF24AddressSize3Bytes)
       && (size != NRF24AddressSize4Bytes)
       && (size != NRF24AddressSize5Bytes))
        return false;
    spiWriteRegister(NRF24_REG_03_SETUP_AW, size);
    delay(100); //just in case..
    NRF24AddressSize actsize = getAddressSize();
    if(size != actsize)
        return false;
    else return true;
}

NRF24::NRF24AddressSize NRF24::getAddressSize(){
    uint8_t reg = spiReadRegister(3);
    reg = (reg & NRF24_AW);
    if(reg == 1)
        return NRF24AddressSize3Bytes;
    else if(reg == 2)
        return NRF24AddressSize4Bytes;
    else if(reg == 3)
        return NRF24AddressSize5Bytes;
}


boolean NRF24::areAddressesEquals(uint8_t *addr1, uint8_t* addr2, uint8_t len){
    for(int i=0; i<len;i++){
        if(addr1[i] != addr2[i])
            return false;
    }
    return true;
}

boolean NRF24::setTransmitAddress(uint8_t* address, boolean autoack) {
    int len = getAddressSize()+2;
    if(autoack){
        //Set both TX_ADDR and RX_ADDR_P0 for auto-ack with Enhanced ShockBurst:
        //From manual:
        //Set RX_ADDR_P0 equal to this address to handle
        //automatic acknowledge if this is a PTX device with
        //Enhanced ShockBurst enabled
        spiBurstWriteRegister(NRF24_REG_0A_RX_ADDR_P0, address, len);
    }
    spiBurstWriteRegister(NRF24_REG_10_TX_ADDR, address, len);
    uint8_t actadd[len];
    if(!getTransmitAddress(actadd))
        return false;
    return areAddressesEquals(address, actadd, len);
}

boolean NRF24::getTransmitAddress(uint8_t * address){
    int len = getAddressSize()+2;
    spiBurstReadRegister(NRF24_REG_10_TX_ADDR, address, len);
    return true;
}

boolean NRF24::setPipeAddress(uint8_t pipe, uint8_t* address) {
    int len = getAddressSize()+2;
    //TODO: only send first byte for byte 1,2,3,4,5, or maybe it works anyway?
    spiBurstWriteRegister(NRF24_REG_0A_RX_ADDR_P0 + pipe, address, len);
    uint8_t curraddr[len];
    if(!getPipeAddress(pipe, curraddr))
        return false;
    return areAddressesEquals(address, curraddr, len);
}

boolean NRF24::getPipeAddress(uint8_t pipe, uint8_t * address){
    uint8_t len = getAddressSize()+2;
    if((pipe == 0) || (pipe == 1)){
        spiBurstReadRegister(NRF24_REG_0A_RX_ADDR_P0 + pipe, address, len);
        return true;
    }
    else if((pipe ==2) || (pipe == 3) || (pipe == 4) || (pipe == 5)){
        if(!getPipeAddress(1, address)) //Get base address
            return false;
        uint8_t lastbyte[1];
        spiBurstReadRegister(NRF24_REG_0A_RX_ADDR_P0 + pipe, lastbyte, 1);
        address[len-1] = lastbyte[0];
        return true;
    }
    else return false;
}

boolean NRF24::enablePipe(uint8_t pipe){
    uint8_t reg = spiReadRegister(NRF24_REG_02_EN_RXADDR);
    if(pipe == 0)
        reg = reg | NRF24_ERX_P0;
    else if(pipe == 1)
        reg = reg | NRF24_ERX_P1;
    else if(pipe == 2)
        reg = reg | NRF24_ERX_P2;
    else if(pipe == 3)
        reg = reg | NRF24_ERX_P3;
    else if(pipe == 4)
        reg = reg | NRF24_ERX_P4;
    else if(pipe == 5)
        reg = reg | NRF24_ERX_P5;
    else return false;
    spiWriteRegister(NRF24_REG_02_EN_RXADDR, reg);
    return isAutoAckEnabled(pipe);
}


boolean NRF24::isPipeEnabled(uint8_t pipe){
    uint8_t reg = spiReadRegister(NRF24_REG_02_EN_RXADDR);
    if(pipe == 0)
        return !((reg & NRF24_ERX_P0) ==0);
    else if(pipe == 1)
        return !((reg & NRF24_ERX_P1) ==0);
    else if(pipe == 2)
        return !((reg & NRF24_ERX_P2) ==0);
    else if(pipe == 3)
        return !((reg & NRF24_ERX_P3) ==0);
    else if(pipe == 4)
        return !((reg & NRF24_ERX_P4) ==0);
    else if(pipe == 5)
        return !((reg & NRF24_ERX_P5) ==0);
    else return false;
}


boolean NRF24::enableAutoAck(uint8_t pipe){
    uint8_t reg = spiReadRegister(NRF24_REG_01_EN_AA);
    if(pipe == 0)
        reg = reg | NRF24_ENAA_P0;
    else if(pipe == 1)
        reg = reg | NRF24_ENAA_P1;
    else if(pipe == 2)
        reg = reg | NRF24_ENAA_P2;
    else if(pipe == 3)
        reg = reg | NRF24_ENAA_P3;
    else if(pipe == 4)
        reg = reg | NRF24_ENAA_P4;
    else if(pipe == 5)
        reg = reg | NRF24_ENAA_P5;
    else return false;
    spiWriteRegister(NRF24_REG_01_EN_AA, reg);
    return isAutoAckEnabled(pipe);
}

boolean NRF24::isAutoAckEnabled(uint8_t pipe){
    uint8_t reg = spiReadRegister(NRF24_REG_01_EN_AA);
    if(pipe == 0)
        return !((reg & NRF24_ENAA_P0) ==0);
    else if(pipe == 1)
        return !((reg & NRF24_ENAA_P1) ==0);
    else if(pipe == 2)
        return !((reg & NRF24_ENAA_P2) ==0);
    else if(pipe == 3)
        return !((reg & NRF24_ENAA_P3) ==0);
    else if(pipe == 4)
        return !((reg & NRF24_ENAA_P4) ==0);
    else if(pipe == 5)
        return !((reg & NRF24_ENAA_P5) ==0);
    else return false;
}

boolean NRF24::setPayloadSize(uint8_t pipe, uint8_t size) {
    if(size >32)
        return false;
    if(size == 0){
        uint8_t reg = spiReadRegister(NRF24_REG_1C_DYNPD);
        reg = reg | (NRF24_DPL_P0 << pipe);
        spiWriteRegister(NRF24_REG_1C_DYNPD, reg);
    }
    else{
        //Unset dynamic payload first
        if(getPayloadSize(pipe) == 0){
            uint8_t reg = spiReadRegister(NRF24_REG_1C_DYNPD);
            reg = reg & ~(NRF24_DPL_P0 << pipe);
            spiWriteRegister(NRF24_REG_1C_DYNPD, reg);
        }
        spiWriteRegister(NRF24_REG_11_RX_PW_P0 + pipe, size);
    }
    return (getPayloadSize(pipe) == size);
}

uint8_t NRF24::getPayloadSize(uint8_t pipe){
    uint8_t reg = spiReadRegister(NRF24_REG_1C_DYNPD);
    if((reg & (NRF24_DPL_P0 << pipe)) == 0){
       reg = spiReadRegister(NRF24_REG_11_RX_PW_P0 + pipe);
       return reg;
    }
    else return 0;
}


boolean NRF24::setRF(NRF24DataRate data_rate, NRF24TransmitPower power) {
    uint8_t value = (power << 1) & NRF24_PWR;
    // Ugly mapping of data rates to noncontiguous 2 bits:
    if (data_rate == NRF24DataRate250kbps)
        value |= NRF24_RF_DR_LOW;
    else if (data_rate == NRF24DataRate2Mbps)
        value |= NRF24_RF_DR_HIGH;
    // else NRF24DataRate1Mbps, 00
    Serial.println(value, BIN);
    spiWriteRegister(NRF24_REG_06_RF_SETUP, value);
    NRF24DataRate actrate = getDatarate();
    NRF24TransmitPower actpow = getTransmitPower();
    if((data_rate == actrate) && (power == actpow))
        return true;
    else return false;
}

NRF24::NRF24DataRate NRF24::getDatarate(){
    uint8_t reg = spiReadRegister(6);
    boolean drl = (reg & NRF24_RF_DR_LOW) != 0;
    boolean drh = (reg & NRF24_RF_DR_HIGH) != 0;
    if(drl && !drh)
        return NRF24DataRate250kbps;
    else if(!drl && !drh)
        return NRF24DataRate1Mbps;
    else
        return NRF24DataRate2Mbps;
}

NRF24::NRF24TransmitPower NRF24::getTransmitPower(){
    uint8_t reg = spiReadRegister(6);
    reg = (reg & NRF24_PWR)>>1;
    if(reg == 0)
        return NRF24TransmitPowerm18dBm;
    if(reg == 1)
        return NRF24TransmitPowerm12dBm;
    if(reg == 2)
        return NRF24TransmitPowerm6dBm;
    if(reg == 3)
        return NRF24TransmitPower0dBm;
}

boolean NRF24::powerDown() {
    spiWriteRegister(NRF24_REG_00_CONFIG, _crc);
    digitalWrite(_chipEnablePin, LOW);
    return true;
}

boolean NRF24::powerUpRx() {
    spiWriteRegister(NRF24_REG_00_CONFIG, _crc | NRF24_PWR_UP | NRF24_PRIM_RX);
    digitalWrite(_chipEnablePin, HIGH);
    return true;
}

boolean NRF24::powerUpTx() {
    // Its the pulse high that puts us into TX mode
    digitalWrite(_chipEnablePin, LOW);
    spiWriteRegister(NRF24_REG_00_CONFIG, _crc | NRF24_PWR_UP);
    digitalWrite(_chipEnablePin, HIGH);
    return true;
}

void NRF24::asyncSend(uint8_t* data, uint8_t len, boolean noack){
    powerUpTx();
    spiBurstWrite(noack ? NRF24_COMMAND_W_TX_PAYLOAD_NOACK : NRF24_COMMAND_W_TX_PAYLOAD, data, len);
    // Radio will return to Standby II mode after transmission is complete
}


boolean NRF24::send(uint8_t* data, uint8_t len, boolean noack) {
    powerUpTx();
    spiBurstWrite(noack ? NRF24_COMMAND_W_TX_PAYLOAD_NOACK : NRF24_COMMAND_W_TX_PAYLOAD, data, len);
    // Radio will return to Standby II mode after transmission is complete

    // Wait for either the Data Sent or Max ReTries flag, signalling the
    // end of transmission
    uint8_t status;
    while (!((status = statusRead()) & (NRF24_TX_DS | NRF24_MAX_RT)))
	;

    // Must clear NRF24_MAX_RT if it is set, else no further comm
    spiWriteRegister(NRF24_REG_07_STATUS, NRF24_TX_DS | NRF24_MAX_RT);
    if (status & NRF24_MAX_RT)
	flushTx();
    // Return true if data sent, false if MAX_RT
    return status & NRF24_TX_DS;
}

boolean NRF24::isSending() {
    return !(spiReadRegister(NRF24_REG_00_CONFIG) & NRF24_PRIM_RX) && !(statusRead() & (NRF24_TX_DS | NRF24_MAX_RT));
}

boolean NRF24::available() {
    if (spiReadRegister(NRF24_REG_17_FIFO_STATUS) & NRF24_RX_EMPTY)
	return false;
    // Manual says that messages > 32 octets should be discarded
    if (spiRead(NRF24_COMMAND_R_RX_PL_WID) > 32) {
        flushRx();
        return false;
    }
    return true;
}

void NRF24::waitAvailable() {
    powerUpRx();
    while (!available())
	;
}

// Blocks until a valid message is received or timeout expires
// Return true if there is a message available
// Works correctly even on millis() rollover
boolean NRF24::waitAvailableTimeout(uint16_t timeout) {
    powerUpRx();
    unsigned long starttime = millis();
    while ((millis() - starttime) < timeout)
        if (available())
           return true;
    return false;
}

boolean NRF24::recv(uint8_t* pipe, uint8_t* buf, uint8_t* len) {
    // Clear read interrupt
    spiWriteRegister(NRF24_REG_07_STATUS, NRF24_RX_DR);

    // 0 microsecs @ 8MHz SPI clock
    if (!available())
	return false;
    // 32 microsecs (if immediately available)
    *len = spiRead(NRF24_COMMAND_R_RX_PL_WID);
    // 44 microsecs
    spiBurstRead(NRF24_COMMAND_R_RX_PAYLOAD, buf, *len);
    // 140 microsecs (32 octet payload)
    uint8_t reg = (spiReadRegister(NRF24_REG_07_STATUS) & NRF24_RX_P_NO) >> 1;
    if(reg > 5) return false;
    else *pipe = reg;
    return true;
}

void NRF24::printRegisters(){
    uint8_t registers[] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0d, 0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x1c, 0x1d};
    uint8_t i;
    for (i = 0; i < sizeof(registers); i++) {
	 Serial.print(i, HEX);
	 Serial.print(": ");
	 Serial.println(spiReadRegister(i), HEX);
    }
}

