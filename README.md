# Cache-Simulator
Implements a simple cache simulator that takes as an input the configurations of the cache to simulate, such as: size, associativity, and replacement policy

## Usage

Clone this repository using `git clone git@github.com:CrChromium/Cache-Simulator.git`

Navigate to the folder

Run `./SIM <CACHE_SIZE> <ASSOC> <REPLACEMENT> <WB> <TRACE_FILE>`

### Arguments

CACHE_SIZE: The cache size in Bytes

ASSOC: The associativity of the cache

REPLACEMENT: Replacement Policy, options below
- LRU = 0 (Least Recently Used)
- FIFO = 1 (First In First Out)

WB: Write policy, options below
- Write-through = 0
- Write-back = 1

TRACE_FILE: Path to the file with your cache trace