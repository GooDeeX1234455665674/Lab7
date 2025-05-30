#ifndef MODEL_H
#define MODEL_H
#define GLEW_DLL
#define GLFW_DLL

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Mesh.h"
#include "ShaderLoader.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
using namespace std;

class Model
{
public:
    vector<Mesh> meshes;
    vector<glm::mat4> meshTransforms;
    string directory;

    Model(string const& path)
    {
        loadModel(path);
        meshTransforms.resize(meshes.size(), glm::mat4(1.0f));
    }

    void Draw(Shader& shader) {
        for (size_t i = 0; i < meshes.size(); i++) {
            shader.setMat4("model", glm::value_ptr(meshTransforms[i]));
            meshes[i].Draw();
        }
    }

    void UpdateTransform(int meshIndex, const glm::mat4& transform) {
        if (meshIndex >= 0 && meshIndex < meshTransforms.size()) {
            meshTransforms[meshIndex] = transform;
        }
    }

private:
    void loadModel(string const& path)
    {
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(path,
            aiProcess_CalcTangentSpace |
            aiProcess_Triangulate |
            aiProcess_JoinIdenticalVertices |
            aiProcess_SortByPType);

        if (nullptr == scene) {
            cout << "ASSIMP ERROR::" << importer.GetErrorString() << endl;
        }

        processNode(scene->mRootNode, scene);
    }

    void processNode(aiNode* node, const aiScene* scene)
    {
        for (unsigned int i = 0; i < node->mNumMeshes; i++) {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            meshes.push_back(processMesh(mesh, scene));
        }
        for (unsigned int i = 0; i < node->mNumChildren; i++) {
            processNode(node->mChildren[i], scene);
        }
    }

    Mesh processMesh(aiMesh* mesh, const aiScene* scene)
    {
        vector <Vertex>       vertices;
        vector <unsigned int> indices;

        for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
            Vertex vertex;
            glm::vec3 vector;
            vector.x = mesh->mVertices[i].x;
            vector.y = mesh->mVertices[i].y;
            vector.z = mesh->mVertices[i].z;
            vertex.Position = vector;

            if (mesh->HasNormals()) {
                vector.x = mesh->mNormals[i].x;
                vector.y = mesh->mNormals[i].y;
                vector.z = mesh->mNormals[i].z;
                vertex.Normal = vector;
            }
            else {
                vertex.Normal = glm::vec3(0.0f, 1.0f, 0.0f);
            }
            vertices.push_back(vertex);
        }

        for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
            aiFace face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; j++) {
                indices.push_back(face.mIndices[j]);
            }
        }
        return Mesh(vertices, indices);
    }
};

#endif