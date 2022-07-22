#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#define GLM_FORCE_CTOR_INIT


#include <GL/gl3w.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>

#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/dual_quaternion.hpp>


#include "Loader.h"
#include "Mesh.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>

#include "assimp\scene.h"
#include "assimp\Importer.hpp"
#include "assimp\postprocess.h"


//#include "debug.h"

#include "Interval.h"

using std::string;

class Model
{
public:

	/*  Model Data */
	vector<Textures> textures_loaded;	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
	vector<Mesh> meshes;


	string directory;


	bool gammaCorrection;
	int type;



	unsigned int total_vertices = 0;

	/*Bone Data*/
	unsigned int m_NumBones = 0;


	vector<VertexBoneData> Bones;  //per vertex, contains bond id and weights




	map<string, unsigned int> Bone_Mapping;
	map<string, map<string, const aiNodeAnim*>> Animations;



	map<unsigned int, glm::vec3> skeleton_pose;
	map<string, unsigned int> Node_Mapping;


	vector<BoneInfo> m_BoneInfo;
	unsigned int NumVertices = 0;


	glm::fdualquat IdentityDQ = glm::fdualquat(glm::quat(1.f, 0.f, 0.f, 0.f), glm::quat(0.f, 0.f, 0.f, 0.f));

	/*  Functions   */
	// constructor, expects a filepath to a 3D model.
	Model(string const& path, bool gamma, int k);

	// draws the model, and thus all its meshes
	void Draw(int num);
	int BoneTransform(float TimeInSeconds, vector<glm::mat4>& Transforms, vector<glm::fdualquat>& dqs);

	std::vector<Interval> m_constraints;
	void setConstraints(float x, float y);
	float getDuration();

private:
	const aiScene* scene;
	Assimp::Importer importer;
	// loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
	void loadModel(string const& path);
	void loadBones(aiNode* node, const aiScene* scene);
	// processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
	void processNode(aiNode* node, const aiScene* scene);


	// process a mesh object
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);


	// checks all material textures of a given type and loads the textures if they're not loaded yet.
	// the required info is returned as a Texture struct.
	vector<Textures> loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName);
	void loadMeshBones(aiMesh* mesh, vector<VertexBoneData>& VertexBoneData);


	//get animation from the bone
	//populate the animation map : animation_map[animation_name][bone_name] -> animation
	void loadAnimations(const aiScene* scene, string BoneName, map<string, map<string, const aiNodeAnim*>>& animations);


	void ReadNodeHeirarchy(const aiScene* scene, float AnimationTime, const aiNode* pNode,
		const glm::mat4& ParentTransform, const glm::fdualquat& ParentDQ, glm::vec3 startpos);



	void CalcInterpolatedScaling(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
	void CalcInterpolatedRotaion(aiQuaternion& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
	void CalcInterpolatedPosition(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);



	unsigned int FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim);


	unsigned int FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim);
	unsigned int FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim);

};
