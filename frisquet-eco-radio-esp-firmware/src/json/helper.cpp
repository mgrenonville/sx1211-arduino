#include "helper.h"

namespace helper
{

    namespace conversion
    {

        int ascii2val(char c)
        {
            int iRetVal;

            if ((c >= '0') && (c <= '9'))
            {
                iRetVal = (c - '0');
            }
            else if ((c >= 'a') && (c <= 'f'))
            {
                iRetVal = (c - 'a' + 10);
            }
            else if ((c >= 'A') && (c <= 'F'))
            {
                iRetVal = (c - 'A' + 10);
            }
            else
            {
                iRetVal = 0;
            }

            return iRetVal;
        }

        ByteBuffer unhexlify(const std::string &InBuffer)
        {
            ByteBuffer OutBuffer(InBuffer.size() / 2 + 1);

            for (size_t i = 0, j = 0; i < InBuffer.size(); i += 2, ++j)
            {
                uint8_t *dest = &OutBuffer[j];
                *dest++ = (((ascii2val(InBuffer[i]) << 4) | (ascii2val(InBuffer[i + 1]))));
            }

            return OutBuffer;
        }

    }

    namespace json
    {

        MqttMessageType parseJsonType(const char *type)
        {
            if (strcasecmp(type, "SEND") == 0)
            {
                return SEND;
            }

            if (strcasecmp(type, "LISTEN") == 0)
            {
                return LISTEN;
            }
            if (strcasecmp(type, "SET_NETWORK_ID") == 0)
            {
                return SET_NETWORK_ID;
            }
            if (strcasecmp(type, "SLEEP") == 0)
            {
                return SLEEP;
            }
            return PARSE_NONE;
        }
    }

}