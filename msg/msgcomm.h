#ifndef MSGCOMM_H
#define MSGCOMM_H

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

#endif // MSGCOMM_H
