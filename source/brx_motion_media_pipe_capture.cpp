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

#include "../../McRT-Malloc/include/mcrt_malloc.h"
#include "brx_motion_media_pipe_capture.h"
#include <cassert>
#include <cstring>
#include <new>

#define MEDIA_PIPE_ENABLE_DEBUG_RENDERER 1

static constexpr uint32_t const MEDIA_PIPE_MORPH_TARGET_COUNT = 52U;

static constexpr char const *const media_pipe_morph_target_names[MEDIA_PIPE_MORPH_TARGET_COUNT] = {};
static constexpr uint32_t const media_pipe_morph_target_indices[MEDIA_PIPE_MORPH_TARGET_COUNT] = {};

extern uint8_t const *const face_landmarker_task_base;
extern size_t const face_landmarker_task_size;

extern uint8_t const *const pose_landmarker_task_base;
extern size_t const pose_landmarker_task_size;

extern "C" brx_motion_video_capture *brx_motion_create_capture(char const *video_url, bool enable_face_detect, bool enable_pose_detect)
{
    void *new_unwrapped_capture_base = mcrt_malloc(sizeof(brx_motion_media_pipe_capture), alignof(brx_motion_media_pipe_capture));
    assert(NULL != new_unwrapped_capture_base);

    brx_motion_media_pipe_capture *new_unwrapped_capture = new (new_unwrapped_capture_base) brx_motion_media_pipe_capture{};
    if (new_unwrapped_capture->init(video_url, enable_face_detect, enable_pose_detect))
    {
        return new_unwrapped_capture;
    }
    else
    {
        new_unwrapped_capture->~brx_motion_media_pipe_capture();
        mcrt_free(new_unwrapped_capture);
        return NULL;
    }
}

extern "C" void brx_motion_destory_capture(brx_motion_video_capture *wrapped_capture)
{
    assert(NULL != wrapped_capture);
    brx_motion_media_pipe_capture *delete_unwrapped_capture = static_cast<brx_motion_media_pipe_capture *>(wrapped_capture);

    delete_unwrapped_capture->uninit();

    delete_unwrapped_capture->~brx_motion_media_pipe_capture();
    mcrt_free(delete_unwrapped_capture);
}

brx_motion_media_pipe_capture::brx_motion_media_pipe_capture() : m_width(0U), m_height(0U), m_fps(0U), m_face_landmarker(NULL), m_pose_landmarker(NULL), m_timestamp_ms(0)
{
}

brx_motion_media_pipe_capture::~brx_motion_media_pipe_capture()
{
    assert(!this->m_video_capture.isOpened());
    assert(NULL == this->m_face_landmarker);
    assert(NULL == this->m_pose_landmarker);
}

bool brx_motion_media_pipe_capture::init(char const *video_url, bool enable_face_detect, bool enable_pose_detect)
{

    assert(!this->m_video_capture.isOpened());
    bool status_open_video_capture;
    if (0 == std::strncmp(video_url, "camera://", 9U))
    {
        int camera_index = 0;
        bool error_camera_index = false;
        for (size_t char_index = 9U; video_url[char_index] != '\0'; ++char_index)
        {
            // INT32_MAX 2147483647
            if (!(char_index < (9U + 10U)))
            {
                error_camera_index = true;
                break;
            }

            camera_index *= 10;

            switch (video_url[char_index])
            {
            case '0':
            {
            }
            break;
            case '1':
            {
                camera_index += 1;
            }
            break;
            case '2':
            {
                camera_index += 2;
            }
            break;
            case '3':
            {
                camera_index += 3;
            }
            break;
            case '4':
            {
                camera_index += 4;
            }
            break;
            case '5':
            {
                camera_index += 5;
            }
            break;
            case '6':
            {
                camera_index += 6;
            }
            break;
            case '7':
            {
                camera_index += 7;
            }
            break;
            case '8':
            {
                camera_index += 8;
            }
            break;
            case '9':
            {
                camera_index += 9;
            }
            break;
            default:
            {
                error_camera_index = true;
            }
            }

            if (error_camera_index)
            {
                break;
            }
        }

        if (!error_camera_index)
        {
            status_open_video_capture = this->m_video_capture.open(camera_index);
        }
        else
        {
            status_open_video_capture = false;
        }
    }
    else if (0 == std::strncmp(video_url, "file://", 7U))
    {
        status_open_video_capture = this->m_video_capture.open(video_url + 7U);
    }
    else
    {
        status_open_video_capture = false;
    }

    if (!status_open_video_capture)
    {
        assert(!this->m_video_capture.isOpened());
        return false;
    }

    bool status_create_face_landmarker;
    this->m_enable_face_detect = enable_face_detect;
    if (this->m_enable_face_detect)
    {
        FaceLandmarkerOptions options;
        options.base_options.model_asset_buffer = reinterpret_cast<char const *>(face_landmarker_task_base);
        options.base_options.model_asset_buffer_count = static_cast<unsigned int>(face_landmarker_task_size);
        options.base_options.model_asset_path = NULL;
        options.running_mode = VIDEO;
        // options.num_faces = 1;
        // options.min_face_detection_confidence = 0.5F;
        // options.min_face_presence_confidence = 0.5F;
        // options.min_tracking_confidence = 0.5F;
        options.output_face_blendshapes = true;
        options.output_facial_transformation_matrixes = true;
        options.result_callback = NULL;

        assert(NULL == this->m_face_landmarker);
        this->m_face_landmarker = face_landmarker_create(&options, NULL);
        if (NULL != this->m_face_landmarker)
        {
            status_create_face_landmarker = true;
        }
        else
        {
            status_create_face_landmarker = false;
        }
    }
    else
    {
        status_create_face_landmarker = true;
    }

    if (!status_create_face_landmarker)
    {
        assert(NULL == this->m_face_landmarker);

        assert(this->m_video_capture.isOpened());
        this->m_video_capture.release();
        return false;
    }

    bool status_create_pose_landmarker;
    this->m_enable_pose_detect = enable_pose_detect;
    if (this->m_enable_pose_detect)
    {
        PoseLandmarkerOptions options;
        options.base_options.model_asset_buffer = reinterpret_cast<char const *>(pose_landmarker_task_base);
        options.base_options.model_asset_buffer_count = static_cast<unsigned int>(pose_landmarker_task_size);
        options.base_options.model_asset_path = NULL;
        options.running_mode = VIDEO;
        // options.num_poses = 1;
        // options.min_pose_detection_confidence = 0.5F;
        // options.min_pose_presence_confidence = 0.5F;
        // options.min_tracking_confidence = 0.5F;
        options.output_segmentation_masks = false;
        options.result_callback = NULL;

        assert(NULL == this->m_pose_landmarker);
        this->m_pose_landmarker = pose_landmarker_create(&options, NULL);
        if (NULL != this->m_pose_landmarker)
        {
            status_create_pose_landmarker = true;
        }
        else
        {
            status_create_pose_landmarker = false;
        }
    }
    else
    {
        status_create_pose_landmarker = true;
    }

    if (!status_create_pose_landmarker)
    {
        assert(NULL == this->m_pose_landmarker);

        assert(NULL != this->m_face_landmarker);
        int status_face_landmarker_close = face_landmarker_close(this->m_face_landmarker, NULL);
        assert(0 == status_face_landmarker_close);
        this->m_face_landmarker = NULL;

        assert(this->m_video_capture.isOpened());
        this->m_video_capture.release();
        return false;
    }

    assert(0U == this->m_width);
    this->m_width = static_cast<uint32_t>(this->m_video_capture.get(cv::CAP_PROP_FRAME_WIDTH));

    assert(0U == this->m_height);
    this->m_height = static_cast<uint32_t>(this->m_video_capture.get(cv::CAP_PROP_FRAME_HEIGHT));

    assert(0U == this->m_fps);
    this->m_fps = static_cast<uint32_t>(this->m_video_capture.get(cv::CAP_PROP_FPS));

    // init morph target weights

    return true;
}

void brx_motion_media_pipe_capture::uninit()
{
    if (this->m_enable_pose_detect)
    {
        assert(NULL != this->m_pose_landmarker);
        int status_pose_landmarker_close = pose_landmarker_close(this->m_pose_landmarker, NULL);
        assert(0 == status_pose_landmarker_close);
        this->m_pose_landmarker = NULL;
    }
    else
    {
        assert(NULL == this->m_pose_landmarker);
    }

    if (this->m_enable_face_detect)
    {
        assert(NULL != this->m_face_landmarker);
        int status_face_landmarker_close = face_landmarker_close(this->m_face_landmarker, NULL);
        assert(0 == status_face_landmarker_close);
        this->m_face_landmarker = NULL;
    }
    else
    {
        assert(NULL == this->m_face_landmarker);
    }

    assert(this->m_video_capture.isOpened());
    this->m_video_capture.release();
}

void brx_motion_media_pipe_capture::set_width(uint32_t width)
{
    if (width != this->m_width)
    {
        static_cast<uint32_t>(this->m_video_capture.set(cv::CAP_PROP_FRAME_WIDTH, width));
        this->m_width = static_cast<uint32_t>(this->m_video_capture.get(cv::CAP_PROP_FRAME_WIDTH));
    }
}

void brx_motion_media_pipe_capture::set_height(uint32_t height)
{
    if (height != this->m_height)
    {
        static_cast<uint32_t>(this->m_video_capture.set(cv::CAP_PROP_FRAME_HEIGHT, height));
        this->m_height = static_cast<uint32_t>(this->m_video_capture.get(cv::CAP_PROP_FRAME_HEIGHT));
    }
}

void brx_motion_media_pipe_capture::set_fps(uint32_t fps)
{
    if (fps != this->m_fps)
    {
        static_cast<uint32_t>(this->m_video_capture.set(cv::CAP_PROP_FPS, fps));
        this->m_fps = static_cast<uint32_t>(this->m_video_capture.get(cv::CAP_PROP_FPS));
    }
}

uint32_t brx_motion_media_pipe_capture::get_width() const
{
    return this->m_width;
}

uint32_t brx_motion_media_pipe_capture::get_height() const
{
    return this->m_height;
}

uint32_t brx_motion_media_pipe_capture::get_fps() const
{
    return this->m_fps;
}

void brx_motion_media_pipe_capture::step(float delta_time)
{
    this->m_timestamp_ms += static_cast<int64_t>((static_cast<double>(delta_time) * 1000.0));

    cv::Mat raw_video_image_matrix;
    if (!(this->m_video_capture.read(raw_video_image_matrix) && (!raw_video_image_matrix.empty())))
    {
        // use the result of previous frame when fail
        assert(false);
        return;
    }

    cv::Mat input_image_matrix;
    {
        cv::cvtColor(raw_video_image_matrix, input_image_matrix, cv::COLOR_BGR2RGB);

        raw_video_image_matrix.release();
    }

    MpImage input_image;
    {
        // mediapipe/examples/desktopdemo_run_graph_main.cc
        // mediapipe/framework/formats/image_frame_opencv.h
        // mediapipe/framework/formats/image_frame_opencv.cc

        constexpr ImageFormat const k_format = SRGB;
        constexpr int const k_number_of_channels_for_format = 3;
        constexpr int const k_channel_size_for_format = sizeof(uint8_t);
        constexpr int const k_mat_type_for_format = CV_8U;

        constexpr uint32_t const k_default_alignment_boundary = 16U;

        input_image.type = MpImage::IMAGE_FRAME;
        input_image.image_frame.format = k_format;
        input_image.image_frame.width = input_image_matrix.cols;
        input_image.image_frame.height = input_image_matrix.rows;

        int const type = CV_MAKETYPE(k_mat_type_for_format, k_number_of_channels_for_format);
        int const width_step = (((input_image.image_frame.width * k_number_of_channels_for_format * k_channel_size_for_format) - 1) | (k_default_alignment_boundary - 1)) + 1;
        assert(type == input_image_matrix.type());
        assert(width_step == input_image_matrix.step[0]);
        input_image.image_frame.image_buffer = static_cast<uint8_t *>(input_image_matrix.data);
        assert(0U == (reinterpret_cast<uintptr_t>(input_image.image_frame.image_buffer) & (k_default_alignment_boundary - 1)));
    }

    FaceLandmarkerResult face_landmarker_result;
    int status_face_landmarker_detect_for_video;
    if (this->m_enable_face_detect)
    {
        assert(NULL != this->m_face_landmarker);
        status_face_landmarker_detect_for_video = face_landmarker_detect_for_video(this->m_face_landmarker, &input_image, this->m_timestamp_ms, &face_landmarker_result, NULL);
    }
    else
    {
        assert(NULL == this->m_face_landmarker);
    }

    PoseLandmarkerResult pose_landmarker_result;
    int status_pose_landmarker_detect_for_video;
    if (this->m_enable_pose_detect)
    {
        assert(NULL != this->m_pose_landmarker);
        status_pose_landmarker_detect_for_video = pose_landmarker_detect_for_video(this->m_pose_landmarker, &input_image, this->m_timestamp_ms, &pose_landmarker_result, NULL);
    }
    else
    {
        assert(NULL == this->m_pose_landmarker);
    }

    {
        constexpr uint32_t const face_index = 0U;
        constexpr uint32_t const pose_index = 0U;

#if defined(MEDIA_PIPE_ENABLE_DEBUG_RENDERER) && MEDIA_PIPE_ENABLE_DEBUG_RENDERER
        // we do NOT need the input image any more
        cv::Mat debug_renderer_output_image_matrix = std::move(input_image_matrix);
#endif

        if (this->m_enable_face_detect && (0 == status_face_landmarker_detect_for_video))
        {
            if (face_landmarker_result.face_blendshapes_count > face_index)
            {
                Categories const &face_blendshape = face_landmarker_result.face_blendshapes[face_index];

                assert(MEDIA_PIPE_MORPH_TARGET_COUNT == face_blendshape.categories_count);

                for (uint32_t blend_shape_index = 0U; blend_shape_index < face_blendshape.categories_count; ++blend_shape_index)
                {
                    Category const *category = &face_blendshape.categories[blend_shape_index];

                    // assert strcmp media_pipe_morph_target_names category->category_name

                    // map to arkit indices
                    // media_pipe_morph_target_indices
                }
            }

#if defined(MEDIA_PIPE_ENABLE_DEBUG_RENDERER) && MEDIA_PIPE_ENABLE_DEBUG_RENDERER
            if (face_landmarker_result.face_landmarks_count > face_index)
            {
                NormalizedLandmarks const &face_landmark = face_landmarker_result.face_landmarks[face_index];

                for (uint32_t landmarks_index = 0U; landmarks_index < face_landmark.landmarks_count; ++landmarks_index)
                {
                    NormalizedLandmark const &normalized_landmark = face_landmark.landmarks[landmarks_index];

                    if (((!normalized_landmark.has_visibility) || (normalized_landmark.visibility > 0.5F)) && ((!normalized_landmark.has_presence) || (normalized_landmark.presence > 0.5F)))
                    {
                        cv::Point point(static_cast<int>(normalized_landmark.x * debug_renderer_output_image_matrix.cols), static_cast<int>(normalized_landmark.y * debug_renderer_output_image_matrix.rows));
                        cv::circle(debug_renderer_output_image_matrix, point, 1, cv::Scalar(0, 255, 0), -1);
                    }
                }
            }
#endif
        }

        if (this->m_enable_pose_detect && (0 == status_pose_landmarker_detect_for_video))
        {
            if (pose_landmarker_result.pose_world_landmarks_count > pose_index)
            {
                Landmarks const &pose_world_landmark = pose_landmarker_result.pose_world_landmarks[pose_index];

                for (uint32_t world_landmark_index = 0U; world_landmark_index < pose_world_landmark.landmarks_count; ++world_landmark_index)
                {
                    Landmark const &landmark = pose_world_landmark.landmarks[world_landmark_index];

                    if (((!landmark.has_visibility) || (landmark.visibility > 0.5F)) && ((!landmark.has_presence) || (landmark.presence > 0.5F)))
                    {
                        std::cout << "x: " << landmark.x << " y: " << landmark.y << " z: " << landmark.z << std::endl;
                    }
                }
            }

#if defined(MEDIA_PIPE_ENABLE_DEBUG_RENDERER) && MEDIA_PIPE_ENABLE_DEBUG_RENDERER
            if (pose_landmarker_result.pose_landmarks_count > pose_index)
            {
                NormalizedLandmarks const &pose_landmark = pose_landmarker_result.pose_landmarks[pose_index];

                if (pose_landmark.landmarks_count > 0)
                {
                    NormalizedLandmark const &normalized_landmark_1 = pose_landmark.landmarks[0];

                    if (((!normalized_landmark_1.has_visibility) || (normalized_landmark_1.visibility > 0.5F)) && ((!normalized_landmark_1.has_presence) || (normalized_landmark_1.presence > 0.5F)))
                    {
                        cv::Point point_1(static_cast<int>(normalized_landmark_1.x * debug_renderer_output_image_matrix.cols), static_cast<int>(normalized_landmark_1.y * debug_renderer_output_image_matrix.rows));

                        for (uint32_t landmarks_index = 1U; landmarks_index < pose_landmark.landmarks_count; ++landmarks_index)
                        {
                            NormalizedLandmark const &normalized_landmark_2 = pose_landmark.landmarks[landmarks_index];

                            if (((!normalized_landmark_2.has_visibility) || (normalized_landmark_2.visibility > 0.5F)) && ((!normalized_landmark_2.has_presence) || (normalized_landmark_2.presence > 0.5F)))
                            {
                                cv::Point point_2(static_cast<int>(normalized_landmark_2.x * debug_renderer_output_image_matrix.cols), static_cast<int>(normalized_landmark_2.y * debug_renderer_output_image_matrix.rows));

                                cv::line(debug_renderer_output_image_matrix, point_1, point_2, cv::Scalar(255, 0, 0), 1);
                            }
                        }
                    }
                }
            }
#endif
        }

#if defined(MEDIA_PIPE_ENABLE_DEBUG_RENDERER) && MEDIA_PIPE_ENABLE_DEBUG_RENDERER
        // Left <-> Right
        cv::Mat debug_renderer_flip_output_image_matrix;
        cv::flip(debug_renderer_output_image_matrix, debug_renderer_flip_output_image_matrix, 1);
        debug_renderer_flip_output_image_matrix.release();

        cv::Mat debug_renderer_raw_show_image_matrix;
        cv::cvtColor(debug_renderer_flip_output_image_matrix, debug_renderer_raw_show_image_matrix, cv::COLOR_RGB2BGR);
        debug_renderer_flip_output_image_matrix.release();

        cv::imshow("k_window_name", debug_renderer_raw_show_image_matrix);
#endif
    }

    // TODO: do we still need to close result when fail
    if (this->m_enable_face_detect)
    {
        face_landmarker_close_result(&face_landmarker_result);
    }

    if (this->m_enable_pose_detect)
    {
        pose_landmarker_close_result(&pose_landmarker_result);
    }
}

void brx_motion_media_pipe_capture::get_morph_target_weights(float *out_blend_shape_weights) const
{
}

void brx_motion_media_pipe_capture::brx_motion_video_capture_apply_pose(brx_motion_skeleton *skeleton) const
{
}
