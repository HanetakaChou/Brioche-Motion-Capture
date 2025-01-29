//
// Copyright (C) YuqiaoZhang(HanetakaChou)
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
//

#include "../include/brx_motion.h"

#include "../thirdparty/mediapipe/include/mediapipe/tasks/c/vision/face_landmarker/face_landmarker.h"
#include "../thirdparty/mediapipe/include/mediapipe/tasks/c/vision/pose_landmarker/pose_landmarker.h"
#define CV_IGNORE_DEBUG_BUILD_GUARD 1
#include <opencv2/opencv.hpp>

static constexpr uint32_t const ARKIT_MORPH_TARGET_COUNT = 52U;

class brx_motion_media_pipe_capture final : public brx_motion_video_capture
{
	uint32_t m_width;
	uint32_t m_height;
	uint32_t m_fps;
	cv::VideoCapture m_video_capture;
	void *m_face_landmarker;
	void *m_pose_landmarker;
	bool m_enable_face_detect;
	bool m_enable_pose_detect;
	int64_t m_timestamp_ms;
	float m_morph_target_weights[ARKIT_MORPH_TARGET_COUNT];
	float m_joint_local_quaternions[15];

public:
	brx_motion_media_pipe_capture();
	~brx_motion_media_pipe_capture();
	bool init(char const *video_url, bool enable_face_detect, bool enable_pose_detect);
	void uninit();

private:
	void set_width(uint32_t width) override;
	void set_height(uint32_t height) override;
	void set_fps(uint32_t fps) override;
	uint32_t get_width() const override;
	uint32_t get_height() const override;
	uint32_t get_fps() const override;
	void step(float delta_time) override;
	void get_morph_target_weights(float *out_blend_shape_weights) const override;
	void brx_motion_video_capture_apply_pose(brx_motion_skeleton *skeleton) const override;
};