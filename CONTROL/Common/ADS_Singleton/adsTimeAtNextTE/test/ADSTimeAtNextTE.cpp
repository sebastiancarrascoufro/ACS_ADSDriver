#include <chrono>
#include "ambDefs.h"
#include "adsDefs.h"
#include <varDefs.h>
#include <ControlExceptions.h>
// for ControlDeviceExceptions::HwLifecycleEx
#include <ControlDeviceExceptions.h>
#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <exception>
#include <unistd.h>


uint32_t getOffset(std::string varName, const AdsDevice& route){
    long nErr;
    uint32_t lHdlVar=0, bytesRead;
    char szVar[varName.size() + 1];
    strcpy(szVar, varName.c_str()); 
    //we get an ADS connection available
    
    nErr = route.ReadWriteReqEx2(0xF003, 0x0, sizeof(lHdlVar), &lHdlVar, sizeof(szVar), szVar, &bytesRead);
	if (nErr){
                 ACS_STATIC_LOG(LM_SOURCE_INFO, __FUNCTION__, (LM_ERROR, "ADS ReadWriteReqEx2 failed with code: %ld. ", nErr));
        
             }
    //we insert the ADS connection to the queue again
    
    return lHdlVar;
}

int main(int argc, char* argv[])
{
	bool result = true;
	static const AmsNetId remoteNetId { arr[0],arr[1],arr[2],arr[3],arr[4],arr[5] };
	AdsDevice route {remoteIp.data(), remoteNetId, port};
	uint32_t offset = getOffset("GVL.TE#a_setNextTE" ,route);
	timeval tim;
	unsigned long long acstimestamp = 0ULL;
	unsigned long long tmp = 0ULL;
	double start;

	gettimeofday(&tim, NULL);
	start = tim.tv_sec + (tim.tv_usec / 1000000.0);

	//local time convert into acstimestamp
	acstimestamp = static_cast<unsigned long long>(start) * 10000000LL + 122192928000000000LL;
	//round it into a number divsible by a TE
	tmp = acstimestamp % 480000LL;
	acstimestamp = acstimestamp - tmp; 
	//we are able only to adjsut time to the future not to the past. Since we substract part of the acstimestamp (tmp), thefore compensate with another TE
	acstimestamp += 480000LL;
	//since we are asking Time at next TE, add another TE
	acstimestamp += 480000LL;

	long nErr;
        uint32_t bytesRead;
        bool rValue;
	TEHandlerData_t message;
	message.time = acstimestamp;
	message.resyncFlag = false;
	nErr = route.ReadWriteReqEx2(ADSIGRP_SYM_VALBYHND, offset, sizeof(rValue), &rValue, sizeof(message), &message, &bytesRead);
	if (nErr){
                 ACS_STATIC_LOG(LM_SOURCE_INFO, __FUNCTION__, (LM_ERROR, "ADS ReadWriteReqEx2 failed with code: %ld. ", nErr));
        
             }
	std::cout << "TEST OK" << std::endl;
	std::this_thread::sleep_for(std::chrono::milliseconds(5000));
	std::this_thread::sleep_for(std::chrono::milliseconds(5000));
	//delete pool;
	return result ? 1 : 0;
}



