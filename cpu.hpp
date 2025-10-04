#include <iostream>
#include <windows.h>
#include <cstring>
#include <atomic>
#include "process.hpp"
#pragma once

class processer_core
{
public:
    void run_commands(char *code, size_t size)
    {
        // exec_mem is a void pointer to allocated memory that will hold executable code
        void *exec_mem = VirtualAlloc(
            nullptr,                  // Let Windows choose the memory address
            size,                     // Number of bytes to allocate
            MEM_COMMIT | MEM_RESERVE, // Reserve and commit memory pages
            PAGE_EXECUTE_READWRITE    // Memory can be read, written, and executed
        );
        // VirtualAlloc syntax:
        // void* VirtualAlloc(LPVOID lpAddress, SIZE_T dwSize, DWORD flAllocationType, DWORD flProtect);
        // lpAddress=nullptr: OS decides where to allocate
        // dwSize=size: number of bytes to allocate
        // flAllocationType: combination of flags to allocate and commit memory
        // flProtect: memory protection flags; here we need execute permission

        if (!exec_mem)
        { // Check if allocation failed
            std::cerr << "VirtualAlloc failed\n";
            return; // Exit function if allocation failed
        }

        // Copy machine code bytes into the allocated executable memory
        std::memcpy(exec_mem, code, size);
        // std::memcpy(dest, src, n) copies n bytes from src to dest
        // Here, exec_mem is the destination (memory we allocated)
        // code is the source (char* with instructions)
        // size is the number of bytes to copy

        // Cast memory to a function pointer type
        void (*func)() = (void (*)())exec_mem;
        // Syntax explanation:
        // - void (*)() is a pointer to a function returning void and taking no parameters
        // - (void (*)())exec_mem casts the void* to this function pointer type
        // - func now points to memory that the CPU will execute as a function

        func(); // Call the function
        // This tells the CPU to start executing instructions at exec_mem
        // Any instructions in that memory will run, just like a normal function call

        // Free the allocated memory
        VirtualFree(exec_mem, 0, MEM_RELEASE);
        // Syntax: BOOL VirtualFree(LPVOID lpAddress, SIZE_T dwSize, DWORD dwFreeType);
        // lpAddress: address of memory to free
        // dwSize: 0 with MEM_RELEASE releases the entire region
        // dwFreeType: MEM_RELEASE indicates release memory back to OS
    }

    void run(process_control_block *process, std::atomic<bool> &stop_flag)
    {
        process->state = process_control_block::process_state::RUNNING;

        run_commands(process->ptr_address_space->text_sec, process->ptr_address_space->text_sec_size);
    }
};
