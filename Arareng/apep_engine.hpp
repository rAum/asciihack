#ifndef __APEP_HPP__
#define __APEP_HPP__

#include <string>
#include <sstream>

// hate this, but must use :(
#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_MEAN
#include <Windows.h>

typedef unsigned char uchar;
#define BKG (BACKGROUND_BLUE)

struct Sprite {
    int w, h;
    uchar* text;
    uchar* colors;

    uchar color_at(int i, int j) const {
        return colors[index(i,j)];
    }

    int index(int i, int j) const {
        return abs(i % w) + abs(j % h) * w;
    }

    uchar sym_at(int i, int j) const {
        return text[index(i,j)];
    }
};

/// Simple engine for gamez with ASCII art.
/// To use, just inherit from ApepEngine class.
/// --------------------------------------------
/// After that, your codeflow should be:
/// 1. run intialize() to setup console window
/// 2. call run() to begin game loop which looks like that:
///    - clean screen
///    - call game_setup()
///    - while game_update() returns true 
///        - game_draw()
///    - call game cleanup();
/// -------------------------------------------
/// Engine provides also simple method for drawing sprites and very simple animations.
/// It is also FPS stable (at 25 FPS).
class Apep {
public:
    Apep();

    void initialize(std::string name, int width, int height);
    void run();

    static int color(int fg, int bkg) {
    return (fg & 7) + ((bkg & 7) << 4);
}
    void blit(uchar s, int x, int y, uchar col);

    //--------------------
    CHAR_INFO& at(int x, int y);
    void clean_screen(uchar c = 0xDB, WORD col = 0);
    void fullscreen(bool full = true);
    //--------------------

    int wait_for_key();

    void input();

    void blit(const Sprite& sprite, int x, int y);
    void blit2(const Sprite& sprite, int x, int y, int repx, int repy);
    void writeScore(int score, int x, int y, uchar col = 1);
protected:
    virtual void key(int keycode) {}
    virtual bool game_setup()   { return true; }
    virtual void game_cleanup() {}
    virtual void game_draw()    {}
    virtual bool game_update()  { return false; }

private:
    void flush();

    HANDLE tout, // terminal out 
           tin;  // terminal in
    SMALL_RECT consoleSize;
    COORD buffSize,
          start_pos;
    CHAR_INFO* back_buffer;
    int total_size;
    int width, height;
    
    CHAR_INFO blank_tile;
};

#endif //__APEP_HPP__