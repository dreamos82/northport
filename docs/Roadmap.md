- Kernel:
    - [ ] Arch Layer:
        - [ ] EFI entry stub.
        - [x] Abstractions:
            - [x] Feature detection.
            - [x] Timers.
            - [x] Interrupts.
            - [x] MMU.
        - [x] x86_64:
            - [x] Core (paging, interrupts).
            - [x] Timers (LAPIC, TSC, HPET, PIT).
            - [x] FPU, SSE, AVX.
            - [x] Map cpu topology.
        - [x] riscv64:
            - [x] Core (paging, interrupts).
            - [x] Timer (SBI, sstc).
            - [ ] Map cpu topology.
            - [ ] F/D/Q and V extension support.
            - [ ] AIA (imsic, aplic) and ACLINT support.
    - [x] Core Layer:
        - [x] Logging Infrastructure:
            - [x] Fully lockless.
            - [x] Per-core buffers.
            - [ ] Runtime sinks.
            - [x] Panic sequence.
            - [x] Stack frame walker.
            - [x] Symbol lookup.
        - [x] Memory Management:
            - [x] PMM with multiple zones.
                - [x] Reclaim bootloader memory.
                - [ ] Hybrid freelist/bitmap.
                - [ ] Hotplug support.
                - [ ] NUMA-aware allocations.
            - [x] VMM, driver based.
                - [x] Anonymous memory backend.
                - [x] VFS backend.
                - [x] MMIO/kernel backend.
                - [ ] Copy-on-write capabilities.
                - [ ] Page-to-disk.
                - [ ] Usage of super-pages.
            - [x] Hybrid slab/freelist heap.
                - [x] Per-core slab caches.
                - [ ] Page heap (canary + fault versions).
                - [ ] Tracable allocations.
        - [x] Software clock:
            - [x] Hardware-derived limitations (resolution, accuracy).
            - [x] Tickless.
            - [x] Infinite expiry.
            - [ ] Continuous calibration.
            - [ ] Per-core timer support.
            - [ ] Stopwatches.
        - [x] IPI mailboxes.
        - [x] Scheduler.
            - [x] SMP-aware.
                - [ ] Topology-aware.
                - [ ] Heterogeneous processor support.
            - [x] DPCs/run level system.
            - [x] Work-stealing.
            - [x] Waitable objects.
            - [ ] Hotpluggable CPU support.
                - [ ] Power-state steering.
        - [x] Virtual filesystem.
            - [x] TempFS, backed only by ram.
            - [x] Initdisk (stored in a TempFS).
            - [x] Page-cache.
            - [ ] Ext2/3/4 driver.
            - [ ] FAT driver.
            - [ ] NTFS driver?
    - [ ] Support Layer:
        - [ ] IPC.
            - [ ] Shared memory.
            - [ ] Pipes.
        - [ ] Sanitizers:
            - [x] Undefined behaviour.
            - [ ] Address.
        - [x] Peripheral discovery.
            - [x] ACPI table parser.
            - [x] DTB parser.
            - [x] PCI enumeration.
                - [ ] PCIe.
        - [ ] Driver management:
            - [x] Scanning and loading ELF-based drivers.
            - [x] Native ABI.
            - [ ] Operation queues.
            - [ ] Example drivers in other languages.
        - [ ] Media stacks:
            - [ ] Graphics.
            - [ ] Audio.
            - [ ] Networking.
    - [ ] Interface Layer:
        - [x] Driver API binds.
        - [ ] System calls.

- Device drivers:
    - [ ] x86 jank: ps2 peripherals, bochs VGA.
    - [ ] AHCI.
    - [x] NVMe.
        - [ ] Port to new API.
    - [x] Virtio:
        - [x] Transport layer (PCI, MMIO).
        - [x] Modern and legacy version support.
        - [x] GPU.
        - [ ] Block.
        - [ ] Network.
        - [ ] Input.
        - [ ] Filesystem.
    - [ ] XHCI (usb3).
    - [ ] E1000(e) NIC.
    - [ ] AC97 audio.
    - [ ] iHD audio.

- np-syslib:
    - [ ] Containers:
        - [x] Vector.
        - [ ] Deque.
        - [x] Queue (+ lockfree variants).
        - [x] Linked list.
        - [ ] Map.
        - [x] Custom allocator support.
        - [x] Binary tree.
    - [ ] File format helpers:
        - [x] ELF64.
        - [x] Tar.
        - [ ] Qoi.
    - [x] Optional.
        - [x] Handle: ref-counted version of Optional.
        - [ ] ErrorOr, which can also carry an error code.
    - [ ] Strings:
        - [x] Basic string.
        - [ ] String builder.
        - [ ] String formatter.
        - [ ] Integer <-> string conversions.
    - [ ] Transaction framework.
    - [x] Basic locks.
    - [x] Psuedo-RNG.
    - [x] 2D primitives (vectors, rects).

- np-gfx:
    - [ ] Resource API.
        - [ ] Shared resources.
    - [ ] Generic 2D rendering API.
        - [ ] Software backend.
        - [ ] Kernel device backend.

- np-syscall:
    - [ ] C++ system call wrappers.

