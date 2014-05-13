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
    static int getTextureID(std::wstring& texName);
    static ID3D11ShaderResourceView* getTexture(UINT texID);

    static void destroyAll();

private:
    static ID3D11Device* mDevice;
    static std::vector<std::wstring> mPath;
    static std::map<std::wstring, ID3D11ShaderResourceView*> mSRV;
    static int mIdCounter;
    static std::vector<std::wstring> mIdMap;
};


#endif