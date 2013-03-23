#include "apep_engine.hpp"
#include <conio.h>
#include <assert.h>

Apep::Apep() : back_buffer(nullptr) {
    blank_tile.Attributes     = 0;   // black on black
    blank_tile.Char.AsciiChar = 0xDB; // full rect
    
    start_pos.X = 0;
    start_pos.Y = 0;
}

void Apep::writeScore(int score, int x, int y, uchar col) {
    std::stringstream str;
    str << "Your score is: " << score;
    std::string res = str.str();

    int i = 0;
    for (auto c = res.begin(); c != res.end(); ++c, ++i) {
        at(x + i,y).Char.AsciiChar = *c;
        at(x + i,y).Attributes = col;
    }
}

void Apep::initialize(std::string name, int width_ = 80, int height_ = 60) {
    tin  = GetStdHandle(STD_INPUT_HANDLE);
    tout = GetStdHandle(STD_OUTPUT_HANDLE);
    
    SetConsoleTitle(name.c_str());
        
    { // hide console cursor
        CONSOLE_CURSOR_INFO cci;
        GetConsoleCursorInfo(tout, &cci);
        cci.bVisible = false;
        cci.dwSize   = 1;
        SetConsoleCursorInfo(tout, &cci);
    }

    width  = width_;
    height = height_;
    
    assert("width must be > 1!" && width > 1);
    assert("height must be > 1!" && height > 1);
    // resizing console window
    {
        SMALL_RECT cs = { 0, 0, width - 1, height - 1 };
        consoleSize = cs;
        COORD bs      = { width, height };
        buffSize = bs;
        SetConsoleWindowInfo(tout, TRUE, &consoleSize);
        SetConsoleScreenBufferSize(tout, buffSize);
    }

    { // turning off showing keys being pressed
        DWORD mode;
        GetConsoleMode(tin, &mode);
        SetConsoleMode(tin, mode & ~ENABLE_ECHO_INPUT);
    }

    // allocating back buffer
    total_size  = width * height;
    back_buffer = new CHAR_INFO[total_size];
}

/// doesn't work on Win7 :(
void Apep::fullscreen(bool full)
{
    SetConsoleDisplayMode(tout, full? CONSOLE_FULLSCREEN_MODE : CONSOLE_WINDOWED_MODE, nullptr);
}

void Apep::clean_screen(uchar c, WORD col) {
    blank_tile.Attributes     = col;
    blank_tile.Char.AsciiChar = c;
    for (int i = 0; i < total_size; ++i)
        back_buffer[i] = blank_tile;
}

int Apep::wait_for_key() {
    return getchar();
}

/// shit happens, not working well :|
void Apep::input() {
    INPUT_RECORD in;
    DWORD count;
    BOOL res = ReadConsoleInput(tin, &in, 1, &count);
    if (count == 1) {
        if (in.EventType == KEY_EVENT) {
            auto k = in.Event.KeyEvent;
            if (k.bKeyDown) {
                if (k.wVirtualKeyCode == VK_SPACE)
                    key((int)' ');
                else if (k.wVirtualKeyCode == VK_UP) key('w');
                else if (k.wVirtualKeyCode == VK_DOWN) key('s');
                else if (k.wVirtualKeyCode == VK_LEFT) key('a');
                else if (k.wVirtualKeyCode == VK_RIGHT) key('d');
            }
        }
    }
}

void Apep::run(){
    assert("You must initialize engine before run!" && back_buffer != nullptr);
    clean_screen();

    if (!game_setup()) return;

    while (game_update()) {

        while (_kbhit())
            key(_getch());
        
        game_draw();
        flush();

        Sleep(10);
    }

    game_cleanup();
}

void Apep::flush() {
    WriteConsoleOutputA(tout, back_buffer, buffSize, start_pos, &consoleSize); 
}

CHAR_INFO& Apep::at(int x, int y) {
    assert("Wrong range for x in at" && x >= 0 && x < width);
    assert("Wrong range for y in at" && y >= 0 && y < height);
    return back_buffer[ x + y * width ];
}

void Apep::blit(uchar s, int x, int y, uchar col) {
    at(x, y).Char.AsciiChar = s;
    at(x, y).Attributes     = col;
}

void Apep::blit(const Sprite& sprite, int x, int y) {
    int xoff = 0,
        yoff = 0;

    if (x < 0) { xoff = -x; x = 0; }
    if (y < 0) { yoff = -y; y = 0; }

    int sx,
        sy = yoff;

    for (int j = y; j < height && sy < sprite.h; ++j, ++sy) {
        sx = xoff;
        for (int i = x; i < width && sx < sprite.w;  ++i, ++sx) {
            if (sprite.sym_at(sx, sy) != 255) { // 0-1 alpha blending
                at(i,j).Char.AsciiChar = sprite.sym_at(sx, sy);
                at(i,j).Attributes     = sprite.color_at(sx, sy);
            }
        }
    }
}

void Apep::blit2(const Sprite& sprite, int x, int y, int limw = 1000 ,int limh = 1) {
    int xoff = 0,
        yoff = 0;

    if (x < 0) { xoff = -x; x = 0; }
    if (y < 0) { yoff = -y; y = 0; }

    int sx,
        sy = yoff;

    int sh = sprite.h * limh,
        sw = sprite.w * limw;

    for (int j = y; j < height && sy < sh; ++j, ++sy) {
        sx = xoff;
        for (int i = x; i < width && sx < sw;  ++i, ++sx) {
            if (sprite.sym_at(sx, sy) != ' ') { // 0-1 alpha blending
                at(i,j).Char.AsciiChar = sprite.sym_at(sx, sy);
                at(i,j).Attributes     = sprite.color_at(sx, sy);
            }
        }
    }
}
