#include "adsDevio.h"

typedef std::map<uint32_t, std::string> DevicePointMap;

const std::string remoteIp = "192.168.1.8";
const uint8_t arr[] = {192, 168, 56, 1, 1, 1};  
const uint32_t port = 866;
const std::string dev = "LORR";  
static DevicePointMap pmap = {{0, "SERIAL_NUMBER"}, {196608, "PROTOCOL_REV_LEVEL"}, {196609, "CAN_ERROR"}, {196610, "TRANS_NUM"}, {196612, "SW_REV_LEVEL"}, {196611, "AMBIENT_TEMPERATURE"}};

static adsDevio *interface_mp;

