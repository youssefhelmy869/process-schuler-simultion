#include <vector>
#include "process.hpp"
#include "memory.hpp"
#include "cpu.hpp"
#include <map>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <thread>
#include <windows.h>

#pragma once

using namespace std;

int randomFiveDigit()
{
    // Seed only once using a static flag
    static bool seeded = false;
    if (!seeded)
    {
        std::srand(std::time(nullptr));
        seeded = true;
    }

    // Always return a 5-digit number
    return 10000 + (std::rand() % 90000);
}

class process_schuler
{
public:
    vector<process_control_block *> process_list;
    map<string, int> process_table;
    vector<process_control_block *> waiting_list;
    thread cpu_thread;
    processer_core cpu;
    process_control_block *running_process;

    process_control_block *create_process(
        string process_name, int size = 1024,
        process_control_block::priority para_priorty = process_control_block::priority::SYSTEM,

        int process_id = 00000)
    {
        // make process id
        int temp_process_id = randomFiveDigit();
        bool unique = false;
        while (unique == false)
        {
            unique = true;
            for (auto &pair : process_table)
            {
                if (pair.second == temp_process_id)
                {
                    temp_process_id = randomFiveDigit();
                    unique = false;
                    break;
                }
            }
        }
        process_id = temp_process_id;
        process_control_block *temp_ptr = new process_control_block(size, para_priorty, process_name, process_id);
        temp_ptr->state = process_control_block::process_state::NEW;

        process_table[process_name] = process_id;

        process_list.push_back(temp_ptr);
        return temp_ptr;
    }
    void debug()
    {
        cout << "============================================================================================================================" << endl;
        cout << "process list" << endl;
        int process_list_size = process_list.size();
        for (int i = 0; i < process_list_size; ++i)
        {
            cout << "Process number " << i << " is at address " << process_list[i] << endl;
        }
        cout << "==================Proccess table==================" << endl;
        for (auto pair : process_table)
        {
            cout << "Process " << pair.first << " has ID of " << pair.second << endl;
        }
        cout << "============================================================================================================================" << endl;
        for (process_control_block *ptr : process_list)
        {
            cout << "Process " << ptr->process_name << " has ID " << ptr->process_id << " at address " << ptr << " and has a proirty of " << ptr->print_priority(ptr->importance) << endl;
            ;
        }
    }
    void run_a_process()
    {
        process_control_block *process_to_run = waiting_list[0];
        running_process = process_to_run;
        cpu_thread = thread(&processer_core::run_commands, &cpu, process_to_run);

        //
    }
    void inrupt_thread(thread &para_thread) // intrupt thread
    {
        HANDLE handle = (HANDLE)para_thread.native_handle(); // make it a native thread
        save_cpu_state_to_process(handle, running_process);

        DWORD exitCode;

        if (GetExitCodeThread(handle, &exitCode))
        {
            cout << "thread termaited susfully" << endl;
        }

        para_thread.detach();
        TerminateThread(handle, 0);
        CloseHandle(handle);
    }
    void save_cpu_state_to_process(HANDLE handle_thread, process_control_block *process)
    {
        DWORD suspended = SuspendThread(handle_thread);
        save_cpu_state(handle_thread, &process->cpu_state);
    }
};