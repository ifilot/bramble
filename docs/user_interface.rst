.. _userinterface:
.. index:: User Interface

User Interface
==============

:program:`Bramble` is shipped with two executables, ``bramble`` and ``brambletool``.
The former is to perform the CNA analysis, the latter is to build, edit, and
validate the pattern library.

.. note::
    :program:`Bramble` is quite flexible in terms of the order of the command
    line arguments as long as any instructions belonging to a specific
    argument are directly after that argument.

.. warning::
    * Note that :program:`Bramble` uses roughly 8GB per execution thread, where
      the number of execution threads is ``N+1`` where ``N`` is the number of GPUs.
      See also :ref:`this page <memory_load>`.
    * For systems having **multiple** GPUs, one needs to explicitly set
      ``--ngpu <number of gpus>`` to make use of all GPUs. If not, only one of
      the GPUs is being used.

Bramble
-------

Bramble has two execution modes:

1. **CNA pattern recognition**

   To every atom a CNA fingerprint is assigned according to the
   :ref:`adaptive CNA algorithm <background>`. If the CNA fingerprint is known
   from the pattern library, a label is given. These labels are based on known
   local atomic configurations of surface terminations.

2. **Similarity analysis**

   It is possible that a given local atomic configuration is not recognized. A
   common situation is when the local atomic environment is somewhat amorphous.
   In these cases, it is helpful to have a metric how much this atomic
   configuration differs from any of the known configurations. To this end,
   a similarity metric based on the minimized Hilbert-Schmidt norm is used.
   The lower this metric, the more alike the configurations are. In the limit
   that this norm is zero, the local environments are the same.

CNA pattern recognition
***********************

To perform the CNA analysis, launch ``./bramble`` with the following three
mandatory command line arguments::

    ./bramble -p <pattern-file> -i <geometry-file> -o <output-file>

* ``-i``, ``--input`` ``<geometry-file>``
    File containing the geometry of the system. :program:`Bramble` supports

    * VASP `POSCAR/CONTCAR <https://www.vasp.at/wiki/index.php/POSCAR>`_ files
    * `XYZ <http://openbabel.org/wiki/XYZ_(format)>`_ files
    * `ReaxFF GEO <https://www.scm.com/doc/plams/interfaces/reaxff.html>`_ files

* ``-p``, ``--pattern`` ``<pattern-file>``
    Pattern library encoded in a JSON file. Details of the pattern library can
    be found :ref:`here <pattern_library>`.

* ``-o``, ``--output`` ``<output-file>``
    Where to write the output to.

* ``-g``, ``--ngpu`` ``<number of gpus>``
    Number of GPUs to use. This option is only available when :program:`Bramble`
    is compiled with the CUDA module. If more GPUs are allocated via this tag
    than the number of GPUs available, the number is automatically lowered to
    match the number of GPUs available. The default value is 1, so for multi-GPU
    systems, the user needs to manually adjust this value.

*Example*: ``./bramble -p ../patterns/patterns.json -i ../src/test/data/co_np.geo -o result.txt``

Typical output looks as follows::

    --------------------------------------------------------------
    Executing Bramble v.0.2.0
    Author: Ivo Filot <i.a.w.filot@tue.nl>
    Documentation: https://bramble.imc-tue.nl
    --------------------------------------------------------------
    Compilation time: May 18 2023 20:20:12
    Git Hash: acfc9bc
    --------------------------------------------------------------
    Loading ../src/test/data/co_np.geo as .geo file.
    Geometry file contains 3375 atoms.
    Calculating interatomic distances...
    Completed calculation of interatomic distances.
    Start common neighbor analysis...
    Common neighbor analysis completed.
    --------------------------------------------------------------
    Done in 0.10766 seconds.

Similarity Analysis
*******************

.. warning::
    The similarity analysis algorithm can easily takes several minutes up to
    several hours (and even days) to perform. Establishing the minimum
    Hilbert-Schmidt norm of two distance matrices is tied to the graph
    isomorphism problem. The algorithm "tackles" this by performing a brute
    force algorithm.

To perform the similarity analysis, launch ``./bramble`` with the following
command line arguments::

    ./bramble -s -i <geometry-file> -o <output-file>

* ``-i``, ``--input`` ``<geometry-file>``
    File containing the geometry of the system. :program:`Bramble` supports

    * VASP `POSCAR/CONTCAR <https://www.vasp.at/wiki/index.php/POSCAR>`_ files
    * `XYZ <http://openbabel.org/wiki/XYZ_(format)>`_ files
    * `ReaxFF GEO <https://www.scm.com/doc/plams/interfaces/reaxff.html>`_ files

* ``-s``, ``--similarity``
    Perform a similarity analysis

* ``-o``, ``--output`` ``<output-file>``
    Where to write the output to.

*Example*: ``./bramble -s -i ../src/test/data/POSCAR_Rh111.geo -o sa.txt``

Bramble Tool
------------

To create, edit or validate a pattern library, one can use ``brambletool``.

.. tip::
    It is recommended to always use the ``.json`` extension for the pattern
    library filename.

Create a pattern library
************************

To create a new pattern library, run::

    ./brambletool -c -p <new_pattern_library.json>

Adding patterns
***************

.. tip::
    It is commended to enclose the full pattern description in quotation
    marks to ensure proper parsing. Also note that each element of the pattern
    entry should be separated by semicolons.

To add a new pattern entry to the library, run::

    ./brambletool -a "<key>;<fingerprint>;<description>;<color>" -p <pattern_library.json>

The entries for ``<key>``, ``<fingerprint>`` and ``<color>`` are automatically
validated. The entry ``<key>`` should only contain alphanumerical characters,
underscores and dashes. The ``<fingerprint>`` entry should be a valid CNA
fingerprint composed of a series of ``n(x,y,z)`` triplets. Finally, the
``<color>`` entry should be a valid hexadecimal color code. Do **not** start
the color code with a leading ``#``.

Editing patterns
****************

Patterns are edited based on the entry key. To edit a pattern, run::

    ./brambletool -e "<key>;<fingerprint>;<description>;<color>" -p <pattern_library.json>

Removing patterns
*****************

Patterns are removed based on their entry key:

    ./brambletool -d "<key>" -p <pattern_library.json>

Listing patterns
*****************

To list all patterns in the library, one can run::

    ./brambletool -l -p <pattern_library.json>

Typical output looks as follows::

    --------------------------------------------------------------
    Running bramble-tool v.0.2.0
    Author: Ivo Filot <i.a.w.filot@tue.nl>
    Documentation: https://bramble.imc-tue.nl
    --------------------------------------------------------------
    Compilation time: May 18 2023 20:20:11
    Git Hash: acfc9bc
    --------------------------------------------------------------
    Printing list of patterns:
                   SC(223)   ffffff 4(6,7,3)2(5,5,3)3(4,3,3)2(3,2,2)
                   SC(135)   ffffff 4(5,5,3)2(4,4,2)2(2,1,1)
                   SC(123)   ffffff 2(6,7,3)2(5,5,3)2(4,4,2)1(4,3,3)2(3,2,2)
                   SC(123)   ffffff 2(5,5,3)4(4,4,2)1(2,1,1)
                   SC(112)   ffffff 3(6,7,3)2(5,5,3)1(4,4,2)2(4,3,3)2(3,2,2)
                   SC(112)   ffffff 1(6,7,3)4(5,5,3)1(4,4,2)2(3,2,2)1(2,1,1)
                   SC(011)   ffffff 4(0,0,0)
                   SC(001)   ffffff 5(0,0,0)
                   SC bulk   ffffff 6(0,0,0)
    --------------------------------------------------------------
    Done.

Validating patterns
*******************

When receiving a third-party JSON library, it is good practice to first validate
all the entries before running ``bramble``. To do so, run::

    ./brambletool -v -p <pattern_library.json>

Typical output looks as follows::

    --------------------------------------------------------------
    Running bramble-tool v.0.2.0
    Author: Ivo Filot <i.a.w.filot@tue.nl>
    Documentation: https://bramble.imc-tue.nl
    --------------------------------------------------------------
    Compilation time: May 18 2023 20:20:11
    Git Hash: acfc9bc
    --------------------------------------------------------------
    Validating list of patterns:
                   SC(223)   ffffff [V] 4(6,7,3)2(5,5,3)3(4,3,3)2(3,2,2) [V]
                   SC(135)   ffffff [V] 4(5,5,3)2(4,4,2)2(2,1,1) [V]
                   SC(123)   ffffff [V] 2(6,7,3)2(5,5,3)2(4,4,2)1(4,3,3)2(3,2,2) [V]
                   SC(123)   ffffff [V] 2(5,5,3)4(4,4,2)1(2,1,1) [V]
                   SC(112)   ffffff [V] 3(6,7,3)2(5,5,3)1(4,4,2)2(4,3,3)2(3,2,2) [V]
                   SC(112)   ffffff [V] 1(6,7,3)4(5,5,3)1(4,4,2)2(3,2,2)1(2,1,1) [V]
                   SC(011)   ffffff [V] 4(0,0,0) [V]
                   SC(001)   ffffff [V] 5(0,0,0) [V]
                   SC bulk   ffffff [V] 6(0,0,0) [V]
    --------------------------------------------------------------
    Done.

Observe that all valid entries are marked by ``[V]``.
