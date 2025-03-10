#pragma once
#include <DataStream.hpp>

class Serializable {
protected:
    ObjectHeader m_header;

public:
    Serializable(ObjectHeader header) : m_header(header) {}

    virtual DataStream serialize() {
        auto ret = DataStream();
        ret.add(m_header);

        return ret;
    }

    virtual bool deserialize(DataStream& stream) {
        if(stream.size() > 0) {
            m_header = stream.get<ObjectHeader>(NULL_PACKET);
            
            return true;
        }

        return false;
    }
};