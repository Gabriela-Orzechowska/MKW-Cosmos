#include <Debug/Draw/DebugDraw.hpp>
#include <Debug/Draw/PerformanceMonitor.hpp>
#include <game/Objects/ObjetHolder.hpp>
#include <game/Race/Kart/KartHolder.hpp>
#include <game/Visual/Effect/EffectMgr.hpp>

typedef void (*GXDrawSyncCallback)(u16 token);

extern void GXSetDrawSyncCallback(u32 ptr);
namespace CosmosDebug
{
    PerformanceMonitor * PerformanceMonitor::sInstance = nullptr;

    PerformanceMonitor::PerformanceMonitor() : EGG::ProcessMeter(true),
    objectBar(nw4r::ut::Color(0xff00ffff), 3.0f),
    physicsBar(nw4r::ut::Color(0x00ffffff), 4.0f),
    effectBar(nw4r::ut::Color(0xffFFffff), 5.0f)
    {
        bgBar.color = 0x000000FF;
        this->append(&objectBar);
        this->append(&physicsBar);
        this->append(&effectBar);
    }

    PerformanceMonitor * PerformanceMonitor::CreateInstance(){
        PerformanceMonitor * monitor = new PerformanceMonitor();
        PerformanceMonitor::sInstance = monitor;
        return monitor;
    }
    kmCall(0x800092cc, PerformanceMonitor::CreateInstance);
    kmWrite32(0x800092bc, 0x60000000);

    void measureCPUBegin(RKSystem * system){
        EGG::PerformanceView * view = (EGG::PerformanceView *) system->processMeter;
        view->measureBeginFrame();
        system->asyncDisplay->beginRender();
        view->measureBeginRender();
    }
    kmCall(0x80009608, measureCPUBegin);
    kmWrite32(0x8000962c, 0x60000000);

    kmWrite32(0x80238a00, 0xd03e0108);
    kmWrite32(0x802389d0, 0xd01e00e0);

    void measureEndCPU(RKSystem* system) {
        static_cast<EGG::PerformanceView*>(system->processMeter)->measureEndFrame();
    }
    kmCall(0x800097a4, measureEndCPU);

    void measureEndGPU(EGG::PerformanceView* view) {
        PerformanceMonitor * meter = (PerformanceMonitor *) RKSystem::mInstance.processMeter;

        view->setVisible(meter->visible);
        view->draw();
        view->measureEndRender();
    }
    kmCall(0x8000968c, measureEndGPU);

    void ObjectMeasurement(ObjectHolder* holder) {
        CpuMonitor * monitor = &((PerformanceMonitor *) RKSystem::mInstance.processMeter)->objectBar;
        monitor->measureBegin();
        holder->Update();
        monitor->measureEnd();
    }
    kmCall(0x80554bb8, ObjectMeasurement);

    void PhysicsMeasurement(KartHolder * holder){
        CpuMonitor * monitor = &((PerformanceMonitor *) RKSystem::mInstance.processMeter)->physicsBar;
        monitor->measureBegin();
        holder->Update();
        monitor->measureEnd();
    }
    kmCall(0x80554bf0, PhysicsMeasurement);

    void EffectMeasurement(EffectsMgr * holder){
        CpuMonitor * monitor = &((PerformanceMonitor *) RKSystem::mInstance.processMeter)->effectBar;
        monitor->measureBegin();
        holder->Update();
        monitor->measureEnd();
    }
    kmCall(0x80554ce8, EffectMeasurement);


} // namespace CosmosDebug
