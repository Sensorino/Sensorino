Sensorino
=========

A library that abstracts the nRF24L01 and implements some basic services on top.
Uses the [nRF24 library](https://github.com/dariosalvi78/nRF24).

Decisions taken:
- pipe 0 is used as broadcast pipe, with shared address and no acks
- pipe 1 is used as private address
- nodes send their address
- addresses are 4 bytes long
- CRC is 2 bytes
- 2Mbps, 750us ack time, 3 retries

The library also implements a set of "services" on top of basic communication means.

Author: Dario Salvi (dariosalvi78 at gmail dot com)

Licensed under the GPL license http://www.gnu.org/copyleft/gpl.html