.. _execution_model:
.. index:: Execution model

Execution model
===============

When :program:`Bramble` is compiled with the CUDA module, one can use GPU
acceleration to speed up the execution. This is especially beneficial when
performing a similarity analysis. :program:`Bramble` supports multi-GPU
setups, so one can use multiple GPUs if more than one GPU is available.

.. warning::
    :program:`Bramble` requires a GPU with at least 8Gb of memory. :program:`Bramble`
    will check whether the GPU supports the calculation prior to execution and throws
    an error when the GPU is not supported. You can also check the memory available
    on your GPU by running ``bramblecuda``.

When performing the similarity analysis, an inventory of all the jobs is made.
``N+1`` OpenMP threads are being spawned where ``N`` equals the number of GPUs.
Each GPU gets assigned a CPU thread and jobs are relayed to the GPU via the CPU
thread. The remaining OpenMP thread employs so-called nested parallellism and
executes another OpenMP parallel environment which uses all CPUs.

Obviously, this implies that the ``N`` CPU threads which are involved in
managing the GPUs are also used for other parts of the calculation. Since the
computational load of managing the GPUs is however relatively minimal, this does
come at a huge impact on performance. In fact, not using these CPUs is worse
than partially also using them to manage the GPUs.

When no GPUs are available, :program:`Bramble` uses no nested parallelism and
simply executes all jobs concurrently wherein each job uses OpenMP parallelism
on a per-job basis.

.. _memory_load:

Memory load
-----------

Performing calculations is quite memory expensive and as a rule of thumb, one
needs roughly 8GB of memory per execution thread. For example, if one uses
two GPUs, one needs roughly 24GB of memory. If memory is limited, one option
is to use swapping, however this comes at a great cost on performance. Nevertheless,
it might still be beneficial.

Assuming the user has root privileges, one can use the following instructions
to increase the amount of swap memory::

    sudo mkswap /swapfile
    sudo chmod 600 /swapfile
    sudo swapon /swapfile
    sudo swapon --show

Typical output would yield::

    NAME      TYPE      SIZE   USED PRIO
    /dev/sdb3 partition 976M   976M   -2
    /swapfile file        8G 196.3M   -3
