#ifndef TEXTURE_MGR_H
#define TEXTURE_MGR_H

#include "Util.h"
#include <map>

// Should be singleton
class TextureMgr
{
public:
    static void initAll(ID3D11Device* device);
    static void addPath(std::wstring& path);
    static ID3D11ShaderResourceView* getTexture(std::wstring& texName);
    static void destroyAll();

private:
    static std::vector<std::wstring> mPath;
    static std::map<std::wstring, ID3D11ShaderResourceView*> mSRV;
    static ID3D11Device* mDevice;
};


#endif