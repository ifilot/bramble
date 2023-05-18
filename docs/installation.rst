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

On Debian-based operating systems, one can run the following::

    sudo apt install -y build-essential cmake libboost-all-dev pkg-config libeigen3-dev libtclap-dev

.. note::
   If you are running Windows and would like to use :program:`Bramble`, one option
   is to use `Debian for Windows Subsystem for Linux (WSL) <https://apps.microsoft.com/store/detail/debian/9MSVKQC78PK6>`_.
   The compilation instructions below can be readily used.

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
    1/8 Test #1: test_basic .......................   Passed    0.00 sec
        Start 2: test_geo
    2/8 Test #2: test_geo .........................   Passed    0.03 sec
        Start 3: test_xyz
    3/8 Test #3: test_xyz .........................   Passed    0.02 sec
        Start 4: test_error
    4/8 Test #4: test_error .......................   Passed    0.01 sec
        Start 5: test_poscar
    5/8 Test #5: test_poscar ......................   Passed    0.01 sec
        Start 6: test_cna
    6/8 Test #6: test_cna .........................   Passed    0.03 sec
        Start 7: test_fingerprint
    7/8 Test #7: test_fingerprint .................   Passed    0.01 sec
        Start 8: test_pattern_library
    8/8 Test #8: test_pattern_library .............   Passed    0.01 sec

    100% tests passed, 0 tests failed out of 8

    Total Test time (real) =   0.25 sec
