#include "ObjLoader.h"
#include <fstream>

bool ObjLoader::load( std::wstring filename, bool coordConvert )
{
    if (loadMeshFromObj(filename, coordConvert) == false)
    {
        return false;
    }
    return true;
}

bool ObjLoader::loadMeshFromObj( std::wstring filename, bool coordConvert)
{
    std::wfstream fin(filename);
    if (fin)
    {
        std::vector<XMFLOAT3> positions;
        std::vector<XMFLOAT3> normals;
        std::vector<XMFLOAT2> texcoords;

        std::wstring cmd;
        ObjSubset* pCurrSubset = new ObjSubset();
        UINT line2 = 0;
        for (;;)
        {
            fin >> cmd;
            line2++;
            if (!fin)
            {
                // Save the previous subset if any
                if (pCurrSubset->matId != -1)
                {
                    pCurrSubset->indexCount= mIndices.size()  - pCurrSubset->indexOffset;
                    mSubsets.push_back(*pCurrSubset);
                }
                delete pCurrSubset;
                pCurrSubset = 0;

                break;
            }
            if(cmd.compare(L"#") == 0)
            {
                // ignore comments
            }
            else if (cmd.compare(L"mtllib") == 0)
            {
                std::wstring mtllibFilename;
                fin >> mtllibFilename;
                mtllibFilename = L"../Models/" + mtllibFilename;
                
                if (loadMtlLib(mtllibFilename) == false)
                {
                    return false;
                }
            }
            else if (cmd.compare(L"usemtl") == 0)
            {
                // Save the previous subset if any
                if (pCurrSubset->matId != -1)
                {
                    pCurrSubset->indexCount= mIndices.size()  - pCurrSubset->indexOffset;
                    mSubsets.push_back(*pCurrSubset);
                }

                // About to add a new subset
                std::wstring mtlName;
                fin >> mtlName;

                pCurrSubset->matId = getMaterialIdByName(mtlName);
                // Corrupted obj file?
                if (pCurrSubset->matId == -1)
                {
                    return false;
                }
                pCurrSubset->indexOffset = mIndices.size();
            }
            else if (cmd.compare(L"v") == 0)
            {
                float x, y, z;
                fin >> x >> y >> z;

                if (coordConvert)
                    z = z*-1.f;

                positions.push_back(XMFLOAT3(x, y, z));
            }
            else if (cmd.compare(L"vn") == 0)
            {
                float x, y, z;
                fin >> x >> y >> z;

                if (coordConvert)
                    z = z*-1.f;

                normals.push_back(XMFLOAT3(x, y, z));
            }
            else if (cmd.compare(L"vt") == 0)
            {
                float u, v;
                fin >> u >> v;

                if (coordConvert)
                    v = 1.f - v;

                texcoords.push_back(XMFLOAT2(u, v));
            }
            else if( cmd.compare(L"f") == 0)
            {
                UINT posIndex, texIndex, normIndex;
                ObjVertex vertex[3];
                // Suppose we have triangulated meshes
                for (UINT i = 0; i < 3; ++i)
                {
                    ZeroMemory(&vertex[i], sizeof(ObjVertex));
                    fin >> posIndex;
                    vertex[i].pos = positions[posIndex - 1];

                    if ('/' == fin.peek())
                    {
                        // Ignore the '/' before texcoord index
                        fin.ignore();

                        // Optional texcoords
                        if ('/' != fin.peek())
                        {
                            fin >> texIndex;
                            vertex[i].texcd = texcoords[texIndex - 1];
                        }

                        if ('/' == fin.peek())
                        {
                            // Ignore the '/' before normal index
                            fin.ignore();
                            fin >> normIndex;
                            vertex[i].norm = normals[normIndex - 1];
                        }
                    }
                }//end of for(...)

                // Do not handle duplicated vertex
                // Convert the drawing order
                for (UINT i = 0; i < 3; ++i)
                {
                    mIndices.push_back(mVertices.size());
                    mVertices.push_back(vertex[2-i]);
                }
            }// end of if(cmd.compare(L"f") == 0)
            else
            {
                // Not supported feature
            }
            // Max line length: 3000
            fin.ignore(3000, '\n');
        }

        fin.close();
        return true;
    }

    // Fail to open obj file
    return false;
}

bool ObjLoader::loadMtlLib( std::wstring filename )
{

    std::wfstream fin(filename);

    if (!fin)
    {
        return false;
    }

    std::wstring cmd;

    ObjMaterial material;
    material.setDefault();

    for (;;)
    {
        fin>>cmd;
        if (!fin)
        {
            // Save previous materials if any
            if (material.materialName.compare(L"") != 0)
            {
                mMaterials.push_back(material);
            }
            break;
        }
        else if (cmd.compare(L"#") == 0)
        {
            // ignore comments
        }
        else if (cmd.compare(L"newmtl") == 0)
        {
            // Save previous materials if any
            if (material.materialName.compare(L"") != 0)
            {
                mMaterials.push_back(material);
            }

            material.setDefault();
            fin >> material.materialName;
        }
        else if (cmd.compare(L"Ka") == 0)
        {
            float x, y, z;
            fin >> x >> y >> z;
            material.ambient = XMFLOAT4(x, y, z, 1.f);
        }
        else if (cmd.compare(L"Kd") == 0)
        {
            float x, y, z;
            fin >> x >> y >> z;
            material.diffuse= XMFLOAT4(x, y, z, 1.f);
        }
        else if (cmd.compare(L"Ks") == 0)
        {
            float x, y, z;
            fin >> x >> y >> z;
            material.specular= XMFLOAT4(x, y, z, 1.f);
        }
        else if (cmd.compare(L"illum") == 0)
        {
            int num;
            fin >> num;
            material.useSpecular = (num==2?true:false);
        }
        else if (cmd.compare(L"Ns") == 0)
        {
            fin >> material.specPower;
        }
        else if (cmd.compare(L"map_Kd") == 0)
        {
            fin >> material.diffuseMapFileName;
        }
        // Other features are not supported
        else
        {

        }
        fin.ignore(3000, '\n');
    }

    fin.close();
    return true;
}

int ObjLoader::getMaterialIdByName( std::wstring mtlName )
{
    int matId = -1;
    int size = static_cast<int>(mMaterials.size());
    for (int i = 0; i < size; ++i)
    {
        if (mMaterials[i].materialName.compare(mtlName) == 0)
        {
            matId = i;
            break;
        }
    }
    return matId;
}

void ObjLoader::clear()
{
    mMaterials.clear();
    mSubsets.clear();
    mVertices.clear();
    mIndices.clear();
}


