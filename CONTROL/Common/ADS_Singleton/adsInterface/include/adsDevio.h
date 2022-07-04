
#include <adsInterface.h>
#include <ambDefs.h>
#include <adsDefs.h>
#include <ControlExceptions.h>
#include <ControlBasicInterfacesC.h>
#include <string>
#include <mutex>
#include <thread>
/**
* This uses the Beckhoff/ADS library https://github.com/Beckhoff/ADS
*/



/**
* This class works as an intermediary between the HWDevice and the ADS client implementation.
*/

class adsDevio
{
 
    public:
        
        adsDevio(const std::string& remoteIp, const uint8_t (&arrayNetId)[6],  const uint32_t& plcPort);

        ~adsDevio();

        void insertRCA(AmbRelativeAddr RCA, std::string pointName);

        void enqueueAdsMessage(const AmbMessage_t& msg) const;

         /// \exception ControlExceptions::CAMBErrorExImpl
        void  flushNode(AmbChannel channel, AmbNodeAddr nodeAddress, ACS::Time TimeEvent, ACS::Time* timestamp, AmbErrorCode_t* status);

        /// \exception ControlExceptions::CAMBErrorExImpl
        void flushRCA(AmbChannel channel, AmbNodeAddr nodeAddress, ACS::Time TimeEvent, AmbRelativeAddr RCA, ACS::Time* timestamp, AmbErrorCode_t* status);
 
    
    protected:
        
        AmbChannel channel_m;
    	AmbNodeAddr nodeAddress_m;
        
        
        adsInterface* interface_mp;
	std::string deviceName;
	pointNodeInfo_t msgPoint;
        
    
};
