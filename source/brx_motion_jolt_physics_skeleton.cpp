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

#include "brx_motion_jolt_physics_skeleton.h"
#include "../../McRT-Malloc/include/mcrt_malloc.h"
#include <cassert>
#include <new>

extern "C" brx_motion_skeleton *brx_motion_create_skeleton(uint32_t joint_count, uint32_t const *parent_indices, uint32_t const *humanoid_joint_indices)
{
    void *new_unwrapped_skeleton_base = mcrt_malloc(sizeof(brx_motion_jolt_physics_skeleton), alignof(brx_motion_jolt_physics_skeleton));
    assert(NULL != new_unwrapped_skeleton_base);

    brx_motion_jolt_physics_skeleton *new_unwrapped_skeleton = new (new_unwrapped_skeleton_base) brx_motion_jolt_physics_skeleton{};
    new_unwrapped_skeleton->init(joint_count, parent_indices, humanoid_joint_indices);
    return new_unwrapped_skeleton;
}

extern "C" void brx_motion_destroy_skeleton(brx_motion_skeleton *wrapped_skeleton)
{
    assert(NULL != wrapped_skeleton);
    brx_motion_jolt_physics_skeleton *delete_unwrapped_skeleton = static_cast<brx_motion_jolt_physics_skeleton *>(wrapped_skeleton);

    delete_unwrapped_skeleton->uninit();

    delete_unwrapped_skeleton->~brx_motion_jolt_physics_skeleton();
    mcrt_free(delete_unwrapped_skeleton);
}

brx_motion_jolt_physics_skeleton::brx_motion_jolt_physics_skeleton()
{
}

void brx_motion_jolt_physics_skeleton::init(uint32_t joint_count, uint32_t const *parent_indices, uint32_t const *humanoid_joint_indices)
{
}

void brx_motion_jolt_physics_skeleton::uninit()
{
}

void brx_motion_jolt_physics_skeleton::brx_motion_skelton_set_pose(float const (*joint_translations)[3], float const (*joint_quaternions)[4])
{
}

void brx_motion_jolt_physics_skeleton::brx_motion_skelton_get_pose(float (*out_joint_translations)[3], float const (*out_joint_quaternions)[4]) const
{
}

#include <Jolt/Core/Memory.h>

JPH_NAMESPACE_BEGIN

static void* AllocateHook(size_t inSize)
{
	return mcrt_malloc(inSize, 16U);
}

static void* ReallocateHook(void* inBlock, size_t inOldSize, size_t inNewSize)
{
    assert(0);
    return NULL;
}

static void FreeHook(void* inBlock)
{
    return mcrt_free(inBlock);
}

static void* AlignedAllocateHook(size_t inSize, size_t inAlignment)
{
    return mcrt_malloc(inSize, inAlignment);
}

static void AlignedFreeHook(void* inBlock)
{
    return mcrt_free(inBlock);
}

AllocateFunction Allocate = AllocateHook;
ReallocateFunction Reallocate = ReallocateHook;
FreeFunction Free = FreeHook;
AlignedAllocateFunction AlignedAllocate = AlignedAllocateHook;
AlignedFreeFunction AlignedFree = AlignedFreeHook;

JPH_NAMESPACE_END
