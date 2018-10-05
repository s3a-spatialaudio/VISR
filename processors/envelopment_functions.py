import math

from sys import version_info

if version_info.major <= 2:
    import OSC
else:
    # Use the self-made port for Python 3 (experimental)
    from ..thirdparty import OSC as OSC

from .utilities import parse_object_type


def send_osc(value,osc_address,host,port):
    c = OSC.OSCClient()
    c.connect((host, port))
    oscmsg = OSC.OSCMessage()
    oscmsg.setAddress(osc_address)

    oscmsg.append(value)

    c.send(oscmsg)


# FUNCTION spread processing
def dospread(obj,env_level):

    # Non-linear mapping to approximate the experiment results (and make the perceived change more linear)

    mapping = 'nonlinear'    # or 'linear'

    if mapping == 'nonlinear':

        if env_level < 0.4:                     # Piecewise function with break at 0.4
            env_level = env_level / 2
        else:
            env_level = (((env_level - 0.4) / (1-0.4)) * (1-0.2)) + 0.2

    elif mapping == 'linear':
        env_level = env_level

    #print "    In dospread..."

    objtype, pos = parse_object_type(obj)

    orig_p1 = float(obj[pos[0]][pos[1]])
    orig_p2 = float(obj[pos[0]][pos[2]])
    orig_p3 = float(obj[pos[0]][pos[3]])


    #print( "Object ID: %i.   Original pos data: %s, %s.     Orig pos: %s" % (int(obj['id']), type, pos, [orig_p1, orig_p2, orig_p3]) )

    # If necessary, convert to degrees
    if pos[1] != 'az':                  # If it's not in degrees (i.e. it's cartesian), convert to degrees
        convert_flag = True
        orig_p3, orig_p1, orig_p2 = cart2sphDeg(orig_p1, orig_p2, orig_p3)
    else:
        convert_flag = False


    #print( "    Object ID: %i.   Original pos data: %s, %s.     Orig pos (edited): %s" % (int(obj['id']), objtype, pos, [orig_p1, orig_p2, orig_p3]) )


    adjustSpread = env_level				# Get the spread value

    orig_p1 = orig_p1 % 360					# Convert the azimuth to the range 0 to 360
    orig_p2 = orig_p2 % 360					# Convert the elevation to the range 0 to 360

    if orig_p1 > 180:						# Convert azimuth greater than 180 to negative, so the spread multiplier works
        orig_p1 = orig_p1 - 360				# ...

    if orig_p2 > 180:						# ... same for elevation
        orig_p2 = orig_p2 - 360				# ...

    p1 = orig_p1 * adjustSpread 			# New position = original position * spread
    p2 = orig_p2 * adjustSpread				# New position = original position * spread
    p3 = orig_p3

    #print("    Orig p1,p2,p3: %s.   New p1,p2,p3: %s.   adjustSpread: %f." % ([orig_p1, orig_p2, orig_p3],[p1, p2, p3],adjustSpread))

    # Convert back if necessary
    if convert_flag is True:
        p1, p2, p3 = sphDeg2cart(p1, p2, p3)


    # Write the new metadata
    #newdic = {pos[1]: p1, pos[2]: p2, pos[3]: p3}
    #print("Envelopment opt (in dospread) about to try this: %s" % newdic)

    #print "    About to change the object vector. Type before change: %s" % type(obj)
    obj[pos[0]] = {pos[1]: p1, pos[2]: p2, pos[3]: p3}
    #print "    Just changed the object vector. Type after change: %s" % type(obj)

    #origpos = [orig_p1, orig_p2, orig_p3]
    #newpos = [p1, p2, p3]

    #print origpos
    #print newpos

    return obj #, origpos. newpos

# FUNCTION level processing
def dolevel(obj,min,max,envelopment,oscaddress):

    #print '    In dolevel...'

    range = max - min 									# Calculate the range between min and max
    value = (range * envelopment) + min					# Calculate the modifying value (determined by envelopment level)

    #print "    Object %i level (in dolevel): %f" % ( int(obj['id']), float(obj['level']) )

    #print

    #print("Original level: %f.  Gain: %f.   New level: %f." % ( float(obj['level']), dB2lin(value), float(obj['level']) * dB2lin(value) )

    obj['level'] = (float(obj['level']) * dB2lin(value))	# Do the modification

    # Send value
    send_osc(value,oscaddress,'127.0.0.1',4556)

    return obj

# FUNCTION eq processing
def doeq(obj,min_lf,max_lf,min_hf,max_hf,envelopment):

    range_lf = max_lf - min_lf
    range_hf = max_hf - min_hf

    gain_lf = (range_lf * envelopment) + min_lf
    gain_hf = (range_hf * envelopment) + min_hf

    # EQ: (needs to add EQ section to objects and then make changes as required)
    eq_low = {u'type': u'lowshelf', u'f': 200.0, u'q': 1.0, u'gain': gain_lf}
    eq_high = {u'type': u'highshelf', u'f': 2000.0, u'q': 1.0, u'gain': gain_hf}
    obj[u'eq'] = [eq_low, eq_high]
    #obj[u'eq'] = [eq_low]

    send_osc(gain_lf,'lflevel','127.0.0.1',4556)
    send_osc(gain_hf,'hflevel','127.0.0.1',4556)


    return obj

def GetCurrentGroup(obj):

    if 'groupname' in obj:
        current_group = obj['groupname']

    elif 'group' in obj:
        current_group = int(obj['group'])

    else:
        current_group = 0

    return current_group

# FUNCTIONS to convert between dB and linear
def lin2dB( linVal ):
    return 20.0 * math.log10( linVal )

def dB2lin( dbVal ):
    return math.pow( 10.0, 0.05 * dbVal )

# FUNCTION to convert radians to degrees
def rad2deg( w ):
    return 180.0/math.pi * w

# FUNCTION to convert degrees to radians
def deg2rad( w ):
    return w / 180.0*math.pi

# FUNCTION to convert cartesian to degrees
def cart2sphDeg(x,y,z):
    XsqPlusYsq = x**2 + y**2
    r = math.sqrt(XsqPlusYsq + z**2)               # r
    elev = math.atan2(z,math.sqrt(XsqPlusYsq))     # theta
    az = math.atan2(y,x)                           # phi
    return r, rad2deg(az), rad2deg(elev)


# FUNCTION to convert spherical degrees to cartesian
def sphDeg2cart(az, el, r):
    az = deg2rad(az)
    el = deg2rad(el)

    z = r * math.sin(el)
    rcoselev = r * math.cos(el)
    x = rcoselev * math.cos(az)
    y = rcoselev * math.sin(az)

    return x, y, z