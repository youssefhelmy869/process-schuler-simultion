// memory.hpp
#include <string>
#include <iostream>

struct address_space
{
    address_space(int size = 1024)
    { // size in bytes
        char *start_address = (char *)malloc(size);
        int data_section_size = size * (50 / 100);
        int test_section_size = size * (50 / 100);
        char *start_address_data = start_address;
        char *start_address_text = start_address + data_section_size;
    }
};
