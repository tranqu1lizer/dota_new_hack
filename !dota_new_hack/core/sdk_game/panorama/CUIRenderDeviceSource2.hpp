#include "../NormalClass.hpp"

class CDX11Texture {
public:
	char pad_0000[16]; // 0x0
	ID3D11ShaderResourceView* m_first_shader; // 0x10
	ID3D11ShaderResourceView* m_second_shader; // 0x18
};

class CSource2UITexture : public VClass {
public:
	char pad_0000[40]; // 0x0
	CDX11Texture* m_texture; // 0x28
};

class CUIRenderDeviceSource2 {
public:
	virtual bool LoadTexture( CSource2UITexture** a2, const char* texture_name ) = 0;
};