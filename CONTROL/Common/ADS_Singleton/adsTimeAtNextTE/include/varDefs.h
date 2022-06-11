#include "adsDevio.h"

typedef struct __attribute__((packed)){
    long time;
    bool resyncFlag;
} TEHandlerData_t;

const std::string remoteIp = "192.168.1.8";
const uint8_t arr[] = {192,168,56,1,1,1};  
const uint32_t port = 852;
const std::string dev = "TEHandler";  
