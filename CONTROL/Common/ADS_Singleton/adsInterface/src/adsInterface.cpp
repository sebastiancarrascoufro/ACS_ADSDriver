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
 *   NAME
 *     adsInterface
 *
 *   SYNOPSIS
 *
 *     This singleton class provides the interface between user space and
 *   kernel space for control of the AMB bus.
 *
 *   PUBLIC METHODS
 *
 *     static adsInterface* getInstance();
 *     static void          deleteInstance();
 *     void                 sendMsg(AmbMessage_t& msg);
 *
 *   PRIVATE METHODS
 *     adsInterface();
 *    ~adsInterface();
 *     static void* replyThread(void*);
 *     void ambServerShutdown(void);
 *
 *   PRIVATE DATA MEMBERS
 *
 *    static adsInterface*    instance_mp;
 *     static pthread_mutex_t  m_instanceLock;
 *     static sem_t            m_sem_count;
 *     static int              m_ShutDownFlag;
 *     pthread_mutex_t         m_cmdFifo_mtx;
 *     int                     m_cmdFifo_fd;
 *     pthread_t               m_replyTID;
 *
 *   FILES
 *
 *    adsInterface.cpp
 *    adsInterface.h
 *    ambDefs.h
 *    ambServerC.h //Created by IDL
 *
 *------------------------------------------------------------------------
 */

#include "adsInterface.h"
#include <pthread.h>
#include <rtTools.h>
#include <string>

#include <RepeatGuard.h>
#include <TETimeUtil.h>
#include <linux/fd.h>

//using namespace ambError;
using namespace std;

/* Static Member Definitions */
adsInterface*    adsInterface::instance_mp = NULL;
pthread_mutex_t  adsInterface::instanceLock_m = PTHREAD_MUTEX_INITIALIZER;
int              adsInterface::shutdownFlag_m;    // When true;exit reply thead
sem_t            adsInterface::semCount_m;
uint32_t         adsInterface::timeout = 500;
uint8_t          adsInterface::maxNextTE = 0;

std::string	 adsInterface::remoteIpInt;
uint8_t          adsInterface::arrayNetIdInt[6];
uint32_t         adsInterface::plcPortInt;

pthread_mutex_t  adsInterface::adsMtx_m;
std::map<uint64_t, uint64_t> adsInterface::adr_map;

/**
* Definition of the attributes of the notification. 
* ADSTRANS_SERVERONCHA indicates that a notification will be generated when the variable changes 
* and {0} indicates the sample rate of the variable on the server
*/

AdsNotificationAttrib adsInterface::attrib = {
        sizeof(AdsResponse_t),
        ADSTRANS_SERVERONCHA,
        0,
        {0}
    };


adsInterface::adsInterface(const std::string& remoteIp, const uint8_t (&arrayNetId)[6],  const uint32_t& plcPort) 
{

    const string fnName = "adsInterface::adsInterface";
    ACS_TRACE(fnName);
    try {
        staticConstructor(remoteIp, arrayNetId, plcPort);
    }
    catch(const ControlExceptions::CAMBErrorExImpl& ex){
        /* Try to unload the module */
        throw ControlExceptions::CAMBErrorExImpl(ex,__FILE__,__LINE__,
                             fnName.c_str());
        }
    }

/**
   This function is used to move the initialization of global variables
   out of the constructor.  This ensures that they are created before
   they are used.
   See: http://websqa.hq.eso.org/common/CodingStandards/manuals/ucs_30.htm
   for more info.

 * x + 1 ADS connections are started and stored in a queue to be used when required, 
 * one of these connections is reserved for notifications, additionally a Threadpool of size x is created, 
 * each ADS request will be queued and processed in a different thread using a free ADS connection
*/
void adsInterface::staticConstructor(const std::string& remoteIp, const uint8_t (&arrayNetId)[6],  const uint32_t& plcPort)
{
    const string fnName = "adsInterface::staticContstructor";
    ACS_TRACE(fnName);
    /* Initialize the semaphore which counts references */
    if (sem_init(&semCount_m, 0, 0) != 0) {
    ControlExceptions::CAMBErrorExImpl ex(__FILE__,__LINE__,fnName.c_str());
    ex.addData(Control::EX_USER_ERROR_MSG,
               "Unable to initialize instance count semaphore");
    ex.log();
    throw ex;
    }

    /**
     * Open the cmdFifo and create a mutex to protect it
     */
    try {
    pthread_mutex_init(&adsMtx_m, NULL);
    
    AmsNetId remoteNetId { arrayNetId[0], arrayNetId[1], arrayNetId[2], arrayNetId[3], arrayNetId[4], arrayNetId[5] };

    try {
	//std::cout << "current TE " << (::getTimeStamp() / 48000ULL * 48000ULL) << std::cout;
        uint32_t ps = 25;
        //ThreadPool instance is created
        pool = new AdsThreadPool(ps);
        //we start the queue of ADS connections
        q = new AdsQueue();
        //ADS connections are started and stored
        for (uint32_t i = 0; i < ps + 1; i++) {
            AdsDevice* tmpDevice = new AdsDevice(remoteIp.c_str(), remoteNetId, plcPort);
            tmpDevice->GetState();
            q->push(tmpDevice); 
        }
        //we reserve a connection for exclusive use of notifications
        notificationRoute = q->pop();
    //we read the size of the response array in TwinCAT 
    AdsVariable<uint16_t> bufferSize {*notificationRoute, ".RSP_BUFFER_SIZE"};
    bSize = bufferSize;
    AdsVariable<uint8_t> nextCounter {*notificationRoute, ".maxNextTE"};
    maxNextTE = nextCounter;

    remoteIpInt = remoteIp;
    memcpy (arrayNetIdInt, arrayNetId, sizeof(arrayNetId));
    plcPortInt = plcPort;
    
    }
    catch (const std::exception & exc)
    {
        ControlExceptions::CAMBErrorExImpl ex(__FILE__, __LINE__,
                              "adsDevio::adsDevio");
        ex.addData(Control::EX_USER_ERROR_MSG,
                   exc.what());
        ex.log();
        throw ex;
    }
    /**
       Spawn a thread which handles all responses from the server
       This gets closed down when the instance is deleted
    */
        sem_t threadSynchSem;
        sem_init(&threadSynchSem, 0 ,0);
        shutdownFlag_m = 0;
        if ( (replyStatus_m = pthread_create(&replyTID_m, NULL,
                                       reinterpret_cast<void*(*)(void*)>
                                       (adsInterface::replyThread),
                                       static_cast<void*>(&threadSynchSem))) != 0 ) {
                ACS_SHORT_LOG((LM_ERROR, "Unable to start reply processing thread - return %d", replyStatus_m));
                sem_destroy(&threadSynchSem);
                ControlExceptions::CAMBErrorExImpl ex(__FILE__,__LINE__,fnName.c_str());
                ex.addData(Control::EX_USER_ERROR_MSG,
                           "Unable to start reply processing thread");
            ex.log();
            throw ex;
        }

       
    }
    catch(const ControlExceptions::CAMBErrorEx& ex){
    /* if the fifo was opened close it */
    /* Destroy the semaphore */
    sem_destroy(&semCount_m);

    /* Continue the Error Trace */
    throw ControlExceptions::CAMBErrorExImpl(ex,__FILE__,__LINE__,
                         fnName.c_str());
    }
}



adsInterface::~adsInterface(){
    const string fnName = "adsInterface::adsInterface";
    ACS_TRACE(fnName);
    if (shutdownFlag_m == 0) {
        // This means that staticConstructor was never successfully executed 
        return;
    }

    /* Send a message to shut down the server */
    if (replyStatus_m == 0)
    {
        shutdownFlag_m = 1;
        ambServerShutdown();
        try {
            delete pool;
            //we remove all ADS connections
            while (!(q->empty())) {
                delete (q->pop()); 
            }
            delete q;
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            delete notificationRoute;
        }
        catch (const std::exception & exc)
        {

        }

        pthread_join(replyTID_m, NULL);     //Wait for thread to shutdown
        replyStatus_m = 1;
    }

    

    int semRet;
    if ((semRet = sem_destroy(&semCount_m)) != 0) {
        ACS_SHORT_LOG((LM_ERROR,"Error destroying semaphore - return %d", semRet));
    }
}

/*
In this thread the memory is cleared for the commands that have been lost
*/
void* adsInterface::replyThread(void* synchSemVoid){
    
	timeout = maxNextTE*48 + 96;
    /* Now start infinite loop */
    while (shutdownFlag_m == 0) {
        std::this_thread::sleep_for(std::chrono::milliseconds((uint32_t)(timeout)));
        uint64_t us = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
        //mtx.lock();
        pthread_mutex_lock(&adsMtx_m);
        for ( auto it = adr_map.begin(); it != adr_map.end(); ++it  )
        {
            if (us - it->second > timeout){
                uint64_t adr = it->first;
                AmbCompletion_t* completion_p = (AmbCompletion_t *) adr;
                *(completion_p->status_p) = AMBERR_WRITEERR;
                if (completion_p->synchLock_p != NULL){
                    sem_post(completion_p->synchLock_p);
                }
                if (completion_p != NULL){
                    delete(completion_p);
                }
                ACS_STATIC_LOG(LM_SOURCE_INFO, __FUNCTION__, (LM_WARNING, "Subscription timeout, possible data loss."));    
		std::cout << "timeout" << std::endl;
                adr_map.erase(it);
            }
        }
        pthread_mutex_unlock(&adsMtx_m); 

    }

    pthread_exit(NULL);
}

/*
This method starts the subscription to responses from a specific device
@params
devName: name of the device we subscribe to
*/
void adsInterface::startNotification(const std::string& devName){
    try
    {
    for (uint16_t i = 0; i <= bSize; i++) {
            auto result = pool->enqueue(&adsInterface::clearBuffer,devName, q, i);  
        }
        
        //we give it a little time to finish cleaning the buffer
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        
    //timeout   = 500;
    //tr = runThread();

    for (uint32_t i = 0; i <= bSize; i++) {
            notif = new AdsNotification (*notificationRoute, devName+".RESP["+std::to_string(i)+"]", attrib, &NotifyCallback, 0xBEEFDEAD );
            //qn->push(notif); 
        }
        
    }
    catch (const std::exception & exc)
    {
        ControlExceptions::CAMBErrorExImpl ex(__FILE__, __LINE__,
                              "adsDevio::startNotificationDevice");
        ex.addData(Control::EX_USER_ERROR_MSG,
                   exc.what());
        ex.log();
        throw ex;
    }

}



/**
 * Get the index offset of the requested symbol.
 @params
 * devRoute: PLC Symbol name
 * q: pointer to ADS connection queue
 @return symbol index offset
*/
uint32_t adsInterface::getOffset(std::string devRoute, AdsQueue* q){
    long nErr;
    uint32_t lHdlVar=0, bytesRead;
    char szVar[devRoute.size() + 1];
    strcpy(szVar, devRoute.c_str()); 
    //we get an ADS connection available
    AdsDevice* route = q->pop();
    nErr = route->ReadWriteReqEx2(0xF003, 0x0, sizeof(lHdlVar), &lHdlVar, sizeof(szVar), szVar, &bytesRead);
    //we insert the ADS connection to the queue again
    q->emplace(route);
    if(nErr){
        ACS_STATIC_LOG(LM_SOURCE_INFO, __FUNCTION__, (LM_ERROR, "adsDevio::getOffset ADS ReadWriteReqEx2 failed with code: %ld. ", nErr)); 
    }
    return lHdlVar;
}


/**
* this function clears the last value stored in the ADS server 
* buffer of the HWDevice (this avoids problems with possible previous values when starting)
@params
* deviceName: HWDevice name
* q: pointer to ADS connection queue
*/
void adsInterface::clearBuffer(std::string deviceName, AdsQueue* q, uint16_t i){
    //we get an ADS connection available
    AdsDevice* route = q->pop();
    AdsVariable<AdsResponse_t> buffer {*route, deviceName+".RESP["+std::to_string(i)+"]"};
    AdsResponse_t resp;
    resp.magic = 3735928559;
    resp.completion_p = NULL;
    buffer = resp;
    //we insert the ADS connection to the queue again
    q->emplace(route);
}

/**
 * The reference of the nodes of interest is obtained and stored.
 @params
 * RCA: monitoring/control point rca
 * pointName: Name of the monitoring/control point (the ADS server has nodes implemented with the same name)
*/
pointNodeInfo_t adsInterface::getNodesRCA(AmbRelativeAddr RCA, std::string deviceName , std::string pointName) const{
    std::string devRoute = deviceName+"."+pointName;
    pointNodeInfo_t pointInfo;
    pointInfo.nodeName = pointName;
    pointInfo.responseNode = (pool->enqueue(&adsInterface::getOffset, devRoute+".OPCUA_ResponseValue", q)).get();
    pointInfo.methodNode = (pool->enqueue(&adsInterface::getOffset, devRoute+"#DispatcherMethod", q)).get();
    pointInfo.cleanNode = (pool->enqueue(&adsInterface::getOffset, devRoute+"#CleanNode", q)).get();
    if (pointInfo.responseNode == 0 || pointInfo.methodNode==0 || pointInfo.cleanNode == 0)
    {
        ControlExceptions::CAMBErrorExImpl ex(__FILE__, __LINE__,
                              "adsDevio::insertRCA");
        ex.addData(Control::EX_USER_ERROR_MSG,
                   "one or more "+pointName+" symbols were not found on the ADS server");
        ex.log();
        throw ex;
    }
    return pointInfo;


}

/**
* The sendAdsMessage call is queued to be processed when there is an available ADS connection
@params
* message: AMBmessage to transmit to the ADS server
*/
void adsInterface::enqueueAdsMessage(const AmbMessage_t& message, const DeviceMap& pointInfoMap ) const
{
    auto result = pool->enqueue(&adsInterface::sendAdsMessage,message,pointInfoMap,q);
}


/**
* Logic of sending the message to ADS, the specific node to send the command is obtained, 
* the data is packed and the message is sent, this is called when a thread is available
@params
* message: AMBmessage to transmit to the ADS server
* pointInfoMap: map with necessary information of the monitoring/control points of the device
* q: pointer to ADS connection queue
*/
void adsInterface::sendAdsMessage(const AmbMessage_t& message, DeviceMap pointInfoMap, AdsQueue* q)
{
    const std::string fnName = "adsDevio::sendAdsMessage";
    
/*
    if ((control.requestType == AMB_BLOCK_READ) ||
    (control.requestType == AMB_BLOCK_REQUEST) ||
    (control.requestType == AMB_GET_NODES) ){

    if ((control.completion_p->synchLock_p == NULL) ||
        (control.completion_p->contLock_p == NULL)) {
        // Error 
        ControlExceptions::CAMBErrorExImpl ex(__FILE__, __LINE__,
                          "adsInterface::sendMessage");
        ex.addData(Control::EX_USER_ERROR_MSG,
                   "Both semaphores are required for this type of request");
        ex.log();
        throw ex;
    }
    }
    */
        pointNodeInfo_t point;
        AmbRelativeAddr RCA;
        RCA = (message.address & 0x3ffff);
    

        //it is searched if the rca of the received address matches any mapped element
        if(pointInfoMap.find(RCA) == pointInfoMap.end()){
            ACS_STATIC_LOG(LM_SOURCE_INFO, __FUNCTION__, (LM_ERROR, "RCA (0x%x) not found. ", RCA));
            if (message.completion_p->status_p  != NULL) {
                *(message.completion_p->status_p) = AMBERR_WRITEERR;
            }
            if(message.completion_p->timestamp_p != 0)
            {
                *(message.completion_p->timestamp_p) = 0;
            }
            if (message.completion_p->synchLock_p != NULL){
                sem_post(message.completion_p->synchLock_p);
            }
            if (message.completion_p != NULL){
                delete(message.completion_p);
            }
            return;
        }else{
            point = pointInfoMap.at(RCA);
        }


        long nErr;
        uint32_t bytesRead;
        uint8_t rValue;
        //we get an ADS connection available
        AdsDevice* msgDevice = q->pop();
        nErr = msgDevice->ReadWriteReqEx2(ADSIGRP_SYM_VALBYHND, point.methodNode, sizeof(rValue), &rValue, sizeof(message), &message, &bytesRead);
        //we insert the ADS connection to the queue again
        q->emplace(msgDevice);
        if (nErr || rValue != 0)
        {
printf("rValue %i\n", rValue);
            if (nErr){
                 ACS_STATIC_LOG(LM_SOURCE_INFO, __FUNCTION__, (LM_ERROR, "ADS ReadWriteReqEx2 failed with code: %ld. ", nErr));
        
             } else    {
                 ACS_STATIC_LOG(LM_SOURCE_INFO, __FUNCTION__, (LM_ERROR, "ADS queue full: %ld. ", nErr));
             }        
            if (message.completion_p->status_p  != NULL) {
                *(message.completion_p->status_p) = AMBERR_WRITEERR;
            }
            if(message.completion_p->timestamp_p != 0)
            {
                *(message.completion_p->timestamp_p) = 0;
            }
            if (message.completion_p->synchLock_p != NULL){
                sem_post(message.completion_p->synchLock_p);
            }
            if (message.completion_p != NULL){
                delete(message.completion_p);
            }
        }else{
        
        uint64_t ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
        pthread_mutex_lock(&adsMtx_m);      
        adr_map.insert({ (uint64_t)(message.completion_p), ms});
        pthread_mutex_unlock(&adsMtx_m);
    }
}



/** This method is called when a notification is received from the ADS server.
 * It receives the response, unpacks the received data and processes it to complete the processing of the AMB Commands.
 @params
 * pAddr: complete address of ADS device
 * pNotification: structure that contains the handle, data, length of the data and timestamp of the notification generated by the server
 * hUser: user reference
 */
void adsInterface::NotifyCallback(const AmsAddr* pAddr, const AdsNotificationHeader* pNotification, uint32_t hUser)
{
    const uint8_t* data = reinterpret_cast<const uint8_t*>(pNotification + 1);
    AdsResponse_t  response;
    memcpy( &response,data, sizeof(response));
    
    pthread_mutex_lock(&adsMtx_m);
    std::map<uint64_t, uint64_t>::iterator it = adr_map.find((uint64_t)response.completion_p);
    
    if(it != adr_map.end()){
        adr_map.erase(it);
    }else{
        pthread_mutex_unlock(&adsMtx_m);
        return;
    }
    pthread_mutex_unlock(&adsMtx_m);
std::cout << "timestamp " << response.timestamp << std::endl;
        if(response.magic == 0xdeadbeef)
        {
            // AMBERR_RETRY is informational. Log it, transform it to 
            // AMBERR_NOERR so calling code wont interpret as error condition
            if (response.status == AMBERR_RETRY)
            {
                response.status = AMBERR_NOERR;
                ACS_STATIC_LOG(LM_SOURCE_INFO, __FUNCTION__, 
                               (LM_INFO, "Retried node/RCA: 0x%x / 0x%x",
                                ((response.address >> 18) - 1) & 0x7ff,
                                response.address & 0x3ffff));
            }
            if (response.completion_p != NULL)
            {
                if (response.completion_p->contLock_p != NULL)
                {
                    sem_wait(response.completion_p->contLock_p);
                }
                int sval;
                if (response.completion_p->synchLock_p != NULL &&
                    sem_getvalue(response.completion_p->synchLock_p, &sval) != 0)
                {
                    /* Some one gave up on this completion - no longer waiting for synchLock */
                    ACS_STATIC_LOG(LM_SOURCE_INFO, __FUNCTION__, 

                                   (LM_WARNING, "Completion 0x%p was abandoned.",
                                   response.completion_p));
                }
                else
                {
                    /* Here we load the data into the return structure if necessary */
                    if (response.completion_p != NULL)
                    {
                        fillCompletion(response);
                    }

                    /* If the sychlock exists post to it */
                    if (response.completion_p->synchLock_p != NULL)
                    {
                        sem_post(response.completion_p->synchLock_p);
                    }
                                
            if (response.status != AMBERR_CONTINUE &&
            response.completion_p != NULL)
            {
            /* Terminal message, delete the completion structure */
            delete response.completion_p;
                    }  
                }
            }
        }
        else
        {

            ACS_STATIC_LOG(LM_SOURCE_INFO, __FUNCTION__, (LM_ERROR, "Broken data "
                                  "in CAN-bus response! Corrupted magic word (0x%x) found. "
                                  "Completion pointer completion_p = 0x%p. ATTENTION: Reloading of "
                                  "the kernel modules is now essential! The CAMB-system may or may "
                                  "not work correct from now on.",
                                  response.magic, response.completion_p));
        }
    
} 



/*
 * This function copies the information into the correct places
 * in the completion structure; provided that they are defined.
 */
void adsInterface::fillCompletion(AdsResponse_t& response) {
    if (response.completion_p->dataLength_p != NULL) {
        if(*(response.completion_p->dataLength_p) > 0 &&
           *(response.completion_p->dataLength_p) < response.dataLen &&
       response.completion_p->type_p == NULL) {  // The correlator always uses the original default behavior and is the only user of the type
            static RepeatGuard repeatGuard(ACS::TimeInterval(60.0/100e-9), 0);
            if(repeatGuard.check()) {
                ostringstream msg;
                msg << "Hardware provided longer response than expected. ("
                    << "timestamp = " << TETimeUtil::toTimeString(static_cast<ACS::Time>(response.timestamp)) << ", "
                    << "channel = " << response.channel << ", "
                    << "address = " << response.address << ", "
                    << "expected length = " << *(response.completion_p->dataLength_p) << ", "
                    << "actual length = " << response.dataLen << ")";
                LOG_TO_DEVELOPER(LM_DEBUG, msg.str());
            }
            // ICT-3655:
            // If a non-zero and shorter length is specified, respect it
            response.dataLen = *(response.completion_p->dataLength_p);
        } else {
            // Original default behavior
            *(response.completion_p->dataLength_p) = response.dataLen;
        }
    }

    if (response.completion_p->data_p != NULL) {
        if(response.dataLen > 8) {
            static RepeatGuard repeatGuard(ACS::TimeInterval(60.0/100e-9), 0);
            if(repeatGuard.check()) {
                ostringstream msg;
                msg << "Hardware returned response longer than 8 bytes, truncating to 8 ("
                    << "actual = " << response.dataLen << ")";
                LOG_TO_DEVELOPER(LM_DEBUG, msg.str());
            }
            response.dataLen = 8;
        }
        memcpy(response.completion_p->data_p, response.data, response.dataLen);
    }

    if (response.completion_p->channel_p != NULL) {
    *(response.completion_p->channel_p) = response.channel;
    }

    if (response.completion_p->address_p != NULL) {
    *(response.completion_p->address_p) = response.address;
    }

    if (response.completion_p->timestamp_p != NULL) {
    *(response.completion_p->timestamp_p) = response.timestamp;
    }

    if (response.completion_p->type_p != NULL)
    *(response.completion_p->type_p) = response.type;

    if (response.completion_p->status_p    != NULL) {
    *(response.completion_p->status_p) = response.status;
    }
}

void adsInterface::ambServerShutdown()
{
    const string fnName = "adsInterface::ambServerShutdown";
    ACS_TRACE(fnName);

    AmbMessage_t      control;

    /* Load the data into the message request structure */
    control.requestType  = AMB_SHUTDOWN;
    control.completion_p = reinterpret_cast<AmbCompletion_t*>(NULL);
    control.dataLen      = 0;
    control.targetTE     = 0;
    memset(control.data,0,8);
    control.channel       =0;
    control.address       =0;

    //enviar mensage
}

adsInterface* adsInterface::getInstance(const std::string& remoteIp, const uint8_t (&arrayNetId)[6],  const uint32_t& plcPort)
{
    const string fnName = "adsInterface::getInstance";
    ACS_TRACE(fnName);
    try {
    pthread_mutex_lock(&instanceLock_m);
    if (instance_mp == NULL) {
        ACS_TRACE("HOLORXCompSimBase::adsInterface -- creatre");
        instance_mp = new adsInterface(remoteIp, arrayNetId, plcPort);
    }
    sem_post(&semCount_m);
    pthread_mutex_unlock(&instanceLock_m);
    return instance_mp;
    }
    catch(const ControlExceptions::CAMBErrorExImpl& ex)
    {
    pthread_mutex_unlock(&instanceLock_m);
    throw ControlExceptions::CAMBErrorExImpl(ex,__FILE__,__LINE__,
                         fnName.c_str());
    }
}





void adsInterface::deleteInstance(){
    const string fnName = "adsInterface::deleteInstance";
    ACS_TRACE(fnName);

    int currentValue;
    pthread_mutex_lock(&instanceLock_m);
    if (sem_trywait(&semCount_m) == 0) {
    sem_getvalue(&semCount_m, &currentValue);
    if (currentValue == 0 && instance_mp != NULL) {
        /* No Instances left, delete the instance */
        delete instance_mp;
        instance_mp = NULL;
    }
    }
    pthread_mutex_unlock(&instanceLock_m);
}






AmbErrorCode_t adsInterface::findSN(AmbDataMem_t   serialNumber[],
                    AmbChannel     channel,
                    AmbNodeAddr    nodeAddress,
                    Time&          timestamp) const
{
    const string fnName = "adsInterface::findSN";

    AmbCompletion_t* completion_p = new AmbCompletion_t;
    AmbMessage_t     message;

    AmbDataLength_t  dataLength;
    AmbErrorCode_t   status;
    sem_t            synchLock;

    /* Build the completion block */
    completion_p->dataLength_p = &dataLength;
    completion_p->data_p       = serialNumber;
    completion_p->channel_p    = NULL;
    completion_p->address_p    = NULL;
    completion_p->timestamp_p  = &timestamp;
    completion_p->status_p     = &status;
    completion_p->synchLock_p  = &synchLock;
    completion_p->contLock_p   = NULL;
    completion_p->type_p       = NULL;

    /* Build the message block */
    message.requestType  = AMB_MONITOR;
    message.channel      = channel;
    try {
    message.address      = createAMBAddr(nodeAddress,0);
    } catch (const ControlExceptions::CAMBErrorExImpl& ex) {
    throw ControlExceptions::CAMBErrorExImpl(ex,__FILE__,__LINE__,
                         fnName.c_str());
    }
    message.dataLen      = 0;
    message.completion_p = completion_p;
    message.targetTE     = 0;

    sem_init(&synchLock, 0, 0);
    /* Send the message and wait for a return */
    try {
    enqueueAdsMessage(message, pointInfoMap);
    }
    catch(const ControlExceptions::CAMBErrorExImpl& ex){
    sem_destroy(&synchLock);
    throw ControlExceptions::CAMBErrorExImpl(ex,__FILE__,__LINE__,
                         fnName.c_str());
    }
    sem_wait(&synchLock);
    sem_destroy(&synchLock);

    switch (status){
    case AMBERR_NOERR:
    break;
    default:
    ControlExceptions::CAMBErrorExImpl ex(__FILE__,__LINE__,fnName.c_str());
    ex.addData("ErrorCode",status);
    ex.log();
    throw ex;
    }

    return status;
}

AmbErrorCode_t adsInterface::findSNUsingBroadcast(unsigned char serialNumber[],
                          AmbChannel    channel,
                          AmbNodeAddr   nodeAddress,
                          Time&       timestamp) const
{
    const string fnName = "adsInterface::findSNUsingBroadcast";

    AmbCompletion_t* completion = new AmbCompletion_t;
    AmbMessage_t     message;
    bool             foundNode = false;

    AmbChannel       channel_resp;
    AmbAddr          node;
    AmbDataLength_t  dataLength;
    AmbDataMem_t     data[8];
    sem_t            synchLock;
    sem_t            contLock;
    AmbErrorCode_t   status;
    sem_init(&synchLock, 0, 0);
    sem_init(&contLock, 0, 1);

    /* Build the completion block */
    completion->dataLength_p = &dataLength;
    completion->data_p       = data;
    completion->channel_p    = &channel_resp;
    completion->address_p    = &node;
    completion->timestamp_p  = &timestamp;
    completion->status_p     = &status;
    completion->synchLock_p  = &synchLock;
    completion->contLock_p   = &contLock;
    completion->type_p       = NULL;

    /* Build the message block */
    message.requestType  = AMB_GET_NODES;
    message.channel      = channel;
    message.address      = 0;
    message.dataLen      = 0;
    message.completion_p = completion;
    message.targetTE     = 0;

    try {
    enqueueAdsMessage(message, pointInfoMap);
    }
    catch (const ControlExceptions::CAMBErrorExImpl& ex) {
    sem_destroy(&synchLock);
    sem_destroy(&contLock);
    throw ControlExceptions::CAMBErrorExImpl(ex,__FILE__,__LINE__,
                         fnName.c_str());
    }

    sem_wait(&synchLock);
    while (status == AMBERR_CONTINUE) {
    if (node == nodeAddress) {
        if (dataLength != 8) {
        ACS_SHORT_LOG((LM_ERROR,
                   "Node Reported incorrect length Serial Number"));
        } else {
        for (int idx = 0; idx < 8; idx++)
            serialNumber[idx] = data[idx];
        foundNode = true;
        }
    }
    sem_post(&contLock);
    sem_wait(&synchLock);
    }

    if(!foundNode) {
    ControlExceptions::CAMBErrorExImpl ex(__FILE__,__LINE__,
                          fnName.c_str());
    ex.log();

    throw ex;
    }
    return status;
}

AmbErrorCode_t adsInterface::flush(AmbChannel channel,
                   Time       flushTime,
                   Time&      timestamp) const
{
    const string fnName = "adsInterface::flush [channel]";

    AmbCompletion_t* completion_p = new AmbCompletion_t;
    AmbMessage_t     message;

    AmbErrorCode_t   status;
    sem_t            synchLock;
    sem_init(&synchLock, 0, 0);

    /* Build the completion block */
    completion_p->dataLength_p = NULL;
    completion_p->data_p       = NULL;
    completion_p->channel_p    = NULL;
    completion_p->address_p    = NULL;
    completion_p->timestamp_p  = &timestamp;
    completion_p->status_p     = &status;
    completion_p->synchLock_p  = &synchLock;
    completion_p->contLock_p   = NULL;
    completion_p->type_p       = NULL;

    /* Build the message block */
    message.requestType  = AMB_FLUSH;
    message.channel      = channel;
    message.address      = 0;
    message.dataLen      = sizeof(Time);
    memcpy(message.data,&flushTime,message.dataLen);
    message.completion_p = completion_p;
    message.targetTE     = 0;

    /* Send the message and wait for a return */
    try {
    enqueueAdsMessage(message, pointInfoMap);
    }
    catch (const ControlExceptions::CAMBErrorExImpl& ex){
    sem_destroy(&synchLock);
    throw ControlExceptions::CAMBErrorExImpl(ex,__FILE__,__LINE__,
                         fnName.c_str());
    }
    sem_wait(&synchLock);
    sem_destroy(&synchLock);

    switch (status){
    case AMBERR_NOERR:
    break;
    default:
    ControlExceptions::CAMBErrorExImpl ex(__FILE__,__LINE__,fnName.c_str());
    ex.addData("ErrorCode",status);
    ex.log();
    throw ex;
    }
    return status;
}
AmbErrorCode_t adsInterface::flush(AmbChannel      channel,
                   AmbNodeAddr     nodeAddress,
                   Time            flushTime,
                   Time&           timestamp) const
{
    const string fnName = "adsInterface::flush [node]";

    AmbCompletion_t* completion_p = new AmbCompletion_t;
    AmbMessage_t     message;

    AmbErrorCode_t   status;
    sem_t            synchLock;

    /* Build the completion block */
    completion_p->dataLength_p = NULL;
    completion_p->data_p       = NULL;
    completion_p->channel_p    = NULL;
    completion_p->address_p    = NULL;
    completion_p->timestamp_p  = &timestamp;
    completion_p->status_p     = &status;
    completion_p->synchLock_p  = &synchLock;
    completion_p->contLock_p   = NULL;
    completion_p->type_p       = NULL;

    /* Build the message block */
    message.requestType  = AMB_FLUSH_NODE;
    message.channel      = channel;
    try {
    message.address      = createAMBAddr(nodeAddress,0);
    } catch (const ControlExceptions::CAMBErrorExImpl& ex) {
    throw ControlExceptions::CAMBErrorExImpl(ex,__FILE__,__LINE__,
                         fnName.c_str());
    }
    message.dataLen      = sizeof(Time);
    memcpy(message.data,&flushTime,message.dataLen);
    message.completion_p = completion_p;
    message.targetTE     = 0;

    sem_init(&synchLock, 0, 0);
    /* Send the message and wait for a return */
    try {
    enqueueAdsMessage(message, pointInfoMap);
    }
    catch (const ControlExceptions::CAMBErrorExImpl& ex){
    sem_destroy(&synchLock);
    throw ControlExceptions::CAMBErrorExImpl(ex,__FILE__,__LINE__,
                         fnName.c_str());
    }
    sem_wait(&synchLock);
    sem_destroy(&synchLock);

    switch (status){
    case AMBERR_NOERR:
    break;
    default:
    ControlExceptions::CAMBErrorExImpl ex(__FILE__,__LINE__,fnName.c_str());
    ex.addData("ErrorCode",status);
    ex.log();
    throw ex;
    }
    return status;
}

AmbErrorCode_t adsInterface::flush(AmbChannel      channel,
                   AmbNodeAddr     nodeAddress,
                   AmbRelativeAddr RCA,
                   Time            flushTime,
                   Time&           timestamp) const
{
    const string fnName = "adsInterface::flush [RCA]";

    AmbCompletion_t* completion_p = new AmbCompletion_t;
    AmbMessage_t     message;

    AmbErrorCode_t   status;
    sem_t            synchLock;

    /* Build the completion block */
    completion_p->dataLength_p = NULL;
    completion_p->data_p       = NULL;
    completion_p->channel_p    = NULL;
    completion_p->address_p    = NULL;
    completion_p->timestamp_p  = &timestamp;
    completion_p->status_p     = &status;
    completion_p->synchLock_p  = &synchLock;
    completion_p->contLock_p   = NULL;
    completion_p->type_p       = NULL;

    /* Build the message block */
    message.requestType  = AMB_FLUSH_RCA;
    message.channel      = channel;
    try {
    message.address      = createAMBAddr(nodeAddress,RCA);
    } catch (const ControlExceptions::CAMBErrorExImpl& ex) {
    throw ControlExceptions::CAMBErrorExImpl(ex,__FILE__,__LINE__,
                         fnName.c_str());
    }
    message.dataLen      = sizeof(Time);
    memcpy(message.data,&flushTime,message.dataLen);
    message.completion_p = completion_p;
    message.targetTE     = 0;

    sem_init(&synchLock, 0, 0);
    /* Send the message and wait for a return */
    try {
    enqueueAdsMessage(message, pointInfoMap);
    }
    catch (const ControlExceptions::CAMBErrorExImpl& ex){
    sem_destroy(&synchLock);
    throw ControlExceptions::CAMBErrorExImpl(ex,__FILE__,__LINE__,
                         fnName.c_str());
    }
    sem_wait(&synchLock);
    sem_destroy(&synchLock);

    switch (status){
    case AMBERR_NOERR:
    break;
    default:
    ControlExceptions::CAMBErrorExImpl ex(__FILE__,__LINE__, fnName.c_str());
    ex.addData("ErrorCode",status);
    ex.addData("NodeAddress",nodeAddress);
    ex.addData("RCA",nodeAddress);
    ex.log();
    throw ex;
    }
    return status;
}
