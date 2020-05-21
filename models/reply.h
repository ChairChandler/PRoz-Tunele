#ifndef REPLY_H
#define REPLY_H
#include <string>

enum struct Reply
{
    Enter,
    Exit,
    Deny
};

inline std::string describe(Reply r)
{
    switch(r) {
        case Reply::Enter:
        return "REPLY_ENTER";
        break;

        case Reply::Exit:
        return "REPLY_EXIT";
        break;

        case Reply::Deny:
        return "REPLY_DENY";
        break;
    }
}

#endif // REPLY_H
