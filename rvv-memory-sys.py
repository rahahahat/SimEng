import sst

DEBUG_L1 = 0
DEBUG_L2 = 0
DEBUG_MEM = 0
DEBUG_LEVEL = 0


# ------------------------------------------------ Utility -------------------------------------------

def getMemoryProps(memory_size: int, si: str):
      props = {
            "start_addr": 0,
            "end_addr": 0,
            "size": ""
      }
      props["size"] = "%s%s" % (memory_size , si)
      if si == "GiB":
            props["end_addr"] = memory_size * 1024 * 1024 * 1024 - 1
      elif si == "MiB":
            props["end_addr"] = memory_size * 1024 * 1024 - 1
      elif si == "KiB":
            props["end_addr"] = memory_size * 1024 - 1
      elif si == "B":
            props["end_addr"] = memory_size - 1
      else:
            raise Exception("Unknown SI units provided to getMemoryProps")
      return props

# ------------------------------------------------ Utility -------------------------------------------



# ------------------------------------------- A64FX Properties ---------------------------------------

CLOCK = "1.8GHz"
CACHE_TYPE = "inclusive"
COHP = "MESI"
MEM_ACCESS = "144.5ns"

# ------------------------------------------- A64FX Properties ---------------------------------------


# ---------------------------------------------- Variables -------------------------------------------

memprops = getMemoryProps(8, "GiB")

# ---------------------------------------------- Variables -------------------------------------------

prog = 
exepath =
fpath =

# --------------------------------------------- SSTSimEng Core ---------------------------------------

# Using sst-info sstsimeng.simengcore to get all cache parameters, ports and subcomponent slots.
cpu = sst.Component("core", "sstsimeng.simengcore")
cpu.addParams({
    "simeng_config_path": "path to rvv-sst.yaml",
    "executable_path": exepath,
    "executable_args": "",
    "clock" : CLOCK,
    "max_addr_memory": memprops["end_addr"],
    "cache_line_width": 64,
})

# Instantiating the StandardInterface which communicates with the SST memory model.
interface = cpu.setSubComponent("memory", "memHierarchy.standardInterface")

# --------------------------------------------- SSTSimEng Core ---------------------------------------


# --------------------------------------------- L1 Cache ---------------------------------------------

# Using sst-info memHierarchy.Cache to get all cache parameters, ports and subcomponent slots.
l1cache = sst.Component("riscv.l1cache", "memHierarchy.Cache")
l1cache.addParams({
      "L1" : 1,
      "cache_type": CACHE_TYPE,
      "access_latency_cycles" : 4,
      "associativity" : 4,
      "cache_line_size" :64,
      "cache_frequency" : CLOCK,
      "cache_size" : "64KiB",
      "debug" : DEBUG_L1,
      "debug_level" : DEBUG_LEVEL,
      "coherence_protocol": COHP,
      "request_link_width": "32B",
      "response_link_width": "32B"
})
# Set MESI L1 coherence controller to the "coherence" slot
coherence_controller_l1 = l1cache.setSubComponent("coherence", "memHierarchy.coherence.mesi_l1")
# Set LRU replacement policy to the "replacement" slot.
# index=0 indicates replacement policy is for cache.
replacement_policy_l1 = l1cache.setSubComponent("replacement", "memHierarchy.replacement.lru", 0)

# --------------------------------------------- L1 Cache ---------------------------------------------


# --------------------------------------------- L2 Cache ---------------------------------------------

# Using sst-info memHierarchy.Cache to get all cache parameters, ports and subcomponent slots.
l2cache = sst.Component("riscv.l2cache", "memHierarchy.Cache")
l2cache.addParams({
      "L1" : 0,
      "cache_type": CACHE_TYPE,
      "access_latency_cycles" : 9,
      "associativity" : 8,
      "cache_line_size" : 64,
      "cache_size" : "512KiB",
      "cache_frequency" : CLOCK,
      "debug" : DEBUG_L2,
      "debug_level" : DEBUG_LEVEL,
      "coherence_protocol": COHP,
      "request_link_width": "32B",
      "response_link_width": "32B",
})
# Set MESI L2 coherence controller to the "coherence" slot
coherence_controller_l2 = l2cache.setSubComponent("coherence", "memHierarchy.coherence.mesi_inclusive")
# Set LRU replacement policy to the "replacement" slot.
# index=0 indicates replacement policy is for cache.
replacement_policy_l2 = l2cache.setSubComponent("replacement", "memHierarchy.replacement.lru", 0)

# Using sst-info memHierarchy.Cache to get all cache parameters, ports and subcomponent slots.
l3cache = sst.Component("riscv.l3cache", "memHierarchy.Cache")
l3cache.addParams({
      "L1" : 0,
      "cache_type": CACHE_TYPE,
      "access_latency_cycles" : 35,
      "associativity" : 16,
      "cache_frequency" : CLOCK,
      "cache_line_size" : 64,
      "cache_size" : "4MiB",
      "debug" : DEBUG_L2,
      "debug_level" : DEBUG_LEVEL,
      "coherence_protocol": COHP,
      "request_link_width": "32B",
      "response_link_width": "32B",
})
# Set MESI L2 coherence controller to the "coherence" slot
coherence_controller_l3 = l3cache.setSubComponent("coherence", "memHierarchy.coherence.mesi_inclusive")
# Set LRU replacement policy to the "replacement" slot.
# index=0 indicates replacement policy is for cache.
replacement_policy_l3 = l3cache.setSubComponent("replacement", "memHierarchy.replacement.lru", 0)

# --------------------------------------------- L2 Cache ---------------------------------------------


# ----------------------------------- Memory Backend & Controller -------------------------------------

memory_controller = sst.Component("a64fx.memorycontroller", "memHierarchy.MemController")
memory_controller.addParams({
      "clock": CLOCK,
      "backend.access_time": MEM_ACCESS,
      "request_width": "32B",
      "debug": DEBUG_MEM,
      "debug_level": DEBUG_LEVEL,
      "addr_range_start": memprops["start_addr"],
      "addr_range_end": memprops["end_addr"]
})

memory_backend = memory_controller.setSubComponent("backend", "memHierarchy.simpleMem")
memory_backend.addParams({
      "access_time": MEM_ACCESS,
      "mem_size": memprops["size"],
      "request_width": 32,
})

# ----------------------------------- Memory Backend & Controller -------------------------------------

sst.setStatisticLoadLevel(7)
sst.setStatisticOutput("sst.statOutputCSV", { "filepath" : fpath, "separator" : "," })
sst.enableStatisticsForComponentName("riscv.l1cache", ["TotalEventsReceived","CacheHits", "CacheMisses"])
sst.enableStatisticsForComponentName("riscv.l2cache", ["TotalEventsReceived","CacheHits", "CacheMisses"])
sst.enableStatisticsForComponentName("riscv.l3cache", ["TotalEventsReceived","CacheHits", "CacheMisses"])


# ---------------------------------------------- Links ------------------------------------------------

link_cpu_l1cache = sst.Link("link_cpu_l1cache_link")
link_cpu_l1cache.connect( (interface, "port", "0ps"), (l1cache, "high_network_0", "0ps") )
link_l1cache_l2cache = sst.Link("link_l1cache_l2cache_link")
link_l1cache_l2cache.connect( (l1cache, "low_network_0", "0ps"), (l2cache, "high_network_0", "0ps") )
link_l2cache_l3cache = sst.Link("link_l2cache_l3cache_link")
link_l2cache_l3cache.connect( (l2cache, "low_network_0", "0ps"), (l3cache, "high_network_0", "0ps") )
link_mem_bus = sst.Link("link_mem_bus_link")
link_mem_bus.connect( (l3cache, "low_network_0", "0ps"), (memory_controller, "direct_link", "0ps") )

# ---------------------------------------------- Links ------------------------------------------------
