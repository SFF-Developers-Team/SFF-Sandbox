#pragma once

#include <vector>
#include <SerializedObjectID.hpp>

#pragma pack(push, 1)


struct SerializedObject {
protected:
    SerializedObjectID m_objectID;
public:
    using SObject = std::vector<unsigned char>;

    virtual SObject encodeObject();
    virtual int decodeObject(SObject &s);

    static int getSizeForObject(unsigned char objectID);
    
    template<typename T>
    static std::vector<T *> convertVector(std::vector<SerializedObject *> objs) {
        std::vector<T *> ret = {};

        for (SerializedObject *obj : objs) {
            T *conv = dynamic_cast<T *>(obj);

            if (conv == nullptr) continue;

            ret.push_back(conv);
        }

        return ret;
    }

    SerializedObject();
};

#pragma pack(pop)