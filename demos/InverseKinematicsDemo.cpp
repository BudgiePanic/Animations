#include "InverseKinematicsDemo.h"

namespace demos {

	void InverseKinematicsDemo::SetFrame(anim::TrackVector& track, int frameIndex, float frameTime, const f3& frameData) {
		track[frameIndex].value[0] = frameData.x;
		track[frameIndex].value[1] = frameData.y;
		track[frameIndex].value[2] = frameData.z;
		track[frameIndex].timestamp = frameTime;
	}

	void InverseKinematicsDemo::Initialize() {
		this->ccdSolver.ResizeChain(6);
		this->ccdSolver[0] = transforms::srt();
		this->ccdSolver[1] = transforms::srt();
		this->ccdSolver[2] = transforms::srt();
		this->ccdSolver[3] = transforms::srt();
		this->ccdSolver[4] = transforms::srt();
		this->ccdSolver[5] = transforms::srt();

		this->fabrikSolver.ResizeChain(6);
		this->fabrikSolver.SetLocalBone(0, transforms::srt());
		this->fabrikSolver.SetLocalBone(1, transforms::srt());
		this->fabrikSolver.SetLocalBone(2, transforms::srt());
		this->fabrikSolver.SetLocalBone(3, transforms::srt());
		this->fabrikSolver.SetLocalBone(4, transforms::srt());
		this->fabrikSolver.SetLocalBone(5, transforms::srt());

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
		this->ccdDrawer->UpdateBuffers();
		this->fabrikDrawer->UpdateBuffers();
		this->ccdDrawer->Draw(SimpleAnimationPlayer::SkeletonDrawer::LineDrawMode::Lines, red, modelViewProj);
		this->fabrikDrawer->Draw(SimpleAnimationPlayer::SkeletonDrawer::LineDrawMode::Lines, blue, modelViewProj);

		LineDrawer& drawer = *(this->goalDrawer);
		drawer[0] = goal.position;
		drawer[1] = goal.position + f3(0.0f, 0.5f, 0.0f);
		drawer.UpdateBuffers();
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
