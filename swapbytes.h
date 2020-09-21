#ifndef SWAPBYTES_H
#define SWAPBYTES_H

template <typename T>
void swapBytes (const char* in, char* out)
{
    for (int i = 0; i < sizeof(T); ++i) {
        out[i] = in[sizeof(T)-1 - i];
    }
}

#endif