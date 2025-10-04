#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <algorithm>
#include "memory.hpp"

#pragma once

using namespace std;

struct process_control_block
{

    address_space *ptr_address_space;

    enum priority
    {
        TOP,
        MIDDLE,
        SYSTEM,
        BACKGROUD

    } importance;
    string print_priority(priority para)
    {
        if (para == priority::TOP)
        {
            return "TOP";
        }
        else if (para == priority::MIDDLE)
        {
            return "MIIDLE";
        }
        else if (para == priority::SYSTEM)
        {
            return "SYSTEM";
        }
        else if (para == priority::BACKGROUD)
        {
            return "BACKGROUND";
        }
    }

    string process_name;
    int process_id;

    enum process_state
    {
        NEW,
        READY,
        RUNNING,
        WAITING,
        TERMINATED
    } state;

    int time_to_excute;
    ;

    int sch_value = 0;
    char *program_counter;
    int remaining_quantum;

    vector<int> regs;

    char *program_couter;

    cpu_regs cpu_state;

    process_control_block(int total_size, priority para_priority, string para_process_name, int para_process_id)
    {
        ptr_address_space = new address_space(total_size);
        state = NEW;
        importance = para_priority;
        process_name = para_process_name;
        time_to_excute = (ptr_address_space->size * 2);
        process_id = para_process_id;
        // simulate 16 general-purpose registers
    }

    ~process_control_block()
    {
        delete ptr_address_space;
    }
};