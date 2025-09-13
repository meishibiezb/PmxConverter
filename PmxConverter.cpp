#include "PmxConverter.h"
#include <filesystem>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STBI_WINDOWS_UTF8  
#include "stb_image_write.h"

std::string outputPath = "./output/";
std::string inputPath = "";

class texture
{
public:
    std::string path;
    std::vector<aiMaterial*> materials;
    texture(std::string path);
    bool convertTex();
    void modifyMatPath();
};
texture::texture(std::string path):path(path){}

bool isInTexs(std::vector<texture>* textures, aiMaterial* mat, int* index);

void workflow(const aiScene *scene, std::string outputname)
{
    // 遍历网格
    std::vector<std::string> meshNames;
    for (int i = 0; i < scene->mNumMeshes; i++)
    {
        auto mesh = scene->mMeshes[i];
        // 读取网格的名称
        std::string name = std::string(mesh->mName.C_Str());
        // 测试网格是否重名
        bool needToFix = false;
        for (auto stored : meshNames)
        {
            if (stored == name)
            {
                needToFix = true;
            }
        }
        meshNames.push_back(name);
        std::cout << "mesh_name : " << name << std::endl;
        name = name + "_" + std::to_string(i);
    }

    std::vector<texture> textures;

    // 遍历材质
    std::vector<std::string> matNames;
    for (int i = 0; i < scene->mNumMaterials; i++)
    {
        auto mat = scene->mMaterials[i];
        aiString aiPath;
        mat->GetTexture(aiTextureType_DIFFUSE, 0, &aiPath);
        // 获取名字
        aiString japaneseName;
        mat->Get(AI_MATKEY_JAPANESE_NAME, japaneseName);
        // 判断材质是否重名
        std::string name = std::string(japaneseName.C_Str());
        bool needToFix = false;
        for (auto stored : matNames)
        {
            if (stored == name)
            {
                needToFix = true;
            }
        }
        matNames.push_back(name);
        // 添加名字后缀
        if (needToFix)
        {
            name = name + "_" + std::to_string(i);
            japaneseName = aiString(name);
        }
        std::cout << "mat_name : " << name << std::endl;
        // 应用日文名
        mat->RemoveProperty(AI_MATKEY_NAME);
        mat->AddProperty(&japaneseName, AI_MATKEY_NAME);

        //处理材质
        int index = 0;
        if (isInTexs(&textures, mat, &index))
        {
            textures[index].materials.push_back(mat);
        }
        else
        {
            std::string texPath = std::string(aiPath.C_Str());
            //将所有"\"替换为"/"
            // texPath.replace(texPath.begin(), texPath.end(), '\\', '/');
            std::replace(texPath.begin(), texPath.end(), '\\', '/');
            textures.push_back(texture(texPath));
            textures[index].materials.push_back(mat);
        }
    }

    //遍历纹理
    // std::cout << "========debug========" << std::endl;
    // for (auto texture : textures)
    // {
    //     std::cout << texture.path << std::endl;
    // }
    for (auto texture : textures)
    {
        if (texture.convertTex())
        {
            texture.modifyMatPath();
        }
    }

    //缩放模型
    //TODO: 缩放模型到0.1倍大小
    // float scale = 0.1f;
    // aiMatrix4x4 scaleMatrix;
    // aiMatrix4x4::Scaling(aiVector3D(scale, scale, scale), scaleMatrix);
    // scene->mRootNode->mTransformation = scaleMatrix * scene->mRootNode->mTransformation;
    // std::cout << "缩放模型为" << scale << "倍" << std::endl;

    // 导出模型
    Assimp::Exporter exporter;

    const char *formatId = "glb2";
    outputname = outputname + ".glb";

    Assimp::ExportProperties *expProp = nullptr;

    // unsigned int exportFlags = aiProcess_Triangulate;
    unsigned int exportFlags = aiProcess_FlipUVs;

    aiReturn result = exporter.Export(scene, formatId, outputPath + outputname, exportFlags, expProp);

    if (result == aiReturn_SUCCESS)
    {
        std::cout << "export success!" << std::endl;
    }
    else
    {
        std::cerr << "export failed: " << exporter.GetErrorString() << std::endl;
    }
}

int main()
{

    // 设定字符集
    system("chcp 65001");

    std::cout << "Assimp version:" << aiGetVersionMajor() << "." << aiGetVersionMinor() << "." << aiGetVersionPatch() << std::endl;

    std::string inputname;
    std::cout << "请输入导入路径(路径不能包含中文,包含后缀):";
    std::cin >> inputname;
    std::string outputname;
    std::cout << "请输入导出路径(不包含后缀):";
    std::cin >> outputname;
    
    std::filesystem::path dir = outputPath;
    if (!std::filesystem::exists(dir)) {
        std::filesystem::create_directory(dir);
        std::cout << "输出目录创建成功 : " << outputPath << std::endl;
    } else {
        std::cout << "输出目录已存在 : " << outputPath << std::endl;
    }
    
    size_t last_slash_pos = inputname.find_last_of('/');
    inputPath = inputname.substr(0, last_slash_pos + 1);
    std::cout << "输入目录 : " << inputPath << std::endl;

    // 导入模型
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(
        inputname,
        aiProcess_Triangulate |
            aiProcess_GenNormals |
            aiProcess_FlipUVs |
            aiProcess_JoinIdenticalVertices);
    if (!scene || !scene->mRootNode)
    {
        std::cerr << "fail to import: " << importer.GetErrorString() << std::endl;
        return 1;
    }

    workflow(scene, outputname);

    // 展示信息
    //  read(outputname);

    return 0;
}

bool isInTexs(std::vector<texture>* textures, aiMaterial* mat, int* index)
{
    aiString aiPath;
    mat->GetTexture(aiTextureType_DIFFUSE, 0, &aiPath);
    auto nProp = mat->mNumProperties;
    auto path = std::string(aiPath.C_Str());

    std::replace(path.begin(), path.end(), '\\', '/');

    int num = 0;
    for (auto tex : *textures)
    {
        std::string texPath = tex.path;
        std::replace(texPath.begin(), texPath.end(), '\\', '/');
        if (texPath == path)
        {
            *index = num;
            return true;
        }
        else
        {
            // std::cout << texPath << "!=" << path << std::endl;
        }
        num++;
    }

    *index = textures->size();
    return false;
}

void createDirectoryRecursively(std::string targetPath)
{
    std::filesystem::path dir = targetPath;
    if (!std::filesystem::exists(dir))
    {
        size_t last_slash_pos = targetPath.find_last_of('/');
        std::string parentPath = targetPath.substr(0, last_slash_pos);
        createDirectoryRecursively(parentPath);
        std::filesystem::create_directory(dir);
    }
}

bool texture::convertTex()
{
    //读取文件
    int x, y, comp;
    std::cout << "正在读取 : " << inputPath + path << std::endl;
    unsigned char *data = stbi_load((inputPath + path).c_str(), &x, &y, &comp, 0);
    if (data == nullptr)
    {
        std::cout << "读取失败 : " << inputPath + path << std::endl;
        return false;
    }
    //更改纹理路径
    size_t last_dot_pos = path.find_last_of('.');
    path = path.substr(0, last_dot_pos + 1) + "png";
    //创建导出目录
    size_t last_slash_pos = path.find_last_of('/');
    std::string targetPath = outputPath + path.substr(0, last_slash_pos + 1);
    createDirectoryRecursively(targetPath);
    // 导出文件
    if(stbi_write_png((outputPath + path).c_str(), x, y, comp, data, 0))
    {
        std::cout << "导出成功 : " << outputPath + path << std::endl;
    }
    else
    {
        std::cout << "导出失败 : " << outputPath + path << std::endl;
    }

    return true;
}
void texture::modifyMatPath()
{
    for(auto mat : materials)
    {
        mat->RemoveProperty(AI_MATKEY_TEXTURE_DIFFUSE(0));
        aiString newPath = aiString(path);
        mat->AddProperty(&newPath, AI_MATKEY_TEXTURE_DIFFUSE(0));
    }
}