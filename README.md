# RaceTrack Memory Explorations
 
### References

1. http://dl.acm.org/citation.cfm?id=2333707
2. http://dl.acm.org/citation.cfm?id=2627651

### Strategy

1. 首先，我们使用 SRAM 作为 tag array，RM 作为 data array。
2. 在同一个 group 中，set 允许以任意顺序摆放，只需要根据 tag array 中的记录访问即可。
3. 基本参数：
	1. Cache size: 4 MB
	2. Cache line size: 64 bytes (512 bits)
	3. Set associativity: 8-way
4. 我们的优化策略分两类：
	1. Static 优化，
		* Uniform-port or hybrid-port
		* Port 的摆放
		* Set 在 group 间的排放
	2. Dynamic 优化
		* Evict 替换策略
		* 闲时整理策略
		* Pre-shift 策略
