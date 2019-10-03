
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
#include "sprite.h"
#include "gui.h"

Window init_gl(int w, int h, char *title)
{
    if (!glfwInit())
    {
        fprintf(stderr, "Failed to initialize GLFW\n");
        exit(1);
    }

    glfwWindowHint(GLFW_SAMPLES, 16); // 16x antialiasing
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // OpenGL v.3.3
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
    glfwSetMouseButtonCallback(window.handle, update_mousestate);
    glfwSetCursorPosCallback(window.handle, update_mousepos);
    glfwSetInputMode(window.handle, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    return window;
}

static GLuint gui_vbuff, gui_uvbuff; // Just to quickly test
void draw_rect(i32 x, i32 y, i32 w, i32 h, Vec4f color)
{
    y = 768 - y - h;
    
    Vec2f vertices[6], uvs[6];

    vertices[0] = init_vec2f(x,   y);
    vertices[1] = init_vec2f(x+w, y+h);
    vertices[2] = init_vec2f(x,   y+h);

    vertices[3] = init_vec2f(x,   y);
    vertices[4] = init_vec2f(x+w, y);
    vertices[5] = init_vec2f(x+w, y+h);

    uvs[0] = init_vec2f(0, 0);
    uvs[1] = init_vec2f(1, 1);
    uvs[2] = init_vec2f(0, 1);

    uvs[3] = init_vec2f(0, 0);
    uvs[4] = init_vec2f(1, 0);
    uvs[5] = init_vec2f(1, 1);

    Texture tex = color_texture(vec4f_scale(color, 1/255.0f));
    
    glBindBuffer(GL_ARRAY_BUFFER, gui_vbuff);
    glBufferData(GL_ARRAY_BUFFER, 6*sizeof(Vec2f), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, gui_uvbuff);
    glBufferData(GL_ARRAY_BUFFER, 6*sizeof(Vec2f), uvs, GL_STATIC_DRAW);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex.diffuse);
    
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, gui_vbuff);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, gui_uvbuff);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisable(GL_BLEND);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
}

void do_gui(Gui_Context *ctx, Window win)
{
    gui_begin(ctx, win);
    gui_row(ctx, 1, (i32[]){0}, 15);
    if (gui_button(ctx, "Button 1", 0, 0))
        printf("Button 1 Pressed\n");
    gui_end(ctx);
}

void draw_gui(Gui_Context *ctx, Shader s)
{
    glUseProgram(s.id);
    
    /* glActiveTexture(GL_TEXTURE0); */
    /* glBindTexture(GL_TEXTURE_2D, s->map.diffuse); */
    
    glUniform2i(s.uniforms.resolution, 1024, 768);
    glUniform1i(s.uniforms.diffuse_tex, 0);
    
    Gui_Draw draw;
    while (gui_next_draw(ctx, &draw))
    {
        switch (draw.kind)
        {
        case GUI_DRAW_RECT: draw_rect(draw.rect.rect.x, draw.rect.rect.y, draw.rect.rect.w, draw.rect.rect.h, draw.rect.color); break;
        default: break;
        }
    }
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

    // Load OBJs
    Model model = make_model("./res/cylinder.obj", true, true);
    Model suzanne_m = make_model("./res/suzanne.obj", true, true);
    Model cube = get_prim_cube();
    Model pyramid = get_prim_pyramid();
    Model diamond = get_prim_diamond();
    //invert_uvs(&cube);
    
    create_model_vbos(&model);
    create_model_vbos(&suzanne_m);
    
    // Load shaders
    Shader shader = init_shaders("./shader/vertex.vs", 0, "./shader/fragment.fs");

    // Load texture
    Texture brick     = load_texture("./res/brick.DDS", "./res/brick_normal.bmp", "./res/brick_specular.DDS");
    Texture suzanne_t = load_texture("./res/suzanne.DDS", 0, 0);

    Texture blue = color_texture(init_vec4f(0, 1, 0, 0.5));

    Sprite sprite = load_sprite("./res/adventurer.sprite");
    sprite_set_anim(&sprite, "idle");
    
    Entity column        = make_entity(&model,     &brick,     init_vec3f(0, 1, 0), init_vec3f(0, 0, -1));
    Entity suzanne       = make_entity(&suzanne_m, &blue, init_vec3f(0, 0, 0), init_vec3f(0, 0, -1));
    Entity suzanne_2     = make_entity(&suzanne_m, &suzanne_t, init_vec3f(5, 0, 0), init_vec3f(-1, 0, -1));

    Entity_2D adventurer = make_entity_2d(&sprite, init_vec2f(512-160, 384-160), init_vec2f(10,10));

    Gui_Context gui_context = gui_init();
    glGenBuffers(1, &gui_vbuff);
    glGenBuffers(1, &gui_uvbuff);
    
    // Create transformation matrices
    Mat4f projection_mat = mat4f_perspective(RAD(45.0f), (float)width/(float)height, 0.1f, 100.0f);
    // Mat4f projection_mat = mat4f_ortho(-10, 10, -10, 10, 0, 100);

    Mat4f view_mat;

    // Initialize delta time
    double last_time = glfwGetTime();
    double current_time;
    float dt = 0;

    // Initialize camera
    Vec3f cam_pos = init_vec3f(4, 4, 4);
    Camera camera = make_camera(cam_pos, vec3f_scale(cam_pos, -1), 3.0f, 0.15f);
    
    // Light settings
    Vec3f light_pos = init_vec3f(4, 4, 4);
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

        draw_entity(shader, column);
        draw_entity(shader, suzanne);
        draw_entity(shader, suzanne_2);

        do_gui(&gui_context, window);
        draw_gui(&gui_context, font.shader);
        draw_entity_2d(font.shader, adventurer);
        print_text(font, fps_str, width-45, height-15, 15);
        
        glfwSwapBuffers(window.handle);
        glfwPollEvents();
        
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
    destroy_model(&model);
    destroy_texture(brick);
    destroy_font(font);

    glDeleteProgram(shader.id);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

    printf("\n");
    
    return 0;
}
