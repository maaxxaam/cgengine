#include "object.h"

Object::Object(Level* level) {
    Object res = level->addObject();
    _level = level;
    _id = res.getID();
}