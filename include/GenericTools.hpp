#pragma once

namespace GenericTools {
    template<typename T>
    inline void addVectors(std::vector<T> *output, std::vector<T> input) {
        for (T &obj : input) {
            output->push_back(obj);
        }
    }

    template<typename T>
    inline std::vector<unsigned char> valueToVector(T value) {
        unsigned int val_size = sizeof(T);
        unsigned char *bytes = (unsigned char *)(&value);

        std::vector<unsigned char> vec;

        for (unsigned int i = 0; i < val_size; i++) {
            vec.push_back(bytes[i]);
        }

        return vec;
    }
};