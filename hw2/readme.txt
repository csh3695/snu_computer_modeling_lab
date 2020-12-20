README for \smpl_new                                              KJC (04/03/05)
--------------------

This directory contains the SMPL "language: as described in the book,
"Simulating Computer Systems, Techniques and Tools" by M. H. MacDougall (MIT
Press, 1987).  Sample programs from the book have not been included.  Changes
have been made, and noted in each file, to make compiling possible with gcc on
UNIX machines.  Everything here should work with Borland and Microsoft compilers
on a Windows PC and gcc on UNIX machines (e.g., Linux on PC, Solaris on SPARC).

The files in this directory are:

  bmeans.c ------ SMPL batch means functions
  build.bat ----- Batch file for bcc32 build
  mm1.c --------- M/M/1 simulation model
  mm1a.c -------- M/M/1 simulation model with bmeans stopping
  rand.c -------- SMPL random number generation functions
  readme.txt ---- This file
  smpl.c -------- SMPL function
  smpl.h -------- SMPL include file

The following files are needed for an SMPL simulation model:

  myprog.c ------ A simulation program that you write
  smpl.c -------- SMPL functions
  smpl.h -------- Include file for SMPL
  rand.c -------- Random number generators
  bmeans.c ------ Batch means functions (stat.c merged into this file)

To build myprog.exe on a PC using a Borland compiler do the following:

  bcc32 myprog.c smpl.c rand.c bmeans.c

where myprog.c is a simulation model using SMPL.  The build batch file (for
bcc32, but easily changed for another compiler) can also be used as:

  build myprog.c

To build a.out on a UNIX machine using the cc or gcc compilers do the following
(the "-lm" links the math library):

  gcc myprog.c smpl.c rand.c bmeans.c -lm

where myprog.c is a simulation model using SMPL.


Ken Christensen
christen@csee.usf.edu
http://www.csee.usf.edu/~christen

---
