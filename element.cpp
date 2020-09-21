#include "element.h"

constexpr const unsigned int Tri::facemap[Tri::nef][Tri::nnf];
constexpr const unsigned int Tri::splitNumbering[Tri::nse][Tri::nen];
constexpr const unsigned int TriP2::adj[TriP2::nen][TriP2::nan];
constexpr const unsigned int TriP2::adjP1[TriP2::nen][TriP2::nan];
constexpr const unsigned int TriP2::facemap[TriP2::nef][TriP2::nnf];
constexpr const unsigned int Tet::facemap[Tet::nef][Tet::nnf];
constexpr const unsigned int Tet::splitNumbering[Tet::nse][Tet::nen];
constexpr const unsigned int TetP2::facemap[TetP2::nef][TetP2::nnf];
constexpr const unsigned int TetP2::adj[TetP2::nen][TetP2::nan];
constexpr const unsigned int TetP2::adjP1[TetP2::nen][TetP2::nan];
