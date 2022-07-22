#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#define GLM_FORCE_CTOR_INIT

#include "Model.h"
#include "texUtils.h"

#include "meshoptimizer.h"
#include  <glm/gtx/string_cast.hpp>



using std::string;

unsigned int TextureFromFile(const char* path, const string& directory, bool gamma = false);

glm::mat3x4 convertMatrix(glm::mat4 s);
glm::quat quatcast(glm::mat4 t);
void debugVertexBoneData(unsigned int total_vertices, vector<VertexBoneData> Bones);
void debugSkeletonPose(map<unsigned int, glm::vec3> skeletonPos);


//Helpers
unsigned int TextureFromFile(const char* path, const string& directory, bool gamma)
{
	string filename = string(path);
	filename = directory + '/' + filename;

	unsigned int textureID;
	glGenTextures(1, &textureID);

	textureID = loadImage(filename.c_str());
	/*

	int width, height, nrComponents;
	unsigned char *data = loadImage(filename.c_str(), &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	*/

	return textureID;
}







struct VertexData
{
	float px, py, pz;
	float nx, ny, nz;
	float tx, ty;
};

struct PackedVertex
{
	unsigned short px, py, pz;
	unsigned short pw; // padding to 4b boundary
	signed char nx, ny, nz, nw;
	unsigned short tx, ty;
};

struct PackedVertexOct
{
	unsigned short px, py, pz;
	signed char nu, nv; // octahedron encoded normal, aliases .pw
	unsigned short tx, ty;
};

struct MeshData
{
	std::vector<VertexData> vertices;
	std::vector<unsigned int> indices;
};



static const size_t lod_count = 5;

size_t lod_index_offsets[lod_count] = {};
size_t lod_index_counts[lod_count] = {};

std::vector<VertexData> verticesNew;


void packMesh(std::vector<PackedVertexOct>& pv, const std::vector<VertexData>& v)
{
	for (size_t i = 0; i < v.size(); ++i)
	{
		const VertexData& vi = v[i];
		PackedVertexOct& pvi = pv[i];

		pvi.px = meshopt_quantizeHalf(vi.px);
		pvi.py = meshopt_quantizeHalf(vi.py);
		pvi.pz = meshopt_quantizeHalf(vi.pz);

		float nsum = fabsf(vi.nx) + fabsf(vi.ny) + fabsf(vi.nz);
		float nx = vi.nx / nsum;
		float ny = vi.ny / nsum;
		float nz = vi.nz;

		float nu = nz >= 0 ? nx : (1 - fabsf(ny)) * (nx >= 0 ? 1 : -1);
		float nv = nz >= 0 ? ny : (1 - fabsf(nx)) * (ny >= 0 ? 1 : -1);

		pvi.nu = char(meshopt_quantizeSnorm(nu, 8));
		pvi.nv = char(meshopt_quantizeSnorm(nv, 8));

		pvi.tx = meshopt_quantizeHalf(vi.tx);
		pvi.ty = meshopt_quantizeHalf(vi.ty);
	}
}




void simplifyComplete(const MeshData& mesh, std::vector<unsigned int> lods[], std::vector<unsigned int>& indices)
{
	// generate 4 LOD levels (1-4), with each subsequent LOD using 70% triangles
	// note that each LOD uses the same (shared) vertex buffer
	;

	lods[0] = mesh.indices;

	for (size_t i = 1; i < lod_count; ++i)
	{
		std::vector<unsigned int>& lod = lods[i];

		float threshold = powf(0.4f, float(i));
		size_t target_index_count = size_t(mesh.indices.size() * threshold) / 3 * 3;
		float target_error = 1e-2f;
		//		float target_error = 0.2;

		// we can simplify all the way from base level or from the last result
		// simplifying from the base level sometimes produces better results, but simplifying from last level is faster
		const std::vector<unsigned int>& source = lods[i - 1];

		if (source.size() < target_index_count)
			target_index_count = source.size();

		lod.resize(source.size());
		size_t reduce = meshopt_simplify(&lod[0], &source[0], source.size(), &mesh.vertices[0].px, mesh.vertices.size(), sizeof(VertexData), target_index_count, target_error);

		lod.resize(reduce);
	}

	// optimize each individual LOD for vertex cache & overdraw
	for (size_t i = 0; i < lod_count; ++i)
	{
		std::vector<unsigned int>& lod = lods[i];

		meshopt_optimizeVertexCache(&lod[0], &lod[0], lod.size(), mesh.vertices.size());
		meshopt_optimizeOverdraw(&lod[0], &lod[0], lod.size(), &mesh.vertices[0].px, mesh.vertices.size(), sizeof(VertexData), 1.0f);
	}

	// concatenate all LODs into one IB
	// note: the order of concatenation is important - since we optimize the entire IB for vertex fetch,
	// putting coarse LODs first makes sure that the vertex range referenced by them is as small as possible
	// some GPUs process the entire range referenced by the index buffer region so doing this optimizes the vertex transform
	// cost for coarse LODs
	// this order also produces much better vertex fetch cache coherency for coarse LODs (since they're essentially optimized first)
	// somewhat surprisingly, the vertex fetch cache coherency for fine LODs doesn't seem to suffer that much.

	size_t total_index_count = 0;

	for (int i = 2 - 1; i >= 0; --i)
	{
		lod_index_offsets[i] = total_index_count;
		lod_index_counts[i] = lods[i].size();

		total_index_count += lods[i].size();
	}

	indices.resize(total_index_count);

	for (size_t i = 0; i < 2; ++i)
	{
		memcpy(&indices[lod_index_offsets[i]], &lods[i][0], lods[i].size() * sizeof(lods[i][0]));
	}

	std::vector<VertexData> vertices = (mesh.vertices);

	// vertex fetch optimization should go last as it depends on the final index order
	// note that the order of LODs above affects vertex fetch results
	meshopt_optimizeVertexFetch(&vertices[0], &indices[0], indices.size(), &vertices[0], vertices.size(), sizeof(VertexData));


	printf("%-9s: %d triangles => %d LOD levels down to %d triangle \n",
		"SimplifyC",
		int(lod_index_counts[0]) / 3, int(lod_count), int(lod_index_counts[lod_count - 1]) / 3);

	// for using LOD data at runtime, in addition to vertices and indices you have to save lod_index_offsets/lod_index_counts.

	{

		const size_t kCacheSize = 16;
		meshopt_VertexCacheStatistics vcs0 = meshopt_analyzeVertexCache(&indices[lod_index_offsets[0]], lod_index_counts[0], vertices.size(), kCacheSize, 0, 0);
		meshopt_VertexFetchStatistics vfs0 = meshopt_analyzeVertexFetch(&indices[lod_index_offsets[0]], lod_index_counts[0], vertices.size(), sizeof(VertexData));
		meshopt_VertexCacheStatistics vcsN = meshopt_analyzeVertexCache(&indices[lod_index_offsets[lod_count - 1]], lod_index_counts[lod_count - 1], vertices.size(), kCacheSize, 0, 0);
		meshopt_VertexFetchStatistics vfsN = meshopt_analyzeVertexFetch(&indices[lod_index_offsets[lod_count - 1]], lod_index_counts[lod_count - 1], vertices.size(), sizeof(VertexData));

		typedef PackedVertexOct PV;

		std::vector<PV> pv(vertices.size());
		packMesh(pv, vertices);

		std::vector<unsigned char> vbuf(meshopt_encodeVertexBufferBound(vertices.size(), sizeof(PV)));
		vbuf.resize(meshopt_encodeVertexBuffer(&vbuf[0], vbuf.size(), &pv[0], vertices.size(), sizeof(PV)));


		std::vector<unsigned char> ibuf(meshopt_encodeIndexBufferBound(indices.size(), vertices.size()));
		ibuf.resize(meshopt_encodeIndexBuffer(&ibuf[0], ibuf.size(), &indices[0], indices.size()));

		printf("%-9s  ACMR %f...%f Overfetch %f..%f Codec VB %.1f bits/vertex IB %.1f bits/triangle\n",
			"",
			vcs0.acmr, vcsN.acmr, vfs0.overfetch, vfsN.overfetch,
			double(vbuf.size()) / double(vertices.size()) * 8,
			double(ibuf.size()) / double(indices.size() / 3) * 8);
	}

	verticesNew = vertices;
}





void debugVertexBoneData(unsigned int total_vertices, vector<VertexBoneData> Bones)
{
	std::ofstream log;
	log.open("VertexBoneData.txt");


	unsigned int i, j;
	for (i = 0; i < total_vertices; i++)
	{
		log << "\nBone[" << Bones[i].BoneIDs[0] << "], weight= " << Bones[i].Weights[0];
		log << "\nBone[" << Bones[i].BoneIDs[1] << "], weight= " << Bones[i].Weights[1];
		log << "\nBone[" << Bones[i].BoneIDs[2] << "], weight= " << Bones[i].Weights[2];
		log << "\nBone[" << Bones[i].BoneIDs[3] << "], weight= " << Bones[i].Weights[3];
		log << "---------------\n";
	}

	log.close();
}

void debugSkeletonPose(map<unsigned int, glm::vec3> skeletonPos) {
	for (auto it = skeletonPos.cbegin(); it != skeletonPos.cend(); ++it)
	{
		std::cout << it->first << " " << it->second.x << " " << it->second.y << " " << it->second.z << "\n";
	}
}

glm::quat quatcast(glm::mat4 t) {
	glm::quat q;
	float T = 1 + t[0][0] + t[1][1] + t[2][2];
	float S, X, Y, Z, W;

	if (T > 0.0000001f) {
		S = glm::sqrt(T) * 2.f;
		X = (t[1][2] - t[2][1]) / S;
		Y = (t[2][0] - t[0][2]) / S;
		Z = (t[0][1] - t[1][0]) / S;
		W = 0.25f * S;
	}
	else
	{
		if (t[0][0] > t[1][1] && t[0][0] > t[2][2])
		{
			// Column 0 :
			S = sqrt(1.0f + t[0][0] - t[1][1] - t[2][2]) * 2.f;
			X = 0.25f * S;
			Y = (t[0][1] + t[1][0]) / S;
			Z = (t[2][0] + t[0][2]) / S;
			W = (t[1][2] - t[2][1]) / S;
		}
		else if (t[1][1] > t[2][2])
		{
			// Column 1 :
			S = sqrt(1.0f + t[1][1] - t[0][0] - t[2][2]) * 2.f;
			X = (t[0][1] + t[1][0]) / S;
			Y = 0.25f * S;
			Z = (t[1][2] + t[2][1]) / S;
			W = (t[2][0] - t[0][2]) / S;
		}
		else
		{   // Column 2 :
			S = sqrt(1.0f + t[1][1] - t[0][0] - t[1][1]) * 2.f;
			X = (t[2][0] + t[0][2]) / S;
			Y = (t[1][2] + t[2][1]) / S;
			Z = 0.25f * S;
			W = (t[0][1] - t[1][0]) / S;
		}
	}
	q.w = W; q.x = -X; q.y = -Y; q.z = -Z;
	return q;
}

glm::mat3x4 convertMatrix(glm::mat4 s) {
	glm::mat3x4 t;
	t[0][0] = s[0][0];
	t[0][1] = s[0][1];
	t[0][2] = s[0][2];
	t[0][3] = s[0][3];

	t[1][0] = s[1][0];
	t[1][1] = s[1][1];
	t[1][2] = s[1][2];
	t[1][3] = s[1][3];

	t[2][0] = s[2][0];
	t[2][1] = s[2][1];
	t[2][2] = s[2][2];
	t[2][3] = s[2][3];

	return t;
}


// constructor, expects a filepath to a 3D model.
Model::Model(string const& path, bool gamma = false, int k = 0) : gammaCorrection(gamma)
{
	type = k;
	loadModel(path);


}

float Model::getDuration()
{
	float duration;
	if (scene->mAnimations != NULL) {
		unsigned int numPosKeys = scene->mAnimations[0]->mChannels[0]->mNumPositionKeys;
		duration = scene->mAnimations[0]->mChannels[0]->mPositionKeys[numPosKeys - 1].mTime;
	}
	else
		duration = 1.0f;
	return duration;
}

void Model::setConstraints(float x, float y)
{

	m_constraints.push_back(Interval(x, y));
}

// draws the model, and thus all its meshes
void Model::Draw(int num)
{
	for (unsigned int i = 0; i < meshes.size(); i++)
		meshes[i].Draw(num);
}

int Model::BoneTransform(float TimeInSeconds, vector<glm::mat4>& Transforms, vector<glm::fdualquat>& dqs)
{
	glm::mat4 Identity = glm::mat4(1.0f);

	//initialization
	if (scene->mNumAnimations == 0) {
		Transforms.resize(m_NumBones);
		dqs.resize(m_NumBones);
		for (unsigned int i = 0; i < m_NumBones; ++i) {
			Transforms[i] = glm::mat4(1.0f);
			dqs[i] = IdentityDQ;
		}
		return 0;
	}


	unsigned int numPosKeys = scene->mAnimations[0]->mChannels[0]->mNumPositionKeys;

	float TicksPerSecond = scene->mAnimations[0]->mTicksPerSecond != 0 ? scene->mAnimations[0]->mTicksPerSecond : 25.0f;


	float TimeInTicks = TimeInSeconds * TicksPerSecond;

	float startTime = scene->mAnimations[0]->mChannels[0]->mPositionKeys[0].mTime;
	float duration = scene->mAnimations[0]->mChannels[0]->mPositionKeys[numPosKeys - 1].mTime - startTime;
	float AnimationTime = fmod(TimeInTicks, duration);



	//
	ReadNodeHeirarchy(scene, AnimationTime, scene->mRootNode, Identity, IdentityDQ, glm::vec3(0.0f, 0.0f, 0.0f));


	Transforms.resize(m_NumBones);
	dqs.resize(m_NumBones);


	for (unsigned int i = 0; i < m_NumBones; ++i) {
		Transforms[i] = glm::mat4(1.0f);
		Transforms[i] = m_BoneInfo[i].FinalTransformation;
	}

	for (unsigned int i = 0; i < dqs.size(); ++i) {
		dqs[i] = IdentityDQ;
		dqs[i] = m_BoneInfo[i].FinalTransDQ;
	}


	for (int i = 0; i < m_constraints.size(); i++) {
		if (m_constraints[i].contains(AnimationTime)) return 1;
	}

	return 0;
}

void Model::loadModel(string const& path)
{

	//	if (type == 0)
	scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_ValidateDataStructure | aiProcess_OptimizeMeshes | aiProcess_CalcTangentSpace | aiProcess_GenSmoothNormals);  //aiProcess_FixInfacingNormals

	//scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_CalcTangentSpace |  aiProcess_FlipUVs | aiProcess_GenNormals | aiProcess_FixInfacingNormals);

	// check for errors
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
	{
		cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
		return;
	}

	// retrieve the directory path of the filepath
	directory = path.substr(0, path.find_last_of('/'));

	loadBones(scene->mRootNode, scene);

	m_BoneInfo.resize(Bone_Mapping.size());

	// process ASSIMP's root node recursively
	processNode(scene->mRootNode, scene);
}



void Model::loadBones(aiNode* node, const aiScene* scene)
{
	for (unsigned int i = 0; i < node->mNumChildren; i++) {
		string NodeName(node->mChildren[i]->mName.data);
		if (NodeName.find(":") != string::npos) {
			string BoneName = NodeName;
			unsigned int BoneIndex = 0;

			if (Bone_Mapping.find(BoneName) == Bone_Mapping.end()) {
				BoneIndex = m_NumBones;
				m_NumBones++;
				Bone_Mapping[BoneName] = BoneIndex;
			}
		}

		if (NodeName != "Body" && NodeName != "metarig" && NodeName != "parasiteZombie" && NodeName != "Armature" && NodeName != "MutantMesh" && NodeName != "Cylinder") {
			string BoneName = NodeName;
			unsigned int BoneIndex = 0;

			if (Bone_Mapping.find(BoneName) == Bone_Mapping.end()) {
				BoneIndex = m_NumBones;
				m_NumBones++;
				Bone_Mapping[BoneName] = BoneIndex;
			}
		}
		//only uncomment if we need to load cylinder model
	/*else {
	string BoneName(node->mChildren[i]->mName.data);
	unsigned int BoneIndex = 0;
	if (NodeName != "parasiteZombie" || NodeName != "Armature") {
		if (Bone_Mapping.find(BoneName) == Bone_Mapping.end()) {
			BoneIndex = m_NumBones;
			m_NumBones++;
			Bone_Mapping[BoneName] = BoneIndex;
		}
	}

}*/

	}

	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		loadBones(node->mChildren[i], scene);
	}

}

// processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
void Model::processNode(aiNode* node, const aiScene* scene)
{
	// process each mesh located at the current node
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		// the node object only contains indices to index the actual objects in the scene. 
		// the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		total_vertices += mesh->mNumVertices;
		meshes.push_back(processMesh(mesh, scene));
	}
	// after we've processed all of the meshes (if any) we then recursively process each of the children nodes
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		processNode(node->mChildren[i], scene);
	}
}

// process a mesh object
Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene)
{
	// data to fill
	vector<Vertex> vertices;
	vector<unsigned int> indices;
	vector<Textures> textures;

	Bones.resize(total_vertices);

	// Walk through each of the mesh's vertices
	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;
		glm::vec3 vector;
		glm::vec3 normal;

		//retreive positions
		vector.x = mesh->mVertices[i].x;
		vector.y = mesh->mVertices[i].y;
		vector.z = mesh->mVertices[i].z;
		vertex.Position = vector;

		if (mesh->HasNormals()) {

			//retreive normals
			normal.x = mesh->mNormals[i].x;
			normal.y = mesh->mNormals[i].y;
			normal.z = mesh->mNormals[i].z;
			vertex.Normal = normal;
		}
		//	std::cout << glm::to_string(normal) << std::endl;

			//retreive texture coordinates
		if (mesh->mTextureCoords[0])
		{
			glm::vec2 vec;
			vec.x = mesh->mTextureCoords[0][i].x;
			vec.y = mesh->mTextureCoords[0][i].y;
			vertex.TexCoords = vec;
		}
		else {
			vertex.TexCoords = glm::vec2(0.0f, 0.0f);
		}
		vertices.push_back(vertex);
	}

	//retreive indices
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}

	if (mesh->mMaterialIndex >= 0)
	{
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		vector<Textures> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
	}

	// process materials
	aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
	// 1. diffuse maps
	vector<Textures> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
	textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
	// 2. specular maps
	vector<Textures> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
	textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
	// 3. normal maps
	std::vector<Textures> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
	textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
	// 4. height maps
	std::vector<Textures> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
	textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());


	///////// for simplification /////////

	MeshData data;
	for (int i = 0; i < mesh->mNumVertices; ++i) {
		VertexData v;
		v.px = mesh->mVertices[i].x;
		v.py = mesh->mVertices[i].y;
		v.pz = mesh->mVertices[i].z;

		v.nx = mesh->mNormals[i].x;
		v.ny = mesh->mNormals[i].y;
		v.nz = mesh->mNormals[i].z;


		v.tx = 0;
		v.ty = 0;


		data.vertices.push_back(v);

	}
	for (int i = 0; i < mesh->mNumFaces; ++i) {

		data.indices.push_back(mesh->mFaces[i].mIndices[0]);
		data.indices.push_back(mesh->mFaces[i].mIndices[1]);
		data.indices.push_back(mesh->mFaces[i].mIndices[2]);
	}

	std::vector<unsigned int> lods[5];
	std::vector<unsigned int> indicesCombined;
	simplifyComplete(data, lods, indicesCombined);


	///////

	//retreive bone information
	loadMeshBones(mesh, Bones);
	// return a mesh object created from the extracted mesh data
	return Mesh(vertices, indices, textures, m_BoneInfo, Bones);  //size of bones = # of vertices
}

// checks all material textures of a given type and loads the textures if they're not loaded yet.
// the required info is returned as a Texture struct.
vector<Textures> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName)
{
	vector<Textures> textures;
	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);
		// check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
		bool skip = false;
		for (unsigned int j = 0; j < textures_loaded.size(); j++)
		{
			if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
			{
				textures.push_back(textures_loaded[j]);
				skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
				break;
			}
		}
		if (!skip)
		{   // if texture hasn't been loaded already, load it
			Textures texture;

			texture.id = TextureFromFile(str.C_Str(), this->directory);

			texture.type = typeName;
			texture.path = str.C_Str();
			textures.push_back(texture);
			textures_loaded.push_back(texture);  // store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
		}
	}
	return textures;
}

void Model::loadMeshBones(aiMesh* mesh, vector<VertexBoneData>& VertexBoneData)
{

	for (unsigned int i = 0; i < mesh->mNumBones; i++)
	{

		unsigned int BoneIndex = 0;
		string BoneName(mesh->mBones[i]->mName.data);


		//	std::cout << BoneName << std::endl;


		if (Bone_Mapping.find(BoneName) != Bone_Mapping.end())
		{
			BoneIndex = Bone_Mapping[BoneName];  //assign name and index

			aiMatrix4x4 tp1 = mesh->mBones[i]->mOffsetMatrix;
			m_BoneInfo[BoneIndex].offset = glm::transpose(glm::make_mat4(&tp1.a1));
		}
		else {
			std::cout << "error" << std::endl;
		}

		int nn = mesh->mBones[i]->mNumWeights;

		for (unsigned int n = 0; n < nn; n++) {
			unsigned int vid = mesh->mBones[i]->mWeights[n].mVertexId + NumVertices;//absolute index
			float weight = mesh->mBones[i]->mWeights[n].mWeight;
			VertexBoneData[vid].AddBoneData(BoneIndex, weight);// for a particular vertex, which bones affect it and its weight
		}
		//from the scene, for the boneName, find the position, orientation and scale
		loadAnimations(scene, BoneName, Animations);
	}
	NumVertices += mesh->mNumVertices;
}

//get animation from the bone
//populate the animation map : animation_map[animation_name][bone_name] -> animation
void Model::loadAnimations(const aiScene* scene, string BoneName, map<string, map<string, const aiNodeAnim*>>& animations)
{
	for (unsigned int i = 0; i < scene->mNumAnimations; i++) {

		const aiAnimation* pAnimation = scene->mAnimations[i];
		string animName = pAnimation->mName.data;

		bool found = false;
		for (unsigned int j = 0; j < pAnimation->mNumChannels; j++) {
			string name = pAnimation->mChannels[j]->mNodeName.data;

			if (name == BoneName) {
				animations[animName][BoneName] = pAnimation->mChannels[j];
				found = true;
				break;
			}


		}
		if (!found)
			std::cout << "Error : no find animation for " << BoneName << std::endl;
	}
}

void Model::ReadNodeHeirarchy(const aiScene* scene, float AnimationTime, const aiNode* pNode, const glm::mat4& ParentTransform, const glm::fdualquat& ParentDQ, glm::vec3 startpos)
{
	string NodeName(pNode->mName.data);
	const aiAnimation* pAnimation = scene->mAnimations[0];

	glm::mat4 NodeTransformation = glm::mat4(1.0f);
	glm::fdualquat NodeTransformationDQ = IdentityDQ;


	aiMatrix4x4 tp1 = pNode->mTransformation;
	NodeTransformation = glm::transpose(glm::make_mat4(&tp1.a1));


	const aiNodeAnim* pNodeAnim = nullptr;
	pNodeAnim = Animations[pAnimation->mName.data][NodeName];


	if (pNodeAnim) {

		//Interpolate rotation and generate rotation transformation matrix
		aiQuaternion RotationQ;
		CalcInterpolatedRotaion(RotationQ, AnimationTime, pNodeAnim);


		aiMatrix3x3 tp = RotationQ.GetMatrix();
		//convert to glm 4x4 matrix
		glm::mat4 RotationM = glm::transpose(glm::make_mat3(&tp.a1));


		//Interpolate translation and generate translation transformation matrix
		aiVector3D Translation;
		CalcInterpolatedPosition(Translation, AnimationTime, pNodeAnim);


		aiVector3D Scale;
		CalcInterpolatedScaling(Scale, AnimationTime, pNodeAnim);



		glm::mat4 TranslationM = glm::mat4(1.0f);
		//get the 4x4 translation matrix
		TranslationM = glm::translate(TranslationM, glm::vec3(Translation.x, Translation.y, Translation.z));


		glm::mat4 ScaleM = glm::mat4(1.0f);
		//get the 4x4 translation matrix

		ScaleM = glm::scale(ScaleM, glm::vec3(Scale.x, Scale.y, Scale.z));

		//combine rotation and translation matrix
		NodeTransformation = TranslationM * RotationM;

		//full dual quaternion 
		NodeTransformationDQ = glm::normalize(glm::fdualquat(glm::normalize(glm::quat_cast(NodeTransformation)), glm::vec3(NodeTransformation[3][0], NodeTransformation[3][1], NodeTransformation[3][2])));

	}

	//world transform in matrix
	glm::mat4 GlobalTransformation = ParentTransform * NodeTransformation;

	//world dual quaternion 
	glm::fdualquat GlobalDQ = glm::normalize(ParentDQ * NodeTransformationDQ);



	unsigned int ID = 0;



	if (Bone_Mapping.find(NodeName) != Bone_Mapping.end()) {
		startpos.x = GlobalTransformation[3][0];
		startpos.y = GlobalTransformation[3][1];
		startpos.z = GlobalTransformation[3][2];
		ID = Bone_Mapping[NodeName];
		skeleton_pose[ID] = startpos;


	}

	if (Bone_Mapping.find(NodeName) != Bone_Mapping.end()) {
		unsigned int NodeIndex = Bone_Mapping[NodeName];


		m_BoneInfo[NodeIndex].FinalTransformation = GlobalTransformation * m_BoneInfo[NodeIndex].offset;

		glm::fdualquat offsetDQ = glm::normalize(glm::fdualquat(glm::normalize(glm::quat_cast(m_BoneInfo[NodeIndex].offset)), glm::vec3(m_BoneInfo[NodeIndex].offset[3][0], m_BoneInfo[NodeIndex].offset[3][1], m_BoneInfo[NodeIndex].offset[3][2])));

		m_BoneInfo[NodeIndex].FinalTransDQ = glm::normalize(GlobalDQ * offsetDQ);



		//	m_BoneInfo[NodeIndex].FinalTransDQ = 
			//glm::mat3x4 t = convertMatrix(m_BoneInfo[NodeIndex].FinalTransformation);
			//m_BoneInfo[NodeIndex].FinalTransDQ = glm::dualquat_cast(t);

			//dual quaternion 
			//glm::fquat FinalQuat = glm::quat_cast(m_BoneInfo[NodeIndex].FinalTransformation);
			//glm::quat finalQuat = quatcast(m_BoneInfo[NodeIndex].FinalTransformation);

			/* m_BoneInfo[NodeIndex].FinalTransDQ = glm::fdualquat(glm::normalize(FinalQuat), glm::vec3(m_BoneInfo[NodeIndex].FinalTransformation[3][0],
																										m_BoneInfo[NodeIndex].FinalTransformation[3][1],
																										m_BoneInfo[NodeIndex].FinalTransformation[3][2]));
			*/
	}

	for (unsigned int i = 0; i < pNode->mNumChildren; i++) {
		ReadNodeHeirarchy(scene, AnimationTime, pNode->mChildren[i], GlobalTransformation, GlobalDQ, startpos);
	}
}




void Model::CalcInterpolatedScaling(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	if (pNodeAnim->mNumScalingKeys == 1) {
		Out = pNodeAnim->mScalingKeys[0].mValue;
		return;
	}

	unsigned int ScalingIndex = FindScaling(AnimationTime, pNodeAnim);
	unsigned int NextScalingIndex = (ScalingIndex + 1);
	assert(NextScalingIndex < pNodeAnim->mNumScalingKeys);
	float DeltaTime = (float)(pNodeAnim->mScalingKeys[NextScalingIndex].mTime - pNodeAnim->mScalingKeys[ScalingIndex].mTime);
	float Factor = (AnimationTime - (float)pNodeAnim->mScalingKeys[ScalingIndex].mTime) / DeltaTime;
	assert(Factor >= 0.0f && Factor <= 1.0f);
	const aiVector3D& Start = pNodeAnim->mScalingKeys[ScalingIndex].mValue;
	const aiVector3D& End = pNodeAnim->mScalingKeys[NextScalingIndex].mValue;
	aiVector3D Delta = End - Start;
	Out = Start + Factor * Delta;
}

void Model::CalcInterpolatedRotaion(aiQuaternion& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	// we need at least two values to interpolate...
	if (pNodeAnim->mNumRotationKeys == 1) {
		Out = pNodeAnim->mRotationKeys[0].mValue;
		return;
	}

	unsigned int RotationIndex = FindRotation(AnimationTime, pNodeAnim);
	unsigned int NextRotationIndex = (RotationIndex + 1);
	assert(NextRotationIndex < pNodeAnim->mNumRotationKeys);
	float DeltaTime = (float)(pNodeAnim->mRotationKeys[NextRotationIndex].mTime - pNodeAnim->mRotationKeys[RotationIndex].mTime);
	float Factor = (AnimationTime - (float)pNodeAnim->mRotationKeys[RotationIndex].mTime) / DeltaTime;
	assert(Factor >= 0.0f && Factor <= 1.0f);
	const aiQuaternion& StartRotationQ = pNodeAnim->mRotationKeys[RotationIndex].mValue;
	const aiQuaternion& EndRotationQ = pNodeAnim->mRotationKeys[NextRotationIndex].mValue;
	aiQuaternion::Interpolate(Out, StartRotationQ, EndRotationQ, Factor);
	Out = Out.Normalize();//normalized
}

void Model::CalcInterpolatedPosition(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	if (pNodeAnim->mNumPositionKeys == 1) {
		Out = pNodeAnim->mPositionKeys[0].mValue;
		return;
	}

	unsigned int PositionIndex = FindPosition(AnimationTime, pNodeAnim);
	unsigned int NextPositionIndex = (PositionIndex + 1);
	assert(NextPositionIndex < pNodeAnim->mNumPositionKeys);
	float DeltaTime = (float)(pNodeAnim->mPositionKeys[NextPositionIndex].mTime - pNodeAnim->mPositionKeys[PositionIndex].mTime);
	float Factor = (AnimationTime - (float)pNodeAnim->mPositionKeys[PositionIndex].mTime) / DeltaTime;
	assert(Factor >= 0.0f && Factor <= 1.0f);
	const aiVector3D& Start = pNodeAnim->mPositionKeys[PositionIndex].mValue;
	const aiVector3D& End = pNodeAnim->mPositionKeys[NextPositionIndex].mValue;
	aiVector3D Delta = End - Start;
	Out = Start + Factor * Delta;
}

unsigned int Model::FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	assert(pNodeAnim->mNumScalingKeys > 0);
	for (unsigned int i = 0; i < pNodeAnim->mNumScalingKeys - 1; i++) {
		if (AnimationTime < (float)pNodeAnim->mScalingKeys[i + 1].mTime) {
			return i;
		}
	}
	assert(0);
	return 0;
}

unsigned int Model::FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	assert(pNodeAnim->mNumRotationKeys > 0);
	for (unsigned int i = 0; i < pNodeAnim->mNumRotationKeys - 1; i++) {
		if (AnimationTime < (float)pNodeAnim->mRotationKeys[i + 1].mTime) {
			return i;
		}
	}

	assert(0);
	return 0;
}

unsigned int Model::FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	for (unsigned int i = 0; i < pNodeAnim->mNumPositionKeys - 1; i++) {
		if (AnimationTime < (float)pNodeAnim->mPositionKeys[i + 1].mTime) {
			return i;
		}
	}
	assert(0);
	return 0;
}


