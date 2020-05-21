#ifndef MSGCOMM_H
#define MSGCOMM_H
#include <string>
#include "appdebug.h"

enum struct MsgComm
{
    RequestRecvTag,
    ReplyRecvTag
};

enum struct SpecificTarget
{
    Self,
    All
};

#ifdef APP_DEBUG_COMMUNICATION
inline std::string describe(MsgComm r)
{
    switch(r) {
        case MsgComm::RequestRecvTag:
        return "REQUEST_RECV";
        break;

        case MsgComm::ReplyRecvTag:
        return "REPLY_RECV";
        break;
    }
}
#endif

#endif // MSGCOMM_H
