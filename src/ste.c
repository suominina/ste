#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#include "raylib.h"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 450

#define TEXT_BUFSIZE_STEP 1024

int font_size = 30;

void *alloc_linebuf(int text_bufsize)
{
    return malloc(sizeof(char) * text_bufsize);

}

int get_textinput(int key, int input_char_num, char *line)
{
    line[input_char_num] = (char)key;
    line[input_char_num+1] = '\0';
    input_char_num++;

    return input_char_num;
}

void draw_blinking_cursor(Rectangle text_canvas, 
                         Font font,
                         int font_size,
                         char *text_line, 
                         int current_line,
                         int frames_counter)
{
    if (((frames_counter / 20) % 2) == 0)
    {
        DrawText("_", text_canvas.x + font.glyphPadding + MeasureText(text_line, font_size), 
                      (text_canvas.y + font_size + font.glyphPadding) * current_line, font_size, MAROON);
    }
}


// TODO: Save content in text_buf
// TODO: Read an existing file
int main(void)
{
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "ste - suomi text editor");
    SetTargetFPS(60);

    Rectangle text_canvas = { 1, 1, WINDOW_WIDTH-1, WINDOW_HEIGHT-1 };

    // Font font = LoadfontEx("pixantiqua.ttf", font_size, NULL, 0);
    Font font = GetFontDefault();
    font.glyphPadding = 10;

    int max_lines = 4096;
    int input_char_num = 0;
    int text_bufsize = TEXT_BUFSIZE_STEP + 1; /* One extra byte for null terminator. */
    int current_line = 0;
    char **text_buf = malloc(sizeof(char *) * max_lines);
    assert(text_buf != NULL);
    text_buf[current_line] = malloc(sizeof(char) * text_bufsize);
    assert(text_buf[current_line] != NULL);
    *text_buf[current_line] = '\0';


    int frames_counter = 0;
    bool mouse_on_screen = false;
    while (!WindowShouldClose()) 
    {
        /* Text input */
        if (CheckCollisionPointRec(GetMousePosition(), text_canvas))
            mouse_on_screen = true;
        else
            mouse_on_screen = false;

        if (mouse_on_screen) 
        {
            SetMouseCursor(MOUSE_CURSOR_IBEAM);

            int key = GetCharPressed();

            while (key > 0) 
            {
                if ((key >= 32) && (key <= 125) && (input_char_num < text_bufsize)) 
                {
                    if (input_char_num == text_bufsize) 
                    {
                        text_bufsize += TEXT_BUFSIZE_STEP;
                        text_buf = realloc(text_buf, text_bufsize);
                        assert(text_buf != NULL);
                    }
                    input_char_num = get_textinput(key, input_char_num, text_buf[current_line]);
                }

                key = GetCharPressed();
            }

            if (IsKeyPressed(KEY_BACKSPACE))
            {
                if (input_char_num > 0)
                {
                    input_char_num--;
                }
                text_buf[current_line][input_char_num] = '\0';

                if (current_line > 0 && input_char_num == 0)
                {
                    current_line--;
                    input_char_num = strlen(text_buf[current_line]);
                }
            }
            if (IsKeyPressed(KEY_ENTER))
            {
                text_buf[current_line][input_char_num] = '\n';
                text_buf[current_line][input_char_num+1] = '\0';
                input_char_num = 0;
                current_line++;

                text_buf[current_line] = alloc_linebuf(text_bufsize);
                assert(text_buf[current_line] != NULL);
                *text_buf[current_line] = '\0';

            }
        } 
        else
            SetMouseCursor(MOUSE_CURSOR_DEFAULT);

        if (mouse_on_screen) 
            frames_counter++;
        else
            frames_counter = 0;


        /* Drawing */
        BeginDrawing();
        {

            DrawRectangleRec(text_canvas, RAYWHITE);
            if (mouse_on_screen) 
            {
                DrawRectangleLines((int)text_canvas.x, (int)text_canvas.y,
                                   (int)text_canvas.width, (int)text_canvas.height, PURPLE);
            }
            else
            {
                DrawRectangleLines((int)text_canvas.x, (int)text_canvas.y,
                                   (int)text_canvas.width, (int)text_canvas.height, DARKGRAY);
            }


            for (int i = 0; i <= current_line; i++)
            {
                DrawText(text_buf[i], (text_canvas.x + font.glyphPadding), 
                        (text_canvas.y + font_size + font.glyphPadding) * i, font_size, DARKGRAY);
            }

            if (mouse_on_screen)
            {
                draw_blinking_cursor(text_canvas, font, font_size, text_buf[current_line], 
                                     current_line, frames_counter);
            }
        }
        EndDrawing();
    }

    free(text_buf);
    CloseWindow();

    return 0;
}
