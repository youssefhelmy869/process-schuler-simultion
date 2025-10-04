#include "process_schuler.hpp"

int main()
{
    process_schuler p;
    p.create_process("word");
    p.create_process("excel");
    p.create_process("vs code");
    p.debug();

    return 0;
}
