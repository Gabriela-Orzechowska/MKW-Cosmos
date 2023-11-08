#include <kamek.hpp>
#include <main.hpp>

void * CheckHeapAlloc(void * alloc)
{
    if(alloc == nullptr)
        DX::Panic(__FILE__, __LINE__, "Out of Memory!");

    return alloc;
}

kmBranch(0x80229ad0, CheckHeapAlloc);
kmBranch(0x80198e34, CheckHeapAlloc);
kmBranch(0x80199600, CheckHeapAlloc);
kmBranch(0x80199ac0, CheckHeapAlloc);