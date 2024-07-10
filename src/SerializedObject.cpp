#include <SerializedObject.hpp>
#include <GenericTools.hpp>

SerializedObject::SObject SerializedObject::encodeObject() {
    return {m_objectID._id};
}
int SerializedObject::decodeObject(SObject &s) {
    m_objectID._id = s[0];

    return 1;
}

SerializedObject::SerializedObject() : m_objectID(0xFF) {

}

int SerializedObject::getSizeForObject(unsigned char objectID) {
    switch (objectID) {
        case 0: {
            return 10;
        }
        case 1: {
            return 9;
        }
        default: {
            return 0;
        }
    }
}