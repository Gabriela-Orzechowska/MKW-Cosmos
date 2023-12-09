#include <Storage/StorageDevice.hpp>
#include <main.hpp>

class SDStorage : public StorageDevice
{
    public:
    static bool Init();

    u32 diskSectorSize() override;
    bool diskRead(u32 sector, u32 count, void * buffer) override;
    bool diskWrite(u32 sector, u32 count, const void * buffer) override;
    bool diskErase(u32 sector, u32 count) override;
    bool diskSync() override;
    u32 diskGetMessageId() override;
};