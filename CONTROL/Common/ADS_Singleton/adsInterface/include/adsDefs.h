#ifndef ADSDEFS_H
#define ADSDEFS_H



/**
* This is passed back to the users space process from ADS server
* DevicePointMap
*/

typedef struct {
AmbDataMem_t     data[AMB_DATA_MSG_SIZE];      //  The Returned data
Time             timestamp;    //  Timestamp of completion
AmbCompletion_t* completion_p;
AmbChannel       channel;      //  Channel Address
AmbAddr          address;      //  AMB Address
AmbNodeType      type;     //  Type of node responding (when applicable, ignored otherwise)
AmbErrorCode_t   status;       //  Status flag 0 if successful
unsigned int       magic;        // Magic word to Verify Structure is complete
AmbDataLength_t  dataLen;      //  Length of returned data
} AdsResponse_t;


/**
* structure to store references of the OPC UA nodes of each monitoring/control point of the HWDevice
*/
typedef struct {
    std::string nodeName;
    uint32_t responseNode;
    uint32_t cleanNode;
    uint32_t methodNode;
} pointNodeInfo_t;  



typedef std::map<uint32_t, pointNodeInfo_t>  DeviceMap;





#endif /*!ADSDEFS_H*/
