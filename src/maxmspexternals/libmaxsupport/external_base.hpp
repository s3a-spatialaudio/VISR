/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_MAXMSP_MAXSUPPORT_EXTERNAL_BASE_HPP_INCLUDED
#define VISR_MAXMSP_MAXSUPPORT_EXTERNAL_BASE_HPP_INCLUDED 

/* Super-safe determination of the MAX define for setting the operating system. */
#ifdef __APPLE_CC__
#ifndef MAC_VERSION 
#define MAC_VERSION
#undef WIN_VERSION
#endif
#else
#ifdef _MSC_VER
#ifndef WIN_VERSION 
#define WIN_VERSION
#endif
#undef MAC_VERSION
#endif
#endif

#include <ext.h>
#include <ext_obex.h>
#include <z_dsp.h>

namespace visr
{
namespace maxmsp
{

/**
 * Abstract base class for Max/MSP externals
 * C++ classes to use the abstractions shall derive from this class.
 */
class ExternalBase
{
public:
  /**
   * Constructor.
   * Initializes the pointer to the Max proxy object associated with the external.
   */
  explicit ExternalBase( t_pxobject * maxProxy );

  /**
   * Destructor.
   * Declared as virtual, although it is unlikely that external objects are used in a polymorhic way (if they
   * are created using the ExternalWrapper/ClassRegistrar infrastructure, they are not used polymorphically).
   */
  virtual ~ExternalBase();

  virtual void initDsp( t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags) = 0;

  virtual void perform( t_object *dsp64, double **ins,
                       long numins, double **outs, long numouts,
                       long sampleframes, long flags, void *userparam) = 0;

  virtual void getFloat( double f ) {}

  virtual void assist( void *b, long msg, long arg, char *dst ) = 0;

protected:
  t_pxobject * getMaxProxy() { return mMaxProxy; }
  t_pxobject const * getMaxProxy() const { return mMaxProxy; }

private:
  t_pxobject * mMaxProxy;
};

} // namespace visr
} // namespace maxmsp

#endif // VISR_MAXMSP_MAXSUPPORT_EXTERNAL_BASE_HPP_INCLUDED 
