#pragma once

#include <vector>

class SerializedObject {
public:
    using SObject = std::vector<unsigned char>;

    virtual SObject encodeObject() = 0;
    virtual void decodeObject(SObject &s) = 0;
};