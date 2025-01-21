#include "WalkingDemo.h"
#include <functional>
#include "../cgltf.h"
#include "../io/gltfLoader.h"
#include <iostream>
#include "../render/Uniform.h"
#include "../animation/Blending.h"

#define numbLegBones 3
#define rayHeightAboveGeometry 10.0f
#define walkSlowDown 0.3f
#define walkPathDuration 6.0f
// speedUp helps ease the models position and rotation updates on corners to be smoother
#define speedUp 10.0f

namespace demos {

	std::vector<WalkingDemo::Triangle> WalkingDemo::TrianglesFromMesh(render::Mesh& meshy) {
		std::vector<WalkingDemo::Triangle> answer;
		const std::vector<f3>& verts = meshy.GetPositions();
		const std::vector<unsigned int>& indices = meshy.GetVertexIndices();

		const unsigned int numbIndices = (unsigned int) indices.size();
		const unsigned int numbVerts = (unsigned int)verts.size();
		
		// choose if we should create triangles directly from the vertex vector or use indices to access vertex vector
		const auto vert = [&verts](int index) -> f3 { return verts[index]; };
		const auto vertsFromIndices = [&verts, &indices](int index) -> f3 { return verts[indices[index]]; };
		std::function<f3(int)> vertexGetter = vert;
		if (numbIndices > 0) { vertexGetter = vertsFromIndices; }

		const unsigned int maxIterations = numbIndices > 0 ? numbIndices : numbVerts;
		for (int i = 0; i < maxIterations; i += 3) {
			answer.push_back(WalkingDemo::Triangle(
				vertexGetter(i + 0), 
				vertexGetter(i + 1), 
				vertexGetter(i + 2)
			));
		}
		return answer;
	}

	std::vector<WalkingDemo::Triangle> WalkingDemo::TrianglesFromMeshes(std::vector<render::Mesh>& meshes) {
		std::vector<WalkingDemo::Triangle> answer;
		unsigned int numbMeshes = (unsigned int)meshes.size();
		for (unsigned int meshIndex = 0; meshIndex < numbMeshes; meshIndex++) {
			render::Mesh& mesh = meshes[meshIndex];
			std::vector<WalkingDemo::Triangle> triangles = TrianglesFromMesh(mesh);
			answer.insert(answer.end(), triangles.begin(), triangles.end());
		}
		return answer;
	}

	bool WalkingDemo::Intersect(const Ray& ray, const Triangle& triangle, f3& intersectPointOut) {
		constexpr float epsilon = 0.000001f; // = std::numeric_limits<float>::epsilon();
		// Moller-Trumbore ray-triangle intersection algorithm
		f3 edge1 = triangle.v1 - triangle.v0;
		f3 edge2 = triangle.v2 - triangle.v0;

		f3 dirCrossE2 = cross(ray.direction, edge2);
		float determinant = dot(edge1, dirCrossE2);

		// ray is parrallel to the triangle?
		if (determinant < epsilon && determinant > -epsilon) { return false; }

		float f = 1.0f / determinant;
		f3 s = ray.origin - triangle.v0;

		float u = f * dot(s, dirCrossE2);

		// u is smaller than zero OR u is greater than 1
		// also check for values that are within epsilon of 0 and 1 
		if ((u < 0 && std::abs(u) > epsilon) || (u > 1 && std::abs(u - 1.0f) > epsilon)) { return false; }

		f3 sCrossEdge1 = cross(s, edge1);
		float v = f * dot(ray.direction, sCrossEdge1);

		if ((v < 0 && std::abs(u) > epsilon) || (u + v > 1 && std::abs(u + v - 1) > epsilon)) { return false; }

		float distance = f * dot(edge2, sCrossEdge1);

		// is the intersection behind the ray origin?
		if (distance < epsilon) { return false; }

		intersectPointOut = ray.origin + ray.direction * distance;

		return true;
	}

	namespace InitializationHelpers {
	// not sure why the book author just copy pastes like 90% of the gltf loader code here, but oh well.
	// there are minor changes, like not looking for skinning data 
		void ExtractValuesFromNodes(std::vector<float>& values, unsigned int numbFloats, const cgltf_accessor& accessor) {
			values.resize(accessor.count * numbFloats);
			for (unsigned int i = 0; i < (unsigned int)accessor.count; i++) {
				cgltf_accessor_read_float(&accessor, i, &values[i * numbFloats], numbFloats);
			}
		}

		void MeshFromAttribute(render::Mesh& mesh, cgltf_attribute& attribute) {
			cgltf_accessor& accessor = *attribute.data;
			unsigned int numbComponents = 0;
			if (accessor.type == cgltf_type_vec2) {
				numbComponents = 2;
			}
			else if (accessor.type == cgltf_type_vec3) {
				numbComponents = 3;
			}
			else if (accessor.type == cgltf_type_vec4) {
				numbComponents = 4;
			} else {
				return;
			}
			std::vector<float> values;
			ExtractValuesFromNodes(values, numbComponents, accessor);
			auto& verts = mesh.GetPositions();
			auto& norms = mesh.GetNormals();
			auto& texs = mesh.GetTextureCoords();
			unsigned int numbAccessors = accessor.count;
			cgltf_attribute_type type = attribute.type;
			for (unsigned int i = 0; i < numbAccessors; i++) {
				int index = i * numbComponents;
				switch (type) {
				case cgltf_attribute_type_position:
					verts.push_back(f3(values[index], values[index + 1], values[index + 2]));
					break;
				case cgltf_attribute_type_texcoord:
					texs.push_back(f2(values[index], values[index + 1]));
					break;
				case cgltf_attribute_type_normal: {
					f3 normal = f3(values[index], values[index + 1], values[index + 2]);
					if (lengthSquared(normal) < 0.00001f) { // bad normal data
						normal = f3(0, 1, 0);
					}
					norms.push_back(normal);
				}
					break;
				}
			}
		}

		std::vector<render::Mesh> LoadMeshes(cgltf_data* data) {
			// check all nodes, skip nodes that do not possess a mesh and skin
			std::vector<render::Mesh> result;
			cgltf_node* nodes = data->nodes;
			unsigned int numbNodes = data->nodes_count;
			for (unsigned int i = 0; i < numbNodes; i++) {
				cgltf_node* node = &nodes[i];
				if (node->mesh == 0) { continue; }
				cgltf_size primCount = node->mesh->primitives_count; // basically an unsigned int
				for (unsigned int j = 0; j < primCount; j++) {
					result.push_back(render::Mesh());
					render::Mesh& mesh = result.back();
					cgltf_primitive* primative = &node->mesh->primitives[j];
					unsigned int attributeCount = primative->attributes_count;
					for (unsigned int k = 0; k < attributeCount; k++) {
						// extracting verts, norms, textCoords, and more attributes for the mesh
						cgltf_attribute* attribute = &primative->attributes[k];
						InitializationHelpers::MeshFromAttribute(mesh, *attribute);
					}
					if (primative->indices != 0) {
						unsigned int indicesCount = primative->indices->count;
						auto& indices = mesh.GetVertexIndices();
						indices.resize(indicesCount);
						for (unsigned int k = 0; k < indicesCount; k++) {
							indices[k] = cgltf_accessor_read_index(primative->indices, k);
						}
					}
					mesh.SyncOpenGL();
				}
			}
			return result;
		}
	}

	void WalkingDemo::Initialize() {
		// manually construct the frames that define the actor walking path
		this->actorPathTrack.Resize(5);
		this->actorPathTrack.SetInterpolationMethod(anim::Interpolate::Linear);
		
		constexpr int x = 0, y = 1, z = 2;
		anim::FrameVector* frame = &this->actorPathTrack[0];
		frame->timestamp = 0.0f;
		frame->value[x] = 0; frame->value[y] = 0; frame->value[z] = 1;

		frame = &this->actorPathTrack[1];
		frame->timestamp = 1.0f;
		frame->value[x] = 0; frame->value[y] = 0; frame->value[z] = 10;

		frame = &this->actorPathTrack[2];
		frame->timestamp = 3.0f;
		frame->value[x] = 22; frame->value[y] = 0; frame->value[z] = 10;

		frame = &this->actorPathTrack[3];
		frame->timestamp = 4.0f;
		frame->value[x] = 22; frame->value[y] = 0; frame->value[z] = 2;

		frame = &this->actorPathTrack[4];
		frame->timestamp = walkPathDuration;
		frame->value[x] = 0; frame->value[y] = 0; frame->value[z] = 1;
		
		// load woman mesh and skeleton
		std::cout << "Looking for \'\\resource\\assets\\Woman.gltf\' in working directory.\n";
		cgltf_data* data = io::LoadGLTFFile("./resource/assets/Woman.gltf");
		if (data == nullptr) {
			std::cout << "couldn't load gltf data\n";
		}
		this->armature = io::MakeArmature(data);
		this->clips = io::LoadClips(data);
		this->actorMeshes = io::LoadMeshes(data);
		io::FreeGLTFData(data);
		// load walking track course geometry
		std::cout << "Looking for \'\\resource\\assets\\IKCourse.gltf\' in working directory.\n";
		data = io::LoadGLTFFile("./resource/assets/IKCourse.gltf");
		if (data == nullptr) {
			std::cout << "couldn't load gltf data\n";
		}
		this->floorMeshes = InitializationHelpers::LoadMeshes(data);
		io::FreeGLTFData(data);
		this->floorTriangles = TrianglesFromMeshes(this->floorMeshes);

		// load textures
		std::cout << "Looking for \'\\resource\\assets\\uv.png\' in working directory.\n";
		this->floorTexture = new render::Texture("./resource/assets/uv.png");

		std::cout << "Looking for \'\\resource\\assets\\Woman.png\' in working directory.\n";
		this->actorTexture = new render::Texture("./resource/assets/Woman.png");

		// make shaders
		this->actorShader = new render::Shader(
			"./resource/shaders/skinned_vert.glsl",
			"./resource/shaders/frag_lit.glsl"
		);
		this->floorShader = new render::Shader(
			"./resource/shaders/static_vert.glsl",
			"./resource/shaders/frag_lit.glsl"
		);

		// make ik legs
		// note, the bone names need to match the names of the bones in the gltf file
		this->leftLeg = new IKLeg(this->armature, "LeftUpLeg", "LeftLeg", "LeftFoot", "LeftToeBase");
		this->leftLeg->SetAnkleGroundOffset(0.2f);

		this->rightLeg = new IKLeg(this->armature, "RightUpLeg", "RightLeg", "RightFoot", "RightToeBase");
		this->rightLeg->SetAnkleGroundOffset(0.2f);
		
		// manually construct the data track that describes the actor's foot height over the duration of the walk cycle
		auto& rightLegTrack = rightLeg->GetLegHeightTrack();
		rightLegTrack.SetInterpolationMethod(anim::Interpolate::Cubic);
		rightLegTrack.Resize(4); 
		// https://stackoverflow.com/questions/9293674/can-we-reassign-the-reference-in-c We can't reseat a reference
		// need to use pointers instead
		anim::FrameScalar* frameLeg = &rightLegTrack[0];
		frameLeg->timestamp = 0.0f; frameLeg->value[x] = 1.0f;
		frameLeg = &rightLegTrack[1];
		frameLeg->timestamp = 0.3f; frameLeg->value[x] = 0.0f;
		frameLeg = &rightLegTrack[2];
		frameLeg->timestamp = 0.7f; frameLeg->value[x] = 0.0f;
		frameLeg = &rightLegTrack[3];
		frameLeg->timestamp = 1.0f; frameLeg->value[x] = 1.0f;

		auto& leftLegTrack = leftLeg->GetLegHeightTrack();
		leftLegTrack.SetInterpolationMethod(anim::Interpolate::Cubic);
		leftLegTrack.Resize(4);
		frameLeg = &leftLegTrack[0];
		frameLeg->timestamp = 0.0f; frameLeg->value[x] = 0.0f;
		frameLeg = &leftLegTrack[1];
		frameLeg->timestamp = 0.4f; frameLeg->value[x] = 1.0f;
		frameLeg = &leftLegTrack[2];
		frameLeg->timestamp = 0.6f; frameLeg->value[x] = 1.0f;
		frameLeg = &leftLegTrack[3];
		frameLeg->timestamp = 1.0f; frameLeg->value[x] = 0.0f;

		this->pose = armature.GetRestPose();

		this->poseDrawer = new InverseKinematicsDemo::LineDrawer();
		this->poseDrawer->PointsFromPose(this->pose);
		this->poseDrawer->UpdateBuffers();

		for (unsigned int i = 0, numbClips = this->clips.size(); i < numbClips; i++) {
			std::string& clipName = this->clips[i].GetClipName();
			if (clipName == "Walking") {
				this->clipIndex = i;
				break;
			}
		}
		// Set the rendering options
		this->options.IKPose = true;
		this->options.currentPose = true;
		this->options.floor = true;

		// toggle to what the animation would look like without IK
		this->demoOptions.useFootIK = true; 
		// slow the demo down to closely inspect the effect of IK on foot placement
		this->demoOptions.playBackSpeed = 1.0f * 0.5f;
		// toggle rotating the foot to lie flat on the floor
		this->demoOptions.useToeIk = true;

		this->groundOffset = 0.15f;
		this->toeLength = 0.3f;

		// set initial actor position to be on the course above (0,0,0)
		Ray ray(f3(this->actorTransform.position.x, rayHeightAboveGeometry, this->actorTransform.position.z));
		f3 hitOut;
		for (unsigned int i = 0, numbTriangles = this->floorTriangles.size(); i < numbTriangles; i++) {
			if (Intersect(ray, this->floorTriangles[i], hitOut)) {
				this->actorTransform.position = hitOut;
				break;
			}
		}
		this->actorTransform.position.y -= this->groundOffset;
		this->prevModelHeight = this->actorTransform.position.y;
	}

	void WalkingDemo::ShutDown() {
		delete this->actorShader;
		delete this->actorTexture;

		delete this->floorShader;
		delete this->floorTexture;

		delete this->leftLeg;
		delete this->rightLeg;

		delete this->poseDrawer;

		this->clips.clear();
		this->actorMeshes.clear();
		this->floorMeshes.clear();
	}

	void WalkingDemo::Update(float deltaTime) {
		deltaTime *= demoOptions.playBackSpeed;
		this->actorTime += deltaTime * walkSlowDown;
		while (this->actorTime > walkPathDuration) { this->actorTime -= walkPathDuration; }

		// update actor position by sampling the walking path
		float lastY = this->actorTransform.position.y;
		f3 currentPosition = this->actorPathTrack.Sample(this->actorTime, true);
		f3 nextPosition = this->actorPathTrack.Sample(this->actorTime + 0.1f, true);
		currentPosition.y = lastY;
		nextPosition.y = lastY;
		this->actorTransform.position = currentPosition;
		// update actor rotation
		f3 modelForward = normalized(nextPosition - currentPosition);
		rotation::quaternion facing = rotation::lookRotation(modelForward, f3(0,1,0));
		if (dot(actorTransform.rotation, facing) < 0.0f) {
			facing = facing * -1.0f;
		}
		actorTransform.rotation = rotation::nlerp(actorTransform.rotation, facing, deltaTime * speedUp);
		f3 forward = actorTransform.rotation * f3(0,0,1); // world is +z forward

		// update actor height by checking which track triangle we are above
		unsigned int numbTriangles = this->floorTriangles.size();
		Ray ray(f3(actorTransform.position.x, rayHeightAboveGeometry, actorTransform.position.z));
		f3 hitOut;
		for (unsigned int t = 0; t < numbTriangles; t++) {
			if (Intersect(ray, this->floorTriangles[t], hitOut)) {
				actorTransform.position = hitOut - f3(0,this->groundOffset,0);
				break;
			}
		}

		// animation sample
		this->playbackTime = this->clips[this->clipIndex].Sample(this->pose, this->playbackTime + deltaTime);
		this->poseDrawer->PointsFromPose(this->pose);
		float nTime = (this->playbackTime - this->clips[this->clipIndex].GetStartTime()) / this->clips[this->clipIndex].GetDuration();
		
		// figure out where the feet should be placed		
		f3 leftAnklePos = transforms::combine(actorTransform, this->pose.GetWorldTransform(this->leftLeg->GetAnkle())).position;
		f3 predictedLeftAnklePos = leftAnklePos;
		Ray leftRay(leftAnklePos + f3(0,2,0));

		f3 rightAnklePos = transforms::combine(actorTransform, this->pose.GetWorldTransform(this->rightLeg->GetAnkle())).position;
		f3 predictedRightAnklePos = rightAnklePos;
		Ray rightRay(rightAnklePos + f3(0, 2, 0));

		f3 ground = actorTransform.position;
		float rayHeight = 2.1f; // derived from the actual model height
		for (unsigned int t = 0; t < numbTriangles; t++) {
			if (Intersect(leftRay, this->floorTriangles[t], hitOut)) {
				if (lengthSquared(hitOut - leftRay.origin) < (rayHeight * rayHeight)) {
					leftAnklePos = hitOut;
					if (hitOut.y < ground.y) {
						ground = hitOut - f3(0,this->groundOffset,0);
					}
				}
				predictedLeftAnklePos = hitOut;
			}
			if (Intersect(rightRay, this->floorTriangles[t], hitOut)) {
				if (lengthSquared(hitOut - rightRay.origin) < (rayHeight * rayHeight)) {
					rightAnklePos = hitOut;
					if (hitOut.y < ground.y) {
						ground = hitOut - f3(0, this->groundOffset, 0);
					}
				}
				predictedRightAnklePos = hitOut;
			}
		}
		// move the model forwards a little bit
		actorTransform.position.y = this->prevModelHeight;
		actorTransform.position = lerp(actorTransform.position, ground, deltaTime * speedUp);
		this->prevModelHeight = actorTransform.position.y;
		// LERP the foot position between where the foot would be if it was clamped to the floor vs in the air, based on the leg walk cycle tracks
		// clamp the normalized time value (n variable prefix means the value is normalized)
		nTime = (nTime > 1.0f) ? 1.0f : (nTime < 0.0f) ? 0.0f : nTime;
		float nLeftLegHeight = this->leftLeg->GetLegHeightTrack().Sample(nTime, true);
		float nRightLegHeight = this->rightLeg->GetLegHeightTrack().Sample(nTime, true);
		leftAnklePos = lerp(leftAnklePos, predictedLeftAnklePos, nLeftLegHeight);
		rightAnklePos = lerp(rightAnklePos, predictedRightAnklePos, nRightLegHeight);
		// now that we know where we want the feet to be placed, use the IK solvers to figure out where the other leg
		// bones should be placed to achieve the desired foot placement
		this->leftLeg->Sovle(this->actorTransform, this->pose, leftAnklePos);
		this->rightLeg->Sovle(this->actorTransform, this->pose, rightAnklePos);
		// replace the pose's leg configuration with the configuration from the IK solvers via blending
		float blendAmount = this->demoOptions.useFootIK ? 1.0f : 0.0f;
		anim::Blend(this->pose, this->pose, this->leftLeg->GetPose(), blendAmount, this->leftLeg->GetHip());
		anim::Blend(this->pose, this->pose, this->rightLeg->GetPose(), blendAmount, this->rightLeg->GetHip());

		// adjust the toe rotation to lie flat with the floor


		this->pose.ToMatrixPalette(this->bonesAsMatrices);
	}

	void WalkingDemo::Render(float aspectRatio) {
		mat4f model = transforms::toMatrix(this->actorTransform);
		mat4f view = lookAt(
			f3(this->actorTransform.position.x + 0, 0 + 5, this->actorTransform.position.z + 10),
			f3(this->actorTransform.position.x, 0 + 3, this->actorTransform.position.z),
			f3(0,1,0)
		);
		float fovDeg = 60.0f;
		mat4f projection = perspective(fovDeg, aspectRatio, 0.01f, 1000.0f);
		mat4f viewProjection = projection * view;
		mat4f modelViewProjection = projection * view * model; // right to left application of matrix effects

		bool wuf = false; // was uniform found?

		if (this->options.mesh) {
			render::Shader* shader = this->actorShader;
			shader->Bind();
			this->actorTexture->Set(shader->GetUniform("tex0", wuf), 0);
			render::Uniform<mat4f>::Set(shader->GetUniform("model_transform", wuf), model);
			render::Uniform<mat4f>::Set(shader->GetUniform("view_transform", wuf), view);
			render::Uniform<mat4f>::Set(shader->GetUniform("projection", wuf), projection);
			render::Uniform<f3>::Set(shader->GetUniform("sky_light_direction", wuf), f3(1, 1, 1));
			render::Uniform<mat4f>::Set(shader->GetUniform("posedBones", wuf), this->bonesAsMatrices);
			render::Uniform<mat4f>::Set(shader->GetUniform("inverseBindPose", wuf), this->armature.GetInverseBindPose());

			for (unsigned int i = 0, numbMesh = this->actorMeshes.size(); i < numbMesh; i++) {
				unsigned int position = shader->GetAttribute("position", wuf);
				unsigned int normal = shader->GetAttribute("normal", wuf);
				unsigned int textureCoordinate = shader->GetAttribute("textureCoordinate", wuf);
				unsigned int boneWeights = shader->GetAttribute("boneWeights", wuf);
				unsigned int boneIndices = shader->GetAttribute("boneIndices", wuf);
				this->actorMeshes[i].Bind(position, normal, textureCoordinate, boneIndices, boneWeights);
				this->actorMeshes[i].Draw();
				this->actorMeshes[i].UnBind(position, normal, textureCoordinate, boneIndices, boneWeights);
			}

			this->actorTexture->Unset(0);
			shader->Unbind();
		}

		if (this->options.floor) {
			render::Shader* shader = this->floorShader;
			shader->Bind();
			this->floorTexture->Set(shader->GetUniform("tex0", wuf), 0);
			render::Uniform<mat4f>::Set(shader->GetUniform("model_transform",wuf), mat4f());
			render::Uniform<mat4f>::Set(shader->GetUniform("view_transform", wuf), view);
			render::Uniform<mat4f>::Set(shader->GetUniform("projection", wuf), projection);
			render::Uniform<f3>::Set(shader->GetUniform("sky_light_direction", wuf), f3(1,1,1));

			for (unsigned int i = 0, numbMesh = this->floorMeshes.size(); i < numbMesh; i++) {
				unsigned int position = shader->GetAttribute("position", wuf);
				unsigned int normal = shader->GetAttribute("normal", wuf);
				unsigned int textureCoordinate = shader->GetAttribute("textureCoordinate", wuf);
				this->floorMeshes[i].Bind(position, normal, textureCoordinate, -1, -1);
				this->floorMeshes[i].Draw();
				this->floorMeshes[i].UnBind(position, normal, textureCoordinate, -1, -1);
			}

			this->floorTexture->Unset(0);
			shader->Unbind();
		}

		if (!this->options.depthTest) {
			render::SetDepthBuffer(false);
		}

		if (this->options.currentPose) {
			this->poseDrawer->UpdateBuffers();
			this->poseDrawer->Draw(SimpleAnimationPlayer::SkeletonDrawer::LineDrawMode::Lines, f3(0,0,1), modelViewProjection);
		}

		if (this->options.IKPose) {
			this->leftLeg->drawDebugLeg(viewProjection, f3(1,0,0));
			this->rightLeg->drawDebugLeg(viewProjection, f3(0,1,0));
		}

		if (!this->options.depthTest) {
			render::SetDepthBuffer(true);
		}
	}

	WalkingDemo::IKLeg::IKLeg() {
		this->ankleGroundOffset = 0.0f;
		this->legVisualizer = new InverseKinematicsDemo::LineDrawer();
		this->solver.ResizeChain(numbLegBones);
		this->legVisualizer->Resize(numbLegBones + 1);
	}
	
	WalkingDemo::IKLeg::IKLeg(anim::Armature& armature, const std::string hip, const std::string knee, const std::string ankle, const std::string toe) {
		this->ankleGroundOffset = 0.0f;
		this->legVisualizer = new InverseKinematicsDemo::LineDrawer();
		this->solver.ResizeChain(numbLegBones);
		this->legVisualizer->Resize(numbLegBones + 1);
		this->hipIndex = this->kneeIndex = this->ankleIndex = this->toeIndex = 0;
		// populate the bone indices
		for (unsigned int i = 0, size = armature.GetRestPose().Size(); i < size; i++) {
			std::string& boneName = armature.GetBoneName(i);
			if (boneName == hip) { hipIndex = i; }
			else if (boneName == knee) { kneeIndex = i; }
			else if (boneName == ankle) { ankleIndex = i; }
			else if (boneName == toe) { toeIndex = i; }
		}
	}
	
	WalkingDemo::IKLeg::IKLeg(const IKLeg& leg) {
		this->ankleGroundOffset = 0.0f;
		this->legVisualizer = new InverseKinematicsDemo::LineDrawer();
		this->solver.ResizeChain(numbLegBones);
		this->legVisualizer->Resize(numbLegBones + 1);
		*this = leg;
	}
	
	WalkingDemo::IKLeg& WalkingDemo::IKLeg::operator=(const IKLeg& leg) {
		if (this == &leg) { return *this; }
		this->solver = leg.solver;
		this->ankleGroundOffset = leg.ankleGroundOffset;
		this->hipIndex = leg.hipIndex;
		this->kneeIndex = leg.kneeIndex;
		this->ankleIndex = leg.ankleIndex;
		this->toeIndex = leg.toeIndex;
		return *this;
	}
	
	WalkingDemo::IKLeg::~IKLeg() {
		delete this->legVisualizer;
	}

	void WalkingDemo::IKLeg::Sovle(const transforms::srt& modelTransform, anim::Pose& pose, const f3& footPlacementTarget) {
		this->solver.SetLocalBone(0, transforms::combine(modelTransform, pose.GetWorldTransform(this->hipIndex)));
		this->solver.SetLocalBone(1, pose.GetLocalTransform(kneeIndex));
		this->solver.SetLocalBone(2, pose.GetLocalTransform(ankleIndex));

		this->legPose = pose;

		transforms::srt footTarget(footPlacementTarget + (f3(0,1,0) * this->ankleGroundOffset), rotation::quaternion(), f3(1,1,1));

		this->solver.SolveChain(footTarget);
		// write the solution out to the leg pose
		transforms::srt root = transforms::combine(modelTransform, pose.GetWorldTransform(pose.ParentIndexOf(this->hipIndex)));
		this->legPose.SetLocalTransform(this->hipIndex, transforms::combine(transforms::inverse(root), this->solver.GetLocalBone(0)));
		this->legPose.SetLocalTransform(this->kneeIndex, this->solver.GetLocalBone(1));
		this->legPose.SetLocalTransform(this->ankleIndex, this->solver.GetLocalBone(2));

		this->legVisualizer->FromIKSolver(this->solver);
	}

	anim::Pose& WalkingDemo::IKLeg::GetPose() { return this->legPose; }

	anim::TrackScalar& WalkingDemo::IKLeg::GetLegHeightTrack() { return this->legHeight; }

	void WalkingDemo::IKLeg::drawDebugLeg(const mat4f& viewProjection, const f3& color) {
		this->legVisualizer->UpdateBuffers();
		this->legVisualizer->Draw(SimpleAnimationPlayer::SkeletonDrawer::LineDrawMode::Lines, color, viewProjection);
	}

	unsigned int WalkingDemo::IKLeg::GetHip() { return this->hipIndex; }

	unsigned int WalkingDemo::IKLeg::GetKnee() { return this->kneeIndex; }

	unsigned int WalkingDemo::IKLeg::GetAnkle() { return this->ankleIndex; }

	unsigned int WalkingDemo::IKLeg::GetToe() { return this->toeIndex; }

	void WalkingDemo::IKLeg::SetAnkleGroundOffset(float offset) { this->ankleGroundOffset = offset; }

}
