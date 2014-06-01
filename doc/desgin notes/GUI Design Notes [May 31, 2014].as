Customized Font Format:
>> char left top right bottom 
>> A    100  0   150   20
>> B    151  0   200   20
>> C    201  0   250   20
>> D    251  0   300   20

texture:
    id3d11Resource
    file_name

save(out_ar)
    d3dx11SaveTextureToFile(file_name, id3d11Resource)
    out_ar << file_name
    

load(in_ar)
    in_ar >> file_name
    d3dx11LoadTextureFromFile(file_name)

Pseudo code TextureAtlas
-----------------------------------------
SrcRect:
    int left, top, right, bottom;

AtlasDesc:
    string  sub_texture_name;
    SrcRect sub_texture_rect;

TextureAtlas:
    string              dds_file_name;
    id3d11Resource      resource;
    id3d11SRV           srv;
    vector<AtlasDesc>   desc;
    save(Archive& ar):
        ar & dds_file_name & desc;
        d3dx11SaveTextureToFile(dds_file_name, resource);

    load(Archive& ar):
        ar & dds_file_name & desc;
        d3dx11LoadTextureFromFile(dds_file_name, resource, srv);

serialize(Archive& ar, vector<TextureAtlas>& objs):
    ar & objs;

TextureMgr:
    vector<TextureAtlas> Textures;
    init():
        ifstream ifs(filename);
        boost::archive::some_archive ar(ifs);
        ar & Textures;

    





