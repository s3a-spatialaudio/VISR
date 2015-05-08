//
//  delay_vector~.c
//  delay_vector~
//
//  Created by Ferdinando Olivieri on 15/03/2015.
//
//
//****************************
// 1. Header files necessary for Max MSP
#include "ext.h"
#include "z_dsp.h"
#include "ext_obex.h"

#include <cstddef>
#include <sstream>

namespace visr
{
namespace maxmsp
{


class DelayVector
{
public:
  /**
   * A plain C struct to be allocated by Max/MSP
   * containing the Max proxy object and a pointer to the actual DelayVector object.
   * @note constructors and destructors for this object are probably not called, because this struct will be allocated in
   * memory by Max.
   */
  struct PlainObject
  {
  public:
    /**
     * MaxMSP proxy
     */
    t_pxobject mMaxProxy;
    /**
     * Plain C pointer to the object itself.
     */
    DelayVector* mObject;
  };

  static t_class *sStaticClassInstance;
  
  explicit DelayVector( PlainObject * plainObj, int argc, t_atom* argv );
  
  ~DelayVector();
  
  /**
   * The interface required by max.
   */
  //@{
  static void * newObject(t_symbol *s, short argc, t_atom *argv);

  static void dsp64( PlainObject *obj, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags)
  {
    // post("DelayVector::dsp64() called.");
    obj->mObject->initDsp( dsp64, count, samplerate, maxvectorsize, flags );
  }


  static void perform64( PlainObject *x, t_object *dsp64, double **ins,
                              long numins, double **outs, long numouts,
                              long sampleframes, long flags, void *userparam);
  
  static void free( PlainObject *x);
  

  static void assist( PlainObject  *x, void *b, long msg, long arg, char *dst);
  
  static void getFloat( PlainObject *x, double f);

private:
  void applyFloat( double f );
  
  void initDsp( t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags);


  void performInternal( t_object *dsp64, double **ins,
                        long numins, double **outs, long numouts,
                       long sampleframes, long flags, void *userparam);

  std::size_t mNumberOfChannels;
  
  double mCurrentGain;
  
  PlainObject* mPlainStruct;
};
  
DelayVector::DelayVector( PlainObject * plainObj, int argc, t_atom* argv )
  : mPlainStruct(plainObj)
{
  post("DelayVector::DelayVector() constructor called.");

  float numChannels = 2.0; // Default number of channels

  atom_arg_getfloat(&numChannels, 0, argc, argv);
  mNumberOfChannels = static_cast<std::size_t>(numChannels);

  // Creating the inlets
  dsp_setup(&plainObj->mMaxProxy, (int)mNumberOfChannels);

  // Creating the outlets
  for( std::size_t chIdx = 0; chIdx < mNumberOfChannels; ++chIdx )
  {
    // Again: Using plainObject->mMaxProxy would require a less nasty cast.
    outlet_new( reinterpret_cast<t_object *>(plainObj), "signal");
  }
  
  float gain = 1.0;
  atom_arg_getfloat(&gain, 1, argc, argv);
  mCurrentGain = gain;
}

DelayVector::~DelayVector()
{
  post("DelayVector::~DelayVector() destructor called.");
  // nothing to be done yet, as all members are cleaned automatically by their respective destructors.
}

  
/*static*/ void * DelayVector::newObject(t_symbol *s, short argc, t_atom *argv)
{
  post("DelayVector: *delay_vector_new called.");
  PlainObject* newPlainObj = static_cast<PlainObject *>( object_alloc(sStaticClassInstance));
  if( !newPlainObj )
  {
    post( "Creation of new object failed." );
    return nullptr;
  }
  newPlainObj->mObject = new DelayVector( newPlainObj, argc, argv );
  return newPlainObj; // returning the pointer to the plain C struct encapsulating the object.
}
  
/*static*/ void DelayVector::free( PlainObject *x)
{
  post("DelayVector::free() called.");
  /* We must call dsp_free() before freeing any dynamic memory
   allocated for the external. This removes the object from the
   Max/MSP DSP chain. */
  // TODO: Why do we not call dsp_free for x->mMaxProxy (which has the same address), which would avoid the nasty C cast?
  dsp_free((t_pxobject *) x);
  delete x->mObject;
}

void DelayVector::initDsp( t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags)
{
  post("DelayVector::initDsp() called.");
#if 1
  //post("Executing the 64-bit perform routine");
  dsp_add64(dsp64, (t_object*)mPlainStruct, reinterpret_cast<t_perfroutine64>(&DelayVector::perform64), 0, NULL);
#endif
}

/*static*/ void DelayVector::perform64( PlainObject *x, t_object *dsp64, double **ins,
                                       long numins, double **outs, long numouts,
                                       long sampleframes, long flags, void *userparam)
{
  // post("DelayVector::perform64() called.");
  x->mObject->performInternal(dsp64, ins, numins, outs, numouts, sampleframes, flags, userparam );
}

void DelayVector::performInternal( t_object *dsp64, double **ins,
                                  long numins, double **outs, long numouts,
                                  long sampleframes, long flags, void *userparam)
{
  if( numins != mNumberOfChannels )
  {
    error("DelayVector: Parameter \"numins\" does not match the number of predefined channels.");
  }
  double zin;
  
  
  /* Perform the DSP loop */
  for( std::size_t chIdx( 0 ); chIdx < mNumberOfChannels; ++chIdx)
  {
    double const * in = ins[chIdx];
    double * out = outs[chIdx];

    long n = sampleframes;

    while (n--)
    {
      zin = *in++;
      *out++ = zin * mCurrentGain;
    }
  }
}

/*static*/ void DelayVector::getFloat( PlainObject *x, double f)
{
  x->mObject->applyFloat(f);
}

void DelayVector::applyFloat( double f )
{
  int inlet = ((t_pxobject*)mPlainStruct)->z_in;
  std::stringstream stream;
  // stream << "DelayVector::applyFloat() called with inlet = " << inlet << ", f=" << f << "." <<std::endl;
  post( stream.str().c_str() );
  switch(inlet)
  {
    case 0: // The number of channels. I need to move it to the INT function
      mNumberOfChannels = (int)f;
      break;
    case 1:
      if(f < 0.0 || f > 1.0 )
      {
        error("delay_vector~: illegal gain: %f reset to 1", f);
      }
      else
      {
        mCurrentGain = f;
      }
      break; // Do nothing! ()
  }
}
  
  /*static*/ void DelayVector::assist( PlainObject  *x, void *b, long msg, long arg, char *dst)
  {
    if (msg==ASSIST_INLET) {
      switch (arg) { // if there is more than one inlet or outlet, a switch is necessary
        case 0: sprintf(dst,"(int/signal1 Input) Number of channels (TO BE IMPLEMENTED)"); break;
        case 1: sprintf(dst,"(float/signal2 Input) gain (between 0 and 1)"); break;
        default: sprintf(dst,"(signal %ld) Input", arg + 1); break;
      }
    }
    else if (msg==ASSIST_OUTLET)
    {
      switch (arg) {
        default: sprintf(dst,"(signal %ld) Output", arg + 1); break;
      }
    }
  }

/**
 * Instantiation of the member variable.
 */
/*static*/ t_class* DelayVector::sStaticClassInstance;


} // namespace
} // namespace


extern "C"
{

//****************************
// 2. Objects structure
typedef struct _delay_vector {
  t_pxobject obj; // MaxMSP proxy
  double gain; // current delay time
  long N_CHANNELS; // Number of channels
} t_delay_vector;

//****************************
// 3. Definition of pointer to delay_vector~ class (global variable)
static t_class *delay_vector_class; 

//****************************
// 4. Function prototypes
void *delay_vector_new(t_symbol *s, short argc, t_atom *argv);


void delay_vector_assist(t_delay_vector *x, void *b, long msg, long arg, char *dst);void delay_vector_float(t_delay_vector *x, double f);
void delay_vector_float(t_delay_vector *x, double f);


void delay_vector_dsp64(t_delay_vector *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags);
void delay_vector_perform64(t_delay_vector *x, t_object *dsp64, double **ins, 
     long numins, double **outs, long numouts, 
     long sampleframes, long flags, void *userparam);
void delay_vector_free(t_delay_vector *x);

// - 4.5
double const MAX_GAIN = 1.0f;

//****************************
// 5. Initialization routine MAIN
int C74_EXPORT main()
{
#if 1
  post( "visr::maxmsp::DelayVector::main() called." );
  
  // Initialize
  visr::maxmsp::DelayVector::sStaticClassInstance
    = class_new("delay_vector~",
                reinterpret_cast<method>(&visr::maxmsp::DelayVector::newObject),
                reinterpret_cast<method>(&visr::maxmsp::DelayVector::free),
                sizeof(visr::maxmsp::DelayVector::PlainObject), 0, A_GIMME, 0);
  // The rest of the initialization could be done within a method of DelayVector

  class_addmethod(visr::maxmsp::DelayVector::sStaticClassInstance,
                  reinterpret_cast<method>(&visr::maxmsp::DelayVector::dsp64), "dsp64", A_CANT, 0);
  
  // Binding of methods
  class_addmethod(visr::maxmsp::DelayVector::sStaticClassInstance,
                  reinterpret_cast<method>(&visr::maxmsp::DelayVector::assist), "assist", A_CANT, 0);
  class_addmethod(visr::maxmsp::DelayVector::sStaticClassInstance,
                  reinterpret_cast<method>(&visr::maxmsp::DelayVector::getFloat),
                  "float", A_FLOAT, 0);

  // Other calls
  class_dspinit(visr::maxmsp::DelayVector::sStaticClassInstance);
  class_register(CLASS_BOX, visr::maxmsp::DelayVector::sStaticClassInstance);
  post("DelayVector: main() finished.");
  
#else
  post( "visr::maxmsp::DelayVector::main() called." );

  delay_vector_class = class_new("delay_vector~", (method)delay_vector_new, (method)delay_vector_free, sizeof(t_delay_vector), 0, A_GIMME, 0);

	// Binding of DSP method 64 bit
	class_addmethod(delay_vector_class, (method)delay_vector_dsp64, "dsp64", A_CANT, 0);

	// Binding of methods
	class_addmethod(delay_vector_class,(method)delay_vector_assist, "assist", A_CANT, 0);
	class_addmethod(delay_vector_class,(method)delay_vector_float, "float", A_FLOAT, 0);
	
	// Other calls
	class_dspinit(delay_vector_class);
	class_register(CLASS_BOX, delay_vector_class);

	post("DelayVector: main() finished.");
#endif
	return 0;
}

//****************************
// 6. The New Instance Routine 
void *delay_vector_new(t_symbol *s, short argc, t_atom *argv) {

  post("DelayVector: *delay_vector_new called.");
  
    int i;
    float GAIN = 1.0;
    float N_CHANNELS = 2.0; // Default number of channels

	t_delay_vector *x = static_cast<t_delay_vector *>( object_alloc(delay_vector_class));

    atom_arg_getfloat(&N_CHANNELS, 0, argc, argv);
    x->N_CHANNELS = N_CHANNELS;

    // Creating the inlets
    dsp_setup(&x->obj, (int)N_CHANNELS);
    
    // Creating the outlets
    for (i = 0; i < (int)N_CHANNELS; i++)	{ outlet_new((t_object *)x, "signal"); }

    atom_arg_getfloat(&GAIN, 1, argc, argv);
    x->gain = (double)GAIN;

	return x; // returning the pointer to the delay_vector obj
}

// 6b. Connecting obj to DSP chain 64 bits
void delay_vector_dsp64(t_delay_vector *x, t_object *dsp64, short *count, 
	double samplerate, // Sampling rate and vector size
	long maxvectorsize, // are passed as arguments
	long flags)
{
  
  post("DelayVector: *delay_vector_dsp64 called.");
  
	//post("Executing the 64-bit perform routine");
	dsp_add64(dsp64, (t_object*)x, (t_perfroutine64)delay_vector_perform64, 0, NULL);
}

// 7b. Perform routine 64 bits
void delay_vector_perform64(t_delay_vector *x, t_object *dsp64, double **ins, 
    long numins, double **outs, long numouts, 
    long sampleframes, long flags, void *userparam)
{
	
    double gain = x->gain;
	double zin;
    
	/* Perform the DSP loop */
	for (int ch_idx = 0; ch_idx < x->N_CHANNELS; ch_idx++) { // For each input and output
        double *in = ins[ch_idx];
        double *out = outs[ch_idx];
        
        long n = sampleframes;

        while (n--) {
            zin = *in++;
            *out++ = zin * gain;
        }
    }
}

//****************************
// 8. Assist method
void delay_vector_assist(t_delay_vector *x, void *b, long msg, long arg, char *dst)
{ 
	if (msg==ASSIST_INLET) {
		switch (arg) { // if there is more than one inlet or outlet, a switch is necessary
			case 0: sprintf(dst,"(int/signal1 Input) Number of channels (TO BE IMPLEMENTED)"); break;
			case 1: sprintf(dst,"(float/signal2 Input) gain (between 0 and 1)"); break;
            default: sprintf(dst,"(signal %ld) Input", arg + 1); break;
                
		}
	}
	else if (msg==ASSIST_OUTLET) {
        switch (arg) {
            default: sprintf(dst,"(signal %ld) Output", arg + 1); break;
        }
	} 
}

void delay_vector_free(t_delay_vector *x) {
	/* We must call dsp_free() before freeing any dynamic memory 
	   allocated for the external. This removes the object from the
	   Max/MSP DSP chain. */
	dsp_free((t_pxobject *) x);
}

//****************************
// 9. The float method
void delay_vector_float(t_delay_vector *x, double f)
// This method gets called whenever a float is sent to the inlets of the object
{
    
	int inlet = ((t_pxobject*)x)->z_in;
	switch(inlet){
		case 0: // The number of channels. I need to move it to the INT function
			x->N_CHANNELS = (int)f;
		case 1:
			if(f < 0.0 || f > MAX_GAIN){ error("delay_vector~: illegal gain: %f reset to 1", f);
			} else { x->gain = f; }
			
			break; // Do nothing! ()
		
	}
}

} // extern "C"