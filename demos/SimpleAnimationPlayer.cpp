#include "SimpleAnimationPlayer.h"
#include "../render/Uniform.h"
#include <string>
#include <iostream>
#include "../io/gltfLoader.h"

namespace demos {

	void SimpleAnimationPlayer::Initialize() {
		std::cout << "Looking for \'\\resource\\assets\\Woman.gltf\' in working directory.\n";
		cgltf_data* data = io::LoadGLTFFile("./resource/assets/Woman.gltf");
		if (data == nullptr) {
			std::cout << "couldn't load gltf data\n";
		}
		this->restPose = io::MakeRestPose(data);
		this->clips = io::LoadClips(data);
		io::FreeGLTFData(data);

		this->restPoseDrawer = new SkeletonDrawer();
		this->restPoseDrawer->PointsFromPose(this->restPose);
		this->restPoseDrawer->UpdateBuffers();

		this->activeClip = 0;
		this->pose = restPose;

		this->poseDrawer = new SkeletonDrawer();
		this->poseDrawer->PointsFromPose(this->pose);
		this->poseDrawer->UpdateBuffers();

		unsigned int numbClips = this->clips.size();
		for (unsigned int clip = 0; clip < numbClips; clip++) {
			if (this->clips[clip].GetClipName() == "walking") {
				activeClip = clip;
				break;
			}
		}
	}

	void SimpleAnimationPlayer::ShutDown() {
		delete this->poseDrawer;
		delete this->restPoseDrawer;
		this->clips.clear();
	}

	void SimpleAnimationPlayer::Update(float deltaTime) {
		this->timePlayback = this->clips[activeClip].Sample(this->pose, timePlayback + deltaTime);
		this->poseDrawer->PointsFromPose(this->pose);
	}

	void SimpleAnimationPlayer::Render(float aspectRatio) {
		mat4f projection = perspective(60.0f, aspectRatio, 0.01f, 1000.0f); // converts camera space to screen space
		mat4f view = lookAt(f3(0,4,7), f3(0,4,0), f3(0,1,0)); // converts world space to camera space
		mat4f model_view_projection = projection * view; // the model is already in world space

		// rest pose is red
		this->restPoseDrawer->Draw(SkeletonDrawer::LineDrawMode::Lines, f3(1,0,0), model_view_projection);

		this->poseDrawer->UpdateBuffers();
		// animated pose is blue
		this->poseDrawer->Draw(SkeletonDrawer::LineDrawMode::Lines, f3(0,0,1), model_view_projection);
	}

	SimpleAnimationPlayer::SkeletonDrawer::SkeletonDrawer() {
		this->drawModeConverter = {
			{LineDrawMode::Lines, render::DrawMode::Lines},
			{LineDrawMode::Loop, render::DrawMode::LineLoop},
			{LineDrawMode::Points, render::DrawMode::Points},
			{LineDrawMode::Strip, render::DrawMode::LineStrip}
		};
		this->attributes = new render::Attribute<f3>();
		const std::string vertexSource = 
			"#version 330 core\n" 
			"uniform mat4 model_view_projection;\n"
			"in vec3 vertex_position;\n"
			"void main() {\n"
			"gl_Position = model_view_projection * vec4(vertex_position, 1.0);\n"
			"}\n";
		const std::string fragmentSource = 
			"#version 330 core\n"
			"uniform vec3 color;\n"
			"out vec4 FragColor;\n"
			"void main(){\n"
			"FragColor = vec4(color, 1);\n"
			"}\n"
			;
		this->shader = new render::Shader(vertexSource, fragmentSource);
	}

	SimpleAnimationPlayer::SkeletonDrawer::SkeletonDrawer(unsigned int size) : SkeletonDrawer() {
		Resize(size);
	}

	SimpleAnimationPlayer::SkeletonDrawer::~SkeletonDrawer() {
		delete this->attributes;
		delete this->shader;
	}

	unsigned int SimpleAnimationPlayer::SkeletonDrawer::Size() {
		return (unsigned int) this->points.size();
	}

	void SimpleAnimationPlayer::SkeletonDrawer::Resize(unsigned int size) {
		this->points.resize(size);
	}

	f3& SimpleAnimationPlayer::SkeletonDrawer::operator[](unsigned int index) {
		return this->points[index];
	}

	void SimpleAnimationPlayer::SkeletonDrawer::Add(const f3& vector) {
		this->points.push_back(vector);
	}

	void SimpleAnimationPlayer::SkeletonDrawer::PointsFromPose(anim::Pose& pose) {
		unsigned int numbBones = pose.Size();
		unsigned int vertCount = 0;
		// a bone has no parent if it's 'parent' index is negative
		for (unsigned int bone = 0; bone < numbBones; bone++) {
			if (pose.ParentIndexOf(bone) < 0) { continue; }
			vertCount += 2; // need to draw a line from 'this' bone to parent bone
		}
		this->points.resize(vertCount);
		for (unsigned int bone = 0; bone < numbBones; bone++) {
			int parentIndex = pose.ParentIndexOf(bone);
			if (parentIndex < 0) { continue; }
			points.push_back(pose.GetWorldTransform(bone).position);
			points.push_back(pose.GetWorldTransform(parentIndex).position);
		}
	}

	void SimpleAnimationPlayer::SkeletonDrawer::UpdateBuffers() {
		this->attributes->Set(this->points);
	}

	void SimpleAnimationPlayer::SkeletonDrawer::Draw(LineDrawMode mode, const f3& lineColor, const mat4f worldCameraProjection) 	{
		this->shader->Bind();
		bool mvpFound;
		render::Uniform<mat4f>::Set(this->shader->GetUniform("model_view_projection", mvpFound), worldCameraProjection);
		bool colorFound;
		render::Uniform<f3>::Set(this->shader->GetUniform("color", colorFound), lineColor);
		bool positionFound;
		unsigned int slot = shader->GetAttribute("vertex_position", positionFound);
		this->attributes->BindTo(slot);
		render::DrawMode drawMode = this->drawModeConverter[mode];
		render::Draw(this->Size(), drawMode);
		this->attributes->UnBindFrom(slot);
		this->shader->Unbind();
	}

}

