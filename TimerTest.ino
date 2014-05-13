#include "Timers.h"

void simple_timeout(void) {
  Serial.println("simple timeout elapsed");
}

class Example {
  void objectTimeout(void) {
    Serial.println("object timeout elapsed");
  }
public:
  void setTimeout(void) {
    Timers::setObjTimeout(Example::objectTimeout, F_CPU * 20);
  }
};

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  Timers::setTimeout(simple_timeout, F_CPU * 10);
  
  (new Example())->setTimeout();
}

void loop() {
  // put your main code here, to run repeatedly: 
  
}