/*
 * 16-bit timer 1 used for Sensorino timekeeping as well as arbitrary
 * timeouts.
 *
 * Licensed under AGPLv3.
 */

#include <stdint.h>

class GenCallback {
public:
	virtual void call(void) = 0;
};

class Timers {
	Timers(void);
	static class init { public: init(void); } initializer;
public:
	static uint32_t now(void);
	static uint32_t millis(void);
	static void delay(uint16_t msecs);
	static void setTimeout(void (*callback)(void), uint32_t timeout);
	static void setTimeout(GenCallback *callback, uint32_t timeout);
	static bool pending(void);
};

#define setObjTimeout(method, timeout) \
	setTimeout(new Callback<typeof(*this)>(this, &method), timeout)

template <typename T>
class Callback : public GenCallback {
	T *obj;
	void (T::*method)(void);
public:
	Callback(T *nobj, void (T::*nmethod)(void)) : obj(nobj), method(nmethod) {}
	void call(void) { (obj->*method)(); }
};