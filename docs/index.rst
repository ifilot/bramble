Bramble: Fast Common Neighbor Analysis
======================================

.. image:: https://img.shields.io/github/v/tag/ifilot/bramble?label=version
   :alt: GitHub tag (latest SemVer)
.. image:: https://github.com/ifilot/bramble/actions/workflows/build.yml/badge.svg
   :target: https://github.com/ifilot/bramble/actions/workflows/build.yml
.. image:: https://img.shields.io/badge/License-GPLv3-blue.svg
   :target: https://www.gnu.org/licenses/gpl-3.0

:program:`Bramble` is a single atom pattern recognition algorithm based on the
:ref:`Common Neighbor Analysis method <background>`. It can efficiently construct
:ref:`CNA fingerprints <background>` per atom and connect these fingerprints
to a (customizable) :ref:`pattern library <pattern_library>` to
add labels to the fingerprints. Complimentary scripts are available for easy
visualization in `Blender <https://www.blender.org/>`_ and/or
`Matplotlib <https://matplotlib.org/>`_.

.. figure:: _static/img/nanoparticle_identified_atoms.png
    :align: center

    Cobalt nanoparticle of 15625 atoms generated using a simulated annealing
    procedure. All colored atoms are identified by the CNA algorithm. The
    unknown atoms, i.e. atoms with an unknown CNA fingerprint, are rendered
    using a glass material.

:program:`Bramble` has been developed at the Eindhoven University of Technology,
Netherlands. :program:`Bramble` and its development are hosted on `github
<https://github.com/ifilot/bramble>`_.  Bugs and feature
requests are ideally submitted via the `github issue tracker
<https://github.com/ifilot/bramble/issues>`_.

.. toctree::
   :maxdepth: 2
   :caption: Contents:

   installation
   background
   gallery
   user_interface
   examples
   pattern_library
   publications
   community_guidelines

Indices and tables
------------------

* :ref:`genindex`
* :ref:`search`
