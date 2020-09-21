#ifndef MIXDELEMENTFILE_H
#define MIXDELEMENTFILE_H

#include "mixdnodefile.h"
#include <stdint.h>

template<class T>
class MIXDElementFile
{
public:
    int read(std::string filename, MIXDNodeFile<T>& nodefile)
    {
        struct stat filestat;
        if (stat(filename.c_str(), &filestat)<0) {
            return -1;
        }
        if (filestat.st_size % (4*T::nen)) {
            return -1;
        }
        m_nElements = filestat.st_size / (4*T::nen);
        m_elements.resize(m_nElements);

        return MIXDBaseClass<uint32_t>::readData(filename, m_nElements, T::nen, (uint32_t*) &m_elements[0]);
    }


    int write(std::string filename)
    {
        MIXDBaseClass<uint32_t>::writeData(filename, m_nElements, T::nen, (uint32_t*) &m_elements[0]);
        return 0;
    }


    int readRng(std::string filename)
    {
        struct stat filestat;
        if (stat(filename.c_str(), &filestat)<0) {
            return -1;
        }
        if (filestat.st_size != (4*T::nef)*m_nElements) {
            return -1;
        }
        m_rng.resize(m_nElements);
        MIXDBaseClass<int32_t>::readData(filename, m_nElements, T::nef, (int32_t*) &m_rng[0]);
        return 0;
    }


    int writeRng(std::string filename)
    {
        MIXDBaseClass<int32_t>::writeData(filename, m_nElements, T::nef, (int32_t*) &m_rng[0]);
        return 0;
    }


    struct Element
    {
        uint32_t node[T::nen];
    };

    struct ElementRng
    {
        int32_t rng[T::nef];
    };
    size_t m_nElements;
    std::vector<Element> m_elements;
    std::vector<ElementRng> m_rng;
    typedef typename std::vector<Element>::iterator ElementIterator;
    typedef typename std::vector<Element>::const_iterator ElementConstIterator;
};

#endif // MIXDELEMENTFILE_H
