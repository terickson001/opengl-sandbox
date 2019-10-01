#ifndef _CAMERA_H
#define _CAMERA_H

#include "glmath2.h"
#include "window.h"

typedef struct Camera
{
    Vec3f pos, dir, up, right;
    float pitch, yaw;
    float move_speed, rotate_speed;
    bool detach, detach_key_down;
} Camera;

Camera make_camera(Vec3f pos, Vec3f dir, float move_speed, float rotate_speed);
Mat4f get_camera_view(Camera cam);
void update_camera_angle(Window win, Camera *cam, float dt);
void update_camera_position(Window win, Camera *cam, float dt);
void update_camera(Window win, Camera *cam, float dt);

#endif // _CAMERA_H
