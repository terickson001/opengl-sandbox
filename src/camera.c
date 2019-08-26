#include "camera.h"

#include <GLFW/glfw3.h>
#include <math.h>

#include "util.h"
#include "keyboard.h"

#define ABS(x) ((x)<0?-(x):(x))
#define MAX(x, y) ((x)>(y)?(x):(y))
#define MIN(x, y) ((x)<(y)?(x):(y))
#define SIGNUM(x) ((x)<0?-1:1)

Camera make_camera(Vec3f pos, Vec3f dir, float move_speed, float rotate_speed)
{
    Camera cam = {0};
    cam.pos = pos;
    cam.dir = vec3f_normalize(dir);
    cam.up = init_vec3f(0, 1, 0);

    get_direction_angles(cam.dir, &cam.h_angle, &cam.v_angle);
    
    cam.move_speed = move_speed;
    cam.rotate_speed = rotate_speed;

    return cam;
}

Mat4f get_camera_view(Camera cam)
{
    return mat4f_look_at(
        cam.pos,
        vec3f_add(cam.pos, cam.dir),
        cam.up
    );
}

void update_camera_angle(Window win, Camera *cam, float dt)
{
    if (cam->detach)
        return;
    
    double xpos, ypos;
    glfwGetCursorPos(win.handle, &xpos, &ypos);
    glfwSetCursorPos(win.handle, win.width/2, win.height/2);

    cam->h_angle += cam->rotate_speed * dt * (float)(win.width /2 - xpos);
    cam->v_angle += cam->rotate_speed * dt * (float)(win.height/2 - ypos);

    if (ABS(cam->h_angle) > 2*M_PI)
        cam->h_angle -= SIGNUM(cam->h_angle)*(2*M_PI);
    cam->v_angle = MIN(MAX(cam->v_angle, -(M_PI/2)), M_PI/2);
    
    cam->dir = init_vec3f(
        cos(cam->v_angle) * sin(cam->h_angle),
        sin(cam->v_angle),
        cos(cam->v_angle) * cos(cam->h_angle)
    );

    cam->right = init_vec3f(
        sin(cam->h_angle - M_PI/2),
        0,
        cos(cam->h_angle - M_PI/2)
    );

    cam->up = vec3f_cross(cam->right, cam->dir);
}

void update_camera_position(Window win, Camera *cam, float dt)
{

    if (get_keystate(GLFW_KEY_ENTER) == KeyState_PRESSED)
    {
        cam->detach_key_down = true;
        cam->detach = !cam->detach;
        if (cam->detach)
            glfwSetInputMode(win.handle, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        else
            glfwSetInputMode(win.handle, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }
    if (cam->detach)
        return;

    if (key_down(GLFW_KEY_W))
        cam->pos = vec3f_add(cam->pos, vec3f_scale(cam->dir, dt*cam->move_speed));
    if (key_down(GLFW_KEY_S))
        cam->pos = vec3f_sub(cam->pos, vec3f_scale(cam->dir, dt*cam->move_speed));
    
    if (key_down(GLFW_KEY_D))
        cam->pos = vec3f_add(cam->pos, vec3f_scale(cam->right, dt*cam->move_speed));
    if (key_down(GLFW_KEY_A))
        cam->pos = vec3f_sub(cam->pos, vec3f_scale(cam->right, dt*cam->move_speed));

    if (key_down(GLFW_KEY_SPACE))
        cam->pos = vec3f_add(cam->pos, vec3f_scale(cam->up, dt*cam->move_speed));
    if (key_down(GLFW_KEY_LEFT_SHIFT))
        cam->pos = vec3f_sub(cam->pos, vec3f_scale(cam->up, dt*cam->move_speed));
}

void update_camera(Window win, Camera *cam, float dt)
{
    update_camera_angle(win, cam, dt);
    update_camera_position(win, cam, dt);
}
