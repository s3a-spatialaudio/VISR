//
//  test_FIR.cpp
//
//  Created by Dylan Menzies on 9/3/2015.
//  Copyright (c) 2014 ISVR, University of Southampton. All rights reserved.
//


#include <librbbl/fir.hpp>

#include <libefl/basic_matrix.hpp>

#define BOOST_TEST_DYN_LINK
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
                int i,j;
                
                boost::filesystem::path const decoderDir(CMAKE_CURRENT_SOURCE_DIR);
                
                boost::filesystem::path bfile = decoderDir / boost::filesystem::path("fir/quasiAllpassFIR_f32_n63.txt");
//                boost::filesystem::path bfile = decoderDir / boost::filesystem::path("FIR/test_f2_n3.txt");
                
                FILE *file = fopen(bfile.string().c_str(),"r");
                BOOST_CHECK( file != nullptr );
            
                
                filter.setNumFIRs(numOutputs); //32 //2 test
                filter.setNumFIRsamples(63); //63 //3 test
                filter.setUpsampleRatio(1); //2 // lengthens impulse without increase in time cost.
                
                BOOST_CHECK( filter.loadFIRs( file ) == 0 );
                
                // filter.createWhiteTristateFIRs(0.1);  // 1.0
                
                for(i = 0; i < FIR::nBlockSamples; i++) {
                    in[i] = 0;
                }
                
                in[0] = 1;  // delta input
                //in[1] = 1;

                j = 0;
                while(1) {
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
