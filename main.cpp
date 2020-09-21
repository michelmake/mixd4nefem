#include "mixdnodefile.h"
#include "mixdelementfile.h"
#include "element.h"

#include <iostream>
#include <fstream>
#include <cfloat>
#include <cmath>
#include <time.h>
#include <string.h>

#include <boost/program_options.hpp>

// Templates
template <typename T1>
int readFiles(MIXDNodeFile<T1>& P1nodefile, 
              MIXDElementFile<T1>& P1elementfile,
              std::string mxyz_filename,
              std::string mien_filename, 
              std::string mrng_filename);

template <typename T1>
void renumberNodes(MIXDElementFile<T1>& P1elementfile, 
                   MIXDNodeFile<T1>& P1nodefile,
                   MIXDElementFile<T1>& renumberedementfile, 
                   int32_t nefemrng);


template <typename T1>
void writeFiles(MIXDNodeFile<T1>& P1nodefile, 
                MIXDElementFile<T1>& P1elementfile,
                int32_t nefemrng,
                std::string out_minf_filename, std::string out_minf_space_filename,
                std::string out_mxyz_filename, std::string out_mxyz_space_filename,
                std::string out_mien_filename, std::string out_mrng_filename);


// Main program
int main(int argc, char **argv)
{
    namespace po = boost::program_options;

    std::cout << "mixd4nefem:" << std::endl;
    std::cout << "========================================" << std::endl << std::endl;

    std::string mxyz_filename     = "mxyz.space";
    std::string mien_filename     = "mien";
    std::string mrng_filename     = "mrng";
    std::string prefix            = "nefem_";
    std::string out_minf_filename = "minf";
    std::string out_minf_space_filename = "minf.space";
    std::string out_mxyz_filename = "mxyz";
    std::string out_mxyz_space_filename = "mxyz.space";
    std::string out_mien_filename = "mien";
    std::string out_mrng_filename = "mrng";
    std::int32_t nefemrng;

    // Default element type
    elemtype_t etype = elemtria;
    
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "show help message")
        ("prefix,p", po::value<std::string>(&prefix)->default_value("nefem_"),
                 "prefix for the output files")
        ("mxyz", po::value<std::string>(&mxyz_filename)->default_value("mxyz.space"),
                 "the mxyz.space file (only semidiscrete supported atm)")
        ("mien", po::value<std::string>(&mien_filename)->default_value("mien"),
                 "mien file")
        ("mrng", po::value<std::string>(&mrng_filename)->default_value("mrng"),
                 "mrng file")
        ("ominf", po::value<std::string>(&out_minf_filename)->default_value("minf"),
                 "output minf filename (spacetime)")
        ("ominf_space", po::value<std::string>(&out_minf_space_filename)->default_value(
                 "minf.space"), "output minf filename (semidiscrete)")
        ("omxyz", po::value<std::string>(&out_mxyz_filename)->default_value("mxyz"),
                 "output mxyz filename (spacetime)")
        ("omxyz_space", po::value<std::string>(&out_mxyz_space_filename)->default_value(
                 "mxyz.space"), "output mxyz filename (semidiscrete)")
        ("omien", po::value<std::string>(&out_mien_filename)->default_value("mien"),
                 "output mien filename")
        ("omrng", po::value<std::string>(&out_mrng_filename)->default_value("mrng"),
                 "output mrng filename")
        ("tri", "Use triangular elements (default)")
        ("tet", "Use tetrahedral elements")
        ("nefemrng", po::value<int32_t>(&nefemrng)->default_value(1), "specify which RNG needs renumbering")
    ;

    po::variables_map vm;
    try {
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);
    } catch(...) {
        std::cout << "Argument parsing failed! Please consult the --help output.\n";
        return 1;
    }

    // Print out usage if --help is parsed
    if (vm.count("help")) {
        std::cout << desc << "\n";
        return 1;
    }

    // Set correct element type
    if (vm.count("tri")) {
        etype = elemtria;
    } else if (vm.count("tet")) {
        etype = elemtetr;
    } 

    // Parse which RNG needs renumbering
    if (vm.count("nefemrng")) {
        std::cout << "RNG to renumber: "
        << vm["nefemrng"].as<int>() << ".\n\n";
    } else {
        std::cout << "NEFEM RNG for renumbering not defined.\n";
        std::cout << "Run 'nefemrenumber --help' to see all options.\n" ;
        std::exit(1);
    }
    
    // Prepend prefix to filenames of output
    out_minf_filename = prefix + out_minf_filename;
    out_minf_space_filename = prefix + out_minf_space_filename;
    out_mxyz_filename = prefix + out_mxyz_filename;
    out_mxyz_space_filename = prefix + out_mxyz_space_filename;
    out_mien_filename = prefix + out_mien_filename;
    out_mrng_filename = prefix + out_mrng_filename;

   
   
    // Print file names used as input
    std::cout << "Files used as input:" << std::endl;
    std::cout << "========================================\n";
    std::cout << "mxyz file:              " << mxyz_filename << std::endl;
    std::cout << "mien file:              " << mien_filename << std::endl;
    std::cout << "mrng file:              " << mrng_filename << std::endl;
    std::cout << "========================================\n\n";

    // Print file names generated as output  
    std::cout << "Renumbered Output:" << std::endl;
    std::cout << "========================================\n";
    std::cout << "minf output file:       " << out_minf_filename << std::endl;
    std::cout << "minf_space output file: " << out_minf_space_filename << std::endl;
    std::cout << "mxyz output file:       " << out_mxyz_filename << std::endl;
    std::cout << "mxyz_space output file: " << out_mxyz_space_filename << std::endl;
    std::cout << "mien output file:       " << out_mien_filename << std::endl;
    std::cout << "mrng output file:       " << out_mrng_filename << std::endl;
    std::cout << "========================================\n\n";
    
    
    // Instantiate correct element type from input 
    switch(etype)
    {
    case elemtria:
    {
        MIXDNodeFile<Tri> P1nodefile;
        MIXDElementFile<Tri> P1elementfile;

        if (readFiles(P1nodefile, 
                      P1elementfile, 
                      mxyz_filename, 
                      mien_filename, 
                      mrng_filename) < 0) {
            return -1;
        }

        writeFiles(P1nodefile, 
                   P1elementfile, 
                   nefemrng,
                   out_minf_filename,
                   out_minf_space_filename, 
                   out_mxyz_filename,
                   out_mxyz_space_filename, 
                   out_mien_filename, 
                   out_mrng_filename);
        break;
    }
    case elemtetr:
    {
        MIXDNodeFile<Tet> P1nodefile;
        MIXDElementFile<Tet> P1elementfile;

        if (readFiles(P1nodefile,
                      P1elementfile,  
                      mxyz_filename, 
                      mien_filename, 
                      mrng_filename) < 0) {
            return -1;
        }

        writeFiles(P1nodefile, 
                   P1elementfile, 
                   nefemrng,
                   out_minf_filename, 
                   out_minf_space_filename, 
                   out_mxyz_filename,
                   out_mxyz_space_filename, 
                   out_mien_filename, 
                   out_mrng_filename);
        break;
    }
    };
    return 0;
}

// Read mesh files routine 
template <typename T1>
int readFiles(MIXDNodeFile<T1>& P1nodefile,
              MIXDElementFile<T1>& P1elementfile,
              std::string mxyz_filename,
              std::string mien_filename, 
              std::string mrng_filename)
{
    if (P1nodefile.read(mxyz_filename) < 0) {
        std::cout << "reading nodefile failed\n";
        return -1;
    }
    
    std::cout << "Reading in input mesh:\n";
    std::cout << "========================================\n";
    std::cout << "Number of nodes found in mesh:    "  << P1nodefile.m_nNodes << std::endl;
    if (P1elementfile.read(mien_filename, P1nodefile) < 0) {
        std::cout << "reading elementfile failed\n";
        return -1;
    }
    std::cout << "Number of elements found in mesh: " << P1elementfile.m_nElements << "\n\n";
    if (P1elementfile.readRng(mrng_filename) < 0) {
        std::cout << "reading rng failed\n";
        return -1;
    }
    return 0;
}

// Write mesh files routine including reordering
template <typename T1>
void writeFiles(MIXDNodeFile<T1>& P1nodefile, 
                MIXDElementFile<T1>& P1elementfile,
                int32_t nefemrng,
                std::string out_minf_filename, std::string out_minf_space_filename,
                std::string out_mxyz_filename, std::string out_mxyz_space_filename,
                std::string out_mien_filename, std::string out_mrng_filename)
{
    // Instantiate output struct
    MIXDElementFile<T1> renumberedelementfile;
    
    std::cout << "Renumbering mesh:\n";
    std::cout << "========================================\n";
    
    // Call renumbering routine
    renumberNodes(P1elementfile, P1nodefile, renumberedelementfile, nefemrng);
    
    std::cout << "No. of nodes after renumbering:    " << P1nodefile.m_nodes.size() << std::endl;
    std::cout << "No. of elements after renumbering: " << renumberedelementfile.m_nElements << std::endl;
    
    // Write output to file
    renumberedelementfile.write(out_mien_filename);
    renumberedelementfile.writeRng(out_mrng_filename);

    P1nodefile.write (out_mxyz_space_filename);
    P1nodefile.writeSpacetime (out_mxyz_filename);
    P1elementfile.write (out_mien_filename);
    P1elementfile.writeRng(out_mrng_filename);
    std::ofstream minfFile(out_minf_filename.c_str(), std::ios::out);
    std::ofstream minfSpaceFile(out_minf_space_filename.c_str(), std::ios::out);
    minfFile << "nn " << P1nodefile.m_nodes.size()*2 << "\n";
    minfSpaceFile << "nn " << P1nodefile.m_nodes.size() << "\n";
    minfFile << "ne " << P1elementfile.m_nElements << "\n";
    minfSpaceFile << "ne " << P1elementfile.m_nElements << "\n";
    minfFile << "nsd " << T1::nsd << "\n";
    minfSpaceFile << "nsd " << T1::nsd << "\n";
    minfFile << "nen " << T1::nen*2 << "\n";
    minfSpaceFile << "nen " << T1::nen << "\n";
    minfFile << "reordered for NEFEM\n";
    minfSpaceFile << "reordered for NEFEM\n";

    // Strip possible leading directories
    size_t found;
    found  = out_mxyz_filename.find_last_of("/\\");
    if (found != std::string::npos) {
        out_mxyz_filename = out_mxyz_filename.substr(found+1);
    }
    found  = out_mxyz_space_filename.find_last_of("/\\");
    if (found != std::string::npos) {
        out_mxyz_space_filename = out_mxyz_space_filename.substr(found+1);
    }
    found  = out_mien_filename.find_last_of("/\\");
    if (found != std::string::npos) {
        out_mien_filename = out_mien_filename.substr(found+1);
    }
    found  = out_mrng_filename.find_last_of("/\\");
    if (found != std::string::npos) {
        out_mrng_filename = out_mrng_filename.substr(found+1);
    }
    minfFile << "mxyz " << out_mxyz_filename << "\n";
    minfSpaceFile << "mxyz " << out_mxyz_space_filename << "\n";
    minfFile << "mien " << out_mien_filename << "\n";
    minfSpaceFile << "mien " << out_mien_filename << "\n";
    minfFile << "mrng " << out_mrng_filename << "\n";
    minfSpaceFile << "mrng " << out_mrng_filename << "\n";
}


template <typename T1>
void renumberNodes(MIXDElementFile<T1>& P1elementfile, 
                   MIXDNodeFile<T1>& P1nodefile,
                   MIXDElementFile<T1>& renumberedelementfile, 
                   int32_t nefemrng)
{
    int nel = P1elementfile.m_nElements;
    int nn = P1nodefile.m_nNodes;
    int nnf = T1::nnf;
    int mrng = 32;
    int32_t nurbsElementTypes[nel]; 
    int32_t nurbsElementNodes[nel][T1::nen]; 
    int32_t nurbsFaceNodes[nel][T1::nen];
    int32_t nurbsNodes[nn]; 

    // Initialize array nurbsFaceNodes
    for (int iec=0; iec < nel; ++iec){ // Loop over all elements
        for (int ien=0; ien < T1::nen; ++ien){ // loop over all nodes of the element 
            
            // Store boundary nodes of current element. 
            nurbsFaceNodes[iec][ien] = 0;
         }
    }
    // In the following part an array is generated of dimensions [nen][nel]. It is containing 
    // non-zero entries for all nodes that have a face on the boundary.
    for (int irng=0; irng < mrng; ++irng){

        if (irng == nefemrng) {  // Loop over RNGs
            
            for (int iec=0; iec < nel; ++iec){ // Loop over all elements
                
                for (int ifl=0; ifl < T1::nef; ++ifl){ // Loop over all element faces
                        
                    if (P1elementfile.m_rng[iec].rng[ifl] == irng ) { // Check if element face is on the boundary
                    
                        for (int inf=0; inf < T1::nnf; ++inf){ // loop over all nodes of the face on the boundary
                        
                            // Store boundary nodes of current element. 
                            nurbsFaceNodes[iec][T1::facemap[ifl][inf]] = 1;
                        }
                    }
                }
            }
        }
    }

    // From nurbsFaceNodes we need to create a list of all nodes. All non-zeros in this entry are
    // the nodes touching the boundary
    // 

    // Initialize array nurbsFaceNodes
    for (int inn=0; inn < nel; ++inn){ // Loop over all elements
        nurbsNodes[inn] = 0;
    }
    
    for (int iec=0; iec < nel; ++iec){ // Loop over all element faces
        for (int ien=0; ien < T1::nen; ++ien){ // loop over all nodes of the elements  
            if (nurbsFaceNodes[iec][ien] == 1 ){ 
                
                // Store boundary nodes of current element. 
                nurbsNodes[P1elementfile.m_elements[iec].node[ien]] = 1;
            
            }
        }
    }

    
    
    // In the following part an array is generated of dimensions [nen][nel]. It is containing 
    // non-zero entries for all nodes that have a element on the boundary.
    
    
    // Initialize arrays
    for (int iec=0; iec < nel; ++iec){ // Loop over all elements
        for (int ien=0; ien < T1::nen; ++ien){ // loop over all nodes of the element 
            
            // Store boundary nodes of current element. 
            nurbsElementNodes[iec][ien] = 0;
         }
         nurbsElementTypes[iec] = 0;
    }
    
    for (int iec=0; iec < nel; ++iec){ // loop over all elements
        
        for (int ien=0; ien < T1::nen; ++ien){ // loop over all element nodes 
            
            // retrieve current node number from connectivity
            int32_t currentnode = P1elementfile.m_elements[iec].node[ien]; 
            
            if (nurbsNodes[ currentnode ] > 0 ) { // check if current node is on the nurbs boundary 
                nurbsElementNodes[iec][ien] = 1;  
                nurbsElementTypes[iec] = nurbsElementTypes[iec] +1;
            }
        
        }
    }

    // Find totals of all types of elements:
    int32_t totalNE = 0;
    int32_t totalFaceNE = 0;
    int32_t totalEdgeNE = 0;
    int32_t totalRegularNE = 0;

    for (int iec=0; iec < nel; ++iec){ // loop over all elements
        
        if (nurbsElementTypes[iec] < 2 ) { // check if current node is on the nurbs boundary 
            totalRegularNE = totalRegularNE + 1;
        } else if (nurbsElementTypes[iec] == 2 ) {
            totalEdgeNE = totalEdgeNE + 1;
        } else if (nurbsElementTypes[iec] == 3 ) {
            totalFaceNE = totalFaceNE + 1;
        } else {
            std::cout << "Unknown element type found!: " << nurbsElementTypes[iec] << std::endl;
            std::exit(1);
        }
        totalNE = totalNE + 1;
        
    }
    

    // Print some info on nurbs elements:
    std::cout << "Number of face elements:    " << totalFaceNE << std::endl;
    std::cout << "Number of edge elements:    " << totalEdgeNE << std::endl;
    std::cout << "Number of regular elements: " << totalRegularNE << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    std::cout << "Total number of elements:   " << totalNE << std::endl << std::endl;
    if (totalFaceNE + totalEdgeNE + totalRegularNE != totalNE) {
        std::cout << "ERROR: Sum of elements does not" << std::endl;
        std::cout << "match total number of elements!" << std::endl;
        std::exit(1);
    }


    int32_t ienReorderTmp[T1::nen];
    int32_t rngReorderTmp[T1::nef];        
   
    int32_t ienReorder[nel][T1::nen];
    for ( int i = 0; i < nel; ++i){
        for (int l = 0; l < T1::nen; ++l) {
            ienReorder[i][l] = P1elementfile.m_elements[i].node[l];
        }
    }
    
    int32_t rngReorder[nel][T1::nef];
    for ( int i = 0; i < nel; ++i){
        for (int l = 0; l < T1::nef; ++l) {
            rngReorder[i][l] = P1elementfile.m_rng[i].rng[l];
        }
    }
    
    if (T1::type == elemtria ) {
        for (int iec=0; iec < nel; ++iec) {

            if (nurbsElementTypes[iec] > 0) {
        
        
                //  Store original ordering
                ienReorderTmp[0] = ienReorder[iec][0];
                ienReorderTmp[1] = ienReorder[iec][1];
                ienReorderTmp[2] = ienReorder[iec][2];
                                                        
                rngReorderTmp[0] = rngReorder[iec][0];
                rngReorderTmp[1] = rngReorder[iec][1];
                rngReorderTmp[2] = rngReorder[iec][2];
   
                // In case of x_3 being the interior node.
                // Rotate counter-clockwise
                if (nurbsElementNodes[iec][2] == 0) {
                    rngReorder[iec][0] = rngReorderTmp[1];
                    rngReorder[iec][1] = rngReorderTmp[2];
                    rngReorder[iec][2] = rngReorderTmp[0];

                    ienReorder[iec][0] = ienReorderTmp[1];
                    ienReorder[iec][1] = ienReorderTmp[2];
                    ienReorder[iec][2] = ienReorderTmp[0];
                    
                // In case of x_1 being the interior node.
                // Rotate clockwise
                } else if (nurbsElementNodes[iec][0] == 0) {
                    rngReorder[iec][0] = rngReorderTmp[2];
                    rngReorder[iec][1] = rngReorderTmp[0];
                    rngReorder[iec][2] = rngReorderTmp[1];

                    ienReorder[iec][0] = ienReorderTmp[2];
                    ienReorder[iec][1] = ienReorderTmp[0];
                    ienReorder[iec][2] = ienReorderTmp[1];
                }
            }        
        }

    } else if (T1::type == elemtetr) { 
        
        for (int iec=0; iec < nel; ++iec) {
            if (nurbsElementTypes[iec] > 1) {
        
        
                //  Store original ordering
                ienReorderTmp[0] = ienReorder[iec][0];
                ienReorderTmp[1] = ienReorder[iec][1];
                ienReorderTmp[2] = ienReorder[iec][2];
                ienReorderTmp[3] = ienReorder[iec][3];
                                                        
                rngReorderTmp[0] = rngReorder[iec][0];
                rngReorderTmp[1] = rngReorder[iec][1];
                rngReorderTmp[2] = rngReorder[iec][2];
                rngReorderTmp[3] = rngReorder[iec][3];
                
                if (nurbsElementTypes[iec] == 3) {
                    // nsd-1 dimensional part (face) of element on NURBS boundary.
                    // interior element node must be node with number 2.

                    // In case of x_4 being the interior node.
                    // Rotate counter-clockwise around face 3.
                    if (nurbsElementNodes[iec][3] == 0) {
                        rngReorder[iec][0] = rngReorderTmp[1];
                        rngReorder[iec][1] = rngReorderTmp[3];
                        rngReorder[iec][3] = rngReorderTmp[0];

                        ienReorder[iec][1] = ienReorderTmp[3];
                        ienReorder[iec][2] = ienReorderTmp[1];
                        ienReorder[iec][3] = ienReorderTmp[2];
                        
                    // In case of x_3 being the interior node.
                    // Rotate counter-clockwise around face 1.
                    } else if (nurbsElementNodes[iec][2] == 0) {
                        rngReorder[iec][0] = rngReorderTmp[3];
                        rngReorder[iec][1] = rngReorderTmp[0];
                        rngReorder[iec][3] = rngReorderTmp[1];

                        ienReorder[iec][1] = ienReorderTmp[2];
                        ienReorder[iec][2] = ienReorderTmp[3];
                        ienReorder[iec][3] = ienReorderTmp[1];
                    
                    // In case of x_1 being the interior node.
                    // Rotate counter-clockwise around face 2.
                    } else if (nurbsElementNodes[iec][0] == 0) {
                        rngReorder[iec][0] = rngReorderTmp[3];
                        rngReorder[iec][2] = rngReorderTmp[0];
                        rngReorder[iec][3] = rngReorderTmp[2];

                        ienReorder[iec][0] = ienReorderTmp[3];
                        ienReorder[iec][1] = ienReorderTmp[0];
                        ienReorder[iec][3] = ienReorderTmp[1];
                    
                    // In case of x_2 being the interior node.
                    // Element already in correct orientation.
                    } else if (nurbsElementNodes[iec][1] == 0) {
                        // Our work is done here.
                    }
                // Only nsd-2 dimensional part (edge) of element on NURBS boundary.
                // Interior element nodes must be nodes with number 2 & 3, i.e. 
                // nodes 1 & 4 must lie on NURBS.
                } else if (nurbsElementTypes[iec] == 2) {


                    // In case of x_3 and x_4 being the interior nodes.
                    // Rotate counter-clockwise around face 3 (keeeping node 1 fixed).
                    if (nurbsElementNodes[iec][2] == 0 &&
                        nurbsElementNodes[iec][3] == 0) {
                        rngReorder[iec][0] = rngReorderTmp[1];
                        rngReorder[iec][1] = rngReorderTmp[3];
                        rngReorder[iec][3] = rngReorderTmp[0];

                        ienReorder[iec][1] = ienReorderTmp[3];
                        ienReorder[iec][2] = ienReorderTmp[1];
                        ienReorder[iec][3] = ienReorderTmp[2];
                        
                    // In case of x_1 and x_4 being the interior nodes.
                    // Rotate clockwise around face 2 (keeeping node 3 fixed).
                    } else if (nurbsElementNodes[iec][0] == 0 &&
                               nurbsElementNodes[iec][3] == 0) {
                        rngReorder[iec][0] = rngReorderTmp[2];
                        rngReorder[iec][2] = rngReorderTmp[3];
                        rngReorder[iec][3] = rngReorderTmp[0];

                        ienReorder[iec][0] = ienReorderTmp[1];
                        ienReorder[iec][1] = ienReorderTmp[3];
                        ienReorder[iec][3] = ienReorderTmp[0];
                        
                    // In case of x_1 and x_3 being the interior nodes.
                    // Multi rotation required to exchange all nodes and
                    // faces.
                    } else if (nurbsElementNodes[iec][0] == 0 &&
                               nurbsElementNodes[iec][2] == 0) {
                        rngReorder[iec][0] = rngReorderTmp[2];
                        rngReorder[iec][1] = rngReorderTmp[3];
                        rngReorder[iec][2] = rngReorderTmp[0];
                        rngReorder[iec][3] = rngReorderTmp[1];

                        ienReorder[iec][0] = ienReorderTmp[3];
                        ienReorder[iec][1] = ienReorderTmp[2];
                        ienReorder[iec][2] = ienReorderTmp[1];
                        ienReorder[iec][3] = ienReorderTmp[0];
                    
                    // In case of x_1 and x_2 being the interior nodes.
                    // Element already in correct orientation.
                    } else if (nurbsElementNodes[iec][1] == 0 &&
                               nurbsElementNodes[iec][3] == 0) {
                        // Our work is done here.
                    
                    // In case of x_1 and x_2 being the interior nodes.
                    // Rotate counter-clockwise around face 2 (keeeping node 3 fixed).
                    } else if (nurbsElementNodes[iec][0] == 0 &&
                               nurbsElementNodes[iec][1] == 0) {
                        rngReorder[iec][0] = rngReorderTmp[3];
                        rngReorder[iec][3] = rngReorderTmp[2];
                        rngReorder[iec][2] = rngReorderTmp[0];

                        ienReorder[iec][0] = ienReorderTmp[3];
                        ienReorder[iec][3] = ienReorderTmp[1];
                        ienReorder[iec][1] = ienReorderTmp[0];
                    
                    // In case of x_2 and x_3 being the interior nodes.
                    // Rotate counter-clockwise around face 3keeeping node 1 fixed).
                    } else if (nurbsElementNodes[iec][1] == 0 &&
                               nurbsElementNodes[iec][2] == 0) {
                        rngReorder[iec][0] = rngReorderTmp[3];
                        rngReorder[iec][3] = rngReorderTmp[1];
                        rngReorder[iec][1] = rngReorderTmp[0];

                        ienReorder[iec][1] = ienReorderTmp[2];
                        ienReorder[iec][2] = ienReorderTmp[3];
                        ienReorder[iec][3] = ienReorderTmp[1];
                    }    
                }
            }               
        
        }
    }
   
    // Define size of renumbered element struct
    renumberedelementfile.m_nElements = 1*nel;
    renumberedelementfile.m_elements.reserve(renumberedelementfile.m_nElements);
    renumberedelementfile.m_rng.reserve(renumberedelementfile.m_nElements);
    
    // Store renumbered elements in element struct
    for ( int i = 0; i < nel; ++i){
        for (int l = 0; l < T1::nen; ++l) {
            renumberedelementfile.m_elements[i].node[l] = ienReorder[i][l];
        }
    }
    
    // Store renumbered rng faces in element struct
    for ( int i = 0; i < nel; ++i){
        for (int l = 0; l < T1::nef; ++l) {
            renumberedelementfile.m_rng[i].rng[l] = rngReorder[i][l];
        }
    }
    
}
