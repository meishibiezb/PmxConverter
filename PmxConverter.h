#pragma once

#include <assimp/Importer.hpp>
#include <assimp/Exporter.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/version.h>
#include <iostream>
#include <fstream>
#include <string>

int check(std::string &test);

int combine(std::string &unnamed, std::string &pmx);

int read(std::string &name);

void indentPrint(unsigned int indent, std::string key, std::string value);

int combine(std::string &unnamed, std::string &pmx)
{
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(
        unnamed,
        aiProcess_Triangulate |
            aiProcess_GenNormals |
            aiProcess_FlipUVs |
            aiProcess_JoinIdenticalVertices);
    if (!scene || !scene->mRootNode)
    {
        std::cerr << "fail to import: " << importer.GetErrorString() << std::endl;
        return 1;
    }

    Assimp::Importer importer1;
    const aiScene *scene1 = importer1.ReadFile(
        pmx,
        aiProcess_Triangulate |
            aiProcess_GenNormals |
            aiProcess_FlipUVs |
            aiProcess_JoinIdenticalVertices);
    if (!scene1 || !scene1->mRootNode)
    {
        std::cerr << "fail to import: " << importer1.GetErrorString() << std::endl;
        return 1;
    }

    std::cout << scene->mNumMaterials << std::endl;

    for (int i = 0; i < scene1->mNumMaterials; i++)
    {
        aiString texturePath;
        aiString materialName;
        auto mat = scene->mMaterials[i];
        auto mat1 = scene1->mMaterials[i];

        mat1->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath);
        materialName = scene->mMaterials[i]->GetName();
        // scene->mMaterials[i]->Get(AI_MATKEY_JAPNAME, materialName);

        if (i < scene->mNumMaterials)
        {
            mat->AddProperty(&texturePath, AI_MATKEY_TEXTURE_DIFFUSE(0));
        }

        std::cout << materialName.C_Str() << ":" << texturePath.C_Str() << std::endl;
    }

    std::cout << scene->mNumMeshes << std::endl;

    for (int i = 0; i < scene->mNumMeshes; i++)
    {
        auto mesh = scene->mMeshes[i];
        unsigned int index = mesh->mMaterialIndex;
        std::cout << mesh->mName.C_Str() << ":" << index << std::endl;
    }

    Assimp::Exporter exporter;

    const char *formatId = "glb2";

    Assimp::ExportProperties *expProp = nullptr;

    unsigned int exportFlags = aiProcess_FlipUVs;

    aiReturn result = exporter.Export(scene, formatId, "combined.glb", exportFlags, expProp);

    if (result == aiReturn_SUCCESS)
    {
        std::cout << "export success!" << std::endl;
    }
    else
    {
        std::cerr << "export failed: " << exporter.GetErrorString() << std::endl;
    }

    std::cout << "测试" << std::endl;

    return 0;
}
int read(std::string &name)
{
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(
        name,
        aiProcess_Triangulate |
            aiProcess_FlipUVs |
            aiProcess_GenNormals |
            aiProcess_JoinIdenticalVertices);
    if (!scene || !scene->mRootNode)
    {
        std::cerr << "fail to import: " << importer.GetErrorString() << std::endl;
        return 1;
    }

    using namespace std;
    unsigned int layer = 0;

    indentPrint(layer, "网格", "");
    auto nMes = scene->mNumMeshes;
    layer++;
    for (int i = 0; i < nMes; i++)
    {
        auto mesh = scene->mMeshes[i];
        auto name = mesh->mName.C_Str();
        indentPrint(layer, "name", name);

        auto nVer = mesh->mNumVertices;
        auto nFace = mesh->mNumFaces;

        layer++;
        indentPrint(layer, "NumberOfVertices", to_string(nVer));
        indentPrint(layer, "NumberOfFaces", to_string(nFace));
        layer--;
    }
    layer--;

    indentPrint(layer, "材质", "");
    auto nMat = scene->mNumMaterials;
    layer++;
    for (int i = 0; i < nMat; i++)
    {
        auto mat = scene->mMaterials[i];
        auto name = mat->GetName().C_Str();
        // 打印每个材质的名字
        indentPrint(layer, "name", name);

        aiString path;
        mat->GetTexture(aiTextureType_DIFFUSE, 0, &path);
        auto nProp = mat->mNumProperties;

        layer++;
        indentPrint(layer, "path", path.C_Str());
        indentPrint(layer, "NumberOfProperty", to_string(nProp));
        layer--;
    }
    layer--;

    return 0;
}

void indentPrint(unsigned int indent, std::string key, std::string value)
{
    for (unsigned int i = 0; i < indent; i++)
    {
        std::cout << "    ";
    }
    std::cout << key << " : " << value << std::endl;
}
int check(std::string &test)
{
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(
        test,
        aiProcess_Triangulate |
            aiProcess_GenNormals |
            aiProcess_FlipUVs |
            aiProcess_JoinIdenticalVertices);
    if (!scene || !scene->mRootNode)
    {
        std::cerr << "fail to import: " << importer.GetErrorString() << std::endl;
        return 1;
    }

    std::cout << scene->mNumMaterials << std::endl;

    for (int i = 0; i < scene->mNumMaterials; i++)
    {
        aiString texturePath;
        aiString materialName;
        auto mat = scene->mMaterials[i];

        mat->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath);
        materialName = scene->mMaterials[i]->GetName();
        // scene->mMaterials[i]->Get(AI_MATKEY_JAPNAME, materialName);

        // if (i<scene->mNumMaterials)
        // {
        //     mat->AddProperty(&texturePath,AI_MATKEY_NAME);
        // }

        std::cout << materialName.C_Str() << ":" << texturePath.C_Str() << std::endl;
    }
    return 0;
}