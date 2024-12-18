/*
 * Kamek Standard Library
 * Wii game patching engine
 * (c) Treeki 2010-2018
 */

#ifndef __KAMEK_H
#define __KAMEK_H
#include "types.hpp"


#pragma cpp1x on
#pragma gcc_extensions

#include <include/c_stdlib.h>
#include <include/c_math.h>
#include <include/c_stdio.h>
#include <include/c_wchar.h>
#include <include/c_string.h>
#include "hooks.hpp"

#include <game/Math/Matrix.hpp>
#include <game/Math/Vector.hpp>
#include <core/rvl/os/OS.hpp>
//just for usability in other structs, so that you don't have to type EGG::type every time

//#define WORLDWIDE_TEST

#define SetupHandler(handler, c, function) handler.subject = this; handler.ptmf = static_cast<c> (function);

template <class Subject, typename Ret>
struct Ptmf_0A{
    int this_delta;
    int vtableOffset;
    Ret (Subject::*functionPointer)();
};//total size 0xc

template <class Subject, typename Ret, typename A1>
struct Ptmf_1A{
    int this_delta;
    int vtableOffset;
    Ret (Subject::*functionPointer)(A1);
};//total size 0xc

template <class Subject, typename Ret, typename A1, typename A2>
struct Ptmf_2A{
    int this_delta;
    int vtableOffset;
    Ret (Subject::*functionPointer)(A1, A2);
};//total size 0xc

template <class Subject, typename Ret, typename A1, typename A2, typename A3>
struct Ptmf_3A{
    int this_delta;
    int vtableOffset;
    Ret (Subject::*functionPointer)(A1, A2, A3);
};//total size 0xc

template <typename Ret>
struct PtmfHolderBase_0A {
    virtual Ret operator()() = 0;
};

template <class Subject, typename Ret>
struct PtmfHolder_0A : PtmfHolderBase_0A<Ret> {
    Subject * subject;
    Ret (Subject::*ptmf)();
    virtual Ret operator()() {
        return (subject->*ptmf)();
    }
};

template <typename Ret, typename A1>
struct PtmfHolderBase_1A {
    virtual Ret operator()(A1 a1) = 0;
};

template <class Subject, typename Ret, typename A1>
struct PtmfHolder_1A : PtmfHolderBase_1A<Ret, A1> {
    Subject * subject;
    Ret (Subject::*ptmf)(A1);
    virtual Ret operator()(A1 a1) {
        return (subject->*ptmf)(a1);
    }
};

template <typename Ret, typename A1, typename A2>
struct PtmfHolderBase_2A {
    virtual Ret operator()(A1 a1, A2 a2) = 0;
};
template <class Subject, typename Ret, typename A1, typename A2>
struct PtmfHolder_2A : PtmfHolderBase_2A<Ret, A1, A2> {
    Subject * subject;
    Ret (Subject::*ptmf)(A1, A2);
    
    virtual Ret operator()(A1 a1, A2 a2) {
        return (subject->*ptmf)(a1, a2);
    }
};

template <typename Ret, typename A1, typename A2, typename A3>
struct PtmfHolderBase_3A {
    virtual Ret operator()(A1 a1, A2 a2, A3 a3) = 0;
};
template <class Subject, typename Ret, typename A1, typename A2, typename A3>
struct PtmfHolder_3A : PtmfHolderBase_3A<Ret, A1, A2, A3> {
    Subject * subject;
    Ret (Subject::*ptmf)(A1, A2, A3);
    virtual Ret operator()(A1 a1, A2 a2, A3 a3) {
        return (subject->*ptmf)(a1, a2, a3);
    }
};

#define DEBUG_COSMOS
#define CosmosLog(f, ...) OSReport("[Cosmos:%s:%d] " f, __FILE__, __LINE__, ##__VA_ARGS__)
#define CosmosError(f, ...) OSReport("[Cosmos Error:%s:%d] " f, __FILE__, __LINE__, ##__VA_ARGS__)

#define COSMOS_ASSERT(c) if(!(c)) { OSReport("[Cosmos Assert Failed:%s:%d]" #c, __FILE__, __LINE__); u32 _blackColor = 0; u32 _whiteColor = ~0; OSFatal(&_whiteColor, &_blackColor, "Assertion Failed! " #c); }
#define COSMOS_ASSERT_NOT_NULL(c) COSMOS_ASSERT(c != nullptr);

#define COSMOS_SECURITY

#if defined(COSMOS_SECURITY) && !defined(DEBUG_COSMOS)
#define COSMOS_HANG() Cosmos::Security::KillAllStack()
#else
#define COSMOS_HANG() for(;;){}
#endif

#define __COMPILER_VERSION__ "4305_224"
#define __COSMOS_VERSION__ "v0.1.5"

class MenuLoadHook {
private:
    typedef void (Func)();
    Func *mFunc;
    MenuLoadHook * mNext;

    static MenuLoadHook * sHooks;

public:
    MenuLoadHook(Func * f) {
        mNext = sHooks;
        sHooks = this;
        mFunc = f;
    }

    static void exec() {
        for (MenuLoadHook * p = sHooks; p; p = p->mNext)
            p->mFunc();
    }
};

static void EndStrapScene(u32 scene);

class StrapEndHook {
    private:
        typedef void (Func)();
        Func *mFunc;
        StrapEndHook * mNext;

        static StrapEndHook * sHooks;
    public:
        StrapEndHook(Func * f) {
            mNext = sHooks;
            sHooks = this;
            mFunc = f;
        }

        static void exec(u32 scene) {
            for (StrapEndHook * p = sHooks; p; p = p->mNext)
                p->mFunc();
            EndStrapScene(scene);
        }
};

class LeCodeLoadHook {
private:
    typedef void (Func)();
    Func *mFunc;
    LeCodeLoadHook * mNext;

    static LeCodeLoadHook * sHooks;

public:
    LeCodeLoadHook(Func * f) {
        mNext = sHooks;
        sHooks = this;
        mFunc = f;
    }

    static void exec() {
        for (LeCodeLoadHook * p = sHooks; p; p = p->mNext)
            p->mFunc();
    }
};

class RaceLoadHook {
private:
    typedef void (Func)();
    Func *mFunc;
    RaceLoadHook * mNext;

    static RaceLoadHook * sHooks;

public:
    RaceLoadHook(Func * f) {
        mNext = sHooks;
        sHooks = this;
        mFunc = f;
    }

    static void exec() {
        for (RaceLoadHook * p = sHooks; p; p = p->mNext)
            p->mFunc();
    }
};

class RacePostLoadHook {
private:
    typedef void (Func)();
    Func *mFunc;
    RacePostLoadHook * mNext;

    static RacePostLoadHook * sHooks;

public:
    RacePostLoadHook(Func * f) {
        mNext = sHooks;
        sHooks = this;
        mFunc = f;
    }

    static void exec() {
        for (RacePostLoadHook * p = sHooks; p; p = p->mNext)
            p->mFunc();
    }
};

class RaceFrameHook {
private:
    typedef void (Func)();
    Func *mFunc;
    RaceFrameHook * mNext;

    static RaceFrameHook * sHooks;

public:
    RaceFrameHook(Func * f) {
        mNext = sHooks;
        sHooks = this;
        mFunc = f;
    }

    static void exec() {
        for (RaceFrameHook * p = sHooks; p; p = p->mNext) p->mFunc();
    }
};
typedef int (RepFunc)();
#define REPLACED(func, type, ...) ((type (*)(...))&func.instruction1)(__VA_ARGS__);
#define REPLACE(name, addr, func) static ReplaceFunction rp##name(addr, (RepFunc *) &func);


class ReplaceFunction {
public:


    RepFunc *mFunc;
    static ReplaceFunction * sHooks;
    ReplaceFunction * mNext;
    u32 address;
    u32 instruction1;
    u32 instruction2;

    ReplaceFunction(u32 addr, RepFunc* f) {
        mFunc = f;
        address = addr;
        mNext = sHooks;
        sHooks = this;
    }
    ReplaceFunction(void* addr, RepFunc* f) {
        mFunc = f;
        address = (u32)addr;
        mNext = sHooks;
        sHooks = this;
    }

    static void exec()
    {
        for(ReplaceFunction * p = sHooks; p != nullptr; p = p->mNext)
        {
            p->instruction1 = *(u32 *)p->address;
            u32 offset = ((u32)p->address)-(u32)&p->instruction1;
            u32 command = 0x48000000 | (offset & 0x03FFFFFF);
            p->instruction2 = command;

            offset = ((u32)p->mFunc)-(u32)p->address;
            command = 0x48000000 | (offset & 0x03FFFFFF);
            *(u32 *)p->address = command;
        }
    }
};


enum HookPriority{
    LOW,
    MEDIUM,
    HIGH,
    FIRST,
    LINK,
};

class BootHook {
private:
    typedef void (Func)();
    Func *mFunc;
    BootHook * mNext;
    HookPriority mPriority;
    static BootHook * sHooks;

public:
    BootHook(Func * f, HookPriority prio) {
        mNext = sHooks;
        sHooks = this;
        mFunc = f;
        mPriority = prio;
    }

    static void exec() {

        OSReport("[Cosmos] Cosmos %s Loaded (0x%s)\n", __COSMOS_VERSION__, __COMPILER_VERSION__);
        OSReport("[Cosmos Module] FatFs R0.15\n");

        ReplaceFunction::exec();

        for (BootHook * p = sHooks; p; p = p->mNext)
            if(p->mPriority == LINK) p->mFunc();

        for (BootHook * p = sHooks; p; p = p->mNext)
            if(p->mPriority == FIRST) p->mFunc();

        for (BootHook * p = sHooks; p; p = p->mNext)
            if(p->mPriority == HIGH) p->mFunc();

        for (BootHook * p = sHooks; p; p = p->mNext)
            if(p->mPriority == MEDIUM) p->mFunc();

        for (BootHook * p = sHooks; p; p = p->mNext)
            if(p->mPriority == LOW) p->mFunc();
    }
};



#endif
