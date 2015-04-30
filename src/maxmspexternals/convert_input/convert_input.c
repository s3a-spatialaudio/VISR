/**
	convert_input
	A Max/MSP external to convert OSC streams into Max-readable object metadata

	Philip Coleman, University of Surrey; and
	Andreas Franck, University of Southampton
*/

#include "ext.h"							// standard Max include, always required
#include "ext_obex.h"						// required for new style Max object

////////////////////////// object struct
typedef struct _convertin 
{
	t_object	ob;
	t_atom		val;
	t_symbol	*name;
	// inlets
	long		portnum;
	long		nobjects;
	// outlets
	void		*out1;
	void		*out2;
	void		*out3;
	void		*out4;
	
} t_convertin;

///////////////////////// function prototypes
//// standard set
void *convertin_new(t_symbol *s, long argc, t_atom *argv);
void convertin_free(t_convertin *x);

//// additional functions
void convertin_in1(t_convertin *x, long n);
void convertin_in2(t_convertin *x, long n);

//////////////////////// global class pointer variable
void *convertin_class;


int C74_EXPORT main(void)
{	
	t_class *c;
	c = class_new("convert_input", (method)convertin_new, (method)convertin_free, (long)sizeof(t_convertin), 
				  0L /* leave NULL!! */, A_GIMME, 0);
	class_addmethod(c, (method)convertin_in1, "in1", A_LONG, 0); // first inlet 
	class_addmethod(c, (method)convertin_in2, "in2", A_LONG, 0); // second inlet
	
	class_register(CLASS_BOX, c);
	convertin_class = c;
	return 0;
}

void convertin_in1(t_convertin *x, long n)
{
  
	post("input2 is %ld", n);
	x->portnum = n;
}

void convertin_in2(t_convertin *x, long n)
{
	post("input1 is %ld", n);
	x->nobjects = n;
}
void convertin_free(t_convertin *x)
{
	;
}



void *convertin_new(t_symbol *s, long argc, t_atom *argv) // initializes the object: "new instance routine"
{
	t_convertin *x = NULL;
	long i;
	
	if (x = (t_convertin *)object_alloc(convertin_class)) {
		object_post((t_object *)x, "a new %s object was instantiated: %p", s->s_name, x);
		object_post((t_object *)x, "it has %ld arguments", argc);

		for (i = 0; i < argc; i++) {
			if ((argv + i)->a_type == A_LONG) {
				object_post((t_object *)x, "arg %ld: long (%ld)", i, atom_getlong(argv + i));
			}
			else if ((argv + i)->a_type == A_FLOAT) {
				object_post((t_object *)x, "arg %ld: float (%f)", i, atom_getfloat(argv + i));
			}
			else if ((argv + i)->a_type == A_SYM) {
				object_post((t_object *)x, "arg %ld: symbol (%s)", i, atom_getsym(argv + i)->s_name);
			}
			else {
				object_error((t_object *)x, "forbidden argument");
			}
		}
	}

	// create inlets
	intin(x, 2); // rightmost inlet, create from right to left
	intin(x, 1);

    // create outlets
	x->out1 = outlet_new((t_object *)x, (C74_CONST char *)s);
	x->out2 = outlet_new((t_object *)x, (C74_CONST char *)s);
	x->out3 = outlet_new((t_object *)x, (C74_CONST char *)s);
	x->out4 = outlet_new((t_object *)x, (C74_CONST char *)s);

	return (x);
}
