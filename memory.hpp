#include <cstddef> // for size_t
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <windows.h>
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
    DWORD eax, ebx, ecx, edx, esi, edi, ebp, esp, eip;
    void print_regs()
    {
        cout << eax << endl;
        cout << ebx << endl;
        cout << ecx << endl;
        cout << edx << endl;
        cout << esi << endl;
        cout << edi << endl;
        cout << ebp << endl;
        cout << esp << endl;
    }
};

void save_cpu_state(cpu_regs *cpu_state)
{

    asm("mov %%eax, %0\n\t"
        "mov %%ebx, %1\n\t"
        "mov %%ecx, %2\n\t"
        "mov %%edx, %3\n\t"
        "mov %%esi, %4\n\t"
        "mov %%edi, %5\n\t"
        "mov %%ebp, %6"

        : "=r"(cpu_state->eax),
          "=r"(cpu_state->ebx),
          "=r"(cpu_state->ecx),
          "=r"(cpu_state->edx),
          "=r"(cpu_state->esi),
          "=r"(cpu_state->edi),
          "=r"(cpu_state->ebp)

    );
    asm volatile("mov %%esp, %0" : "=r"(cpu_state->esp));
}

void load_cpu_state(cpu_regs *cpu_state)
{

    asm("movl %0, %%eax\n\t"
        "movl %1, %%ebx\n\t"
        "movl %2, %%ecx\n\t"
        "movl %3, %%edx\n\t"
        "movl %4, %%esi\n\t"
        "movl %5, %%edi\n\t"

        :
        : "r"(cpu_state->eax), "r"(cpu_state->ebx), "r"(cpu_state->ecx), "r"(cpu_state->edx), "r"(cpu_state->esi), "r"(cpu_state->edi)
        : "%eax", "%ebx", "%ecx", "%edx", "%esi", "%edi");

    cpu_state->ebp = (uintptr_t)__builtin_frame_address(0);
}
