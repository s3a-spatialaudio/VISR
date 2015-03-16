/* Copyright Institute of Sound and Vibration Research - All rights reserved */


/**
* @file librcl/test/test_main.cpp
* File to place global statements for the unit test suite.
*/


#define BOOST_TEST_MODULE "RuntimeComponentLibrary"

#ifdef BOOST_ALL_DYN_LINK
#define BOOST_TEST_DYN_LINK
#else
#define BOOST_TEST_MAIN
#endif 
#include <boost/test/unit_test.hpp>


// Nothing more to do here for this linkage model.
