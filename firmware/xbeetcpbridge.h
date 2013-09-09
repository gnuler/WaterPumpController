#ifndef __XBEE_TCP_BRIDGE_H__
#define __XBEE_TCP_BRIDGE_H__

#include "XBee.h"
#include <inttypes.h>
#include <stdio.h>

class XBeeTCPBridge {

 public:
    enum Status  { 
        DISCONNECTED       = 0,
        CONNECT_REQUESTED  = 1,
        CONNECTING         = 2,
        CONNECTED          = 3
        };


  private:

    XBeeAddress64 proxyAddress;
    DMTxRequest txRequest;
    DMRxResponse rxResponse;
    XBee* xbee;
    Status status;
    unsigned int connectTimeout;


    // Very simple send buffer (only one message)
    uint8_t sendBuffer[255];

    // Length of the data in the buffer
    uint8_t sendBufferSize;

    // Is there data in the buffer to send?
    bool    dataInBuffer;


    // Is there data available to read?
    bool dataAvailable;


    //bool waitForAck();
    void handleControlMessage(void);

    // Send data in the sendBuffer, if any.
    bool doSend(void);

    void doConnect(void);

    void doCommunicationsStuff(void);


  public:

    XBeeTCPBridge(XBee* xbee);

    /*
      Initiates the connect phase
    */
    bool connect(void);

    /*
      Sends a disconnect message
    */
    void disconnect(void);

    /* 
      Sends a debug message to the tcp proxy, it will not be forwarded to the
      client
    */
    bool sendDebugMessage(uint8_t* msg, uint8_t size);

    /*
      Returns true if there is data available to be read
    */
    bool available(void);

    /*
       Stores in buff the ptr to the received data
       returns the length of it.
       If there is no data returns 0
    */
    uint8_t read(uint8_t** buff);

    /*
       Enqueues data in the send buffer, so when it is possible it will be sent
    */
    bool write(uint8_t* buffer, int size);

    /*
      Returns true if the proxy is connected to the tcp client, false otherwise
    */
    bool connected();

    /* 
      Returns the connection status
    */
    Status getStatus(); 

    /* 
      Does the proxy stuff :)   
      Must be called periodically
    */
    void loop();
};
#endif
