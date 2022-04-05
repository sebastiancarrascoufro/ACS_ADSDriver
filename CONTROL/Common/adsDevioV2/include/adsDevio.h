#include <AdsLib/standalone/AdsLib.h>
#include <AdsLib/AdsNotificationOOI.h>
#include <AdsLib/AdsVariable.h>

#include <adsThreadPool.h>
#include <adsSafeQueue.h>
#include <ambInterface.h>
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

/* structure to handle timeouts  */
typedef struct {
    AmbCompletion_t* completion_p;
    uint64_t time;
} timeoutData_t;

typedef AdsSafeQueue<timeoutData_t> tQueue;
typedef AdsSafeQueue<AdsDevice*> AdsQueue;

/**
* Here the connection of the ADS client and the sending of the AMBCommand is implemented,
* the response is retrieved via a notification where the server reports a change in a device-specific variable
* To avoid limitations of sending ADS commands, a threadpool and a queue of ADS connections are used, 
* each request is processed by an independent thread in which an available connection is obtained to send the message, 
* when the request ends the connection is inserted again in The ADSQueue, the number of threads is 
* consistent with the number of connections in the queue, this guarantees that there will always be available connections 
* and guarantees that the commands will be sent to the ADS server. An additional connection is used for notifications.
*/

class adsDevio
{
 
    public:
        
        adsDevio(const std::string& deviceName, const std::string& remoteIp, const uint8_t (&arrayNetId)[6],  const uint32_t& plcPort, const uint32_t& ps);

        ~adsDevio();

        void insertRCA(AmbRelativeAddr RCA, std::string pointName);

        
        void enqueueAdsMessage(const AmbMessage_t& msg) const;

        


        /// \exception ControlExceptions::CAMBErrorExImpl
        AmbErrorCode_t findSN(unsigned char  serialNumber[],
                          AmbChannel     channel,
                          AmbNodeAddr    nodeAddress,
                          Time&          timestamp) const;

        /**
        * This method is a work around because of the bug in the early
        * version of the AMBSI-1.  As soon as all AMBSI-1 Devices have
        * been updated to respond to node ID requests at RCA 0 this
        * method will be deleted.
        * \exception ControlExceptions::CAMBErrorExImpl
        */
        AmbErrorCode_t       findSNUsingBroadcast(unsigned char  serialNumber[],
                            AmbChannel     channel,
                            AmbNodeAddr    nodeAddress,
                            Time&          timestamp) const;

        /// \exception ControlExceptions::CAMBErrorExImpl
        AmbErrorCode_t       flush(AmbChannel      channel,
                     Time            flushTime,
                     Time&           timestamp) const;

        /// \exception ControlExceptions::CAMBErrorExImpl
        AmbErrorCode_t       flush(AmbChannel      channel,
                     AmbNodeAddr     nodeAddress,
                     Time            flushTime,
                     Time&           timestamp) const;

        /// \exception ControlExceptions::CAMBErrorExImpl
        AmbErrorCode_t       flush(AmbChannel      channel,
                     AmbNodeAddr     nodeAddress,
                     AmbRelativeAddr RCA,
                     Time            flushTime,
                     Time&           timestamp) const;

     
    
    protected:
        
        AdsDevice* notificationRoute;   //connection to receive notifications from the ADS server
        std::string deviceName;
        
        AdsThreadPool* pool;    // thread pool to queue calls that need an ADS connection
        AdsQueue* q;            // queue for available ADS connections
        uint32_t poolSize;      // number of threads to send commands via ADS (this defines the number of ADS connections will be started)
        uint32_t bSize;		// buffer size
        DeviceMap pointInfoMap; // each address of the sent AMBCommands is stored and the time in microseconds in which
        AdsNotification* notif;
        
    uint32_t timeout;   	// Definition of the timeout to force termination of AMBCommands
    bool exit_signal = false;	// variable to terminate the timeout verification thread
    tQueue* tQ;  		// queue to store messages waiting for a response 
    std::thread tr; 		// timeout thread
    
        
    private:
        
        void startNotificationDevice();
        std::thread  runThread();
        void verifyTimeouts();    
        static uint32_t getOffset(std::string devRoute, AdsQueue* q);
        static void clearBuffer(std::string devRoute, AdsQueue* q, uint16_t i);
        static void sendAdsMessage(const AmbMessage_t& msg, DeviceMap pointInfoMap, tQueue* tQ, AdsQueue* q);
        static void NotifyCallback(const AmsAddr* pAddr, const AdsNotificationHeader* pNotification, uint32_t hUser);
        static void fillCompletion(AdsResponse_t& response);
};
