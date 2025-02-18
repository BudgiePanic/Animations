#include "InverseKinematicsDemo.h"

namespace demos {

	void InverseKinematicsDemo::SetFrame(anim::TrackVector& track, int frameIndex, float frameTime, const f3& frameData) {
		track[frameIndex].value[0] = frameData.x;
		track[frameIndex].value[1] = frameData.y;
		track[frameIndex].value[2] = frameData.z;
		track[frameIndex].timestamp = frameTime;
	}

	void InverseKinematicsDemo::Initialize() {
		const float halfPi = 1.570796327f;
		const f3 scale(1,1,1);
		const f3 boneSegment(0.0f, 0.0f, 0.5f);
		this->ccdSolver.ResizeChain(6); 
		this->ccdSolver[0] = transforms::srt(f3(0.0f, 0.0f, 1.0f), rotation::angleAxis(halfPi, f3(1,0,0)), scale);
		this->ccdSolver[1] = transforms::srt(f3(0.0f, 0.0f, 1.5f), rotation::quaternion(), scale);
		this->ccdSolver[2] = transforms::srt(boneSegment, rotation::quaternion(), scale);
		this->ccdSolver[3] = transforms::srt(boneSegment, rotation::angleAxis(halfPi, f3(0, 1, 0)), scale);
		this->ccdSolver[4] = transforms::srt(boneSegment, rotation::quaternion(), scale);
		this->ccdSolver[5] = transforms::srt(boneSegment, rotation::quaternion(), scale);

		this->fabrikSolver.ResizeChain(6);
		this->fabrikSolver.SetLocalBone(0, transforms::srt(f3(0.0f, 0.0f, 1.0f), rotation::angleAxis(halfPi, f3(1, 0, 0)), scale));
		this->fabrikSolver.SetLocalBone(1, transforms::srt(f3(0.0f, 0.0f, 1.5f), rotation::quaternion(), scale));
		this->fabrikSolver.SetLocalBone(2, transforms::srt(boneSegment, rotation::quaternion(), scale));
		this->fabrikSolver.SetLocalBone(3, transforms::srt(boneSegment, rotation::angleAxis(halfPi, f3(0, 1, 0)), scale));
		this->fabrikSolver.SetLocalBone(4, transforms::srt(boneSegment, rotation::quaternion(), scale));
		this->fabrikSolver.SetLocalBone(5, transforms::srt(boneSegment, rotation::quaternion(), scale));

		this->goal.position = f3(1, -2, 0);

		anim::TrackVector& goalPath = this->goalPath.GetTranslationTrack();
		goalPath.Resize(14);
		this->SetFrame(goalPath, 0, 0.0f, f3(1, -2, 0) * 0.5f);
		this->SetFrame(goalPath, 1, 1.0f, f3(1, 2, 0) * 0.5f);
		this->SetFrame(goalPath, 2, 2.0f, f3(1, 4, 0) * 0.5f);
		this->SetFrame(goalPath, 3, 3.0f, f3(3, 4, 0) * 0.5f);
		this->SetFrame(goalPath, 4, 4.0f, f3(5, 4, 0) * 0.5f);
		this->SetFrame(goalPath, 5, 5.0f, f3(5, 4, 2) * 0.5f);
		this->SetFrame(goalPath, 6, 6.0f, f3(5, 4, 4) * 0.5f);
		this->SetFrame(goalPath, 7, 7.0f, f3(3, 4, 4) * 0.5f);
		this->SetFrame(goalPath, 8, 8.0f, f3(3, 2, 4) * 0.5f);
		this->SetFrame(goalPath, 9, 9.0f, f3(3, 2, 2) * 0.5f);
		this->SetFrame(goalPath, 10, 10.0f, f3(1, 2, 2) * 0.5f);
		this->SetFrame(goalPath, 11, 11.0f, f3(1, 0, 2) * 0.5f);
		this->SetFrame(goalPath, 12, 12.0f, f3(1, -2, 2) * 0.5f);
		this->SetFrame(goalPath, 13, 13.0f, f3(1, -2, 0) * 0.5f);

		this->fabrikDrawer = new LineDrawer();
		this->ccdDrawer = new LineDrawer();
		this->goalDrawer = new LineDrawer(2);

		this->pitch = 45.0f;
		this->yaw = 60.0f;
		this->distance = 7.0f;
		this->time = 0.0f;
	}

	void InverseKinematicsDemo::ShutDown() {
		delete this->ccdDrawer;
		delete this->fabrikDrawer;
		delete this->goalDrawer;
	}

	void InverseKinematicsDemo::Update(float deltaTime) {
		this->time += deltaTime;
		if (this->time > this->goalPath.GetEndTime()) {
			// run it back
			this->time -= this->goalPath.GetEndTime();
		}
		this->goal = this->goalPath.Sample(this->goal, time, true);
		
		this->ccdSolver.SolveChain(this->goal);
		this->fabrikSolver.SolveChain(this->goal);
	}

	void InverseKinematicsDemo::Render(float aspectRatio) {
		f3 cameraPosition(
			this->distance * cosf(this->yaw) * sinf(this->pitch),
			this->distance * cosf(this->pitch),
			this->distance * sinf(this->yaw) * sinf(this->pitch)
		);
		float fov = 60.0f;
		mat4f projection = perspective(fov, aspectRatio, 0.01f, 1000.0f);
		mat4f view = lookAt(cameraPosition, f3(0,0,0), f3(0,1,0));
		mat4f modelViewProj = projection * view; // working in model space, so the model transform is the identity

		f3 red(1,0,0);
		f3 green(0,1,0);
		f3 blue(0,0,1);

		this->ccdDrawer->FromIKSolver(this->ccdSolver);
		this->fabrikDrawer->FromIKSolver(this->fabrikSolver);
		LineDrawer& drawer = *(this->goalDrawer);
		// the indication of the goal position will be a vertical line
		drawer[0] = goal.position;
		drawer[1] = goal.position + f3(0.0f, 0.5f, 0.0f);
		
		this->ccdDrawer->UpdateBuffers();
		this->fabrikDrawer->UpdateBuffers();
		drawer.UpdateBuffers();

		this->ccdDrawer->Draw(SimpleAnimationPlayer::SkeletonDrawer::LineDrawMode::Lines, red, modelViewProj);
		this->fabrikDrawer->Draw(SimpleAnimationPlayer::SkeletonDrawer::LineDrawMode::Lines, blue, modelViewProj);
		drawer.Draw(SimpleAnimationPlayer::SkeletonDrawer::LineDrawMode::Lines, green, modelViewProj);
	}

	void InverseKinematicsDemo::LineDrawer::FromIKSolver(ik::CCDSolver& solver)	{
		unsigned int size = solver.ChainSize();
		if (size < 2) { return; }
		unsigned int numbVerts = 2 * (size - 1);
		this->points.resize(numbVerts);
		unsigned int idx = 0;
		for (unsigned int i = 0; i < size - 1; i++) {
			this->points[idx] = solver.GetBoneAt(i).position;
			idx++;
			this->points[idx] = solver.GetBoneAt(i + 1).position;
			idx++;
		}
	}

	void InverseKinematicsDemo::LineDrawer::FromIKSolver(ik::FABRIKSolver& solver) {
		unsigned int size = solver.ChainSize();
		if (size < 2) { return; }
		unsigned int numbVerts = 2 * (size - 1);
		this->points.resize(numbVerts);
		unsigned int idx = 0;
		for (unsigned int i = 0; i < size - 1; i++) {
			this->points[idx] = solver.GetBone(i).position;
			idx++;
			this->points[idx] = solver.GetBone(i + 1).position;
			idx++;
		}
	}

	InverseKinematicsDemo::LineDrawer::LineDrawer(unsigned int size) : SimpleAnimationPlayer::SkeletonDrawer(size) {}

	InverseKinematicsDemo::LineDrawer::LineDrawer() : SimpleAnimationPlayer::SkeletonDrawer() {}

}
