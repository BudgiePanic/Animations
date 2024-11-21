#include "QuickTrack.h"

namespace anim {
	
	template QuickTrack<float, 1>;
	template QuickTrack<f3, 3>;
	template QuickTrack<rotation::quaternion, 4>;

	template<typename T, int FrameDimension>
	int QuickTrack<T, FrameDimension>::FrameIndexAt(float time, bool isTrackLooping) {
		std::vector<Frame<FrameDimension>>& frames = this->frames;
		// code lifted from original track implementation of nearestFrameAt
		unsigned int trackSize = (unsigned int) frames.size();
		if (trackSize <= 1) { return -1; }
		float start = frames[0].timestamp;
		float end = frames[trackSize - 1].timestamp;
		float trackDuration = end - start;
		if (isTrackLooping) {
			time = fmodf(time - start, trackDuration);
			time = (time < 0.0f) ? (time + trackDuration) : time;
			time += start;
		}
		else {
			if (time <= frames[0].timestamp) { return 0; }
			if (time >= frames[trackSize - 2].timestamp) {
				return (int)trackSize - 2;
			}
		}
		// new 'quick' implementation is here
		// normalize the time
		float t = time / trackDuration;
		unsigned int numbSamples = trackDuration * this->expectedFramesPerSecond;
		// figure out what sample corresponds to the sample time
		unsigned int index = (t * ((float)numbSamples));
		if (index >= this->nearestFrameIndices.size()) { return -1; }
		// the vector already knows the index of the frame that corresponds to this sample time
		return (int)this->nearestFrameIndices[index];
	}

	// Called once during data loading, to allow _constant_ look up during sampling later on
	template<typename T, int FrameDimension>
	void QuickTrack<T, FrameDimension>::RecalculateFrameIndexCache() {
		int numbFrames = (int) this->Size();
		if (numbFrames <= 1) { return; }
		float duration = this->GetEndTime() - this->GetStartTime();
		unsigned int numbSamples = duration * this->expectedFramesPerSecond;
		this->nearestFrameIndices.resize(numbSamples);
		// populate the sample cache
		for (unsigned int i = 0; i < numbSamples; i++) {
			float timeNorm = ((float)i) / ((float)numbSamples - 1);
			float time = timeNorm * duration + this->GetStartTime();
			// find the nearest frame for each sample point
			unsigned int frameForSample = 0;
			for (int j = numbFrames - 1; j >= 0; j--) {
				Frame<FrameDimension>& frame = this->frames[j];
				if (time >= frame.timestamp) {
					frameForSample = (unsigned int) j;
					if ((int) frameForSample >= numbFrames - 2) {
						// if the closest frame to the sample is the final frame, supply second to last frame
						// required due to sampling algorithm
						frameForSample = numbFrames - 2;
					}
					break; // move onto the next sample
				}
			}
			this->nearestFrameIndices[i] = frameForSample;
		}
	}

	template<typename T, int FrameDimension>
	QuickTrack<T, FrameDimension>::QuickTrack(float expectedFramesPerSecond) {
		if (expectedFramesPerSecond <= 0.0f) {
			expectedFramesPerSecond = 1.0f;
		}
		this->expectedFramesPerSecond = expectedFramesPerSecond;
	}

}
