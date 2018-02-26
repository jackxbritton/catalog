#include <stdio.h>
#include "catalog.h"
#include <SDL2/SDL.h>
#include <GL/glew.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// Helper functions.
char *load_entire_file(const char *filename);
GLuint gl_create_shader(GLenum type, const char *src);

// Shader struct and functions.
struct Shader {
    char *vert_path,
         *frag_path;
    GLuint program,
           vert_shader,
           frag_shader;
};
typedef struct Shader Shader;

int shader_init(Shader *shader, const char *vert_path, const char *frag_path);
void shader_destroy(Shader *shader);
void shader_reload(Shader *shader);

// Texture struct and functions.
struct Texture {
    char *path;
    GLuint texture;
    int index;
};
typedef struct Texture Texture;

int texture_init(Texture *texture, const char *path);
void texture_destroy(Texture *texture);
void texture_reload(Texture *texture);

int main(int argc, char *argv[]) {

    SDL_Window *window;
    SDL_GLContext context;
    SDL_Event event;
    int running = 1;

    Catalog catalog;

    GLuint vao,
           vbo;

    // SDL window.
    window = SDL_CreateWindow(
        "catalog/examples/opengl",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        640, 480,
        SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE
    );
    if (window == NULL) {
        fprintf(stderr, "%s", SDL_GetError());
        return 1;
    }

    // OpenGL context.
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    context = SDL_GL_CreateContext(window);
    if (context == NULL) {
        fprintf(stderr, "%s", SDL_GetError());
        return 1;
    }

    // GLEW.
    glewExperimental = GL_TRUE;
    glewInit();

    glClearColor(0.0, 0.0, 0.0, 1.0);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);

    // Shader.
    Shader shader;
    if (!shader_init(&shader, "../examples/shaders/basic.vs.glsl", "../examples/shaders/basic.fs.glsl")) return 1;

    // Texture.
    Texture texture;
    if (!texture_init(&texture, "../examples/test.png")) return 1;

    // Catalog.
    if (!catalog_init(&catalog)) {
        perror("catalog_init");
        return 1;
    }

    if (!catalog_add(&catalog, "../examples/shaders/basic.vs.glsl", shader_reload, &shader) ||
        !catalog_add(&catalog, "../examples/shaders/basic.fs.glsl", shader_reload, &shader) ||
        !catalog_add(&catalog, "../examples/test.png", texture_reload, &texture)) {
        perror("catalog_add");
        return 1;
    }

    // Textured quad.
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    const GLfloat quad[] = {
        -0.5f,-0.5f, 0.0f, 0.0f,
        -0.5f, 0.5f, 0.0f, 1.0f,
         0.5f,-0.5f, 1.0f, 0.0f,
         0.5f, 0.5f, 1.0f, 1.0f
    };
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_STATIC_DRAW);

    // Attrib pointers.
    glVertexAttribPointer(
        0,
        2,
        GL_FLOAT,
        GL_FALSE,
        4*sizeof(GLfloat),
        0
    );
    glEnableVertexAttribArray(0);
    glBindAttribLocation(shader.program, 0, "point");
    glVertexAttribPointer(
        1,
        2,
        GL_FLOAT,
        GL_FALSE,
        4*sizeof(GLfloat),
        (void *) (2*sizeof(GLfloat))
    );
    glEnableVertexAttribArray(1);
    glBindAttribLocation(shader.program, 1, "uv");

    // Uniform.
    GLint u_texture = glGetUniformLocation(shader.program, "texture");
    if (u_texture == -1) fprintf(stderr, "Couldn't find u_texture.");

    while (running) {

        while (SDL_PollEvent(&event)) {
            if      (event.type == SDL_QUIT) running = 0;
            else if (event.type == SDL_WINDOWEVENT) {
                if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
                    //w->width = event.window.data1;
                    //w->height = event.window.data2;
                    glViewport(0, 0, event.window.data1, event.window.data2);
                }
            }
        }

        // Refresh the catalog.
        if (!catalog_service(&catalog)) perror("catalog");

        // Draw.
        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(shader.program);
        glUniform1i(u_texture, texture.index);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        SDL_GL_SwapWindow(window);

    }

    catalog_destroy(&catalog);
    shader_destroy(&shader);

    return 0;
}

char *load_entire_file(const char *filename) {

    FILE *f;
    size_t len;
    char *out;

    f = fopen(filename, "r");
    if (f == NULL) {
        perror("load_entire_file");
        return NULL;
    }

    fseek(f, 0, SEEK_END);
    len = ftell(f);
    rewind(f);

    out = malloc(len + 1);
    if (out == NULL) {
        perror("load_entire_file");
        fclose(f);
        return NULL;
    }

    fread(out, len, 1, f);
    fclose(f);
    out[len] = '\0';

    return out;
}

GLuint gl_create_shader(GLenum type, const char *src) {

    GLuint shader;
    GLint status;

    shader = glCreateShader(type);
    if (shader == 0) {
        fprintf(stderr, "glCreateShader failed.");
        return shader;
    }

    glShaderSource(shader, 1, &src, NULL);

    // Compile.
    glCompileShader(shader);
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE) {
        char error[1024];
        glGetShaderInfoLog(shader, 1024, NULL, error);
        fprintf(stderr, "glCompileShader failed: '%s'.", error);
        // Clean up as best as we can.
        glDeleteShader(shader);
        return shader;
    }

    return shader;
}

int shader_init(Shader *shader, const char *vert_path, const char *frag_path) {
    shader->vert_path = strdup(vert_path);
    shader->frag_path = strdup(frag_path);
    shader_reload(shader);
    return 1;
}

void shader_destroy(Shader *shader) {
    free(shader->vert_path);
    free(shader->frag_path);
}

void shader_reload(Shader *shader) {

    char *vert_src = NULL,
         *frag_src = NULL;
    GLint status;

    vert_src = load_entire_file(shader->vert_path);
    if (vert_src == NULL) goto shader_reload_exit;
    frag_src = load_entire_file(shader->frag_path);
    if (frag_src == NULL) goto shader_reload_exit;

    shader->vert_shader = gl_create_shader(GL_VERTEX_SHADER, vert_src);
    shader->frag_shader = gl_create_shader(GL_FRAGMENT_SHADER, frag_src);

    // Create and link.
    shader->program = glCreateProgram();
    if (shader->program == 0) {
        fprintf(stderr, "glCreateProgram failed.");
        return;
    }
    glAttachShader(shader->program, shader->vert_shader);
    glAttachShader(shader->program, shader->frag_shader);
    glLinkProgram(shader->program);
    // Assess errors.
    glGetProgramiv(shader->program, GL_LINK_STATUS, &status);
    if (status == GL_FALSE) {
        char error[1024];
        glGetProgramInfoLog(shader->program, 1024, NULL, error);
        fprintf(stderr, "glCompileShader failed: '%s'.", error);
        // Clean up as best as we can.
        glDeleteProgram(shader->program);
        return;
    }

    free(vert_src);
    free(frag_src);
    
    printf("reloaded\n");

shader_reload_exit:

    if (vert_src == NULL) free(vert_src);
    if (frag_src == NULL) free(frag_src);

}

int texture_init(Texture *texture, const char *path) {

    static int index = 0;

    texture->path = strdup(path);
    texture->index = index;
    texture_reload(texture);

    index++;

    return 1;
}

void texture_destroy(Texture *texture) {
    free(texture->path);
}

void texture_reload(Texture *texture) {
    int w, h, n;
    unsigned char *data = stbi_load(texture->path, &w, &h, &n, 0);
    if (data == NULL) {
        fprintf(stderr, "Couldn't load image '%s'.", texture->path);
        return;
    }

    glGenTextures(1, &texture->texture);
    glActiveTexture(GL_TEXTURE0 + texture->index);
    glBindTexture(GL_TEXTURE_2D, texture->texture);

    if (n == 1) {
        // It's an alpha mask.
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, w, h, 0, GL_RED, GL_UNSIGNED_BYTE, data);
    } else if (n == 3) {
        // 3-channel image.
        glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, data);
    } else if (n == 4) {
        // 4-channel image.
        glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_ABGR_EXT, GL_UNSIGNED_BYTE, data);
    } else {
        fprintf(stderr, "texture_reload: n = %d not handled yet.", n);
    }

    // Clean up.
    stbi_image_free(data);
}
