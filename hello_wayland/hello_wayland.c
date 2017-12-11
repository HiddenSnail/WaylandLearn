#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wayland-client.h>

// 是hello_*函数的头文件
#include "helpers.h"

// 窗口，图片，光标大小
static const unsigned WIDTH = 320;
static const unsigned HEIGHT = 200;
static const unsigned CURSOR_WIDTH = 100;
static const unsigned CURSOR_HEIGHT = 30;
static const int32_t CURSOR_HOT_SPOT_X = 10;
static const int32_t CURSOR_HOT_SPOT_Y = 35;

static bool done = false;

// 鼠标按下的回调函数，会将done置为true然后离开while循环
void on_button(uint32_t button)
{
    done = true;
}

int main(void)
{
    struct wl_buffer *buffer;
    struct wl_shm_pool *pool;
    struct wl_shell_surface *surface;
    int image;

    hello_setup_wayland();

    image = open("images.bin", O_RDWR);

    if (image < 0) {
        perror("Error opening surface image");
        return EXIT_FAILURE;
    }
    // 这个地方会创建一个共享内存区，里面的image供client和server使用，从而提升效率
    pool = hello_create_memory_pool(image);
    surface = hello_create_surface();
    buffer = hello_create_buffer(pool, WIDTH, HEIGHT);
    
    // bind操作会将surface提交至服务器
    hello_bind_buffer(buffer, surface);
   
    hello_set_cursor_from_pool(pool, CURSOR_WIDTH,
        CURSOR_HEIGHT, CURSOR_HOT_SPOT_X, CURSOR_HOT_SPOT_Y);
    hello_set_button_callback(surface, on_button);

    while (!done) {
        if (wl_display_dispatch(display) < 0) {
            perror("Main loop error");
            done = true;
        }
    }

    fprintf(stderr, "Exiting sample wayland client...\n");

    hello_free_cursor();
    hello_free_buffer(buffer);
    hello_free_surface(surface);
    hello_free_memory_pool(pool);
    close(image);
    hello_cleanup_wayland();

    return EXIT_SUCCESS;
}
