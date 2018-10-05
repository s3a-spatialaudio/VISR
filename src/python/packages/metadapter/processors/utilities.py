def parse_object_type(obj):
    # THIS FUNCTION DETERMINES THE OBJECT TYPE AND THE VERSION OF OBJECT POSITION THAT'S USED
    # It returns

    verbose = False

    if verbose is True:
        print("Parsing object id: %i" % int(obj['id']))

    type = obj['type']
    pos = []

    if type == 'point':

        pos.append('position')

        if 'x' in obj['position']:

            pos.extend(('x', 'y', 'z'))

        elif 'az' in obj['position']:

            pos.extend(('az', 'el', 'radius'))

        else:

            print ('parse_object_type: Object position specifiers not recognised')
            pos = ['unknown', 'unknown', 'unknown']

    elif type == 'plane':

        pos.append('direction')

        pos.extend(('az', 'el', 'refdist'))

    elif type == 'pointdiffuse':

        pos.append('position')
        pos.extend(('x', 'y', 'z'))

    else:

        print ('parse_object_type: Object type (%s) not recognised or not supported' % type)
        pos = ['unknown', 'unknown', 'unknown']

    if verbose is True:
        print("parse object type. Object ID: %i.    Object type: %s.    Object pos: %s. " % (int(obj['id']), type, pos))
        print

    return type, pos


def print_empty(n):

    for _ in range(0, n):
        print
