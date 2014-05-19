// Dummy.cpp
//
// Copyright (C) 2014 Mike McCauley
// $Id: Dummy.cpp,v 1.8 2014/05/03 00:20:36 mikem Exp mikem $

#include <Dummy.h>


Dummy::Dummy()
{
}

bool Dummy::init()
{
    return true;
}



bool Dummy::available()
{
    return false;
}

bool Dummy::recv(uint8_t* buf, uint8_t* len)
{
	return true;
}

// Caution: this may block
bool Dummy::send(const uint8_t* data, uint8_t len)
{
    return true;
}

uint8_t Dummy::maxMessageLength()
{
    return RH_SERIAL_MAX_MESSAGE_LEN;
}
