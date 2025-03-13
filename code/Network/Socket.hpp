#include <kamek.hpp>
#include <core/rvl/so.hpp>

namespace Aurora {
    namespace Network {
        class Socket{
        public:
            Socket() {}
            ~Socket();

            bool Connect(const char* host, u16 port);
            s32 Send(const void* data, u32 size);
            s32 Recv(void* buffer, u32 size);
            bool Close();

        protected:
            s32 m_handle;
        };
    }
}
