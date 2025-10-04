#include <cstddef> // for size_t
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <windows.h>
#include <thread>
#pragma once

using namespace std;

struct address_space
{
    char *start_address; // base of the whole address space
    size_t size;         // total size

    // Text (code) section
    char *text_sec;
    size_t text_sec_size;

    // Data section
    char *data_sec;
    size_t data_sec_size;

    // BSS (uninitialized globals)
    char *bss_sec;
    size_t bss_sec_size;

    // Heap (dynamic memory)
    char *heap;
    size_t heap_size;

    // Stack
    char *stack;
    size_t stack_size;

    // Memory-mapped region
    char *mmap_region;
    size_t mmap_size;

    char *not_used;
    size_t not_used_sec_size;

    address_space(int total_size)
    {
        size = total_size;

        text_sec_size = static_cast<size_t>(size * 10.0 / 100);
        data_sec_size = static_cast<size_t>(size * 10.0 / 100);
        bss_sec_size = static_cast<size_t>(size * 10.0 / 100);
        heap_size = static_cast<size_t>(size * 30.0 / 100);
        stack_size = static_cast<size_t>(size * 10.0 / 100);
        mmap_size = static_cast<size_t>(size * 20.0 / 100);
        not_used_sec_size = size - (text_sec_size + data_sec_size + bss_sec_size + heap_size + stack_size + mmap_size);

        start_address = new char[size];

        text_sec = start_address;

        data_sec = text_sec + text_sec_size;

        bss_sec = data_sec + data_sec_size;

        heap = bss_sec + bss_sec_size;

        stack = heap + heap_size;

        mmap_region = stack + stack_size;

        not_used = mmap_region + mmap_size;
    }

    ~address_space()
    {
        delete[] start_address;
    }

    void copy_section(char *dest, const char *src, size_t n)
    {
        if (dest && src && n > 0)
            memcpy(dest, src, n);
    }
};

struct cpu_regs
{
#ifdef _WIN64
    DWORD64 rax, rbx, rcx, rdx, rsi, rdi, rbp, rsp, rip;
    M128A xmm[16]; // 128-bit SIMD registers
#else
    DWORD eax, ebx, ecx, edx, esi, edi, ebp, esp, eip;
    BYTE extended[512]; // raw FPU/SSE data (legacy x86 format)
#endif
    WORD fpuControlWord;
    WORD fpuStatusWord;
    WORD mxcsr;
    void print_regs()
    {
        cout << rax << endl;
        cout << rbx << endl;
        cout << rcx << endl;
        cout << rdx << endl;
        cout << rsi << endl;
        cout << rdi << endl;
        cout << rbp << endl;
        cout << rsp << endl;
        cout << rip << endl;
    }
};

bool save_cpu_state(HANDLE thread_handle, cpu_regs *out)
{
    CONTEXT ctx;
    ctx.ContextFlags = CONTEXT_FULL;

    if (SuspendThread(thread_handle) == -1)
    {
        return false;
    }
    if (!GetThreadContext(thread_handle, &ctx))
    {

        return false;
    }

#ifdef _WIN64
    out->rax = ctx.Rax;
    out->rbx = ctx.Rbx;
    out->rcx = ctx.Rcx;
    out->rdx = ctx.Rdx;
    out->rsi = ctx.Rsi;
    out->rdi = ctx.Rdi;
    out->rbp = ctx.Rbp;
    out->rsp = ctx.Rsp;
    out->rip = ctx.Rip;

    // Copy XMM registers individually
    out->xmm[0] = ctx.Xmm0;
    out->xmm[1] = ctx.Xmm1;
    out->xmm[2] = ctx.Xmm2;
    out->xmm[3] = ctx.Xmm3;
    out->xmm[4] = ctx.Xmm4;
    out->xmm[5] = ctx.Xmm5;
    out->xmm[6] = ctx.Xmm6;
    out->xmm[7] = ctx.Xmm7;
    out->xmm[8] = ctx.Xmm8;
    out->xmm[9] = ctx.Xmm9;
    out->xmm[10] = ctx.Xmm10;
    out->xmm[11] = ctx.Xmm11;
    out->xmm[12] = ctx.Xmm12;
    out->xmm[13] = ctx.Xmm13;
    out->xmm[14] = ctx.Xmm14;
    out->xmm[15] = ctx.Xmm15;

    out->fpuControlWord = ctx.FltSave.ControlWord;
    out->fpuStatusWord = ctx.FltSave.StatusWord;
    out->mxcsr = ctx.FltSave.MxCsr;

#else
    out->eax = ctx.Eax;
    out->ebx = ctx.Ebx;
    out->ecx = ctx.Ecx;
    out->edx = ctx.Edx;
    out->esi = ctx.Esi;
    out->edi = ctx.Edi;
    out->ebp = ctx.Ebp;
    out->esp = ctx.Esp;
    out->eip = ctx.Eip;
    memcpy(out->extended, ctx.ExtendedRegisters, sizeof(ctx.ExtendedRegisters));
#endif

    return true;
}

bool load_cpu_state(HANDLE thread_handle, cpu_regs *in)
{
    CONTEXT ctx;
    ctx.ContextFlags = CONTEXT_FULL;

    if (SuspendThread(thread_handle) == -1)
    {
        return false;
    }
    if (!GetThreadContext(thread_handle, &ctx))
    {

        return false;
    }

#ifdef _WIN64
    ctx.Rax = in->rax;
    ctx.Rbx = in->rbx;
    ctx.Rcx = in->rcx;
    ctx.Rdx = in->rdx;
    ctx.Rsi = in->rsi;
    ctx.Rdi = in->rdi;
    ctx.Rbp = in->rbp;
    ctx.Rsp = in->rsp;
    ctx.Rip = in->rip;

    ctx.Xmm0 = in->xmm[0];
    ctx.Xmm1 = in->xmm[1];
    ctx.Xmm2 = in->xmm[2];
    ctx.Xmm3 = in->xmm[3];
    ctx.Xmm4 = in->xmm[4];
    ctx.Xmm5 = in->xmm[5];
    ctx.Xmm6 = in->xmm[6];
    ctx.Xmm7 = in->xmm[7];
    ctx.Xmm8 = in->xmm[8];
    ctx.Xmm9 = in->xmm[9];
    ctx.Xmm10 = in->xmm[10];
    ctx.Xmm11 = in->xmm[11];
    ctx.Xmm12 = in->xmm[12];
    ctx.Xmm13 = in->xmm[13];
    ctx.Xmm14 = in->xmm[14];
    ctx.Xmm15 = in->xmm[15];

    ctx.FltSave.ControlWord = in->fpuControlWord;
    ctx.FltSave.StatusWord = in->fpuStatusWord;
    ctx.FltSave.MxCsr = in->mxcsr;

#else
    ctx.Eax = in->eax;
    ctx.Ebx = in->ebx;
    ctx.Ecx = in->ecx;
    ctx.Edx = in->edx;
    ctx.Esi = in->esi;
    ctx.Edi = in->edi;
    ctx.Ebp = in->ebp;
    ctx.Esp = in->esp;
    ctx.Eip = in->eip;
    memcpy(ctx.ExtendedRegisters, in->extended, sizeof(ctx.ExtendedRegisters));
#endif

    bool ok = SetThreadContext(thread_handle, &ctx);

    return ok;
}
