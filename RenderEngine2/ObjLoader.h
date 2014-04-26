#ifndef OBJ_LOADER_H
#define OBJ_LOADER_H

#include <string>
#include <vector>
#include <Windows.h>
#include <xnamath.h>

// ObjLoader is completely self-contained thus re-usable
class ObjLoader
{
public:
    struct ObjVertex
    {
        XMFLOAT3 pos;
        XMFLOAT3 norm;
        XMFLOAT2 texcd;
    };

    /*
    Wavefront obj illumination models:
    0. Color on and Ambient off
    1. Color on and Ambient on
    2. Highlight on
    3. Reflection on and Ray trace on
    4. Transparency: Glass on, Reflection: Ray trace on
    5. Reflection: Fresnel on and Ray trace on
    6. Transparency: Refraction on, Reflection: Fresnel off and Ray trace on
    7. Transparency: Refraction on, Reflection: Fresnel on and Ray trace on
    8. Reflection on and Ray trace off
    9. Transparency: Glass on, Reflection: Ray trace off
    10. Casts shadows onto invisible surfaces

    Ns ranges
     Ns 10.000                # ranges between 0 and 1000
    */
    struct ObjMaterial
    {
        std::wstring materialName;
        std::wstring diffuseMapFileName;

        XMFLOAT4 ambient;
        XMFLOAT4 diffuse;
        XMFLOAT4 specular; // w = SpecPower
        bool useSpecular;
        int specPower; 

        void setDefault()
        {
            materialName = L"";
            diffuseMapFileName = L"";
            ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.f);
            diffuse = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.f);
            specular = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.f);
            useSpecular = false;
            specPower = -1;
        }
    };

    struct ObjSubset
    {
        ObjSubset(): matId(-1), indexOffset(0), indexCount(0)
        {}
        int matId;
        UINT indexOffset;
        UINT indexCount;
    };

    bool load(std::wstring filename, bool coordConvert);
    void clear();

    std::vector<ObjLoader::ObjVertex> mVertices;
    std::vector<UINT> mIndices;
    std::vector<ObjLoader::ObjSubset> mSubsets;
    std::vector<ObjLoader::ObjMaterial> mMaterials;

    ObjLoader(){}
    ~ObjLoader(){clear();}

private:
    bool loadMeshFromObj(std::wstring  filename, bool coordConvert);
    bool loadMtlLib(std::wstring filename);
    int getMaterialIdByName(std::wstring mtlName);


};

#endif