#ifndef SRC_HELPER_H_
#define SRC_HELPER_H_
#include "Arduino.h"
#include <vector>
#include <iostream>

namespace helper
{

    namespace conversion
    {

        typedef std::vector<uint8_t> ByteBuffer;
        typedef std::string ByteString;

        ByteBuffer unhexlify(const std::string &data);
    }

    namespace json
    {

        enum MqttMessageType
        {
            PARSE_NONE = 0,
            SET_NETWORK_ID = 1,
            SEND = 2,
            LISTEN = 3,
            SLEEP = 4
        };
        MqttMessageType parseJsonType(const char *type);
    }

}
#endif