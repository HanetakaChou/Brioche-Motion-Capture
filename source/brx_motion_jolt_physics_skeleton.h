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

// #define JPH_DEBUG_RENDERER
// #define JPH_PROFILE_ENABLED
// #define JPH_OBJECT_STREAM
// #define JPH_USE_AVX2
// #define JPH_USE_AVX
// #define JPH_USE_SSE4_1
// #define JPH_USE_SSE4_2
// #define JPH_USE_LZCNT
// #define JPH_USE_TZCNT
// #define JPH_USE_F16C
// #define JPH_USE_FMADD
#include <Jolt/Jolt.h>
#include <Jolt/Skeleton/Skeleton.h>
#include <Jolt/Skeleton/SkeletalAnimation.h>
#include <Jolt/Skeleton/SkeletonPose.h>
#include <Jolt/Skeleton/SkeletonMapper.h>

class brx_motion_jolt_physics_skeleton final : public brx_motion_skeleton
{
	JPH::SkeletonPose m_pose;

public:
	brx_motion_jolt_physics_skeleton();
	void init(uint32_t joint_count, uint32_t const *parent_indices, uint32_t const *humanoid_joint_indices);
	void uninit();

private:
	void brx_motion_skelton_set_pose(float const (*joint_translations)[3], float const (*joint_quaternions)[4]) override;
	void brx_motion_skelton_get_pose(float (*out_joint_translations)[3], float const (*out_joint_quaternions)[4]) const override;
};
