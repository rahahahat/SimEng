import sst

DEBUG_L1 = 10
DEBUG_MEM = 0
DEBUG_LEVEL = 10

clw = "64"

# Define the simulation components
cpu = sst.Component("core", "sstsimeng.simengcore")
cpu.addParams({
    "simeng_config_path": "/home/rahat/mphil-project/SimEng/configs/DEMO_RISCV.yaml",
    "executable_path": "/home/rahat/mphil-project/SimEng/progs/prog_tsvc2101_b",
    "executable_args": "",
    "clock" : "1GHz",
    "max_addr_memory": 2*1024*1024*1024-1,
    "cache_line_width": clw,
    "source": "",
    "assemble_with_source": False,
    "heap": "",
    "debug": False
})

iface = cpu.setSubComponent("memory", "memHierarchy.standardInterface")

l1cache = sst.Component("l1cache.mesi", "memHierarchy.Cache")
l1cache.addParams({
      "access_latency_cycles" : "5",
      "cache_frequency" : "2Ghz",
      "replacement_policy" : "nmru",
      "coherence_protocol" : "MESI",
      "associativity" : "4",
      "cache_line_size" : clw,
      "debug" : DEBUG_L1,
      "debug_level" : DEBUG_LEVEL,
      "L1" : "1",
      "cache_size" : "200KiB"
})

# Explicitly set the link subcomponents instead of having cache figure them out based on connected port names
l1toC = l1cache.setSubComponent("cpulink", "memHierarchy.MemLink")
l1toM = l1cache.setSubComponent("memlink", "memHierarchy.MemLink")

# Memory controller
memctrl = sst.Component("memory", "memHierarchy.MemController")
memctrl.addParams({
    "clock" : "1GHz",
    "request_width" : "64",
    "debug" : DEBUG_MEM,
    "debug_level" : DEBUG_LEVEL,
    "addr_range_end" : 2*1024*1024*1024-1,
})
Mtol1 = memctrl.setSubComponent("cpulink", "memHierarchy.MemLink")

# Memory model
memory = memctrl.setSubComponent("backend", "memHierarchy.simpleMem")
memory.addParams({
      "access_time" : "1ns",
      "mem_size" : "2GiB",
      "request_width": "64"
})

# Define the simulation links
link_cpu_cache_link = sst.Link("link_cpu_cache_link")
link_cpu_cache_link.connect( (iface, "port", "100ps"), (l1toC, "port", "100ps") )
link_mem_bus_link = sst.Link("link_mem_bus_link")
link_mem_bus_link.connect( (l1toM, "port", "50ps"), (Mtol1, "port", "50ps") )

sst.setStatisticLoadLevel(7)
sst.setStatisticOutput("sst.statOutputConsole")
sst.enableStatisticsForComponentName("l1cache.mesi", ["GetS_recv", "GetX_recv", "Write_recv", "GetSX_recv", "PutM_recv", "PutX_recv","PutS_recv", "PutE_recv" ,"TotalEventsReceived","CacheHits", "CacheMisses", "eventSent_GetS", "eventSent_GetX", "eventSent_GetSX", "eventSent_Write", "eventSent_PutS", "eventSent_PutM", "eventSent_PutE", "eventSent_Put", "eventSent_Get"])
# sst.enableStatisticsForComponentName("a64fx.l2cache", ["GetS_recv", "GetX_recv", "Write_recv", "GetSX_recv", "PutM_recv", "PutX_recv","PutS_recv", "PutE_recv" ,"TotalEventsReceived","CacheHits", "CacheMisses", "eventSent_GetS", "eventSent_GetX", "eventSent_GetSX", "eventSent_Write", "eventSent_PutS", "eventSent_PutM", "eventSent_PutE", "eventSent_Put", "eventSent_Get"])

