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

#ifndef _BRX_MOTION_H_
#define _BRX_MOTION_H_ 1

#include <stddef.h>
#include <stdint.h>

class brx_motion_video_capture;
class brx_motion_skeleton;

// camera://0
// file://a.mp4
extern "C" brx_motion_video_capture *brx_motion_create_capture(char const *video_url, bool enable_face_detect, bool enable_pose_detect);

extern "C" void brx_motion_destory_capture(brx_motion_video_capture *capture);

class brx_motion_video_capture
{
public:
    virtual void set_width(uint32_t width) = 0;
    virtual void set_height(uint32_t height) = 0;
    virtual void set_fps(uint32_t fps) = 0;

    virtual uint32_t get_width() const = 0;
    virtual uint32_t get_height() const = 0;
    virtual uint32_t get_fps() const = 0;

    virtual void step(float delta_time) = 0;

    // https://developer.apple.com/documentation/arkit/arfaceanchor/blendshapelocation
    virtual void get_morph_target_weights(float *out_blend_shape_weights) const = 0;

    // consider openpose example?
    // 
    
    // neck - head rotation

    // IK
    //
    // Reaching IK (position-based IK): target position (end effector joint)
    // two joints: hinge Axis (second joint)
    // three joints: hinge Axis (first joint) 
    // more joints: CCD or FABRIK
    // FABRIK: http://www.andreasaristidou.com/FABRIK.html  
    // 
    // Look At IK (rotation-based IK) target position (not really need to reach)
    //
    // Foot IK  
    // use ground detection to find the correct foot placement  
    // the problem can be converted to Reaching IK  


    // extern "C" void brx_motion_video_capture_instance_get_head_from_neck_rotation(uint32_t face_index, uint32_t pose_index, float (*out_head_ik_rotation)[4]);

    // face index defines the neck-head transform
    virtual void brx_motion_video_capture_apply_pose(brx_motion_skeleton *skeleton) const = 0;
};

// https://github.com/vrm-c/vrm-specification/blob/master/specification/0.0/README.md#defined-bones
// https://github.com/saturday06/VRM-Addon-for-Blender/blob/main/src/io_scene_vrm/common/human_bone_mapper/mmd_mapping.py
// https://ai.google.dev/edge/mediapipe/solutions/vision/pose_landmarker
// https://cmu-perceptual-computing-lab.github.io/openpose/web/html/doc/md_doc_02_output.html
extern "C" brx_motion_skeleton *brx_motion_create_vrm_skeleton(uint32_t joint_count, uint32_t const *parent_indices, uint32_t const *humanoid_joint_indices);

extern "C" void brx_motion_destroy_vrm_skeleton(brx_motion_skeleton *skeleton);

class brx_motion_skeleton
{
public:
    virtual void brx_motion_skelton_set_pose(float const (*joint_translations)[3], float const (*joint_quaternions)[4]) = 0;
    virtual void brx_motion_skelton_get_pose(float (*out_joint_translations)[3], float const (*out_joint_quaternions)[4]) const = 0;
};

#endif
