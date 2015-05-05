//
//  LoudspeakerArray.cpp
//
//  Created by Dylan Menzies on 18/11/2014.
//  Copyright (c) ISVR, University of Southampton. All rights reserved.
//

// avoid annoying warning about unsafe STL functions.
#ifdef _MSC_VER 
#pragma warning(disable: 4996)
#endif

#include "LoudspeakerArray.h"

#include <algorithm>

LoudspeakerArray::LoudspeakerArray()
 : m_is2D( false )
 , m_isInfinite( false )
 , m_nSpeakers( 0 )
 , m_nTriplets( 0 )
{
  std::fill( m_channel, m_channel + MAX_NUM_SPEAKERS, -1 );
  std::fill( &m_triplet[0][0], &m_triplet[0][0] + MAX_NUM_LOUDSPEAKER_TRIPLETS*3, -1 );
}

int LoudspeakerArray::load(FILE *file)
{
    //system("pwd");
    int n,i,chan;
    char c;
    Afloat xy, x,y,z;
    Afloat az,el,r;
    int l1,l2,l3;
    int nSpk, nTri;
    
    i = nSpk = nTri = 0;

    std::fill( m_channel, m_channel + MAX_NUM_SPEAKERS, -1 );
    std::fill( &m_triplet[0][0], &m_triplet[0][0] + MAX_NUM_LOUDSPEAKER_TRIPLETS*3, -1 );

    m_is2D = false;
    m_isInfinite = false;
    
    if (file == 0) return -1;

    do {
        c = fgetc(file);

        if (c == 'c') {        // cartesians
            n = fscanf(file, "%d %d %f %f %f\n", &i, &chan, &x, &y, &z);
            if( n != 5 )
            {
              return -1;
            }
            if (i <= MAX_NUM_SPEAKERS) {
                setPosition(i,x,y,z,m_isInfinite);
                setChannel(i,chan);
                if (i > nSpk) nSpk = i;
            }
        }
        else if (c == 'p') {   // polars, using degrees
            n = fscanf(file, "%d %d %f %f %f\n", &i, &chan, &az, &el, &r);
            if( n != 5 )
            {
              return -1;
            }
            if( i <= MAX_NUM_SPEAKERS ) {
                az *= PI/180;
                el *= PI/180;
                xy = r*cos(el);
                x = xy*cos(az);
                y = xy*sin(az);
                z = r*sin(el);
                setPosition(i,x,y,z,m_isInfinite);
                setChannel(i,chan);
                if (i > nSpk) nSpk = i;
            }
        }
        else if (c == 't') {    // tuplet - triplet or duplet
            n = fscanf(file, "%d %d %d %d\n", &i, &l1, &l2, &l3);
            if( n < 3 || n > 4 )
            {
              return -1;
            }
            if( i <= MAX_NUM_LOUDSPEAKER_TRIPLETS ) {
                if (n == 3) l3 = 1;
                setTriplet(i-1, l1-1, l2-1, l3-1);
                if (i > nTri) nTri = i;
            }
        }
        else if (c == '2') {    // switch to '2D' mode
            m_is2D = true;
        }
        else if (c == 'i') {    // switch to 'infinite' mode
            m_isInfinite = true;
        }
        else if (c == '%') {    // comment
            while(fgetc(file) != '\n' && !feof(file) );
        }

    } while (!feof(file));
    
    m_nSpeakers = nSpk;
    m_nTriplets = nTri;
    
    return 0;
}
