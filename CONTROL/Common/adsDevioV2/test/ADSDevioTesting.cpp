#include "ADSDevioTesting.h"
#include <iostream>
#include <string>
#include <chrono>

#include <thread>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


#include <ControlExceptions.h>



void *testSendMessageRCA(void *threadid) {
 

    uint32_t rca;
    rca = (long)threadid;

    AmbDataLength_t dataLength(1U);
    unsigned char rawBytes[8];
    uint8_t raw(0U);
    raw = 0x01;
    unsigned char* praw = reinterpret_cast< unsigned char* >(&raw);
    for(unsigned short i(0U); i < 1; ++i)
    {
        rawBytes[i] = praw[i];
    }

    ACS::Time timestamp(0ULL);  
    sem_t synchLock;
    AmbErrorCode_t status(AMBERR_PENDING);
    sem_init(&synchLock, 0, 0);


    AmbCompletion_t* completion = new AmbCompletion_t;
    ::memset(completion, 0, sizeof(AmbCompletion_t));
    // Build the completion block
    completion->dataLength_p = &dataLength;
    completion->data_p = rawBytes;
    completion->timestamp_p = &timestamp;
    completion->status_p = &status;
    completion->synchLock_p = &synchLock;

    AmbMessage_t message;
    // Build the message block
    //message.requestType = AMB_MONITOR;
    message.channel = 0;
    message.address = rca;
    for(unsigned int idx(0U); idx < dataLength; ++idx)
    {
    message.data[idx] = rawBytes[idx];
    }
    message.dataLen = dataLength;
    message.completion_p = completion;
    message.targetTE = 0;

       
    interface_mp->enqueueAdsMessage(message);

    sem_wait(&synchLock);
    sem_destroy(&synchLock);
    pthread_exit(NULL);
}


void ADSDevioTesting::setUp(){
    gethostname(hostname, 1024);   
    
}
void ADSDevioTesting::tearDown(){ 
}

    
 
CPPUNIT_TEST_SUITE_REGISTRATION( ADSDevioTesting );
 
void ADSDevioTesting::testInstance() {
    std::cout << __FUNCTION__ << "():" << std::endl;
    interface_mp = new adsDevio(dev, remoteIp, arr, port, pmap.size()-1); 
    CPPUNIT_ASSERT( true ); 
}
 
void ADSDevioTesting::testInsertRCA() {
    std::cout << __FUNCTION__ << "():" << std::endl;
        for( DevicePointMap::const_iterator it = pmap.begin(); it != pmap.end(); ++it )
        {
            uint32_t key = it->first;
            std::string value = it->second;
        interface_mp->insertRCA(key, value);
    }
    CPPUNIT_ASSERT( true );    
}

void ADSDevioTesting::testInsertWrongRCA() {
    
        uint32_t key = 1000001;
        std::string value = "wrongSymbol";
    interface_mp->insertRCA(key, value);
     
}

void ADSDevioTesting::testWrongRCAMessage() {
    std::cout << __FUNCTION__ << "():" << std::endl;
    AmbDataLength_t dataLength(1U);
    unsigned char rawBytes[8];
    uint8_t raw(0U);
    raw = 0x01;
    unsigned char* praw = reinterpret_cast< unsigned char* >(&raw);
    for(unsigned short i(0U); i < 1; ++i)
    {
        rawBytes[i] = praw[i];
    }

    ACS::Time timestamp(0ULL);  
    sem_t synchLock;
    AmbErrorCode_t status(AMBERR_PENDING);
    
    AmbRelativeAddr rca = 2000020;
    sem_init(&synchLock, 0, 0);


    AmbCompletion_t* completion = new AmbCompletion_t;
    ::memset(completion, 0, sizeof(AmbCompletion_t));
    // Build the completion block
    completion->dataLength_p = &dataLength;
    completion->data_p = rawBytes;
    completion->timestamp_p = &timestamp;
    completion->status_p = &status;
    completion->synchLock_p = &synchLock;

    AmbMessage_t message;
    // Build the message block
    //message.requestType = AMB_MONITOR;
    message.channel = 0;
    message.address = rca;
    for(unsigned int idx(0U); idx < dataLength; ++idx)
    {
    message.data[idx] = rawBytes[idx];
    }
    message.dataLen = dataLength;
    message.completion_p = completion;
    message.targetTE = 0;

       
    interface_mp->enqueueAdsMessage(message);

    sem_wait(&synchLock);
    sem_destroy(&synchLock);
    CPPUNIT_ASSERT( status == AMBERR_WRITEERR );

}

void ADSDevioTesting::testSendMessage() {
    std::cout << __FUNCTION__ << "():" << std::endl;
    AmbDataLength_t dataLength(1U);
    unsigned char rawBytes[8];
    uint8_t raw(0U);
    raw = 0x01;
    unsigned char* praw = reinterpret_cast< unsigned char* >(&raw);
    for(unsigned short i(0U); i < 1; ++i)
    {
        rawBytes[i] = praw[i];
    }

    ACS::Time timestamp(0ULL);  
    sem_t synchLock;
    AmbErrorCode_t status(AMBERR_PENDING);
    
    AmbRelativeAddr rca = 9175040;
    sem_init(&synchLock, 0, 0);


    AmbCompletion_t* completion = new AmbCompletion_t;
    ::memset(completion, 0, sizeof(AmbCompletion_t));
    // Build the completion block
    completion->dataLength_p = &dataLength;
    completion->data_p = rawBytes;
    completion->timestamp_p = &timestamp;
    completion->status_p = &status;
    completion->synchLock_p = &synchLock;

    AmbMessage_t message;
    // Build the message block
    //message.requestType = AMB_MONITOR;
    message.channel = 0;
    message.address = rca;
    for(unsigned int idx(0U); idx < dataLength; ++idx)
    {
    message.data[idx] = rawBytes[idx];
    }
    message.dataLen = dataLength;
    message.completion_p = completion;
    message.targetTE = 0;

       
    interface_mp->enqueueAdsMessage(message);

    sem_wait(&synchLock);
    sem_destroy(&synchLock);
    CPPUNIT_ASSERT( status == AMBERR_NOERR );
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

}


void ADSDevioTesting::testInstanceMultiple(){
    std::cout << __FUNCTION__ << "():" << std::endl;
    adsDevio *interface_mp1 = new adsDevio("AMBSI_DEVICE", remoteIp, arr, port, pmap.size()-1); 
    adsDevio *interface_mp2 = new adsDevio("AMBSI_DEVICE", remoteIp, arr, port, pmap.size()-1); 

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    delete interface_mp1;
    delete interface_mp2;
    CPPUNIT_ASSERT( true ); 
}

void ADSDevioTesting::testSendMessageThread(){
    std::cout << __FUNCTION__ << "():" << std::endl;



    int rc;
    uint32_t i;
    uint32_t tnum = pmap.size();
    uint32_t lrca = 9175040;
    bool result = true;
    pthread_t threads[tnum];
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    void *status;
    for( i = 0; i < tnum; i++ ) {
        rc = pthread_create(&threads[i], &attr, testSendMessageRCA, (void *)(intptr_t)lrca);
        if (rc) {
            cout << "Error:unable to create thread," << rc << endl;
            result = false;
            exit(-1);
        }
    }

    pthread_attr_destroy(&attr);
    for( i = 0; i < tnum; i++ ) {
        rc = pthread_join(threads[i], &status);
        if (rc) {
            result = false;
            cout << "Error:unable to join," << rc << endl;
            exit(-1);
        }
    }
    CPPUNIT_ASSERT( result ); 
}



void ADSDevioTesting::testInstanceremoteIpError() {
    std::cout << __FUNCTION__ << "():" << std::endl;
    std::string testRemoteIp = "192.168.1.250";
    interface_mp = new adsDevio(dev, testRemoteIp, arr, port, pmap.size()-1); 
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
}

void ADSDevioTesting::testInstanceDeviceNameError() {
    std::cout << __FUNCTION__ << "():" << std::endl;
    std::string testDev = "HWDevice";  
    interface_mp = new adsDevio(testDev, remoteIp, arr, port, pmap.size()-1); 
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
}

void ADSDevioTesting::testInstancePlcPortError() {
    std::cout << __FUNCTION__ << "():" << std::endl;
    uint32_t testPort = 849;
    interface_mp = new adsDevio(dev, remoteIp, arr, testPort, pmap.size()-1); 
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}
void ADSDevioTesting::testInstanceAMSNetIdError() {
    std::cout << __FUNCTION__ << "():" << std::endl;
    uint8_t testArr[] = {192, 168, 56, 1, 1, 255}; 
    interface_mp = new adsDevio(dev, remoteIp, testArr, port, pmap.size()-1); 
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
}

void ADSDevioTesting::testDeleteInstance(){
    std::cout << __FUNCTION__ << "():" << std::endl;
    delete interface_mp;
    CPPUNIT_ASSERT( true ); 
}


