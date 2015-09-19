#include <scene/scene.hpp>
#include <maths/matrixWeird.hpp>
#include <texturesLoading/texturesLoading.hpp>

extern int checker_texture;

/*
 * Init, animate and draw aiScene.
 *
 * We build our own list of the meshes to keep the vertex data
 * in a format suitable for use with OpenGL. We also store the
 * resulting meshes during skeletal animation here.
 */

extern int meshcount;

struct mesh {
	struct aiMesh *mesh;
	unsigned int texture;
	int vertexcount, elementcount;
	float *position;
	float *normal;
	float *texcoord;
	int *element;
} *meshlist = NULL;

// find a node by name in the hierarchy (for anims and bones)
struct aiNode *findnode(struct aiNode *node, char *name)
{
	int i;
	if (!strcmp(name, node->mName.data))
		return node;
	for (i = 0; i < node->mNumChildren; i++) {
		struct aiNode *found = findnode(node->mChildren[i], name);
		if (found)
			return found;
	}
	return NULL;
}

// calculate absolute transform for node to do mesh skinning
void transformnode(aiMatrix4x4 *result, struct aiNode *node)
{
	if (node->mParent) {
		transformnode(result, node->mParent);
		aiMultiplyMatrix4(result, &node->mTransformation);
	} else {
		*result = node->mTransformation;
	}
}

void transformmesh(struct aiScene *scene, struct mesh *mesh)
{
	struct aiMesh *amesh = mesh->mesh;
	aiMatrix4x4 skin4;
	aiMatrix3x3 skin3;
	int i, k;

	if (amesh->mNumBones == 0)
		return;

	memset(mesh->position, 0, mesh->vertexcount * 3 * sizeof(float));
	memset(mesh->normal, 0, mesh->vertexcount * 3 * sizeof(float));

	for (k = 0; k < amesh->mNumBones; k++) {
		struct aiBone *bone = amesh->mBones[k];
		struct aiNode *node = findnode(scene->mRootNode, bone->mName.data);

		transformnode(&skin4, node);
		aiMultiplyMatrix4(&skin4, &bone->mOffsetMatrix);
		extract3x3(&skin3, &skin4);

		for (i = 0; i < bone->mNumWeights; i++) {
			int v = bone->mWeights[i].mVertexId;
			float w = bone->mWeights[i].mWeight;

			aiVector3D position = amesh->mVertices[v];
			aiTransformVecByMatrix4(&position, &skin4);
			mesh->position[v*3+0] += position.x * w;
			mesh->position[v*3+1] += position.y * w;
			mesh->position[v*3+2] += position.z * w;

			aiVector3D normal = amesh->mNormals[v];
			aiTransformVecByMatrix3(&normal, &skin3);
			mesh->normal[v*3+0] += normal.x * w;
			mesh->normal[v*3+1] += normal.y * w;
			mesh->normal[v*3+2] += normal.z * w;
		}
	}
}

void initmesh(struct aiScene *scene, struct mesh *mesh, struct aiMesh *amesh)
{
	int i;

	mesh->mesh = amesh; // stow away pointer for bones

	mesh->texture = loadmaterial(scene->mMaterials[amesh->mMaterialIndex]);

	mesh->vertexcount = amesh->mNumVertices;
	mesh->position = (float *)calloc(mesh->vertexcount * 3, sizeof(float));
	mesh->normal = (float *)calloc(mesh->vertexcount * 3, sizeof(float));
	mesh->texcoord = (float *)calloc(mesh->vertexcount * 2, sizeof(float));

	for (i = 0; i < mesh->vertexcount; i++) {
		mesh->position[i*3+0] = amesh->mVertices[i].x;
		mesh->position[i*3+1] = amesh->mVertices[i].y;
		mesh->position[i*3+2] = amesh->mVertices[i].z;

		if (amesh->mNormals) {
			mesh->normal[i*3+0] = amesh->mNormals[i].x;
			mesh->normal[i*3+1] = amesh->mNormals[i].y;
			mesh->normal[i*3+2] = amesh->mNormals[i].z;
		}

		if (amesh->mTextureCoords[0]) {
			mesh->texcoord[i*2+0] = amesh->mTextureCoords[0][i].x;
			mesh->texcoord[i*2+1] = 1 - amesh->mTextureCoords[0][i].y;
		}
	}

	mesh->elementcount = amesh->mNumFaces * 3;
	mesh->element = (int *)calloc(mesh->elementcount, sizeof(int));

	for (i = 0; i < amesh->mNumFaces; i++) {
		struct aiFace *face = amesh->mFaces + i;
		mesh->element[i*3+0] = face->mIndices[0];
		mesh->element[i*3+1] = face->mIndices[1];
		mesh->element[i*3+2] = face->mIndices[2];
	}
}

void initscene(struct aiScene *scene)
{
	int i;
	meshcount = scene->mNumMeshes;
	meshlist = (struct mesh *)calloc(meshcount, sizeof (*meshlist));
	for (i = 0; i < meshcount; i++) {
		initmesh(scene, meshlist + i, scene->mMeshes[i]);
		transformmesh(scene, meshlist + i);
	}
}

void drawmesh(struct mesh *mesh)
{
	// if (mesh->texture > 0) {
	// 	glColor4f(1, 1, 1, 1);
	// 	glBindTexture(GL_TEXTURE_2D, mesh->texture);
	// } else {
	// 	glColor4f(0.9, 0.7, 0.7, 1);
	// 	glBindTexture(GL_TEXTURE_2D, checker_texture);
	// }
	// glVertexPointer(3, GL_FLOAT, 0, mesh->position);
	// glNormalPointer(GL_FLOAT, 0, mesh->normal);
	// glTexCoordPointer(2, GL_FLOAT, 0, mesh->texcoord);
	// glDrawElements(GL_TRIANGLES, mesh->elementcount, GL_UNSIGNED_INT, mesh->element);
}

void drawnode(aiNode *node, aiMatrix4x4 world)
{
	// float mat[16];
	// int i;

	// aiMultiplyMatrix4(&world, &node->mTransformation);
	// transposematrix(mat, &world);

	// for (i = 0; i < node->mNumMeshes; i++) {
	// 	struct mesh *mesh = meshlist + node->mMeshes[i];
	// 	if (mesh->mesh->mNumBones == 0) {
	// 		// non-skinned meshes are in node-local space
	// 		glPushMatrix();
	// 		glMultMatrixf(mat);
	// 		drawmesh(mesh);
	// 		glPopMatrix();
	// 	} else {
	// 		// skinned meshes are already in world space
	// 		drawmesh(mesh);
	// 	}
	// }

	// for (i = 0; i < node->mNumChildren; i++)
	// 	drawnode(node->mChildren[i], world);
}

void drawscene(struct aiScene *scene)
{
	// aiMatrix4x4 world;

	// glEnableClientState(GL_VERTEX_ARRAY);
	// glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	// glEnableClientState(GL_NORMAL_ARRAY);

	// aiIdentityMatrix4(&world);
	// drawnode(scene->mRootNode, world);

	// glDisableClientState(GL_VERTEX_ARRAY);
	// glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	// glDisableClientState(GL_NORMAL_ARRAY);
}

void measuremesh(struct mesh *mesh, aiMatrix4x4 transform, float bboxmin[3], float bboxmax[3])
{
	aiVector3D p;
	int i;
	for (i = 0; i < mesh->vertexcount; i++) {
		p.x = mesh->position[i*3+0];
		p.y = mesh->position[i*3+1];
		p.z = mesh->position[i*3+2];
		aiTransformVecByMatrix4(&p, &transform);
		bboxmin[0] = MIN(bboxmin[0], p.x);
		bboxmin[1] = MIN(bboxmin[1], p.y);
		bboxmin[2] = MIN(bboxmin[2], p.z);
		bboxmax[0] = MAX(bboxmax[0], p.x);
		bboxmax[1] = MAX(bboxmax[1], p.y);
		bboxmax[2] = MAX(bboxmax[2], p.z);
	}
}

void measurenode(aiNode *node, aiMatrix4x4 world, float bboxmin[3], float bboxmax[3])
{
	aiMatrix4x4 identity;
	int i;

	aiMultiplyMatrix4(&world, &node->mTransformation);
	aiIdentityMatrix4(&identity);

	for (i = 0; i < node->mNumMeshes; i++) {
		struct mesh *mesh = meshlist + node->mMeshes[i];
		if (mesh->mesh->mNumBones == 0) {
			// non-skinned meshes are in node-local space
			measuremesh(mesh, world, bboxmin, bboxmax);
		} else {
			// skinned meshes are already in world space
			measuremesh(mesh, identity, bboxmin, bboxmax);
		}
	}

	for (i = 0; i < node->mNumChildren; i++)
		measurenode(node->mChildren[i], world, bboxmin, bboxmax);
}

float measurescene(struct aiScene *scene, float center[3])
{
	aiMatrix4x4 world;
	float bboxmin[3];
	float bboxmax[3];
	float dx, dy, dz;

	bboxmin[0] = 1e10; bboxmax[0] = -1e10;
	bboxmin[1] = 1e10; bboxmax[1] = -1e10;
	bboxmin[2] = 1e10; bboxmax[2] = -1e10;

	aiIdentityMatrix4(&world);
	measurenode(scene->mRootNode, world, bboxmin, bboxmax);

	center[0] = (bboxmin[0] + bboxmax[0]) / 2;
	center[1] = (bboxmin[1] + bboxmax[1]) / 2;
	center[2] = (bboxmin[2] + bboxmax[2]) / 2;

	dx = MAX(center[0] - bboxmin[0], bboxmax[0] - center[0]);
	dy = MAX(center[1] - bboxmin[1], bboxmax[1] - center[1]);
	dz = MAX(center[2] - bboxmin[2], bboxmax[2] - center[2]);

	return sqrt(dx*dx + dy*dy + dz*dz);
}

int animationlength(struct aiAnimation *anim)
{
	int i, len = 0;
	for (i = 0; i < anim->mNumChannels; i++) {
		struct aiNodeAnim *chan = anim->mChannels[i];
		len = MAX(len, chan->mNumPositionKeys);
		len = MAX(len, chan->mNumRotationKeys);
		len = MAX(len, chan->mNumScalingKeys);
	}
	return len;
}

void animatescene(struct aiScene *scene, struct aiAnimation *anim, float tick)
{
	struct aiVectorKey *p0, *p1, *s0, *s1;
	struct aiQuatKey *r0, *r1;
	aiVector3D p, s;
	aiQuaternion r;
	int i;

	// Assumes even key frame rate and synchronized pos/rot/scale keys.
	// We should look at the key->mTime values instead, but I'm lazy.

	int frame = floor(tick);
	float t = tick - floor(tick);

	for (i = 0; i < anim->mNumChannels; i++) {
		struct aiNodeAnim *chan = anim->mChannels[i];
		struct aiNode *node = findnode(scene->mRootNode, chan->mNodeName.data);
		p0 = chan->mPositionKeys + (frame+0) % chan->mNumPositionKeys;
		p1 = chan->mPositionKeys + (frame+1) % chan->mNumPositionKeys;
		r0 = chan->mRotationKeys + (frame+0) % chan->mNumRotationKeys;
		r1 = chan->mRotationKeys + (frame+1) % chan->mNumRotationKeys;
		s0 = chan->mScalingKeys + (frame+0) % chan->mNumScalingKeys;
		s1 = chan->mScalingKeys + (frame+1) % chan->mNumScalingKeys;
		mixvector(&p, &p0->mValue, &p1->mValue, t);
		mixquaternion(&r, &r0->mValue, &r1->mValue, t);
		mixvector(&s, &s0->mValue, &s1->mValue, t);
		composematrix(&node->mTransformation, &p, &r, &s);
	}

	for (i = 0; i < meshcount; i++)
		transformmesh(scene, meshlist + i);
}