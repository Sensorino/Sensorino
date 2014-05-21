#include <cstdlib>


void delay(unsigned long ms){

    struct timeval tv;
    tv.tv_sec = ms/1000L;
    tv.tv_usec = ms%1000L;
    select(0, 0, 0, 0, &tv);

}
unsigned long millis(){
    return 42;
}


long random(long to){
    return std::rand();

}
long random(long from, long to){
    return std::rand();
}

