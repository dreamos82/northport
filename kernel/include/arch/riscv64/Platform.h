#include <stdint.h>
#include <stddef.h>
#include <Maths.h>
#include <arch/riscv64/Sbi.h>

//risc-v has multiple names per register, this allows us to use any of the given names.
#define REG_ALIAS(a, b) union { uint64_t a; uint64_t b; };

namespace Npk
{
    struct TrapFrame
    {
        REG_ALIAS(x1, ra)
        REG_ALIAS(x2, sp)
        REG_ALIAS(x3, gp)
        REG_ALIAS(x4, tp)
        REG_ALIAS(x5, t0)
        REG_ALIAS(x6, t1)
        REG_ALIAS(x7, t2)
        union { uint64_t x8; uint64_t s0; uint64_t fp; };
        REG_ALIAS(x9, s1)
        REG_ALIAS(x10, a0)
        REG_ALIAS(x11, a1)
        REG_ALIAS(x12, a2)
        REG_ALIAS(x13, a3)
        REG_ALIAS(x14, a4)
        REG_ALIAS(x15, a5)
        REG_ALIAS(x16, a6)
        REG_ALIAS(x17, a7)
        REG_ALIAS(x18, s2)
        REG_ALIAS(x19, s3)
        REG_ALIAS(x20, s4)
        REG_ALIAS(x21, s5)
        REG_ALIAS(x22, s6)
        REG_ALIAS(x23, s7)
        REG_ALIAS(x24, s8)
        REG_ALIAS(x25, s9)
        REG_ALIAS(x26, s10)
        REG_ALIAS(x27, s11)
        REG_ALIAS(x28, t3)
        REG_ALIAS(x29, t4)
        REG_ALIAS(x30, t5)
        REG_ALIAS(x31, t6)

        uint64_t vector;
        uint64_t ec;
        uint64_t sepc;

        struct
        {
            uint32_t spp; //0 = user, 1 = supervisor
            uint32_t spie;
        } flags;
    };

    static_assert(sizeof(TrapFrame) == 280, "Riscv64 TrapFrame size changed, update assembly sources.");

    constexpr inline size_t PageSize = 0x1000;
    constexpr inline size_t IntVectorAllocBase = 0x10;
    constexpr inline size_t IntVectorAllocLimit = 0xFF; //we can theoritically have up to SXLEN/2 interrupts.

    #define ReadCsr(csr) \
    ({ uint64_t value; asm volatile("csrr %0, " csr : "=r"(value) :: "memory"); value; })

    #define WriteCsr(csr, v) \
    ({ uint64_t value = (v); asm volatile("csrw " csr ", %0" :: "r"(value) : "memory"); })

    #define SetCsrBits(csr, bits) \
    ({ uint64_t value = (bits); asm volatile("csrs " csr ", %0" :: "r"(value) : "memory"); })

    #define ClearCsrBits(csr, bits) \
    ({ uint64_t value = (bits); asm volatile("csrc " csr ", %0" :: "r"(value) : "memory"); })
    
    [[gnu::always_inline]]
    inline void Wfi()
    {
        asm("wfi");
    }

    [[gnu::always_inline]]
    inline bool InterruptsEnabled()
    {
        return ReadCsr("sstatus") & (1 << 1);
    }

    [[gnu::always_inline]]
    inline void EnableInterrupts()
    {
        SetCsrBits("sstatus", 1 << 1);
    }

    [[gnu::always_inline]]
    inline void DisableInterrupts()
    {
        ClearCsrBits("sstatus", 1 << 1);
    }

    [[gnu::always_inline]]
    inline void AllowSumac()
    {
        SetCsrBits("sstatus", 1 << 18);
    }

    [[gnu::always_inline]]
    inline void BlockSumac()
    {
        ClearCsrBits("sstatus", 1 << 18);
    }

    [[gnu::always_inline]]
    inline void HintSpinloop()
    {
        asm(".int 0x0100000F");
    }

    inline void InitTrapFrame(TrapFrame* frame, uintptr_t stack, uintptr_t entry, void* arg, bool user)
    {
        frame->flags.spie = 1;
        frame->flags.spp = user ? 0 : 1;
        frame->a0 = (uintptr_t)arg;
        frame->sepc = entry;
        frame->sp = sl::AlignDown(stack, 8);
        frame->fp = 0;
        frame->vector = (uint64_t)-1; //not necessary, helps with debugging
    }

    [[gnu::always_inline]]
    inline uintptr_t GetReturnAddr(size_t level)
    {
        struct Frame
        {
            Frame* next;
            uintptr_t retAddr;
        };

        //NOTE: this is only possible because we compile with -fno-omit-framepointer.
        //otherwise we'd have to make use of EH/unwind metadata (no thanks).
        Frame* current = reinterpret_cast<Frame*>((uintptr_t)__builtin_frame_address(0) - 16);
        for (size_t i = 0; i <= level; i++)
        {
            if (current == nullptr)
                return 0;
            if (i == level)
                return current->retAddr;
            current = current->next - 1;
        }
        return 0;
    }
    
    [[gnu::always_inline]]
    inline void SendIpi(size_t dest)
    {
        //SBI spec doesn't specify SXLEN-alignment, but some platforms expect it.
        //so we do it anyway, just in case.
        SbiSendIpi(1ul << (dest % 64), dest / 64);
    }

    [[gnu::always_inline]]
    inline uintptr_t MsiAddress(size_t core, size_t vector)
    {
        //TODO: imsic driver
        (void)core; (void)vector;
        return -1ul;
    }

    [[gnu::always_inline]]
    inline uintptr_t MsiData(size_t core, size_t vector)
    {
        (void)core; (void)vector;
        return -1ul;
    }

    [[gnu::always_inline]]
    inline void MsiExtract(uintptr_t addr, uintptr_t data, size_t& core, size_t& vector)
    {
        (void)addr; (void)data;
        core = vector = -1ul;
    }

    struct CoreLocalInfo;

    [[gnu::always_inline]]
    inline CoreLocalInfo& CoreLocal()
    {
        return *reinterpret_cast<CoreLocalInfo*>(ReadCsr("sscratch"));
    }

    [[gnu::always_inline]]
    inline bool CoreLocalAvailable()
    {
        return ReadCsr("sscratch") != 0;
    }
}
