#ifndef PLACE_H
#define PLACE_H
#include "utils/appdebug.h"

enum struct Place
{
    Earth,
    Dimension
};

#include "string"
inline std::string describe(Place p)
{
    switch(p) {
        case Place::Earth:
        return "PLACE_EARTH";
        break;

        case Place::Dimension:
        return "PLACE_DIMENSION";
        break;
    }
}

#endif // PLACE_H
