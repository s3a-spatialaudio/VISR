//
//  main.cpp
//
//  Created by Dylan Menzies on 10/11/2014.
//  Copyright (c) 2014 ISVR, University of Southampton. All rights reserved.
//

#include <libpanning/VBAP.h>
#include <libpanning/AllRAD.h>
#include <boost/filesystem.hpp>

#include <boost/test/unit_test.hpp>

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <vector>


#if 0
{
  vbap.setListenerPosition( 0.0f, 0.0f, 0.0f );
  //    vbap.setListenerPosition(-1.9f, 0.0f, -0.1f);
  //    vbap.setListenerPosition(1.5f, 0.0f, 0.4f);
  //    vbap.setListenerPosition(-1.9f, 0.0f, +0.05f);
  //    vbap.setListenerPosition(0.0f, -0.6f, -0.2f);
  //    vbap.setListenerPosition(0.0f, -0.6f, -0.2f);  // hit triplet 39 with pw(-1,0,0)
  //    vbap.setListenerPosition(2.0f, 0.0f, 0.0f);  // hit triplet 35 with pw(0,0,-1)
  vbap.calcInvMatrices();

  sourcePos[0].set( 2.08f, 1.0f, -5.0f, true );
  //    sourcePos[0].set(-1.0f,	0.0, 0.0f, true);     // plane wave from front/back
  //    sourcePos[0].set(0.0f, -1.0f, 0.0f, true);     // plane wave from left/right
  //    sourcePos[0].set(0.0f, 0.0f, -1.0f, true);     // plane wave from below
  //    sourcePos[0].set(1.78f, 1.73f, -0.86f, false);

  //    vbap.setNumSources( numberOfSources );
  //    sourcePos[0].set(1.0, 1.0, 1.0, false);
  //    sourcePos[0].set(1.0,	0.3, -0.2, false); // for 9.1_audiolab.txt jumps between triplet 11 and 10 as z reduced
  //    sourcePos[0].set(0.0, 1.0, -0.9, false);
  sourcePos[1].set( 1.0f, 0.0f, 0.0f, false );
  sourcePos[2].set( 0.0f, 1.0f, 0.0f, false );
  sourcePos[3].set( 0.0f, 0.0f, 1.0f, false );
  sourcePos[4].set( -1.0f, 0.0f, 0.0f, false );
  sourcePos[5].set( 0.0f, -1.0f, 0.0f, false );
  sourcePos[6].set( 0.0f, 0.0f, -1.0f, false );
  sourcePos[7].set( -1.0f, -1.0f, -1.0f, false );

  vbap.setNumSources( 1 ); // This means that the output is computed only for the zeroeth source in the vector.
  vbap.setSourcePositions( &sourcePos[0] );

  vbap.calcGains();

  efl::BasicMatrix<Afloat> const & vbapGains = vbap.getGains();   // Check in watch window
  // Print the matrix.
  std::size_t const numCols = vbapGains.numberOfColumns();
  for( std::size_t rowIdx(0); rowIdx < vbapGains.numberOfRows(); ++rowIdx )
  {
    Afloat const * rowPtr = vbapGains.row( rowIdx );
    std::cout << "VBAP gain for source #" << rowIdx << ": ";
    std::copy( rowPtr, rowPtr+numCols, std::ostream_iterator<Afloat>( std::cout, ", " ) );
    std::cout << std::endl;
  }  

  // 5.1 test, 2D VBAP

  // file = fopen("arrays/5.1.txt","r");
  bfile = configDir / boost::filesystem::path( "isvr/stereo_audiolab.txt" );
  //bfile = configDir / boost::filesystem::path("isvr/5.1_audiolab.txt");
  file = fopen( bfile.string().c_str(), "r" );
  if( array.load( file ) == -1 ) return -1;
  fclose( file );
  file = 0;

  vbap.setLoudspeakerArray( &array );
  vbap.setListenerPosition( 0.0, 0.0, 0.0 );
  vbap.calcInvMatrices();

  vbap.setNumSources( 4 );
  sourcePos[0].set( 1.0, 0.0, 0.0, false );
  //    sourcePos[0].set(1.0, 0.0, 0.0, true);  // plane wave
  sourcePos[1].set( 0.0, 1.0, 0.0, false );
  sourcePos[2].set( -1.0, 0.0, 0.0, false );
  sourcePos[3].set( 0.0, -1.0, 0.0, false );
  //    sourcePos[0].set(1.0, 0.0, -5.0, false);
  //    sourcePos[1].set(0.0, 1.0, -5.0, false);
  //    sourcePos[2].set(-1.0, 0.0, -5.0, false);
  //    sourcePos[3].set(0.0, -10.0, -50.0, false);

  vbap.setSourcePositions( &sourcePos[0] );

  vbap.calcGains();

  // Check the updated values of vbapGains in watch window
}
#endif