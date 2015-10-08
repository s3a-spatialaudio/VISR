//
//  reverb_object_parser.cpp
//  VISR
//
//  Created by Philip Coleman on 23/06/2015.
//
//

/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <libobjectmodel/object.hpp>
#include <libobjectmodel/object_vector.hpp>
#include <libobjectmodel/object_vector_parser.hpp>
#include <libobjectmodel/point_source.hpp>

#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include <stdexcept>
#include <iostream>

namespace visr
{
    namespace objectmodel
    {
        namespace test
        {
            
            BOOST_AUTO_TEST_CASE( ParsePointSource )
            {
                std::string const msg = "{ \"objects\":[{\"id\": 1, \"channels\": \"0\", \n \"type\": \"point\",\n \"group\": 0,\n \"priority\": 3,\n \"level\": 0.8,\n"
                "\"position\": {\"x\": 2.4, \"y\": 1.2,\n \"z\": -0.3 } }, {\"id\": 5,\"channels\": \"1\",\n \"type\": \"point\",\n \"group\": 1,\n \"priority\": 2,\n \"level\": 0.375,\n"
                "\"position\": {\"x\": 1.5, \"y\": -1.5,\n \"z\": 0 } }] }";
                
                ObjectVector scene;
                
                ObjectVectorParser::fillObjectVector( msg, scene );
                
                BOOST_CHECK( scene.size() == 2 );
            }
            
            
        } // namespace test
    } // namespace objectmodel
} // namespce visr
