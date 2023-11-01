#ifndef _NW4R_SND3DLISTENER_
#define _NW4R_SND3DLISTENER_
#include "types.hpp"
namespace nw4r{
namespace snd{

typedef float Mat_f[3][4];
struct MTX34_f{
	union{
		struct{
		float n00, n01, n02, n03;
		float n10, n11, n12, n13;
		float n20, n21, n22, n23;
		};
		float element[3][4];
		float member[12];
		Mat_f mtx;
	};
};

class Sound3DListener{
public:
	Sound3DListener(); //8009d550
	MTX34_f mat;
	float unknown_0x30[6];
	u32 unknown_0x48;
	u8 unknown_0x4C[4];
	ut::LinkListNode link;
}; //total size 0x58

}//namespace snd
}//namespace nw4r
#endif