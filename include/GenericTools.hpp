#pragma once

#include <vector>
#include <string>
#include <cstdio>

namespace GenericTools {
    template<typename T>
    inline void addVectors(std::vector<T> *output, std::vector<T> input) {
        for (T &obj : input) {
            output->push_back(obj);
        }
    }

    template<typename T>
    inline std::vector<unsigned char> valueToVector(T* value) {
        unsigned int val_size = sizeof(T);
        unsigned char *bytes = (unsigned char *)value;

        std::vector<unsigned char> vec;

        for (unsigned int i = 0; i < val_size; i++) {
            vec.push_back(bytes[i]);
        }

        return vec;
    }

    template<typename T>
    inline std::string valueToHex(T &v) {
        char buffer[sizeof(T) * 2 + 1] = {};

        std::string format = "%0" + std::to_string(sizeof(T) * 2) + "X";

        snprintf(buffer, sizeof(T) * 2 + 1, format.c_str(), v);

        std::string b = buffer;
        return b;
    }

    template<typename T>
    inline std::string vectorToString(std::vector<T> &v) {
        std::string res = "";

        for (T &el : v) {
            res += valueToHex<T>(el) + " ";
        }

        if (res.length() >= 1) {
            res.pop_back();
        }

        return res;
    }

    template<typename T>
    inline void deleteVec(std::vector<T *> &vec) {
        for (T *el : vec) {
            delete el;
        }
    }

    template<typename T>
    inline T vectorToValue(std::vector<unsigned char> &vec, unsigned int offset = 0) {
        T v;
        unsigned char *raw_ref = (unsigned char *)(&v);

        for (unsigned int i = 0; i < sizeof(T); i++) {
            raw_ref[i] = vec[i + offset];
        }

        return v;
    }
};