//#define GLM_ENABLE_EXPERIMENTAL
//#include <vector>
//#include <iostream>
//#include "Mesh.h"
//
//#include <assimp/Importer.hpp>
//#include <assimp/scene.h>
//#include <assimp/postprocess.h>
//
//
//#include "Loader.h"
//
//#define TEXTUREDIR "./Models/textures/"
//
///**
//*	Constructor, loading the specified aiMesh
//**/
//Mesh::MeshEntry::MeshEntry(aiMesh* mesh, Mesh& parent) : parent(parent) {
//
//	vbo[VERTEX_BUFFER] = NULL;
//	vbo[TEXCOORD_BUFFER] = NULL;
//	vbo[NORMAL_BUFFER] = NULL;
//	vbo[INDEX_BUFFER] = NULL;
//	//vbo[MATERIAL_BUFFER] = NULL;
//
//	glGenVertexArrays(1, &vao);
//	glBindVertexArray(vao);
//
//	elementCount = mesh->mNumFaces * 3;
//	materialIndex = mesh->mMaterialIndex;
//
//
//	if (mesh->HasPositions()) {
//		//std::cout << "Vertices num: " << mesh->mNumVertices << std::endl;
//		float* vertices = new float[mesh->mNumVertices * 3];
//		for (int i = 0; i < mesh->mNumVertices; ++i) {
//			vertices[i * 3] = mesh->mVertices[i].x;
//			vertices[i * 3 + 1] = mesh->mVertices[i].y;
//			vertices[i * 3 + 2] = mesh->mVertices[i].z;
//
//			parent.tmpMin.x = std::fminf(mesh->mVertices[i].x, parent.tmpMin.x);
//			parent.tmpMin.y = std::fminf(mesh->mVertices[i].y, parent.tmpMin.y);
//			parent.tmpMin.z = std::fminf(mesh->mVertices[i].z, parent.tmpMin.z);
//
//			parent.tmpMax.x = std::fmaxf(mesh->mVertices[i].x, parent.tmpMax.x);
//			parent.tmpMax.y = std::fmaxf(mesh->mVertices[i].y, parent.tmpMax.y);
//			parent.tmpMax.z = std::fmaxf(mesh->mVertices[i].z, parent.tmpMax.z);
//
//
//		}
//
//		glGenBuffers(1, &vbo[VERTEX_BUFFER]);
//		glBindBuffer(GL_ARRAY_BUFFER, vbo[VERTEX_BUFFER]);
//		glBufferData(GL_ARRAY_BUFFER, 3 * mesh->mNumVertices * sizeof(GLfloat), vertices, GL_STATIC_DRAW);
//
//		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
//		glEnableVertexAttribArray(0);
//
//		delete vertices;
//	}
//
//	if (mesh->HasNormals()) {
//		float* normals = new float[mesh->mNumVertices * 3];
//		for (int i = 0; i < mesh->mNumVertices; ++i) {
//			normals[i * 3] = mesh->mNormals[i].x;
//			normals[i * 3 + 1] = mesh->mNormals[i].y;
//			normals[i * 3 + 2] = mesh->mNormals[i].z;
//		}
//
//		glGenBuffers(1, &vbo[NORMAL_BUFFER]);
//		glBindBuffer(GL_ARRAY_BUFFER, vbo[NORMAL_BUFFER]);
//		glBufferData(GL_ARRAY_BUFFER, 3 * mesh->mNumVertices * sizeof(GLfloat), normals, GL_STATIC_DRAW);
//
//		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
//		glEnableVertexAttribArray(1);
//
//		delete normals;
//	}
//
//	if (mesh->HasTextureCoords(0)) {
//		float* texCoords = new float[mesh->mNumVertices * 2];
//		for (int i = 0; i < mesh->mNumVertices; ++i) {
//			texCoords[i * 2] = mesh->mTextureCoords[0][i].x;
//			texCoords[i * 2 + 1] = mesh->mTextureCoords[0][i].y;
//		}
//
//		glGenBuffers(1, &vbo[TEXCOORD_BUFFER]);
//		glBindBuffer(GL_ARRAY_BUFFER, vbo[TEXCOORD_BUFFER]);
//		glBufferData(GL_ARRAY_BUFFER, 2 * mesh->mNumVertices * sizeof(GLfloat), texCoords, GL_STATIC_DRAW);
//
//		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, NULL);
//		glEnableVertexAttribArray(2);
//
//		delete texCoords;
//	}
//
//
//
//
//
//	if (mesh->HasFaces()) {
//		unsigned int* indices = new unsigned int[mesh->mNumFaces * 3];
//		for (int i = 0; i < mesh->mNumFaces; ++i) {
//			indices[i * 3] = mesh->mFaces[i].mIndices[0];
//			indices[i * 3 + 1] = mesh->mFaces[i].mIndices[1];
//			indices[i * 3 + 2] = mesh->mFaces[i].mIndices[2];
//		}
//
//		glGenBuffers(1, &vbo[INDEX_BUFFER]);
//		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[INDEX_BUFFER]);
//		glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3 * mesh->mNumFaces * sizeof(GLuint), indices, GL_STATIC_DRAW);
//
//		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, NULL);
//		glEnableVertexAttribArray(3);
//
//		delete indices;
//	}
//
//	//aiString texPath;   //contains filename of texture
//	//if (AI_SUCCESS == mtl->GetTexture(aiTextureType_DIFFUSE, 0, &texPath)) {
//	//	material.textureCount = 1;
//	//}
//	//else {
//	//	material.textureCount = 0;
//	//}
//
//	glBindBuffer(GL_ARRAY_BUFFER, 0);
//	glBindVertexArray(0);
//}
//
///**
//*	Deletes the allocated OpenGL buffers
//**/
//Mesh::MeshEntry::~MeshEntry() {
//	if (vbo[VERTEX_BUFFER]) {
//		glDeleteBuffers(1, &vbo[VERTEX_BUFFER]);
//	}
//
//	if (vbo[TEXCOORD_BUFFER]) {
//		glDeleteBuffers(1, &vbo[TEXCOORD_BUFFER]);
//	}
//
//	if (vbo[NORMAL_BUFFER]) {
//		glDeleteBuffers(1, &vbo[NORMAL_BUFFER]);
//	}
//
//	if (vbo[INDEX_BUFFER]) {
//		glDeleteBuffers(1, &vbo[INDEX_BUFFER]);
//	}
//
//
//	glDeleteVertexArrays(1, &vao);
//}
//
///**
//*	Renders this MeshEntry
//**/
//void Mesh::MeshEntry::render(ShaderProgram* shader) {
//	//glBindBufferRange(GL_UNIFORM_BUFFER, materialUniLoc, myMeshes[nd->mMeshes[n]].uniformBlockIndex, 0, sizeof(struct MyMaterial));  
//	glBindVertexArray(vao);
//	//std::cout << materialIndex << std::endl;
//	glDrawElements(GL_TRIANGLES, elementCount, GL_UNSIGNED_INT, NULL);
//	glBindVertexArray(0);
//}
//
//
//void Mesh::Texture::bind(GLenum unit) {
//
//	glActiveTexture(unit);
//	glBindTexture(Mesh::Texture::textType, Mesh::Texture::texId);
//}
//
//void Mesh::Texture::load(std::string path) {
//	Mesh::Texture::texId = loadImage(path.c_str());
//	//Mesh::Texture::textType = GL_TEXTURE_2D;
//}
//
//Mesh::Texture::Texture(std::string path, GLenum textType) {
//
//	Mesh::Texture::path = path;
//	Mesh::Texture::textType = textType;
//	Mesh::Texture::load(path);
//}
//
//Mesh::Texture::~Texture() {
//
//}
//
//
///**
//*	Mesh constructor, loads the specified filename if supported by Assimp
//**/
//Mesh::Mesh(const char* filename)
//{
//	Assimp::Importer importer;
//	const aiScene* scene = importer.ReadFile(filename, aiProcess_Triangulate | aiProcess_GenSmoothNormals); //aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices
//	if (!scene) {
//		printf("Unable to load mesh: %s\n", importer.GetErrorString());
//		return;
//	}
//
//	meshEntries.resize(scene->mNumMeshes);
//	textures.resize(scene->mNumMaterials);
//
//	std::cout << "Num meshes: " << scene->mNumMeshes << std::endl;
//	std::cout << "Num materials: " << scene->mNumMaterials << std::endl;
//	for (int i = 0; i < meshEntries.size(); ++i) {
//		meshEntries[i] = new Mesh::MeshEntry(scene->mMeshes[i], *this);
//	}
//
//
//	//Load materials and textures (aiScene *)
//	initMaterials(scene);
//
//
//
//}
//
//void Mesh::initMaterials(const aiScene* scene) {
//
//	//Loop over all materials
//	for (unsigned int i = 0; i < scene->mNumMaterials; i++) {
//		const aiMaterial* pMaterial = scene->mMaterials[i];
//		textures[i] = NULL;
//
//		aiColor3D diffuse;
//		aiColor3D specular;
//		aiColor3D ambient;
//
//		pMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse);
//		pMaterial->Get(AI_MATKEY_COLOR_SPECULAR, specular);
//		pMaterial->Get(AI_MATKEY_COLOR_AMBIENT, ambient);
//
//
//
//
//		if (pMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
//			aiString Path;
//			std::string dir = TEXTUREDIR;
//			if (pMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &Path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) {
//				std::string FullPath = dir + Path.data;
//				textures[i] = new Mesh::Texture(FullPath, GL_TEXTURE_2D);
//			}
//		}
//
//
//		if (textures[i] == NULL) {
//			//std::cout << "No texture found, using default one!" << std::endl;
//			textures[i] = new Mesh::Texture("./Models/textures/default.png", GL_TEXTURE_2D);
//
//		}
//
//		textures[i]->diffuse = glm::vec3(diffuse.r, diffuse.g, diffuse.b);
//		textures[i]->ambient = glm::vec3(ambient.r, ambient.g, ambient.b);
//		textures[i]->specular = glm::vec3(specular.r, specular.g, specular.b);
//
//		if (glm::length(textures[i]->diffuse) <= 0.0) {
//			textures[i]->diffuse = glm::vec3(0.8, 0.8, 0.8);
//		}
//		if (glm::length(textures[i]->ambient) <= 0.0) {
//			textures[i]->ambient = glm::vec3(0.2, 0.2, 0.2);
//		}
//		if (glm::length(textures[i]->specular) <= 0.0) {
//			textures[i]->specular = glm::vec3(0.3, 0.3, 0.3);
//		}
//
//	}
//}
//
///**
//*	Clears all loaded MeshEntries
//**/
//Mesh::~Mesh(void)
//{
//	for (int i = 0; i < meshEntries.size(); ++i) {
//		delete meshEntries.at(i);
//	}
//	meshEntries.clear();
//
//	for (int i = 0; i < textures.size(); i++) {
//		if (textures[i])
//			delete textures.at(i);
//	}
//	textures.clear();
//
//
//}
//
///**
//*	Renders all loaded MeshEntries
//**/
//void Mesh::render(ShaderProgram* shader) {
//
//	for (int i = 0; i < meshEntries.size(); ++i) {
//		const unsigned int MaterialIndex = meshEntries[i]->materialIndex;
//
//
//		glUniform3fv(shader->uniform("Kd"), 1, glm::value_ptr(textures[MaterialIndex]->diffuse));
//		glUniform3fv(shader->uniform("Ka"), 1, glm::value_ptr(textures[MaterialIndex]->ambient));
//		glUniform3fv(shader->uniform("Ks"), 1, glm::value_ptr(textures[MaterialIndex]->specular));
//
//
//		if (MaterialIndex < textures.size() && textures[MaterialIndex]) {
//			textures[MaterialIndex]->bind(GL_TEXTURE0);
//		}
//
//		meshEntries.at(i)->render(shader);
//	}
//}