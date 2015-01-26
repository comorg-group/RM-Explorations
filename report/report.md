# Explorations on building a cache system based on Racetrack Memory
## RaceTrack Cache simulator design and syetem optimization

### Introduction

The wide adoption of chip multiprocessors (CMPs) has generated an explosive demand for on-chip memory resources. Unfortunately, as technology scaling continues, the reliance on conventional SRAM technology for on-chip caches raises
several concerns including the large memory cell size, the high leakage power consumption, and the low resilience to soft errors. A very recent emerging nonvolatile memory technology, the racetrack memory [x], has been projected to over come the density barrier and high leakage power of existing memory solutions.[x]

//TODO: repharse the paragraph above
//TODO: about other studies in racetrack memory

### Basics of Racetrack Memory

Figure 1 showed a basic structure of a racetrack strip, which consist of a nano wire and serveral reading and writing port. The nano wire has many magnetic domains, where the information stores at, and they are seprated by narrow domain walls for prevention of wrong access. The nano wire is drived by shift ctrl, allowing the information stored inside those domains to shift across the nano wire. Some access port is placed across the nano wire inorder to read and write data on the wire. Typically, because of the large size of access ports comparing to the domains on the nano wire, there are much more domains than access ports. Traditionally, only RW-port is build enabling both write and read access to the domains. Recent development propose R port and W port in a smaller size for more flexiblity in design.

