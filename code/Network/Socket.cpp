#include "core/rvl/so.hpp"
#include <Network/Socket.hpp>

namespace Aurora {
    namespace Network {

        Socket::~Socket(){
            this->Close();
        }   

        bool Socket::Close() {
            if(this->m_handle >= 0)
                SOClose(this->m_handle);
        }

        bool Socket::Connect(const char* host, u16 port){
            char port_str[0x10];
            SOAddrInfo* info;

            snprintf(port_str, sizeof(port_str), "%u", port);
            s32 ret = SOGetAddrInfo(host, port_str, nullptr, &info); 

            if(ret < 0) return false;

            while(info->addrLen < sizeof(SOSockAddrIn)){
                info = info->next;
                if(!info) return false;
            }
            SOSockAddrIn* addr = (SOSockAddrIn*)(info->addr); 

            addr->len = sizeof(SOSockAddrIn);
            this->m_handle = SOSocket(SO_PF_INET, SO_SOCK_STREAM, 0);
            if(m_handle < 0) return false;
            if(SOConnect(this->m_handle, addr) < 0) {
                this->Close();
                return false;
            }
            return true;
        }

        s32 Socket::Send(const void* data, u32 size){
            return SOSend(this->m_handle, data, size, 0);
        }

        s32 Socket::Recv(void* data, u32 size){
            return SORecv(this->m_handle, data, size, 0);
        }
    }
}
