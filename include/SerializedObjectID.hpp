#pragma once

#pragma pack(push, 1)

struct SerializedObjectID {
public:
    unsigned char _id;

    SerializedObjectID() = delete;
    SerializedObjectID(unsigned char id);
};

#pragma pack(pop)