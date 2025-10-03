#include "memory.hpp"

int main()
{
    cpu_regs *regs = new cpu_regs;

    load_cpu_state(regs);
    regs->print_regs();
    save_cpu_state(regs);
    return 0;
}
