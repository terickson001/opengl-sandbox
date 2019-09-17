
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
#include "cube_march.h"

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

    glEnable(GL_CULL_FACE);

    glEnable(GL_MULTISAMPLE);

    glfwSetKeyCallback(window.handle, update_keystate);
    glfwSetInputMode(window.handle, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    return window;
}

Vec4f *gen_noise_field(Vec3f res, Vec2f *limits)
{
    Vec4f *field = malloc(res.x*res.y*res.z*sizeof(Vec4f));

    Vec3f *gradients = gen_gradients(res);
    float min =  100000;
    float max = -100000;
    for (int z = 0; z < res.z; z++)
        for (int y = 0; y < res.y; y++)
            for (int x = 0; x < res.x; x++)
            {
                Vec4f *curr = &field[(int)(z*res.y*res.x + y*res.x + x)];
                *curr = init_vec4f(x, y, z, 0);
                perlin_noise(curr, res, gradients);
                max = curr->w > max ? curr->w : max;
                min = curr->w < min ? curr->w : min;
            }
    
    printf("PERLIN NOISE:\n  MAX: %.2f\n  MIN: %.2f\n", max, min);
    limits->x = min;
    limits->y = max;
    return field;
}

Model generate_terrain(Vec3f res, GLuint program)
{
    printf("Producing Noise...\n");

    glUseProgram(program);
    GLint res_uniform = glGetUniformLocation(program, "res");
    glUniform3i(res_uniform, (int)res.x, (int)res.y, (int)res.z);
    
    Vec4f *field = malloc(res.z*res.y*res.x*sizeof(Vec4f));
    Vec3f *gradients = gen_gradients(res);
    
    GLuint field_buff, gradient_buff;
    glGenBuffers(1, &field_buff);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, field_buff);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Vec4f)*res.x*res.y*res.z, field, GL_DYNAMIC_READ);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, field_buff);
    
    glGenBuffers(1, &gradient_buff);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, gradient_buff);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Vec3f)*(res.x+1)*(res.y+1)*(res.z+1), gradients, GL_STATIC_READ);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, gradient_buff);

    free(field);
    free(gradients);
    
    glDispatchCompute(res.x, res.y, res.z);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    field = glMapNamedBuffer(field_buff, GL_READ_ONLY);
    printf("Done\n");
    
    printf("Marching Cubes...\n");
    Model cube_mesh = cube_march_mesh(field, res, -0.1);
    printf("Done\n");
    compute_tangent_basis(&cube_mesh);
    create_model_vbos(&cube_mesh);

    return cube_mesh;
}

int main(void)
{
    int width = 1024;
    int height = 768;
    Window window = init_gl(width, height, "[$float$] Hello, World");

    Vec3f field_res = init_vec3f(300, 50, 100);

    GLuint noise_compute_id = load_compute_shader("./shader/perlin_noise.compute");
    // GLuint march_compute_id = load_compute_shader("./shader/marching_cube.compute");

    Model cube_mesh = generate_terrain(field_res, noise_compute_id);
    
    Texture brick_texture = load_texture("./res/brick.DDS", 0, 0);
    Entity map = make_entity(&cube_mesh, &brick_texture, init_vec3f(0,0,0), init_vec3f(0,0,1));
    
    // Create VAO
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Load shaders
    Shader shader = init_shaders("./shader/vertex.vs", 0, "./shader/fragment.fs");
    // Shader shader = init_shaders("./shader/vertex.vs", "./shader/geometry.gs", "./shader/fragment.fs");

    // Create transformation matrices
    Mat4f projection_mat = mat4f_perspective(RAD(45.0f), (float)width/(float)height, 0.1f, 100.0f);
    Mat4f view_mat;

    // Initialize delta time
    double last_time = glfwGetTime();
    double current_time;
    float dt = 0;

    // Initialize camera
    Vec3f cam_pos = init_vec3f(4, 4, 4);
    Camera camera = make_camera(cam_pos, vec3f_scale(cam_pos, -1), 3.0f, 0.15f);

    // Light settings
    Vec3f light_pos = init_vec3f(5, 5, -5);
    Vec3f light_col = init_vec3f(1, 1, 1);
    float light_pow = 50.0f;

    glClearColor(0.0f, 0.3f, 0.4f, 0.0f);
    Font font = init_font("./res/font_holstein.DDS");
    int nb_frames = 0;
    float accum_time = 0.0;
    char fps_str[256] = "0";
    
    do
    {
        update_camera(window, &camera, dt);
        view_mat = get_camera_view(camera);
        Mat4f vp = mat4f_mul(projection_mat, view_mat);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shader.id);

        glUniformMatrix4fv(shader.uniforms.view_matrix, 1, GL_FALSE, view_mat.data);
        glUniformMatrix4fv(shader.uniforms.projection_matrix, 1, GL_FALSE, projection_mat.data);
        glUniformMatrix3fv(shader.uniforms.vp_matrix, 1, GL_FALSE, vp.data);

        glUniform3f(shader.uniforms.light_pos, light_pos.data[0], light_pos.data[1], light_pos.data[2]);
        glUniform3f(shader.uniforms.light_col, light_col.data[0], light_col.data[1], light_col.data[2]);
        glUniform1f(shader.uniforms.light_pow, light_pow);

        // draw_model(shader, cube_mesh);
        draw_entity(shader, map);
        print_text(font, fps_str, width-270, height-185, 15);
        
        glfwSwapBuffers(window.handle);
        glfwPollEvents();

        if (key_down(GLFW_KEY_R))
            *map.model = generate_terrain(field_res, noise_compute_id);
        current_time = glfwGetTime();
        dt = (float)(current_time - last_time);
        last_time = current_time;

        
        nb_frames++;
        accum_time += dt;
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
    destroy_font(font);

    glDeleteProgram(shader.id);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

    printf("\n");
    
    return 0;
}
