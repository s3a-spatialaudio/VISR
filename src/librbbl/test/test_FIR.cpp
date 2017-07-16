//
//  test_FIR.cpp
//
//  Created by Dylan Menzies on 9/3/2015.
//  Copyright (c) 2014 ISVR, University of Southampton. All rights reserved.
//

// avoid annoying warning about unsafe STL functions.
#ifdef _MSC_VER 
#pragma warning(disable: 4996)
#endif

#include <librbbl/fir.hpp>

#include <libefl/basic_matrix.hpp>
#include <libefl/vector_functions.hpp>
#include <libril/constants.hpp>

#include <boost/test/unit_test.hpp>

#include <boost/filesystem.hpp>

#include <algorithm>
#include <iostream>
#include <cstdio>
#include <memory>
#include <stdexcept>
#include <string>

namespace visr
{
namespace rbbl
{
namespace test
{
        
            
            BOOST_AUTO_TEST_CASE( TestFIR )
            {
                std::cout << "Size of FIR object: " << sizeof(FIR) << " bytes." << std::endl;
                // Avoid stack overflow problems by allocating the object on the heap.
                std::unique_ptr<FIR> filterPtr( new FIR() );
                // provide a reference to ease the use of the filter object.
                FIR & filter = *filterPtr;

                FIR::Afloat in[FIR::nBlockSamples];

                int const numOutputs = 32;

                efl::BasicMatrix<FIR::Afloat> outMatrix( numOutputs, FIR::nBlockSamples );
                // Create a pointer vector to the output signals
                std::vector<FIR::Afloat*> outPtrs( numOutputs );
                int idx( 0 );
                std::generate( outPtrs.begin(), outPtrs.end(), [&] { return outMatrix.row(idx++); } );
                int i;
                
                boost::filesystem::path const baseDir(CMAKE_CURRENT_SOURCE_DIR);
                
                boost::filesystem::path const bfile = baseDir / boost::filesystem::path("fir/quasiAllpassFIR_f32_n63.txt");
//                boost::filesystem::path const bfile = baseDir / boost::filesystem::path("FIR/test_f2_n3.txt");
                
                FILE *file = fopen(bfile.string().c_str(),"r");
                BOOST_CHECK( file != nullptr );

                const std::size_t filterLength = 63;
                
                filter.setNumFIRs(numOutputs); //32 //2 test
                filter.setNumFIRsamples(filterLength); //63 //3 test
                filter.setUpsampleRatio(1); //2 // lengthens impulse without increase in time cost.

// Define whether to load the filter set from a file or from a matrix (which is currently created from a compiled-in text file).
#if 0
                BOOST_CHECK( filter.loadFIRs( file ) == 0 );
#else
                // create a filter matrix containing all filters
                // from a initializer list that is compiled into the program.
                efl::BasicMatrix<FIR::Afloat> allFirCoeffs( 64, filterLength,
#include "fir/quasiAllpassFIR_f64_n63_initializer_list.txt"
                 , cVectorAlignmentSamples );

                // Create a second filter matrix that matches the number of required filters.
                efl::BasicMatrix<FIR::Afloat> firCoeffs( numOutputs, filterLength, cVectorAlignmentSamples );
                for( std::size_t idx( 0 ); idx < firCoeffs.numberOfRows(); ++idx )
                {
                  efl::vectorCopy( allFirCoeffs.row( idx ), firCoeffs.row( idx ), filterLength, cVectorAlignmentSamples );
                }

                filter.loadFIRs( firCoeffs );
#endif
                // filter.createWhiteTristateFIRs(0.1);  // 1.0
                
                for(i = 0; i < FIR::nBlockSamples; i++) {
                    in[i] = 0;
                }
                
                in[0] = 1;  // delta input
                //in[1] = 1;

                // int j = 0; // not used at the moment
                // Do not run indefinitely, as this is a unit test.
                for( int loopIdx(0); loopIdx < 1000; ++loopIdx ) {
//                    for(i = 0; i < FIR::nBlockSamples; i++) { // ramp input
//                        in[i] = j++;
//                    }
                    filter.process( in, &outPtrs[0] );
                    in[0] = 0; in[1] = 0;  // delta input contd
                }

                
                fclose( file );
                file = 0;
            }
            
} // namespace test
} // namespace rbbl
} // namespace visr
