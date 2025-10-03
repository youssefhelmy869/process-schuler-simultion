#include <vector>
#include "process.hpp"
#include "memory.hpp"
#include <map>
#include <iostream>
#include <cstdlib>
#include <ctime>

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
    vector<process_control_block> process_list;
    map<string, int> process_table;

    process_control_block *create_process(
        string process_name, int size = 1024,
        process_control_block::priority para_priorty = process_control_block::priority::SYSTEM,

        int process_id = 00000)
    {
        // make process id
        int temp_process_id = randomFiveDigit();
        process_control_block *temp_ptr = new process_control_block(size, para_priorty, process_name, process_id);
        return temp_ptr;
    }
};