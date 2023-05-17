.. _pattern_library:
.. index:: Pattern Library

Pattern Library
===============

Given a local atomic environment, a unique CNA pattern can be generated. To
efficiently identify these CNA pattern, we make use of a so-called pattern
library. The purpose of this pattern library is two-fold. First, it allows
us to add a human-readable label to each CNA pattern. Second, we can assign
colors to these CNA patterns which can be used for visualization purposes.

The pattern library is encoded as a `JSON <https://en.wikipedia.org/wiki/JSON>`_
file. Its structure is as follows::

  {
   "patterns":
   {
      "unknown":
      {
         "label": "Unknown",
         "fingerprint": "",
         "color": "7b7b7b"
      },
      "fcc_bulk":
      {
         "label": "FCC bulk",
         "fingerprint": "12(4,2,1)",
         "color": "0b3d05",
         "file": "fcc.am"
      },
      "fcc111":
      {
         "label": "FCC(111)",
         "fingerprint": "3(4,2,1)6(3,1,1)",
         "color": "49ade1",
         "file": "fcc111.am"
      },
      "fcc100":
      {
         "label": "FCC(100)",
         "fingerprint": "4(4,2,1)4(2,1,1)",
         "color": "2f8323"
      }
    }
  }

For each type of CNA pattern, a short-hand label is used that represents the
CNA item. For example, in the above snippet the labels ``fcc_bulk``, ``fcc111``,
and ``fcc100`` are used to represent the items. In turn, for each item a
``label``, a ``fingerprint``, a ``color`` and potentially ``file`` can be
specified.

* ``label``
    A human-readable descriptive label.
* ``fingerprint``
    The CNA pattern.
* ``color``
    The color to be used when rendering the atom.
* ``file``
    A file containing the adjacency matrix. This is an optional argument.
