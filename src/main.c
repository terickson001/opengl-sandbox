
#define _LIB_IMPLEMENTATION
#include "lib.h"

#include <stdio.h>
#include <stdlib.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <GL/gl.h>

#include "glmath2.h"
#include "shaders.h"
#include "image.h"
#include "window.h"
#include "camera.h"
#include "model.h"
#include "text.h"
#include "texture.h"
#include "entity.h"
#include "primitive.h"
#include "keyboard.h"

Window init_gl(int w, int h, char *title)
{
    if (!glfwInit())
    {
        fprintf(stderr, "Failed to initialize GLFW\n");
        exit(1);
    }

    glfwWindowHint(GLFW_SAMPLES, 16); // 16x antialiasing
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4); // OpenGL v.4.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); 
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_DEPTH_BITS, 24);
    
    Window window = create_window(w, h, title);
    if (!window.handle)
    {
        fprintf(stderr, "Failed to open GLFW window\n");
        glfwTerminate();
        exit(1);
    }

    glfwMakeContextCurrent(window.handle);
    glewExperimental = true;
    GLenum err;
    if ((err = glewInit()) != GLEW_OK)
    {
        fprintf(stderr, "Failed to initialize GLEW: %s\n", glewGetErrorString(err));
        exit(1);
    }

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glDisable(GL_CULL_FACE);

    glEnable(GL_MULTISAMPLE);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glfwSetKeyCallback(window.handle, update_keystate);
    glfwSetInputMode(window.handle, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    return window;
}

int main(void)
{
    int width = 1024;
    int height = 768;
    Window window = init_gl(width, height, "[$float$] Hello, World");
    
    // Create VAO
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    GLfloat vertices[] = {
        -1.0f, -1.0f,  0.0f,
         1.0f, -1.0f,  0.0f,
        -1.0f,  1.0f,  0.0f,

         1.0f,  1.0f,  0.0f,
        -1.0f,  1.0f,  0.0f,
         1.0f, -1.0f,  0.0f,
    };

    GLuint vbuff;
    glGenBuffers(1, &vbuff);
    glBindBuffer(GL_ARRAY_BUFFER, vbuff);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    // Load shaders
    Shader shader = init_shaders("./shader/vertex.vs", 0, "./shader/fragment.fs");

    // Initialize delta time
    double last_time = glfwGetTime();
    double current_time;
    float dt = 0;

    Mat4f projection_mat = mat4f_perspective(RAD(45.0f), (float)width/(float)height, 0.1f, 100.0f);

    // Initialize camera
    Vec3f cam_pos = init_vec3f(0, 0, 5);
    Camera camera = make_camera(cam_pos, vec3f_scale(cam_pos, -1), 3.0f, 0.15f);
    Mat4f view_mat;

    // Light settings
    Vec3f light_pos = init_vec3f(4, 4, 4);
    Vec3f light_col = init_vec3f(1, 1, 1);
    float light_pow = 10.0f;

    glClearColor(0.0f, 0.3f, 0.4f, 0.0f);

    Font font = init_font("./res/font_holstein.DDS");
    int nb_frames = 0;
    float accum_time = 0.0f;
    float global_time = 0.0f;
    char fps_str[256] = "0";
    glfwSetCursorPos(window.handle, window.width/2, window.height/2);
    glfwPollEvents();
    
    do
    {
        update_camera(window, &camera, dt);
        view_mat = get_camera_view(camera);
        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shader.id);

        glUniform2f(shader.uniforms.resolution, window.width, window.height);
        glUniform1f(shader.uniforms.global_time, global_time);
        glUniform3f(shader.uniforms.camera_position, camera.pos.x, camera.pos.y, camera.pos.z);

        glUniformMatrix4fv(shader.uniforms.view_matrix, 1, GL_FALSE, view_mat.data);
        glUniformMatrix4fv(shader.uniforms.projection_matrix, 1, GL_FALSE, projection_mat.data);

        glUniform3f(shader.uniforms.light_pos, light_pos.x, light_pos.y, light_pos.z);
        glUniform3f(shader.uniforms.light_col, light_col.r, light_col.g, light_col.b);
        glUniform1f(shader.uniforms.light_pow, light_pow);
    
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vbuff);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glDisableVertexAttribArray(0);
        
        print_text(font, fps_str, width-270, height-180, 15);

        glfwSwapBuffers(window.handle);
        glfwPollEvents();

        current_time = glfwGetTime();
        dt = (float)(current_time - last_time);
        last_time = current_time;
        
        nb_frames++;
        accum_time += dt;
        global_time += dt;
        if (accum_time >= 1.0f)
        {
            sprintf(fps_str, "%3d", nb_frames);
            nb_frames = 0;
            accum_time -= 1.0f;
        }

        if (shader_check_update(&shader))
            printf("===== SHADER RELOADED =====\n");
    }
    while (glfwGetKey(window.handle, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
           glfwWindowShouldClose(window.handle) == 0);

    // Cleanup
    glDeleteProgram(shader.id);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

    printf("\n");
    
    return 0;
}
