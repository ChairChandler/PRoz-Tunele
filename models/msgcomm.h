#ifndef MSGCOMM_H
#define MSGCOMM_H
#include "utils/appdebug.h"

namespace MsgComm
{
    enum struct Sender          { Walker, Dispatcher };
    enum struct Receiver        { Unknown, Walker, Dispatcher };
    enum struct Request         { Enter, Exit };
    enum struct Response        { Enter, Exit, Deny };
    enum struct MsgSourceTag    { Unknown, Walker, Dispatcher };
}

#ifdef APP_DEBUG_COMMUNICATION
#include <string>
inline std::string describe(MsgComm::Sender r)
{
    using namespace MsgComm;
    switch(r) {
        case Sender::Walker:        return "SENDER_WALKER";
        case Sender::Dispatcher:    return "SENDER_DISPATCHER";
    }
}

inline std::string describe(MsgComm::Receiver r)
{
    using namespace MsgComm;
    switch(r) {
        case Receiver::Walker:        return "RECEIVER_WALKER";
        case Receiver::Dispatcher:    return "RECEIVER_DISPATCHER";
    }
}

inline std::string describe(MsgComm::Request r)
{
    using namespace MsgComm;
    switch(r) {
        case Request::Enter:    return "REQUEST_ENTER";
        case Request::Exit:     return "REQUEST_EXIT";
    }
}

inline std::string describe(MsgComm::Response r)
{
    using namespace MsgComm;
    switch(r) {
        case Response::Enter:   return "RESPONSE_ENTER";
        case Response::Exit:    return "RESPONSE_EXIT";
        case Response::Deny:    return "RESPONSE_DENY";
    }
}

inline std::string describe(MsgComm::MsgSourceTag r)
{
    using namespace MsgComm;
    switch(r) {
        case MsgSourceTag::Unknown:     return "MSG_SOURCE_TAG_UNKNOWN";
        case MsgSourceTag::Walker:      return "MSG_SOURCE_TAG_WALKER";
        case MsgSourceTag::Dispatcher:  return "MSG_SOURCE_TAG_DISPATCHER";
    }
}
#endif

#endif // MSGCOMM_H
