#include "TextureMgr.h"

std::vector<std::wstring> TextureMgr::mPath;
std::map<std::wstring, ID3D11ShaderResourceView*> TextureMgr::mSRV;
ID3D11Device* TextureMgr::mDevice = 0;

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
            HRESULT hr = D3DX11CreateShaderResourceViewFromFile(mDevice, dir.c_str(), 0, 0, &srv, 0 );
            if(FAILED(hr))											
                continue;

            mSRV.insert(std::pair<std::wstring, ID3D11ShaderResourceView*>(texName, srv));
            return srv;
        }
    }
    // Texture not found
    return 0;
}

void TextureMgr::destroyAll()
{
    mPath.clear();
    mSRV.clear();
    mDevice = 0;
}

