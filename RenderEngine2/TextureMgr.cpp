#include "TextureMgr.h"

std::vector<std::wstring> TextureMgr::mPath;
std::map<std::wstring, ID3D11ShaderResourceView*> TextureMgr::mSRV;
ID3D11Device* TextureMgr::mDevice = 0;
int TextureMgr::mIdCounter = 0;
std::vector<std::wstring> TextureMgr::mIdMap;

void TextureMgr::initAll( ID3D11Device* device )
{
    mDevice = device;
}

// path should be L"../Models/"
void TextureMgr::addPath( std::wstring& path )
{
    mPath.push_back(path);
}

ID3D11ShaderResourceView* TextureMgr::getTexture( std::wstring& texName )
{
    std::map<std::wstring, ID3D11ShaderResourceView*>::const_iterator cIter; 
    cIter = mSRV.find(texName);
    if (cIter != mSRV.end())
    {
        return (*cIter).second;
    }
    else
    {
        ID3D11ShaderResourceView* srv = 0;
        for (UINT i = 0; i < mPath.size(); ++i)
        {
            std::wstring dir = mPath[i] + texName;


            D3DX11_IMAGE_LOAD_INFO info;
            info.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
            HRESULT hr = D3DX11CreateShaderResourceViewFromFile(mDevice, dir.c_str(), &info, 0, &srv, 0 );
            if(FAILED(hr))											
                continue;

            mSRV.insert(std::pair<std::wstring, ID3D11ShaderResourceView*>(texName, srv));
            mIdMap.push_back(texName);
            return srv;
        }
    }
    // Texture not found
    return 0;
}

ID3D11ShaderResourceView* TextureMgr::getTexture( UINT texID )
{
    if ( texID >= mIdMap.size())
    {
        return 0;
    }

    return getTexture(mIdMap[texID]);
}

void TextureMgr::destroyAll()
{
    mPath.clear();
    mSRV.clear();
    mDevice = 0;
}

int TextureMgr::getTextureID( std::wstring& texName )
{
    for (UINT i = 0; i < mIdMap.size(); ++i)
    {
        if (mIdMap[i].compare(texName) == 0)
        {
            return i;
        }
    }
    return -1;
}



