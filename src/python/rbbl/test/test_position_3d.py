
import numpy as np


import scipy.spatial as ssp

import rbbl

def test_positionDefaultInit():
    pos = rbbl.Position3D()
    refPos = np.zeros(3)
    assert np.linalg.norm( np.asarray(pos) - refPos ) < 1e-7

def test_positionSphericalInterpolationNormalised(plot=False):
    pos0Ref = np.array( [0.2, 0.7, -0.5], dtype=np.float32 )
    pos1Ref = np.array( [0.2, -0.5, 0.3], dtype=np.float32 )
    pos0Ref /= np.linalg.norm(pos0Ref)
    pos1Ref /= np.linalg.norm(pos1Ref)
    pos0 = rbbl.Position3D(pos0Ref)
    pos1 = rbbl.Position3D(pos1Ref)

    numVals = 101
    t = np.linspace( 0.0, 1.0, numVals, endpoint=True)
    # Requitres SciPy >= 1.5
    # refOut = ssp.geometric_slerp( pos0Ref, pos1Ref, t )

    out = np.full((numVals,3), fill_value=np.NaN, dtype=np.float32)
    for idx in range(numVals):
        interpOut = rbbl.interpolateSpherical(pos0, pos1, t[idx])
        out[idx,:] = np.asarray(interpOut)

    if plot:
        import matplotlib.pyplot as plt
        from mpl_toolkits.mplot3d import proj3d
        fig = plt.figure()
        ax = fig.add_subplot(111, projection='3d')
        # PLot the unit sphere
        u = np.linspace(0, 2 * np.pi, 100)
        v = np.linspace(0, np.pi, 100)
        x = np.outer(np.cos(u), np.sin(v))
        y = np.outer(np.sin(u), np.sin(v))
        z = np.outer(np.ones(np.size(u)), np.cos(v))
        ax.quiver( 0, 0, 0, pos0Ref[0], pos0Ref[1], pos0Ref[2], color='b')
        ax.quiver( 0, 0, 0, pos1Ref[0], pos1Ref[1], pos1Ref[2], color='r')
        ax.plot_surface(x, y, z, color='y', alpha=0.1)
        ax.plot(out[...,0],
                out[...,1],
                out[...,2],
                c='k')
        plt.title('Spherical interpolation (unit vectors).')
        plt.show()

    # diff = np.linalg.norm(out - refOut)
    # assert np.all( diff < 1e-7)

def test_positionSphericalInterpolation(plot=False):
    pos0Ref = np.array( [0.2, 0.7, -0.5], dtype=np.float32 )
    pos1Ref = np.array( [0.2, -0.5, 0.3], dtype=np.float32 )
    pos0Ref *= 0.75 / np.linalg.norm(pos0Ref)
    pos1Ref /= np.linalg.norm(pos1Ref)
    pos0 = rbbl.Position3D(pos0Ref)
    pos1 = rbbl.Position3D(pos1Ref)

    numVals = 101
    t = np.linspace( 0.0, 1.0, numVals, endpoint=True)
    # Requitres SciPy >= 1.5
    # refOut = ssp.geometric_slerp( pos0Ref, pos1Ref, t )

    out = np.full((numVals,3), fill_value=np.NaN, dtype=np.float32)
    for idx in range(numVals):
        interpOut = rbbl.interpolateSpherical(pos0, pos1, t[idx])
        out[idx,:] = np.asarray(interpOut)

    if plot:
        import matplotlib.pyplot as plt
        from mpl_toolkits.mplot3d import proj3d
        fig = plt.figure()
        ax = fig.add_subplot(111, projection='3d')
        # PLot the unit sphere
        u = np.linspace(0, 2 * np.pi, 100)
        v = np.linspace(0, np.pi, 100)
        x = np.outer(np.cos(u), np.sin(v))
        y = np.outer(np.sin(u), np.sin(v))
        z = np.outer(np.ones(np.size(u)), np.cos(v))
        ax.quiver( 0, 0, 0, pos0Ref[0], pos0Ref[1], pos0Ref[2], color='b')
        ax.quiver( 0, 0, 0, pos1Ref[0], pos1Ref[1], pos1Ref[2], color='r')
        ax.plot_surface(x, y, z, color='y', alpha=0.1)
        ax.plot(out[...,0],
                out[...,1],
                out[...,2],
                c='k')
        plt.title('Spherical interpolation (unnormalised).')
        plt.show()


if __name__ == "__main__":
    test_positionDefaultInit()
    test_positionSphericalInterpolationNormalised(plot=True)
    test_positionSphericalInterpolation(plot=True)
