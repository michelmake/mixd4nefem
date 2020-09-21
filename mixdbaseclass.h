#ifndef MIXDBASECLASS_H
#define MIXDBASECLASS_H

#include "swapbytes.h"

#include <iostream>
#include <fstream>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdint.h>

template <typename T>
class MIXDBaseClass
{
public:
    static int readData(std::string filename, uint32_t rows,
                        uint32_t cols, T* out)
    {
        struct stat filestat;
        if (stat(filename.c_str(), &filestat)<0) {
            return -1;
        }

        std::ifstream inputStream(filename.c_str(), std::ios::in|std::ios::binary);

        for (size_t i = 0; i < rows; ++i) {
            char data[sizeof(T)*cols];
            if (!inputStream.read(data, sizeof(T)*cols)) {
                return -1;
            }
            for (int j = 0; j < cols; ++j) {
                swapBytes<T> (&data[sizeof(T)*j], (char*) &out[cols*i+j]);
            }
        }
        return 0;
    }

    static int writeData(std::string filename, uint32_t rows,
                         uint32_t cols, T const* in)
    {
        std::ofstream outputStream(filename.c_str(), std::ios::out|std::ios::binary);

        for (size_t i = 0; i < rows; ++i) {
            char data[sizeof(T)*cols];
            for (int j = 0; j < cols; ++j) {
                swapBytes<T> ((char*) &in[cols*i+j], &data[sizeof(T)*j]);
            }
            if (!outputStream.write(data, sizeof(T)*cols)) {
                return -1;
            }
        }
        return 0;
    }

    static int writeDataSpacetime(std::string filename, uint32_t rows,
                                  uint32_t cols, T const* in, T offset = 0)
    {
        std::ofstream outputStream(filename.c_str(), std::ios::out|std::ios::binary);

        for (size_t i = 0; i < rows; ++i) {
            char data[sizeof(T)*cols];
            for (int j = 0; j < cols; ++j) {
                swapBytes<T> ((char*) &in[cols*i+j], &data[sizeof(T)*j]);
            }
            if (!outputStream.write(data, sizeof(T)*cols)) {
                return -1;
            }
        }
        for (size_t i = 0; i < rows; ++i) {
            char data[sizeof(T)*cols];
            for (int j = 0; j < cols; ++j) {
                T dataIn = in[cols*i+j] + offset;
                swapBytes<T> ((char*) &dataIn, &data[sizeof(T)*j]);
            }
            if (!outputStream.write(data, sizeof(T)*cols)) {
                return -1;
            }
        }
        return 0;
    }
};

#endif // MIXDBASECLASS_H
