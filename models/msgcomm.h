#ifndef MSGCOMM_H
#define MSGCOMM_H
#include "utils/appdebug.h"

namespace MsgComm
{
    enum struct Sender          { Walker, Dispatcher };
    enum struct Receiver        { Walker, Dispatcher };
    enum struct Request         { Enter, Exit, Status };
    enum struct Response        { Accept, InQueue, Deny };
    enum struct MsgSourceTag    { Walker, Dispatcher, RollbackWalker };
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
    return "";
}

inline std::string describe(MsgComm::Receiver r)
{
    using namespace MsgComm;
    switch(r) {
        case Receiver::Walker:        return "RECEIVER_WALKER";
        case Receiver::Dispatcher:    return "RECEIVER_DISPATCHER";
    }
    return "";
}

inline std::string describe(MsgComm::Request r)
{
    using namespace MsgComm;
    switch(r) {
        case Request::Enter:            return "REQUEST_ENTER";
        case Request::Exit:             return "REQUEST_EXIT";
        case Request::Status:           return "REQUEST_STATUS";
    }
    return "";
}

inline std::string describe(MsgComm::Response r)
{
    using namespace MsgComm;
    switch(r) {
        case Response::Accept:  return "RESPONSE_ACCEPT";
        case Response::Deny:    return "RESPONSE_DENY";
        case Response::InQueue: return "RESPONSE_IN_QUEUE";
    }
    return "";
}

inline std::string describe(MsgComm::MsgSourceTag r)
{
    using namespace MsgComm;
    switch(r) {
        case MsgSourceTag::Walker:          return "MSG_SOURCE_TAG_WALKER";
        case MsgSourceTag::Dispatcher:      return "MSG_SOURCE_TAG_DISPATCHER";
        case MsgSourceTag::RollbackWalker:  return "MSG_SOURCE_TAG_ROLLBACK_WALKER";
    }
    return "";
}
#endif

#endif // MSGCOMM_H
