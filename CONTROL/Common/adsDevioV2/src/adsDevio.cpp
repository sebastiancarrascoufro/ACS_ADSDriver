#include <TETimeUtil.h>
#include "adsDevio.h"
#include <string>
#include <type_traits>
#include <chrono>

/**
 * Constructor, x + 1 ADS connections are started and stored in a queue to be used when required, 
 * one of these connections is reserved for notifications, additionally a Threadpool of size x is created, 
 * each ADS request will be queued and processed in a different thread using a free ADS connection
 @params
 * devName: HWDevice name
 * remoteIpV4: ipv4 from ADS server
 * arrayNetId: AMS netId of the ADS server
 * port: PLC ADS port
 * ps: size of thread pool
*/
adsDevio::adsDevio(const std::string& devName, const std::string& remoteIp, const uint8_t (&arrayNetId)[6],  const uint32_t& plcPort, const uint32_t& ps)

{
    deviceName = devName;
    AmsNetId remoteNetId { arrayNetId[0], arrayNetId[1], arrayNetId[2], arrayNetId[3], arrayNetId[4], arrayNetId[5] };
    

    try {
        //ThreadPool instance is created
        pool = new AdsThreadPool(ps);
        //we start the queue of ADS connections
        q = new AdsQueue();
        tQ = new tQueue();
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
	for (uint16_t i = 0; i <= bufferSize; i++) {
            auto result = pool->enqueue(&adsDevio::clearBuffer,deviceName, q, i);  
        }
        
        //we give it a little time to finish cleaning the buffer
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        startNotificationDevice();
	timeout	= 500;
	tr = runThread();
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
     
}


adsDevio::~adsDevio()
{
    
    try {
        delete pool;
        //we remove all ADS connections
        while (!(q->empty())) {
            delete (q->pop()); 
        }
        delete q;
	std::this_thread::sleep_for(std::chrono::milliseconds(500));
	exit_signal = true;
        delete notificationRoute;
	tr.join();
    }
    catch (const std::exception & exc)
    {

    }

}

/**
* this function clears the last value stored in the ADS server 
* buffer of the HWDevice (this avoids problems with possible previous values when starting)
@params
* deviceName: HWDevice name
* q: pointer to ADS connection queue
*/
void adsDevio::clearBuffer(std::string deviceName, AdsQueue* q, uint16_t i){
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
 * The notification service is started for the PLC symbol where all the HwDevice responses are sent.
*/
void adsDevio::startNotificationDevice()
{
    
    /**
    * Definition of the attributes of the notification. 
    * ADSTRANS_SERVERONCHA indicates that a notification will be generated when the variable changes 
    * and {0} indicates the sample rate of the variable on the server
    */
    const AdsNotificationAttrib attrib = {
        sizeof(AdsResponse_t),
        ADSTRANS_SERVERONCHA,
        0,
        {0}
    };
    
    try
    {
	for (uint32_t i = 0; i <= bSize; i++) {
            notif = new AdsNotification (*notificationRoute, deviceName+".RESP["+std::to_string(i)+"]", attrib, &NotifyCallback, 0xBEEFDEAD );
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
 * The reference of the nodes of interest is obtained and stored.
 @params
 * RCA: monitoring/control point rca
 * pointName: Name of the monitoring/control point (the ADS server has nodes implemented with the same name)
*/
void adsDevio::insertRCA(AmbRelativeAddr RCA, std::string pointName){
    std::string devRoute = deviceName+"."+pointName;
    pointNodeInfo_t pointInfo;
    pointInfo.nodeName = pointName;
    pointInfo.responseNode = (pool->enqueue(&adsDevio::getOffset, devRoute+".OPCUA_ResponseValue", q)).get();
    pointInfo.methodNode = (pool->enqueue(&adsDevio::getOffset, devRoute+"#DispatcherMethod", q)).get();
    pointInfo.cleanNode = (pool->enqueue(&adsDevio::getOffset, devRoute+"#CleanNode", q)).get();
    if (pointInfo.responseNode == 0 || pointInfo.methodNode==0 || pointInfo.cleanNode == 0)
    {
        ControlExceptions::CAMBErrorExImpl ex(__FILE__, __LINE__,
                              "adsDevio::insertRCA");
        ex.addData(Control::EX_USER_ERROR_MSG,
                   "one or more "+pointName+" symbols were not found on the ADS server");
        ex.log();
        throw ex;
    }
    pointInfoMap.insert({ RCA, pointInfo});


}


/**
 * Get the index offset of the requested symbol.
 @params
 * devRoute: PLC Symbol name
 * q: pointer to ADS connection queue
 @return symbol index offset
*/
uint32_t adsDevio::getOffset(std::string devRoute, AdsQueue* q){
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
* The sendAdsMessage call is queued to be processed when there is an available ADS connection
@params
* message: AMBmessage to transmit to the ADS server
*/
void adsDevio::enqueueAdsMessage(const AmbMessage_t& message) const
{
    auto result = pool->enqueue(&adsDevio::sendAdsMessage,message,pointInfoMap, tQ,q);
}


/**
* Logic of sending the message to ADS, the specific node to send the command is obtained, 
* the data is packed and the message is sent, this is called when a thread is available
@params
* message: AMBmessage to transmit to the ADS server
* pointInfoMap: map with necessary information of the monitoring/control points of the device
* q: pointer to ADS connection queue
*/
void adsDevio::sendAdsMessage(const AmbMessage_t& message, DeviceMap pointInfoMap, tQueue* tQ, AdsQueue* q)
{
    const std::string fnName = "adsDevio::sendAdsMessage";
    

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
        bool rValue;
        //we get an ADS connection available
        AdsDevice* msgDevice = q->pop();
        nErr = msgDevice->ReadWriteReqEx2(ADSIGRP_SYM_VALBYHND, point.methodNode, sizeof(rValue), &rValue, sizeof(message), &message, &bytesRead);
        //we insert the ADS connection to the queue again
        q->emplace(msgDevice);
        if (nErr || !rValue)
        {
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

		uint64_t ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::
				  now().time_since_epoch()).count();
		tQ->emplace( timeoutData_t{message.completion_p, ms});
	}
}




/** This method is called when a notification is received from the ADS server.
 * It receives the response, unpacks the received data and processes it to complete the processing of the AMB Commands.
 @params
 * pAddr: complete address of ADS device
 * pNotification: structure that contains the handle, data, length of the data and timestamp of the notification generated by the server
 * hUser: user reference
 */
void adsDevio::NotifyCallback(const AmsAddr* pAddr, const AdsNotificationHeader* pNotification, uint32_t hUser)
{
    const uint8_t* data = reinterpret_cast<const uint8_t*>(pNotification + 1);
    AdsResponse_t  response;
    memcpy( &response,data, sizeof(response));
	


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
            if (response.completion_p != NULL && response.completion_p->timestamp_p != 0)
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
			                	
			response.completion_p->timestamp_p = NULL;   
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



/** This function copies the information into the correct places in the completion structure; provided that they are defined.
 * This is copied and pasted from AMBInterface.cpp
@params
* response: AMB command response structure
*/
void adsDevio::fillCompletion(AdsResponse_t& response){
    if (response.completion_p->dataLength_p != NULL) {
        if(*(response.completion_p->dataLength_p) > 0 &&
           *(response.completion_p->dataLength_p) < response.dataLen &&
       response.completion_p->type_p == NULL) {  // The correlator always uses the original default behavior and is the only user of the type
            static RepeatGuard repeatGuard(ACS::TimeInterval(60.0/100e-9), 0);
            if(repeatGuard.check()) {
                std::ostringstream msg;
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
                std::ostringstream msg;
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

    if (response.completion_p->status_p != NULL) {
    *(response.completion_p->status_p) = response.status;
    }
}



/**
 * function to start the timeout checking thread 
**/
std::thread adsDevio::runThread() {
	return std::thread([=] { verifyTimeouts(); });
}

/**
 * This function runs in a thread that checks every X time if there are commands that have taken too long to respond, 
 * if this is the case, it ends them without waiting for a response from the server.
 * 
 */
void adsDevio::verifyTimeouts() {
	timeoutData_t tData = {0,0};
	while(!exit_signal) {
		std::this_thread::sleep_for(std::chrono::milliseconds((uint32_t)(timeout)));
		uint64_t ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock:: 				  now().time_since_epoch()).count();

		//mtx.lock();
		//all data stored on the map is observed to verify if there is any that must be manually finalized	
		while (!(tQ->empty())) {
			if (tData.completion_p == NULL){
				tData = tQ->pop();
			}
			if (ms - tData.time > timeout){
			
				if (tData.completion_p->timestamp_p != 0){
					if (tData.completion_p->status_p  != NULL) {
						*(tData.completion_p->status_p) = AMBERR_WRITEERR;
					    }
					    if(tData.completion_p->timestamp_p != 0)
					    {
						*(tData.completion_p->timestamp_p) = 0;
					    }
					if (tData.completion_p->synchLock_p != NULL){
						sem_post(tData.completion_p->synchLock_p);
					}
				// the completion_p timestamps pointer is used to know if the response has already arrived (when it is NULL), if this causes a conflict, a better option would be to add a flag to completion_p
				tData.completion_p->timestamp_p = NULL;  
				
				ACS_STATIC_LOG(LM_SOURCE_INFO, __FUNCTION__, (LM_WARNING, "timeout, message loss."));
			}	
			// all completion_p pointers are removed here, not when the message is received 	
			delete(tData.completion_p);
			tData.completion_p = NULL;
			}else{
				break;
			}		

		} 
		//mtx.unlock();
	}

	delete tQ;
}

/**
 * from here everything is copied and pasted from ambInterface.cpp
 */
AmbErrorCode_t adsDevio::findSN(AmbDataMem_t   serialNumber[],
                    AmbChannel   channel,
                    AmbNodeAddr nodeAddress,
                    Time&         timestamp) const
{
    const std::string fnName = "adsDevio::findSN";
    AmbCompletion_t* completion_p = new AmbCompletion_t;
    AmbMessage_t     message;

    AmbDataLength_t  dataLength;
    AmbErrorCode_t   status;
    sem_t           synchLock;

    /* Build the completion block */
    completion_p->dataLength_p = &dataLength;
    completion_p->data_p       = serialNumber;
    completion_p->channel_p = NULL;
    completion_p->address_p = NULL;
    completion_p->timestamp_p  = &timestamp;
    completion_p->status_p   = &status;
    completion_p->synchLock_p  = &synchLock;
    completion_p->contLock_p   = NULL;
    completion_p->type_p       = NULL;

    /* Build the message block */
    message.requestType  = AMB_MONITOR;
    message.channel   = channel;
    try {
    message.address   = createAMBAddr(nodeAddress,0);
    } catch (const ControlExceptions::CAMBErrorExImpl& ex) {
    throw ControlExceptions::CAMBErrorExImpl(ex,__FILE__,__LINE__,
                         fnName.c_str());
    }
    message.dataLen   = 0;
    message.completion_p = completion_p;
    message.targetTE     = 0;

    sem_init(&synchLock, 0, 0);
    /* Send the message and wait for a return */
    try {
    enqueueAdsMessage(message);
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

AmbErrorCode_t adsDevio::findSNUsingBroadcast(unsigned char serialNumber[],
                          AmbChannel    channel,
                          AmbNodeAddr   nodeAddress,
                          Time&    timestamp) const
{
    const std::string fnName = "adsDevio::findSNUsingBroadcast";
    AmbCompletion_t* completion = new AmbCompletion_t;
    AmbMessage_t     message;
    bool             foundNode = false;

    AmbChannel     channel_resp;
    AmbAddr       node;
    AmbDataLength_t  dataLength;
    AmbDataMem_t     data[8];
    sem_t           synchLock;
    sem_t           contLock;
    AmbErrorCode_t   status;
    sem_init(&synchLock, 0, 0);
    sem_init(&contLock, 0, 1);

    /* Build the completion block */
    completion->dataLength_p = &dataLength;
    completion->data_p     = data;
    completion->channel_p   = &channel_resp;
    completion->address_p   = &node;
    completion->timestamp_p  = &timestamp;
    completion->status_p     = &status;
    completion->synchLock_p  = &synchLock;
    completion->contLock_p   = &contLock;
    completion->type_p     = NULL;

    /* Build the message block */
    message.requestType  = AMB_GET_NODES;
    message.channel   = channel;
    message.address   = 0;
    message.dataLen   = 0;
    message.completion_p = completion;
    message.targetTE     = 0;

    try {
    enqueueAdsMessage(message);
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

AmbErrorCode_t adsDevio::flush(AmbChannel channel,
                   Time    flushTime,
                   Time&      timestamp) const
{
    const std::string fnName = "adsDevio::flush [channel]";
    AmbCompletion_t* completion_p = new AmbCompletion_t;
    AmbMessage_t     message;

    AmbErrorCode_t   status;
    sem_t           synchLock;

    /* Build the completion block */
    completion_p->dataLength_p = NULL;
    completion_p->data_p       = NULL;
    completion_p->channel_p = NULL;
    completion_p->address_p = NULL;
    completion_p->timestamp_p  = &timestamp;
    completion_p->status_p   = &status;
    completion_p->synchLock_p  = &synchLock;
    completion_p->contLock_p   = NULL;
    completion_p->type_p       = NULL;

    /* Build the message block */
    message.requestType  = AMB_FLUSH;
    message.channel   = channel;
    message.address   = 0;
    message.dataLen   = sizeof(Time);
    memcpy(message.data,&flushTime,message.dataLen);
    message.completion_p = completion_p;
    message.targetTE     = 0;

    /* Send the message and wait for a return */
    try {
    //cleanNodes(*points_node);
    status = AMBERR_NOERR;
    }
    catch (const ControlExceptions::CAMBErrorExImpl& ex){
    throw ControlExceptions::CAMBErrorExImpl(ex,__FILE__,__LINE__,
                         fnName.c_str());
    }
    return status;
}
AmbErrorCode_t adsDevio::flush(AmbChannel      channel,
                   AmbNodeAddr   nodeAddress,
                   Time         flushTime,
                   Time&           timestamp) const
{
    const std::string fnName = "adsDevio::flush [node]";
    AmbCompletion_t* completion_p = new AmbCompletion_t;
    AmbMessage_t     message;

    AmbErrorCode_t   status;
    sem_t           synchLock;
    
    /* Build the completion block */
    completion_p->dataLength_p = NULL;
    completion_p->data_p       = NULL;
    completion_p->channel_p = NULL;
    completion_p->address_p = NULL;
    completion_p->timestamp_p  = &timestamp;
    completion_p->status_p   = &status;
    completion_p->synchLock_p  = &synchLock;
    completion_p->contLock_p   = NULL;
    completion_p->type_p       = NULL;

    /* Build the message block */
    message.requestType  = AMB_FLUSH_NODE;
    message.channel   = channel;
    try {
    message.address   = createAMBAddr(nodeAddress,0);
    } catch (const ControlExceptions::CAMBErrorExImpl& ex) {
    throw ControlExceptions::CAMBErrorExImpl(ex,__FILE__,__LINE__,
                         fnName.c_str());
    }
    message.dataLen   = sizeof(Time);
    memcpy(message.data,&flushTime,message.dataLen);
    message.completion_p = completion_p;
    message.targetTE     = 0;

    sem_init(&synchLock, 0, 0);
    /* Send the message and wait for a return */
    try {
    enqueueAdsMessage(message);
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

AmbErrorCode_t adsDevio::flush(AmbChannel      channel,
                   AmbNodeAddr   nodeAddress,
                   AmbRelativeAddr RCA,
                   Time         flushTime,
                   Time&           timestamp) const
{
    const std::string fnName = "adsDevio::flush [RCA]";
    AmbCompletion_t* completion_p = new AmbCompletion_t;
    AmbMessage_t     message;

    AmbErrorCode_t   status;
    sem_t           synchLock;

    /* Build the completion block */
    completion_p->dataLength_p = NULL;
    completion_p->data_p       = NULL;
    completion_p->channel_p = NULL;
    completion_p->address_p = NULL;
    completion_p->timestamp_p  = &timestamp;
    completion_p->status_p   = &status;
    completion_p->synchLock_p  = &synchLock;
    completion_p->contLock_p   = NULL;delete(completion_p);
    completion_p->type_p       = NULL;

    /* Build the message block */
    message.requestType  = AMB_FLUSH_RCA;
    message.channel   = channel;
    try {
    message.address   = createAMBAddr(nodeAddress,RCA);
    } catch (const ControlExceptions::CAMBErrorExImpl& ex) {
    throw ControlExceptions::CAMBErrorExImpl(ex,__FILE__,__LINE__,
                         fnName.c_str());
    }
    message.dataLen   = sizeof(Time);
    memcpy(message.data,&flushTime,message.dataLen);
    message.completion_p = completion_p;
    message.targetTE     = 0;

    sem_init(&synchLock, 0, 0);
    /* Send the message and wait for a return */
    try {
    enqueueAdsMessage(message);
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
