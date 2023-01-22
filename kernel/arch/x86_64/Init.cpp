#include <arch/x86_64/Apic.h>
#include <arch/x86_64/Gdt.h>
#include <arch/x86_64/Idt.h>
#include <arch/Cpu.h>
#include <arch/Timers.h>
#include <arch/Platform.h>
#include <boot/LimineTags.h>
#include <boot/CommonInit.h>
#include <debug/Log.h>
#include <memory/Vmm.h>

namespace Npk
{
    void InitCore(size_t id)
    {
        //ensure we're using the kernel's pagemap.
        VMM::Kernel().MakeActive();

        BlockSumac();
        FlushGdt();
        LoadIdt();

        uint64_t cr0 = ReadCr0();
        cr0 |= 1 << 16; //set write-protect bit
        cr0 &= ~0x6000'0000; //ensure CD/NW are cleared, enabling caching for this core.
        WriteCr0(cr0);

        uint64_t cr4 = ReadCr4();
        if (CpuHasFeature(CpuFeature::Smap))
            cr4 |= 1 << 21; //SMA enable: prevent accessing user pages as the supervisor while AC is clear
        if (CpuHasFeature(CpuFeature::Smep))
            cr4 |= 1 << 20; //Prevent executing from user pages while in ring 0.
        if (CpuHasFeature(CpuFeature::Umip))
            cr4 |= 1 << 11; //prevents system store instructions in user mode (sidt/sgdt)
        if (CpuHasFeature(CpuFeature::GlobalPages))
            cr4 |= 1 << 7; //global pages: pages that are global.
        WriteCr4(cr4);

        if (CpuHasFeature(CpuFeature::NoExecute))
            WriteMsr(MsrEfer, ReadMsr(MsrEfer) | (1 << 11));

        //my stack overfloweth (alt title: maximum maintainability).
        Log("Core %lu enabled features: write-protect%s%s%s%s%s.", LogLevel::Info, 
            id,
            cr4 & (1 << 21) ? ", smap" : "",
            cr4 & (1 << 20) ? ", smep" : "",
            cr4 & (1 << 11) ? ", umip" : "",
            cr4 & (1 << 7) ? ", global-pages" : "",
            CpuHasFeature(CpuFeature::NoExecute) ? ", nx" : "");

        CoreLocalInfo* clb = new CoreLocalInfo();
        clb->id = id;
        clb->selfAddr = (uintptr_t)clb;
        clb->interruptControl = (uintptr_t)new LocalApic();
        clb->runLevel = RunLevel::Normal;
        WriteMsr(MsrGsBase, (uint64_t)clb);

        LocalApic::Local().Init();

        EnableInterrupts();
        Log("Core %lu finished core init.", LogLevel::Info, id);
    }

    void ApEntry(limine_smp_info* info)
    {
        InitCore(info->lapic_id);
        ExitApInit();
    }
}

extern "C"
{
    void DebugconWrite(const char* str, size_t length)
    {
        using namespace Npk;
        for (size_t i = 0; i < length; i++)
            Out8(PortDebugcon, str[i]);
    }
    
    void KernelEntry()
    {
        using namespace Npk;

#ifdef NP_X86_64_E9_ALLOWED
        Debug::AddEarlyLogOutput(DebugconWrite);
#endif

        WriteMsr(MsrGsBase, 0);
        InitEarlyPlatform();
        InitMemory();
        PopulateIdt();
        InitPlatform();

        IoApic::InitAll();
        InitTimers();
        InitCore(0); //BSP is always id=0 on x86_64

        if (Boot::smpRequest.response != nullptr)
        {
            for (size_t i = 0; i < Boot::smpRequest.response->cpu_count; i++)
            {
                limine_smp_info* procInfo = Boot::smpRequest.response->cpus[i];
                if (procInfo->lapic_id == Boot::smpRequest.response->bsp_lapic_id)
                    continue;
                
                procInfo->goto_address = ApEntry;
                Log("Sending bring-up request to core %u.", LogLevel::Verbose, procInfo->lapic_id);
            }
        }

        ExitBspInit();
    }
}
