.. flux-help-include: true

=============
flux-start(1)
=============


SYNOPSIS
========

**flux** **start** [*OPTIONS*] [initial-program [args...]]

DESCRIPTION
===========

flux-start(1) launches a new Flux instance. By default, flux-start
execs a single flux-broker(1) directly. By default it will attempt to use
PMI to fetch job information and bootstrap a flux instance.

If a size is specified via *--size*, an instance of that size is to be
started on the local host with flux-start as the parent. (Mostly used for testing
purposes.)

A failure of the initial program (such as non-zero exit code)
causes flux-start to exit with a non-zero exit code.

Note: in order to launch a Flux instance, you must have generated
long-term CURVE keys using flux-keygen(1).


OPTIONS
=======

**-s, --size**\ =\ *N*
   Launch an instance of size *N* on the local host. Only works with
   *--bootstrap=selfpmi*. Automatically sets *--bootstrap=selfpmi* and prints
   a warning to stderr if no *--bootstrap* option is specified.

**-b, --bootstrap**\ =\ *METHOD*
   Select the flux bootstrap method. Possible values of *METHOD* are:

   -  pmi - Use PMI (Process Management Interface)

   -  selfpmi - The flux-start process will activate its own internal PMI server,
      and then it will launch flux-brokers that will bootstrap using said PMI server.

**-o, --broker-opts**\ =\ *option_string*
   Add options to the message broker daemon, separated by commas.

**-v, --verbose**
   Display commands before executing them.

**-X, --noexec**
   Don't execute anything. This option is most useful with -v.

**--caliper-profile**\ =\ *PROFILE*
   Run brokers with Caliper profiling enabled, using a Caliper
   configuration profile named *PROFILE*. Requires a version of Flux
   built with --enable-caliper. Unless CALI_LOG_VERBOSITY is already
   set in the environment, it will default to 0 for all brokers.

**--scratchdir**\ =\ *DIR*
   For selfpmi bootstrap mode, set the directory that will be
   used as the rundir directory for the instance. If the directory
   does not exist then it will be created during instance startup.
   If a DIR is not set with this option, a unique temporary directory
   will be created. Unless DIR was pre-existing, it will be removed
   when the instance is destroyed.

**--wrap**\ =\ *ARGS,…​*
   Wrap broker execution in a comma-separated list of arguments. This is
   useful for running flux-broker directly under debuggers or valgrind.


EXAMPLES
========

Launch an 8-way local Flux instance with an interactive shell as the
initial program and all logs output to stderr:

::

   flux start -s8 -o,--setattr=log-stderr-level=7

Launch an 8-way Flux instance within a slurm job, with an interactive
shell as the initial program:

::

   srun --pty -N8 flux start


RESOURCES
=========

Github: http://github.com/flux-framework


SEE ALSO
========

flux-broker(1) flux-keygen(1)
