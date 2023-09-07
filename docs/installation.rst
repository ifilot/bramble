.. _installation:
.. index:: Installation

Installation
============

:program:`Bramble` is developed for Linux operating systems. In order to compile
:program:`Bramble` on your system, you need to ensure the following libraries are
available to you:

* `Eigen3 <https://eigen.tuxfamily.org>`_ (matrix algebra)
* `Boost <https://www.boost.org/>`_ (common routines)
* `TCLAP <https://tclap.sourceforge.net/>`_ (command line instruction library)
* `CMake <https://cmake.org/>`_ (build tool)

On Debian-based operating systems, one can run the following::

    sudo apt install -y build-essential cmake libboost-all-dev pkg-config libeigen3-dev libtclap-dev

.. note::
   If you are running Windows and would like to use :program:`Bramble`, one option
   is to use `Debian for Windows Subsystem for Linux (WSL) <https://apps.microsoft.com/store/detail/debian/9MSVKQC78PK6>`_.
   The compilation instructions below can be readily used.

.. warning::
   In order to compile for GPU using CUDA, one needs Eigen3 version **3.4.0** or higher.

Compilation
-----------

Compilation of :program:`Bramble` is fairly straightforward and a typical procedure
looks as follows::

    git clone https://github.com/ifilot/bramble.git
    cd bramble
    mkdir build && cd build
    cmake ../src
    make -j

To install :program:`Bramble`, you can in addition run::

    sudo make install

which will place a copy of the ``bramble`` executable in ``/opt/bramble/bin/bramble``.

CUDA support
------------

.. tip::
   If you are running :program:`Bramble` under Windows Subsystems for Linux (WSL)
   version 2, you can use CUDA from the Linux environment under Windows.
   Detailed instructions are given `here <https://docs.nvidia.com/cuda/wsl-user-guide/index.html>`_.

The similarity analysis functionality of :program:`Bramble` can
benefit from the availability of a graphical card. To compile :program:`Bramble`
with CUDA support, run CMake with::

    cmake ../src -DMOD_CUDA=1

To test that :program:`Bramble` can use your GPU, you can run the ``bramblecuda``
tool whose sole function is to test for the availability of a GPU on the system::

    ./bramblecuda

Typical output should look as follows::

    --------------------------------------------------------------
    Running bramble-tool v.0.3.0
    Author: Ivo Filot <i.a.w.filot@tue.nl>
    Documentation: https://bramble.imc-tue.nl
    --------------------------------------------------------------
    Compilation time: May 19 2023 16:19:56
    Git Hash: 64694c5
    Modules: CUDA
    --------------------------------------------------------------
    Testing CUDA...
    Device Number: 0
      Device name: NVIDIA GeForce RTX 4090
      Memory Clock Rate (KHz): 10501000
      Memory Bus Width (bits): 384
      Peak Memory Bandwidth (GB/s): 1008.1

.. note::
   * There is currently no support for using multiple GPUs. :program:`Bramble`
     automatically selects the first GPU available and executes the code on this
     GPU. Multi-GPU support is however in development.
   * The functionality of `bramblecuda` is only for showing information on the
     GPUs available on your system. The actual GPU-accelerated calculation is
     still handled by the `bramble` executable.

Testing
-------

To test :program:`Bramble`, one can run the following after compilation::

    make test

For verbose testing, run::

    CTEST_OUTPUT_ON_FAILURE=TRUE make test

Typical output should look as follows::

    Running tests...
    Test project /mnt/d/PROGRAMMING/CPP/bramble/build
        Start 1: test_basic
    1/9 Test #1: test_basic .......................   Passed    0.05 sec
        Start 2: test_geo
    2/9 Test #2: test_geo .........................   Passed    0.11 sec
        Start 3: test_xyz
    3/9 Test #3: test_xyz .........................   Passed    0.06 sec
        Start 4: test_error
    4/9 Test #4: test_error .......................   Passed    0.08 sec
        Start 5: test_poscar
    5/9 Test #5: test_poscar ......................   Passed    0.08 sec
        Start 6: test_cna
    6/9 Test #6: test_cna .........................   Passed    0.13 sec
        Start 7: test_fingerprint
    7/9 Test #7: test_fingerprint .................   Passed    0.05 sec
        Start 8: test_pattern_library
    8/9 Test #8: test_pattern_library .............   Passed    0.06 sec
        Start 9: test_similarity
    9/9 Test #9: test_similarity ..................   Passed    1.02 sec

    100% tests passed, 0 tests failed out of 9

    Total Test time (real) =   1.73 sec

EasyBuild Installation
----------------------

For HPC infrastructure, there is also the option to install :program:`Bramble` using EasyBuild.
Make a copy of `bramble-1.1.0.eb` and run::

    eb bramble-1.1.0.eb --minimal-toolchains --add-system-to-minimal-toolchains --robot
