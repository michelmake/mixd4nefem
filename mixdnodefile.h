#ifndef MIXDNODEFILE_H
#define MIXDNODEFILE_H

#include "mixdbaseclass.h"

#include <string>
#include <vector>

#include <stddef.h>

template<class T>
class MIXDNodeFile
{
public:
    int read(std::string filename)
    {
        struct stat filestat;
        if (stat(filename.c_str(), &filestat)<0) {
           return -1;
        }
        if (filestat.st_size % (8*T::nsd)) {
            return -1;
        }

        m_nNodes = filestat.st_size / (8*T::nsd);
        m_nodes.resize(m_nNodes);

        MIXDBaseClass<double>::readData(filename, m_nNodes, T::nsd, (double*) &m_nodes[0]);
        return 0;
    }

    int write(std::string filename)
    {
        MIXDBaseClass<double>::writeData(filename, m_nNodes, T::nsd, (double*) &m_nodes[0]);
        return 0;
    }
    int writeSpacetime(std::string filename)
    {
        MIXDBaseClass<double>::writeDataSpacetime(filename, m_nNodes, T::nsd, (double*) &m_nodes[0]);
        return 0;
    }

    struct Node
    {
        double x[T::nsd];
    };

    size_t m_nNodes;
    std::vector<Node> m_nodes;
    typedef typename std::vector<Node>::iterator NodeIterator;
    typedef typename std::vector<Node>::const_iterator NodeConstIterator;
};

#endif // MIXDNODEFILE_H
