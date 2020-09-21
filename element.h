#ifndef ELEMENT_H
#define ELEMENT_H

enum elemtype_t
{
    elemtria = 3, // XNS semi-discrete numbering used
    elemtetr = 7
};

class TriP2
{
public:
    static const unsigned int nsd = 2;
    static const unsigned int nef = 3;
    static const unsigned int nen = 6;
    static const unsigned int nnf = 3;
    static const unsigned int nan = 2; // number of nodes adjacent to the corner nodes (vertices)
    constexpr static const unsigned int facemap[nef][nnf] = { { 0, 1, 3 },
                                                              { 1, 2, 4 },
                                                              { 2, 0, 5 } };

    // midside nodes adjacent to respective corner ones
    // e.g. node 0 (1st line) has new midside neighbours 3, 5
    constexpr static const unsigned int adj[nen][nan] =     { { 3, 5 },
                                                              { 3, 4 },
                                                              { 5, 4 } };

    constexpr static const unsigned int adjP1[nen][nan] =   { { 1, 2 },
                                                              { 0, 2 },
                                                              { 0, 1 } };
};

class Tri
{
public:
    static const elemtype_t type = elemtria;
    static const unsigned int nsd = 2;
    static const unsigned int nef = 3;
    static const unsigned int nen = 3;
    static const unsigned int nnf = 2;
    static const unsigned int nse = 4; // number of elements created by splitting by edge bisection
    constexpr static const unsigned int facemap[nef][nnf] = { { 0, 1 },
                                                              { 1, 2 },
                                                              { 2, 0 } };

    // lines of this array represent nodes of elements obtained by splitting the original one
    constexpr static const unsigned int splitNumbering[nse][nen] = { { 0, 3, 5 },
                                                                     { 3, 1, 4 },
                                                                     { 5, 4, 2 },
                                                                     { 3, 4, 5 }, };
};

class TetP2
{
public:
    static const unsigned int nsd = 3;
    static const unsigned int nef = 4;
    static const unsigned int nen = 10;
    static const unsigned int nnf = 6;
    static const unsigned int nan = 3; // number of nodes adjacent to the corner nodes (vertices)
    constexpr static const unsigned int facemap[nef][nnf] = { { 2, 0, 1, 6, 4, 5 },
                                                              { 0, 3, 1, 7, 8, 4 },
                                                              { 1, 3, 2, 8, 9, 5 },
                                                              { 2, 3, 0, 9, 7, 6 } };

    // midside nodes adjacent to respective corner ones
    // e.g. node 0 (1st line) has new midside neighbours 4, 6, 7
    constexpr static const unsigned int adj[nen][nan] =     { { 4, 6, 7 },
                                                              { 4, 5, 8 },
                                                              { 6, 5, 9 },
                                                              { 7, 8, 9 } };

    constexpr static const unsigned int adjP1[nen][nan] =   { { 1, 2, 3 },
                                                              { 0, 2, 3 },
                                                              { 0, 1, 3 },
                                                              { 0, 1, 2 } };
};

class Tet
{
public:
    static const elemtype_t type = elemtetr;
    static const unsigned int nsd = 3;
    static const unsigned int nef = 4;
    static const unsigned int nen = 4;
    static const unsigned int nnf = 3;
    static const unsigned int nse = 8; // number of elements created by splitting by edge bisection
    constexpr static const unsigned int facemap[nef][nnf] = { { 2, 0, 1 },
                                                              { 0, 3, 1 },
                                                              { 1, 3, 2 },
                                                              { 2, 3, 0 } };

    // lines of this array represent nodes of elements obtained by splitting the original one
    constexpr static const unsigned int splitNumbering[nse][nen] = { { 0, 4, 6, 7 },
                                                                     { 4, 1, 5, 8 },
                                                                     { 6, 5, 2, 9 },
                                                                     { 7, 8, 9, 3 },
                                                                     { 7, 4, 6, 9 },
                                                                     { 7, 4, 9, 8 },
                                                                     { 4, 8, 5, 9 },
                                                                     { 4, 5, 6, 9 } };
};
#endif // ELEMENT_H
