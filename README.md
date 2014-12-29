RaceTrack Cache Explorations
============================

## Introduction

A Secondary Cache design specifically for RaceTrack memory (domain-wall memory (DWM)).

This project is course project for Peking University Computer Orignization class.

* A L2-Cache simulator will be implemented.
* A reference design for RaceTrack Memery Cache will be implemented.
* A design provided by us with improved performance.
* A few benchmark adoped from spec2006 will be used to demonstrate the improvements.

## Author

* 杨俊睿 Student Number: 1200012860
* 刘当一 Student Number: 1100011625




## Design

### Structure

We propose a cache structure similar to TapeCache[3]

![Alt text](/res/architecture.png "TapeCache")

* Notably, we uses a hybird array method to mamage cache. We realize the data array using RaceTrack memory and the tag array using SRAM.
* The SRAM might store additional data enable better schedule flexablility for data array.
* The RaceTrack Memory is designed with multiple read and write port.
* We will added a Cache Organize Unit for organize and emit additional data replacement control signal while the cache is not accessed by CPU.

### Strategy

* Static Strategy

	1. "Hybird Port"

		We never need to read and write or write and read a same cell in a row, we believe placing a RW port whose length equals a read and write port combination would be a waste.
		For example, in IdleSort strategy, we might want to swap neighbour cell content to align the content in a bubblesort way. In this case, a read and write port combination with aside port can reach 2 cycle / swap and a RW port can only achieve 3 cycle / swap.

	2. "Massive Read Port" and "Swap port"

		Because read operation is often on critical path for computing. We can significantly increase the profromance by increase the read port while maintain the same number of write port.

		"Swap port" is a combination of a pair of adjoining read and write port with their port also placed together. As steated in "Hybird Port" section, this enable fast bubble-sort used in "Idle Defragment" section.

	3. Set placement across group

		We prefer placing neighboor set in different group, as they are often accessed together. This allow the first access to shift first group while the group for the group for second access is still remain in their best position and unaffected by the first access.


* Dynamic Strategy
	
	1. Tape header management

		We choose a "Eager" policy: the port always return to an optimal positon after each access. This decision is basis for many other strategy we adopted below.

	2. "Set Reampping"

		We deploy additional bit in tag array to store the address accross the same group. Thus, a cache line in a set can be remapped to anywhere in the group while maintain same time for tag comparasion.

	3. "Lazy Evict"

		When a cache miss is generated, we will immediately return the data from the main memory to CPU without evict the corrispond cell. The COU will preform the evict action then.

	4. "Idle Defragment"

		We discovered that cell around the position of read port can be accessed faster. Thus, we want the more likely-to-be-accessed cache line be placed near the read port. Fortunately, the history data stored for evict decision can also be used to decide optimal position for each cache line in cell.

		"Idle Defragment" is a action performed during idle period to maintain the most optimal placement (most used cell ) for each cell. Also, It must be able to be interrupted smoothly when CPU access a data in the group being defragmenting.

	5. ("Shift Predictor")

		We can maintain a "Shift Prediction Table" for each cell in group, recording next access position to the same group. This strategy is conflict with most strategies mentioned above and might not be adopted.

	6. "Dual Channel"

		We observe that each access may come with one swap to maintain the order. Swap between two group cost significantly less than swap in one group. Thus, we futher add one bit to address bits in tag array to enable each set to be placed across two group.

## References

* [1]	M. Mao, W. Wen, Y. Zhang, Y. Chen, and H. H. Li, “Exploration of GPGPU Register File Architecture Using Domain-wall-shift-write based Racetrack Memory,” presented at the DAC '14: Proceedings of the 51st Annual Design Automation Conference, New York, New York, USA, 2014, pp. 1–6.
* [2]	Z. Sun, X. Bi, A. K. Jones, and H. Li, “Design exploration of racetrack lower-level caches.,” ISLPED, pp. 263–266, 2014.
* [3]	R. Venkatesan, V. J. Kozhikkottu, C. Augustine, A. Raychowdhury, K. Roy, and A. Raghunathan, “TapeCache: a high density, energy efficient cache based on domain wall memory.,” ISLPED, pp. 185–190, 2012.
