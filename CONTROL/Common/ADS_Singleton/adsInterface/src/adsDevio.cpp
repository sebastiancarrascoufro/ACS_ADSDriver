#include <TETimeUtil.h>
#include "adsDevio.h"
#include <string>
#include <type_traits>
#include <chrono>

/**
 * Constructor
 @params
 * devName: HWDevice name
 * remoteIpV4: ipv4 from ADS server
 * arrayNetId: AMS netId of the ADS server
 * port: PLC ADS port
*/
adsDevio::adsDevio(const std::string& devName, const std::string& remoteIp, const uint8_t (&arrayNetId)[6],  const uint32_t& plcPort) : channel_m(0ULL), nodeAddress_m(0ULL)

{
    deviceName = devName;
    interface_mp = adsInterface::getInstance(remoteIp, arrayNetId, plcPort);
    insertRCA(0x0, "AMBMessageNode");
    interface_mp->startNotification(deviceName);
}


adsDevio::~adsDevio()
{
    
    try {
        if(interface_mp != 0)
    {
        interface_mp->deleteInstance();
        interface_mp = 0;
    }
    }
    catch (const std::exception & exc)
    {

    }

}


/**
 * The reference of the nodes of interest is obtained and stored.
 @params
 * RCA: monitoring/control point rca
 * pointName: Name of the monitoring/control point (the ADS server has nodes implemented with the same name)
*/
void adsDevio::insertRCA(AmbRelativeAddr RCA, std::string pointName){
    
    pointNodeInfo_t pointInfo = interface_mp->getNodesForDevicePoint(deviceName, pointName);
    pointInfoMap.insert({ RCA, pointInfo});
}




/**
* The sendAdsMessage call is queued to be processed when there is an available ADS connection
@params
* message: AMBmessage to transmit to the ADS server
*/
void adsDevio::enqueueAdsMessage(const AmbMessage_t& message) const
{
    interface_mp->enqueueAdsMessage(message, pointInfoMap);
}


void adsDevio::flushNode(AmbChannel channel, AmbNodeAddr nodeAddress, ACS::Time TimeEvent, ACS::Time* timestamp,
    AmbErrorCode_t* status)
{
        if(interface_mp == 0)
    {
        *status = AMBERR_WRITEERR;
        return;
    }

    // Send the message and wait for a return
    try
    {
        *status = interface_mp->flush(channel_m, nodeAddress_m, TimeEvent,
            (Time&)*timestamp);
    }
    catch(const ControlExceptions::CAMBErrorExImpl& ex)
    {
        if(status != 0)
        {
            *status = AMBERR_ADDRERR;
        }
    }
}

void adsDevio::flushRCA(AmbChannel channel, AmbNodeAddr nodeAddress, ACS::Time TimeEvent, AmbRelativeAddr RCA,
    ACS::Time* timestamp, AmbErrorCode_t* status)
{    
 if(interface_mp == 0)
    {
        *status = AMBERR_WRITEERR;
        return;
    }

    // Send the message and wait for a return
    try
    {
        *status = interface_mp->flush(channel_m, nodeAddress_m, RCA,
            TimeEvent, (Time&)*timestamp);
    }
    catch(const ControlExceptions::CAMBErrorExImpl& ex)
    {
        if(status != 0)
        {
            *status = AMBERR_ADDRERR;
        }
    }

        
}
