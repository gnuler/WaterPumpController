/*
 PubSubClient.cpp - A simple client for MQTT.
  Nicholas O'Leary
  http://knolleary.net
*/

#include "PubSubClient.h"
#include <string.h>

PubSubClient::PubSubClient(MQTTMessageReceiver* mqttReceiver, XBeeTCPBridge* tcpbridge) {
    this->_tcpbridge = tcpbridge;
    this->status = DISCONNECTED;
    this->mqttReceiver = mqttReceiver;
}

/*
PubSubClient::PubSubClient(void (*callback)(char*,uint8_t*,unsigned int), XBeeTCPBridge* tcpbridge) {
//PubSubClient::PubSubClient(uint8_t *ip, uint16_t port, void (*callback)(char*,uint8_t*,unsigned int), XBeeTCPBridge* tcpbridge) {
    this->_tcpbridge = tcpbridge;
    this->callback = callback;
    this->ip = ip;
    this->port = port;
    this->status = DISCONNECTED;
}
PubSubClient::PubSubClient(char* domain, uint16_t port, void (*callback)(char*,uint8_t*,unsigned int), XBeeTCPBridge* tcpbridge) {
    this->_tcpbridge = tcpbridge;
    this->callback = callback;
    this->domain = domain;
    this->port = port;
    this->status = DISCONNECTED;


}

*/
boolean PubSubClient::connect(char *id) {
   return connect(id,NULL,NULL,NULL,0,0,NULL);
}

boolean PubSubClient::connect(char *id, char *user, char *pass) {
   return connect(id,user,pass,NULL,0,0,NULL);
}

boolean PubSubClient::connect(char *id, char* willTopic, uint8_t willQos, uint8_t willRetain, char* willMessage)
{
   return connect(id,NULL,NULL,willTopic,willQos,willRetain,willMessage);
}

boolean PubSubClient::connect(char *id, char *user, char *pass, char* willTopic, uint8_t willQos, uint8_t willRetain, char* willMessage) {

    if (id == NULL)
        return false;

    this->id = id;
    this->pass = pass;
    this->user = user;
    this->willTopic = willTopic;
    this->willMessage = willMessage;
    this->willQos = willQos;
    this->willRetain = willRetain;

    doConnectPhase1();
    return true;
}

void PubSubClient::doConnectPhase1(){

    //TODO: be careful about this validations, could resulnt in endless loop, rethink!
    // Already connected
    //if( _tcpbridge->getStatus() ==XBeeTCPBridge::CONNECTED)
    //    return;

    //TODO: be careful about this validations, could resulnt in endless loop, rethink!
    // We need to wait to finish the process
    //if (_tcpbridge->getStatus() != XBeeTCPBridge::DISCONNECTED)
     //   return;

    this->_tcpbridge->connect();

    // Phase 1 completed
    this->status = CONNECTING_PHASE1;
} 

      
void PubSubClient::doConnectPhase2(){

    bool result = false;

    if (status != CONNECTING_PHASE1)
        return;

    //TODO think this
    // en lugar de esto solo necesito el chequeo de abajo pero ademas un timeout
    //onda, si no llega a estado CONNECTED en x tiempo, mandarlo a DISCONNECTED
    if (this->_tcpbridge->getStatus() == XBeeTCPBridge::DISCONNECTED){
        this->status = DISCONNECTED;
        return;
    }


    // If the bridge is not connected we can't continue
    if (this->_tcpbridge->getStatus() != XBeeTCPBridge::CONNECTED)
        return;
    

    nextMsgId = 1;
    uint8_t d[9] = {0x00,0x06,'M','Q','I','s','d','p',MQTTPROTOCOLVERSION};
    // Leave room in the buffer for header and variable length field
    uint16_t length = 5;
    unsigned int j;
    for (j = 0;j<9;j++) {
        buffer[length++] = d[j];
    }

    uint8_t v;
    if (this->willTopic) {
        v = 0x06|(this->willQos<<3)|(this->willRetain<<5);
    } else {
        v = 0x02;
    }

    if(this->user != NULL) {
        v = v|0x80;

        if(this->pass != NULL) {
            v = v|(0x80>>1);
        }
    }

    buffer[length++] = v;

    buffer[length++] = ((MQTT_KEEPALIVE) >> 8);
    buffer[length++] = ((MQTT_KEEPALIVE) & 0xFF);
    length = writeString(this->id,buffer,length);

    if (this->willTopic) {
        length = writeString(this->willTopic,buffer,length);
        length = writeString(this->willMessage,buffer,length);
    }

    if(user != NULL) {
        length = writeString(this->user,buffer,length);
        if(pass != NULL) {
            length = writeString(this->pass,buffer,length);
        }
     }
 
    result = write(MQTTCONNECT,buffer,length-5);
         

    // move to the next step only if we were able to send
    if (!result)
        return;


    lastInActivity = lastOutActivity = millis();

    // Phase 2 completed
    this->status = CONNECTING_PHASE2;

    
}        

void PubSubClient::doConnectPhase3(){


    if (status != CONNECTING_PHASE2)
        return;

    // Check for timeout
    unsigned long t = millis();
    if (t-lastInActivity > MQTT_KEEPALIVE*1000UL) {
        //_tcpbridge->stop(); 
        status = DISCONNECTED;
        return;
    }

    // We should wait until there is data on the bridge
    if (!_tcpbridge->available())
        return;

    // Read a packet
    uint16_t len = readPacket(); 

    if (len == 4 && buffer[3] == 0) {
        lastInActivity = millis();
        pingOutstanding = false;
        status = CONNECTED;
        return;
     }

     status = DISCONNECTED;
     //_tcpbridge->stop();

}


uint16_t PubSubClient::readPacket() {
    uint16_t len = 0;
    uint16_t pos = 0;
    uint8_t multiplier = 1;
    uint16_t length = 0;
    uint8_t digit = 0;
    uint8_t* tmpbuf;

    len = this->_tcpbridge->read(&tmpbuf);

    if (len == 0 )
        return 0;

    buffer[pos] = tmpbuf[pos];
    pos ++;

    do {
      digit = tmpbuf[pos];
      buffer[pos++] = digit;
      length += (digit & 127) * multiplier;
      multiplier *= 128;
   } while ((digit & 128) != 0);

   //TODO add a check of len vs length to avoid buff. ov

   for (uint16_t i = 0;i<length;i++)
   {
      if (pos < MQTT_MAX_PACKET_SIZE){
         buffer[pos] = tmpbuf[pos];
         pos++;
      } else {
         pos = 0; // This will cause the packet to be ignored.
      }
   }

   return pos;
}


PubSubClient::Status PubSubClient::getStatus(){
    return this->status;
}

boolean PubSubClient::loop() {



    switch(status){
        case DISCONNECTED:
            break;
        case CONNECTING_PHASE1:
            doConnectPhase2();
            break;
        case CONNECTING_PHASE2:
            doConnectPhase3();
            break;
        case CONNECTED:

            doConnectedStuff();

            break;
    };


    return true;




}

void PubSubClient::doConnectedStuff(){

    // just in case
    if (status != CONNECTED)
        return;


    // Verify that we are still connected
    if (_tcpbridge->getStatus() != XBeeTCPBridge::CONNECTED){
        status = DISCONNECTED;
        return;
    }



    unsigned long t = millis();
    if ((t - lastInActivity > MQTT_KEEPALIVE*1000UL) || (t - lastOutActivity > MQTT_KEEPALIVE*1000UL)) {

       if (pingOutstanding) {
          _tcpbridge->disconnect(); 
          return ;
       } else {
          buffer[0] = MQTTPINGREQ;
          buffer[1] = 0;
          _tcpbridge->write(buffer, 2);

          lastOutActivity = t;
          lastInActivity = t;
          //TODO testingthis
          //pingOutstanding = true;
       }
    }


      if (_tcpbridge->available()) {
         uint16_t len = readPacket();
         if (len > 0) {
            lastInActivity = t;
            uint8_t type = buffer[0]&0xF0;
            if (type == MQTTPUBLISH) {
               if (mqttReceiver) {
                  uint16_t tl = (buffer[2]<<8)+buffer[3];
                  char topic[tl+1];
                  for (uint16_t i=0;i<tl;i++) {
                     topic[i] = buffer[4+i];
                  }
                  topic[tl] = 0;
                  // ignore msgID - only support QoS 0 subs
                  uint8_t *payload = buffer+4+tl;
                  mqttReceiver->messageReceived(topic,payload,len-4-tl);
               }
            } else if (type == MQTTPINGREQ) {
               buffer[0] = MQTTPINGRESP;
               buffer[1] = 0;
               _tcpbridge->write(buffer,2);
            } else if (type == MQTTPINGRESP) {
               pingOutstanding = false;
            }
         }
      }
   return;
}


boolean PubSubClient::publish(char* topic, char* payload) {
   return publish(topic,(uint8_t*)payload,strlen(payload),false);
}

boolean PubSubClient::publish(char* topic, uint8_t* payload, unsigned int plength) {
   return publish(topic, payload, plength, false);
}

boolean PubSubClient::publish(char* topic, uint8_t* payload, unsigned int plength, boolean retained) {
   if (connected()) {
      // Leave room in the buffer for header and variable length field
      uint16_t length = 5;
      length = writeString(topic,buffer,length);
      uint16_t i;
      for (i=0;i<plength;i++) {
         buffer[length++] = payload[i];
      }
      uint8_t header = MQTTPUBLISH;
      if (retained) {
         header |= 1;
      }
      return write(header,buffer,length-5);
   }
   return false;
}

boolean PubSubClient::publish_P(char* topic, uint8_t* PROGMEM payload, unsigned int plength, boolean retained) {
   uint8_t llen = 0;
   uint8_t digit;
   int rc;
   uint16_t tlen;
   int pos = 0;
   int i;
   uint8_t header;
   unsigned int len;
   
   if (!connected()) {
      return false;
   }
   
   tlen = strlen(topic);
   
   header = MQTTPUBLISH;
   if (retained) {
      header |= 1;
   }
   buffer[pos++] = header;
   len = plength + 2 + tlen;
   do {
      digit = len % 128;
      len = len / 128;
      if (len > 0) {
         digit |= 0x80;
      }
      buffer[pos++] = digit;
      llen++;
   } while(len>0);
   
   pos = writeString(topic,buffer,pos);
   
   rc += _tcpbridge->write(buffer,pos);
  

// TODO TODO TODO   TODO TODO TODO  TODO TODO TODO  TODO TODO TODO 
//   for (i=0;i<plength;i++) {
//      rc += _tcpbridge->write((char)pgm_read_byte_near(payload + i));
//   }
   
   lastOutActivity = millis();
   return rc == len + 1 + plength;
}


boolean PubSubClient::write(uint8_t header, uint8_t* buf, uint16_t length) {
   uint8_t lenBuf[4];
   uint8_t llen = 0;
   uint8_t digit;
   uint8_t pos = 0;
   //uint8_t rc;
   uint8_t len = length;
   bool result;

   do {
      digit = len % 128;
      len = len / 128;
      if (len > 0) {
         digit |= 0x80;
      }
      lenBuf[pos++] = digit;
      llen++;
   } while(len>0);

   buf[4-llen] = header;
   for (int i=0;i<llen;i++) {
      buf[5-llen+i] = lenBuf[i];
   }


   result = _tcpbridge->write(buf+(4-llen), length+1+llen);
   
   lastOutActivity = millis();

   return result;

  // return (rc == 1+llen+length); //TODO
}

boolean PubSubClient::subscribe(char* topic) {
   if (connected()) {
      // Leave room in the buffer for header and variable length field
      uint16_t length = 7;
      nextMsgId++;
      if (nextMsgId == 0) {
         nextMsgId = 1;
      }
      buffer[0] = (nextMsgId >> 8);
      buffer[1] = (nextMsgId & 0xFF);
      length = writeString(topic, buffer,length);
      buffer[length++] = 0; // Only do QoS 0 subs
      return write(MQTTSUBSCRIBE|MQTTQOS1,buffer,length-5);
   }
   return false;
}

void PubSubClient::disconnect() {
   buffer[0] = MQTTDISCONNECT;
   buffer[1] = 0;
   _tcpbridge->write(buffer, 2);
   _tcpbridge->disconnect();
   lastInActivity = lastOutActivity = millis();
}

uint16_t PubSubClient::writeString(char* string, uint8_t* buf, uint16_t pos) {
   char* idp = string;
   uint16_t i = 0;
   pos += 2;
   while (*idp) {
      buf[pos++] = *idp++;
      i++;
   }
   buf[pos-i-2] = (i >> 8);
   buf[pos-i-1] = (i & 0xFF);
   return pos;
}


bool PubSubClient::connected() {
    return (getStatus() == PubSubClient::CONNECTED);
}
