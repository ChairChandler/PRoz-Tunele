#ifndef REQUEST_H
#define REQUEST_H
#include <string>

enum struct  Request
{
    Enter,
    Exit
};

inline std::string describe(Request r)
{
    switch(r) {
        case Request::Enter:
        return "REQUEST_ENTER";
        break;

        case Request::Exit:
        return "REQUEST_EXIT";
        break;
    }
}

#endif // REQUEST_H
