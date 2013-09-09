#include "xbeetcpbridge.h"


/*
 * PROTOCOL:
 * byte 1: type of message
 *    0x0 -> control
 *    0x1 -> data
 *
 * Control messages:
 *   0x0 XX -> where XX is the command
 *
 *   0x10 -> CTRL_CONNECT_REQ
 *           Request to connect 
 *           |0x0|0x10|, length 2 bytes
 *
 *   0x10 -> CTRL_CONNECT_SUCCESS
 *           Response to a CTRL_CONNECT_REQ, indicates that succeeded 
 *           |0x0|0x11|, length 2 bytes
 * 
 *   0x10 -> CTRL_CONNECT_FAIL
 *           Response to a CTRL_CONNECT_REQ, indicates that failed 
 *           |0x0|0x12|, length 2 bytes
 * 
 *   0x30 -> CTRL_DISCONNECT
 *           Disconnect request or notification
 *           |0x0|0x30|, length 2 bytes
 *
 *   0x40 -> CTRL_DEBUG
 *           Debug message (not forwarded to the client)
 *           |0x0|0x40|size|message|, length >= 3 bytes
 *
 *
 *     CONNECTED---->--.
 *        ^            |
 *        |            |
 *        |            |
 *    CONNECTING---->--.
 *        ^            |
 *        |            |
 *        |            |
 *   DISCONNECTED      |
 *        ^            |
 *        |            |
 *        .---------<--.
 *
 *   
 *  Successful connect:
 * 
 *      BEE                     Proxy
 *     {DISCONNECTED}  
 *        ------CTRL_CONNECT_REQ----->
 *     {CONNECTING}
 *        <---CTRL_CONNECT_SUCCESS--->
 *     {CONNECTED}
 *
 *
 *  Connect fail by proxy
 *
 *      BEE                     Proxy
 *     {DISCONNECTED}  
 *        ------CTRL_CONNECT_REQ----->
 *     {CONNECTING}
 *        <---CTRL_CONNECT_FAIL--->
 *     {DISCONNECTED}
 *
 *
 *  Connect fail by timeout
 *
 *      BEE                     Proxy
 *     {DISCONNECTED}  
 *        ------CTRL_CONNECT_REQ----->
 *     {CONNECTING}
 *          ... (No response) ...
 *          ... (No response) ...
 *     {DISCONNECTED}
 *
 *
 * Disconnect by BEE request
 *
 *      BEE                     Proxy
 *     {CONNECTED}
 *        ------CTRL_DISCONNECT----->
 *     {DISCONNECTED}
 *
 *
 * Disconnect by Proxy request
 *
 *      BEE                     Proxy
 *     {CONNECTED}
 *        <------CTRL_DISCONNECT-----
 *     {DISCONNECTED}
 *
 *
 *
 *
 */

#define CONTROL          0x0
#define DATA             0x1

#define CTRL_CONNECT_REQ     0x10
#define CTRL_CONNECT_SUCCESS 0x11
#define CTRL_CONNECT_FAIL    0x12

#define CTRL_DISCONNECT  0x30
#define CTRL_DEBUG       0x40

XBeeTCPBridge::XBeeTCPBridge(XBee* xbee): 
        proxyAddress(0x0013a200, 0x40a01203), //TODO TODO
        txRequest(this->proxyAddress, 0, 0 ){

    this->xbee = xbee;

    this->dataInBuffer  = false;
    this->sendBufferSize = 0;

    this->dataAvailable = false;
    this->connectTimeout = 10000;
    this->status = DISCONNECTED;
}


bool XBeeTCPBridge::sendDebugMessage(uint8_t* msg, uint8_t size){
    // Send connect message
    uint8_t buffer[255];

    buffer[0] = CONTROL;
    buffer[1] = CTRL_DEBUG;
    buffer[2] = size;
    
    for (int i=0;i<size;i++)
        buffer[i+3] = msg[i];

    this->txRequest.setPayload(buffer);
    this->txRequest.setPayloadLength(size+3);
    xbee->send(this->txRequest);

    // Wait for the XBEE protocol ack
    //if (!this->waitForAck())
      //  return 0;

    return 0;



}

//public
bool XBeeTCPBridge::connect(void){

    // We will only proceed to start the connect phase
    // if we are currently disconnected
    if (this->status != DISCONNECTED)
        return false;
    
    this->status = CONNECT_REQUESTED;
    return true;
}

//private
void XBeeTCPBridge::doConnect(void){

    // Send connect message
    uint8_t buffer[] = {CONTROL,CTRL_CONNECT_REQ};


    // We will only proceed to send the CONNECT msg
    // if there is a connect request
    if (this->status != CONNECT_REQUESTED)
        return;
        
    this->txRequest.setPayload(buffer);
    this->txRequest.setPayloadLength(2);
    xbee->send(this->txRequest);


    // Move to CONNECTING status
    this->status = CONNECTING;
    this->connectTimeout = 10000;
    // Wait for the XBEE protocol ack
    //if (!this->waitForAck())
    //    return 0;

}

/*
bool XBeeTCPBridge::waitForAck(){

    this->xbee->readPacketUntilAvailable();
    
    if (this->xbee->getResponse().isAvailable()) {
        if (this->xbee->getResponse().getApiId() == DM_TX_STATUS_RESPONSE) {
            return 1;
        }
    }

    return 0;
}
*/

void XBeeTCPBridge::disconnect(void){
    // Send disconnect message
    if (this->status == CONNECTED){
        this->status = DISCONNECTED;
        uint8_t buffer[] = {0x0,0x30};
        this->txRequest.setPayload(buffer);
        this->txRequest.setPayloadLength(2);
        xbee->send(this->txRequest);
        //this->waitForAck();
    }
}


bool XBeeTCPBridge::available(void){
      return this->dataAvailable;
}

uint8_t XBeeTCPBridge::read(uint8_t** bufferPtr){

    //TODO we should check if the 1st byte is 0x1
    uint8_t* tmp = (this->rxResponse.getData());
    *bufferPtr = ++tmp;
       
    return (this->rxResponse.getDataLength()-1);
}




bool XBeeTCPBridge::write(uint8_t* buffer, int size){

    // If there is still data in the buffer we cant add more
    if (dataInBuffer)
        return false;


    if (status != CONNECTED)
        return false;

    //TODO size<255 validate


    sendBuffer[0] = DATA;
    
    for (int i=0;i<size;i++)
        sendBuffer[i+1] = buffer[i];

    sendBufferSize = size+1;
    dataInBuffer = true;
    return true;

}


bool XBeeTCPBridge::doSend(){


    // Is there anything to send?
    if (!dataInBuffer)
        return false;

    this->txRequest.setPayload(sendBuffer);
    this->txRequest.setPayloadLength(sendBufferSize);
    xbee->send(this->txRequest);


    dataInBuffer = false;
    // Wait for the XBEE protocol ack
    //TODO should have a timeout
//    if (!this->waitForAck())
  //      return 0;

    return 1;

}


XBeeTCPBridge::Status XBeeTCPBridge::getStatus(){
    return this->status;
}


bool XBeeTCPBridge::connected(){
    return this->status == CONNECTED;;
}



void XBeeTCPBridge::handleControlMessage(){

    uint8_t* tmp = (this->rxResponse.getData());

    // TODO, validate size == 2

    switch(this->status){
        case DISCONNECTED:
            // we do not case about messages from the XBEE while on
            // DISCONNECTED status
            break;


        case CONNECTING:

            // On this status we may expect two packets, either
            // a success msg or a fail to connect msg.

            switch(tmp[1]){
                case CTRL_CONNECT_SUCCESS:
                    this->status = CONNECTED;
                    break;

                case CTRL_CONNECT_FAIL:
                    this->status = DISCONNECTED;
                    break;

                default:
                    // ignore other packets in this state
                    break;

            };

            break;

        case CONNECTED:

            // On CONNECTED status we may receive only a DISCONNECTED message
            // indicating that the our python backend was disconnected from the
            // client.
            // we just need to reflect this status on our status
            
            switch(tmp[1]){
                case CTRL_DISCONNECT:
                    this->status = DISCONNECTED;
                    break;

                default:
                    // ignore other packets in this state
                    break;

            };

            break;

    };




}

void XBeeTCPBridge::doCommunicationsStuff(){

    uint8_t* tmp;
    //uint8_t msg[] = "avaiable    ";


    this->dataAvailable = false;
    this->xbee->readPacket();

    if (this->xbee->getResponse().isAvailable()){
        // We say that data is avaiable only if data is a DM_RX_RESPONSE
        uint8_t apiId = this->xbee->getResponse().getApiId();

//        msg[10] = apiId;
//        sendDebugMessage(msg,13);

        switch(apiId){

            case DM_TX_STATUS_RESPONSE:
                // Do something?
                break;

            case DM_RX_RESPONSE:
                // data is available!

                // now fill our zb rx class
                this->xbee->getResponse().getDMRxResponse(this->rxResponse);

                tmp = (this->rxResponse.getData());


                //msg[10] = tmp[0];
                //msg[11] = tmp[1];
                //sendDebugMessage(msg,13);


                if (tmp[0] == DATA){
                    this->dataAvailable = true;
                }
                else if (tmp[0] == CONTROL){
                    this->handleControlMessage();

                }

                break;

           default :
                // handle other stuff?
                break;
        };

    }
    else {
        // If there is no data to read we can send
        doSend();

    }


        //TODO should we handle these?
    /*
        else if (xbee.getResponse().getApiId() == MODEM_STATUS_RESPONSE) {
        xbee.getResponse().getModemStatusResponse(msr);
        // the local XBee sends this response on certain events, like association/dissociation

        if (msr.getStatus() == ASSOCIATED) {
          // yay this is great.  flash led
          flashLed(statusLed, 10, 10);
        } else if (msr.getStatus() == DISASSOCIATED) {
          // this is awful.. flash led to show our discontent
          flashLed(errorLed, 10, 10);
        } else {
          // another status
          flashLed(statusLed, 5, 10);
        }
      } else {
 // not something we were expecting
        flashLed(errorLed, 1, 25);
      }
    } else if (xbee.getResponse().isError()) {
      //nss.print("Error reading packet.  Error code: ");  
      //nss.println(xbee.getResponse().getErrorCode());
    }
*/

}



void XBeeTCPBridge::loop(){

    switch(status){
        case DISCONNECTED:
            break;

        case CONNECT_REQUESTED:
            this->doConnect();
            break;

        case CONNECTING:

            // Check timeout
            this->connectTimeout--;
            if (connectTimeout == 0)
                this->status = DISCONNECTED;

            doCommunicationsStuff();
            break;

        case CONNECTED:
            doCommunicationsStuff();
            break;
    }

}



