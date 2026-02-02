#ifdef _WIN32
#include <windows.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include "cpup/io.h"
#include "cpup/vec.h"
#include "cpup/types.h"

typedef enum {
    TODO = 0,
    INPROGRESS = 1,
    DONE = 2
} TASK_STATUS;

typedef struct {
    const char* name;
    TASK_STATUS status;
} Task;

const unsigned int DEFAULT_TASK_CAPACITY = 1000;
Task* tasks;
const char* TASKS_FILE = "tasks.bin";

void list_tasks();
void add_task();
void remove_task(int _index);
void clear_tasks();
void save_tasks(const char* _file);
void load_tasks(const char* _file);
void run_sdl_window();

int main(int argc, char *argv[])
{
    vec_init(&tasks, DEFAULT_TASK_CAPACITY, sizeof(Task));
    load_tasks(TASKS_FILE);

    run_sdl_window();
    return 0;

    int should_loop = 1;

    while(should_loop == 1)
    {
        int s = request_int("1. List\n2. Add\n3. Remove\n4. Mark Task\n5. Exit\n");

        switch (s) {
            case 1:
                list_tasks();
                break;
            case 2:
                add_task();
                break;
            case 3:
                remove_task(request_int("Enter Tasks ID"));
                break;
            case 4:
                int index = request_int("Enter Tasks ID");
                int status = request_int("Enter Status 0. TODO 1. INPROGRESS 2. DONE");
                
                if (0 > index || vec_count(&tasks) <= index)
                    continue;
                if (status < 0 || 2 < status)
                    continue;

                tasks[index].status = status;
                break;
            case 5:
                should_loop = 0;
                break;
            default:
                break;
        };
    }

    save_tasks(TASKS_FILE);
    clear_tasks();

    vec_free(&tasks);
    return 0;
}

void list_tasks()
{
    for (int i = 0; i < vec_count(&tasks); i++)
        printf("%i Task Name %s Status: %i\n", i, tasks[i].name, tasks[i].status);
}

void add_task()
{
    Task task;
    task.name = request_string("Task");
    task.status = TODO;

    vec_add(&tasks, &task);
}

void remove_task(int _index)
{
    if (0 > _index || vec_count(&tasks) <= _index)
        return;
    
    free((char*)tasks[_index].name);

    vec_remove_at(&tasks, _index);
}

void clear_tasks()
{
    while (vec_count(&tasks) > 0)
        remove_task(vec_count(&tasks) - 1);
}

void save_tasks(const char* _file)
{
    FILE* file = fopen(_file, "wb");
    if (file == NULL) {
        fprintf(stderr, "Error opening file for writing\n");
        exit(1);
    }

    uint32_t count = (uint32_t)vec_count(&tasks);
    if (fwrite(&count, sizeof(count), 1, file) != 1) {
        fprintf(stderr, "Error writing task count\n");
        fclose(file);
        exit(1);
    }

    for (uint32_t i = 0; i < count; i++) {
        uint32_t status = (uint32_t)tasks[i].status;
        const char* name = tasks[i].name ? tasks[i].name : "";
        uint32_t name_len = (uint32_t)strlen(name);

        if (fwrite(&status, sizeof(status), 1, file) != 1 ||
            fwrite(&name_len, sizeof(name_len), 1, file) != 1 ||
            fwrite(name, 1, name_len, file) != name_len) {
            fprintf(stderr, "Error writing task\n");
            fclose(file);
            exit(1);
        }
    }

    fclose(file);
}

void load_tasks(const char* _file)
{
    FILE* file = fopen(_file, "rb");
    if (file == NULL) {
        fprintf(stderr, "Error opening file for reading\n");
        return;
    }

    uint32_t count = 0;
    if (fread(&count, sizeof(count), 1, file) != 1) {
        fprintf(stderr, "Error reading task count\n");
        fclose(file);
        exit(1);
    }

    clear_tasks();

    for (uint32_t i = 0; i < count; i++) {
        uint32_t status = 0;
        uint32_t name_len = 0;

        if (fread(&status, sizeof(status), 1, file) != 1 ||
            fread(&name_len, sizeof(name_len), 1, file) != 1) {
            fprintf(stderr, "Error reading task metadata\n");
            fclose(file);
            exit(1);
        }

        char* name = (char*)malloc((size_t)name_len + 1);
        if (name == NULL) {
            fprintf(stderr, "Failed to allocate task name\n");
            fclose(file);
            exit(1);
        }

        if (fread(name, 1, name_len, file) != name_len) {
            fprintf(stderr, "Error reading task name\n");
            free(name);
            fclose(file);
            exit(1);
        }

        name[name_len] = '\0';

        Task task;
        task.name = name;
        task.status = (TASK_STATUS)status;
        vec_add(&tasks, &task);
    }

    fclose(file);
}

void run_sdl_window()
{
    SDL_SetHint(SDL_HINT_VIDEO_DRIVER, "x11");

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) == false) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_Init Error: %s", SDL_GetError());
        exit(1);
    }

    SDL_Window* window = SDL_CreateWindow("fogpi", 800, 600, 0);
    if (window == NULL) {
        fprintf(stderr, "SDL_CreateWindow failed: %s\n", SDL_GetError());
        SDL_Quit();
        return;
    }

    int running = 1;
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = 0;
            }
        }
        SDL_Delay(16);
    }

    SDL_DestroyWindow(window);
    SDL_Quit();
}
