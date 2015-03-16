//
//  test_FIR.cpp

//
//  Created by Dylan Menzies on 9/3/2015.
//  Copyright (c) 2014 ISVR, University of Southampton. All rights reserved.
//


#include <librcl/biquad_iir_filter.hpp>

#include <libril/audio_signal_flow.hpp>
#include <libpml/biquad_parameter.hpp>

#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>


#include <boost/filesystem.hpp>

#include <iostream>
#include <cstdio>

#include <librcl/FIR.hpp>



#include <stdexcept>
#include <string>

namespace visr
{
    namespace rcl
    {
        namespace test
        {
        
            
            BOOST_AUTO_TEST_CASE( TestFIR )
            {
                FIR filter;
                
                Afloat in[FIR::nBlockSamples], out[FIR::maxnFIRs][FIR::nBlockSamples];
                
                int i,j;
                
                boost::filesystem::path const decoderDir(CMAKE_CURRENT_SOURCE_DIR);
                
                boost::filesystem::path bfile = decoderDir / boost::filesystem::path("FIR/quasiAllpassFIR_f32_n63.txt");
//                boost::filesystem::path bfile = decoderDir / boost::filesystem::path("FIR/test_f2_n3.txt");
                
                FILE *file = fopen(bfile.string().c_str(),"r");
            
                
                filter.setNumFIRs(32); //32 //2 test
                filter.setNumFIRsamples(63); //63 //3 test
                filter.setUpsampleRatio(2); //2 // effectively lengthens filter without increase in time cost.
                
                filter.loadFIRs(file);
                
                for(i = 0; i < FIR::nBlockSamples; i++) {
                    in[i] = 0;
                }
                
                in[0] = 1;  // delta input
                in[1] = 1;

                j = 0;
                while(1) {
//                    for(i = 0; i < FIR::nBlockSamples; i++) {
//                        in[i] = j++;
//                    }
                    filter.process(&in, &out);
                    in[0] = 0;
                }

                
                fclose( file );
                file = 0;
            }
            
        } // namespace test
    } // namespace rcl
} // namespce visr







