#ifndef adsInterface_H
#define adsInterface_H
/*******************************************************************************
* ALMA - Atacama Large Millimiter Array
* (c) Associated Universities Inc., 2003
*
*This library is free software; you can redistribute it and/or
*modify it under the terms of the GNU Lesser General Public
*License as published by the Free Software Foundation; either
*version 2.1 of the License, or (at your option) any later version.
*
*This library is distributed in the hope that it will be useful,
*but WITHOUT ANY WARRANTY; without even the implied warranty of
*MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
*Lesser General Public License for more details.
*
*You should have received a copy of the GNU Lesser General Public
*License along with this library; if not, write to the Free Software
*Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA
*
* who       when      what
* --------  --------  ----------------------------------------------
* jkern  2003-12-17  created
*/

/************************************************************************
 * This file defines the adsInterface class.  This is a singleton class
 * which is the interface between the Obj Oriented world of user space
 * and the C world of RTAI.  All communications to the AMB bus must go
 * through this part.
 *----------------------------------------------------------------------
 */

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif

#include <AdsLib/standalone/AdsLib.h>
#include <AdsLib/AdsNotificationOOI.h>
#include <AdsLib/AdsVariable.h>

#include <adsThreadPool.h>
#include <adsSafeQueue.h>

#include <ambDefs.h>
#include <adsDefs.h>
#include <ControlExceptions.h>
#include <ControlBasicInterfacesC.h>


#include <ambInterface.h>





/* structure to handle timeouts  */
typedef struct {
    AmbCompletion_t* completion_p;
    uint64_t time;
} timeoutData_t;

typedef AdsSafeQueue<timeoutData_t> tQueue;
typedef AdsSafeQueue<AdsDevice*> AdsQueue;

/*
* Here the connection of the ADS client and the sending of the AMBCommand is implemented,
* the response is retrieved via a notification where the server reports a change in a device-specific variable
* To avoid limitations of sending ADS commands, a threadpool and a queue of ADS connections are used, 
* each request is processed by an independent thread in which an available connection is obtained to send the message, 
* when the request ends the connection is inserted again in The ADSQueue, the number of threads is 
* consistent with the number of connections in the queue, this guarantees that there will always be available connections 
* and guarantees that the commands will be sent to the ADS server. An additional connection is used for notifications.
*/
class adsInterface
{
    public:
    /// \exception ControlExceptions::CAMBErrorExImpl
    static adsInterface* getInstance(const std::string& remoteIp, const uint8_t (&arrayNetId)[6],  const uint32_t& plcPort);
    static void deleteInstance();
    void startNotification(const std::string& devName);
    void enqueueAdsMessage(const AmbMessage_t& msg, const DeviceMap& pointInfoMap) const;
  
pointNodeInfo_t getNodesForDevicePoint(std::string deviceName , std::string pointName) const;
  /**
   * This method sends the message to the real time system if te=0 then
   * the command is executed asap, otherwise the command is executed at
   * the specified TE.  If the specified time does not correspond to a
   * timing event the event is executed at the next timing event.
   * \exception ControlExceptions::CAMBErrorExImpl
   */

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
    /**
     * Constructor and Destructor are private because
     * this is a singleton class
     * \exception ControlExceptions::CAMBErrorExImpl
     */
    adsInterface(const std::string& remoteIp, const uint8_t (&arrayNetId)[6],  const uint32_t& plcPort);

    /// Empty constructor to be used by child class(es).
    //adsInterface(const std::string& remoteIp, const uint8_t (&arrayNetId)[6],  const uint32_t& plcPort) ;

    /// Destructor.
    virtual ~adsInterface();

  /**
   * This method is really part of the Constructor, but is moved to a
   * subroutine to force the initialization in the correct order
   * \exception ControlExceptions::CAMBErrorExImpl
   */
  void staticConstructor(const std::string& remoteIp, const uint8_t (&arrayNetId)[6],  const uint32_t& plcPort);

  /**
   * Prevent use of the copy and assignment operator
   */
  adsInterface(const adsInterface&);
  adsInterface& operator= (const adsInterface&);

  /**
   * The replyThread method blocks on the replyFIFO and processes the
   * replies as they are returned to user space.
   * The argument is not used, just defined to make the compiler happy
   * defined static so the compiler can find it
   */
  static void* replyThread(void*);

  /**
   * This routine just copies info into the response locations
   */
  static void fillCompletion(AdsResponse_t&);

  /**
   * This function sends the message to close the server cleanly
   * \exception ControlExceptions::CAMBErrorExImpl
   */
  void ambServerShutdown();


  static adsInterface*    instance_mp;
  static pthread_mutex_t  instanceLock_m;
  static sem_t            semCount_m;
	
  static std::string	  remoteIpInt;
  static uint8_t          arrayNetIdInt[6];
  static uint32_t         plcPortInt;
  static int              shutdownFlag_m;

  static pthread_mutex_t  adsMtx_m;// Mutex protection for cmdFifo
  pthread_t               replyTID_m;    // Thread ID for reply thread
  int                     replyStatus_m; // Thread initialization status
  static AdsNotificationAttrib attrib;
  static std::map<uint64_t, uint64_t> adr_map;
  AdsDevice* notificationRoute;   //connection to receive notifications from the ADS server
  static uint32_t timeout;
  AdsThreadPool* pool;    // thread pool to queue calls that need an ADS connection
  AdsQueue* q;            // queue for available ADS connections
  uint32_t bSize;         // buffer size (ads server)
  static uint8_t  maxNextTE;     // maximum number of future TEs defined in the ads server
  DeviceMap pointInfoMap; // each address of the sent AMBCommands is stored and the time in microseconds in which
  AdsNotification* notif;

  private:
  static void sendAdsMessage(const AmbMessage_t& msg, DeviceMap pointInfoMap, AdsQueue* q);
  static uint32_t getOffset(std::string devRoute, AdsQueue* q);
  static void clearBuffer(std::string devRoute, AdsQueue* q, uint16_t i);
  static void NotifyCallback(const AmsAddr* pAddr, const AdsNotificationHeader* pNotification, uint32_t hUser);
};


#endif /*!adsInterface_H*/
