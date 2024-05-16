#ifndef _MODULE_LINKER_
#define _MODULE_LINKER_

#include <kamek.hpp>
#include <core/egg/Heap.hpp>
#include <core/egg/Disposer.hpp>
#include <core/rvl/os/module.hpp>

class Linker {
public:
    virtual ~Linker();
    OSModuleHeader* GetModule(int index) { return index < 2 ? modules[index] : (OSModuleHeader*) nullptr; }

private:
    EGG::Heap* heaps[2];
    OSModuleHeader* modules[2];
    u8 unknown[0x2c-0x24];
};

class ModuleLinker: public EGG::Disposer {
public:
    static ModuleLinker* CreateStaticInstance(); // 0x80009ff0
    static ModuleLinker* GetStaticInstance() { return sInstance; }
    static void DumpModuleInfo(OSModuleHeader* moduleInfo); //8000a440

    OSModuleHeader* GetModule(int index) { return linker.GetModule(index); }
    
private:
    static ModuleLinker* sInstance; //0x80385ff8
    Linker linker;
};

static_assert(sizeof(ModuleLinker) == 0x2c, "ModuleLinker");

#endif