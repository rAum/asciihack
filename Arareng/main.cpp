#include "apep_engine.hpp"
#include <Mmsystem.h>
#include <algorithm>
#include <vector>
#include <map>
#include <list>

extern uchar rock[];
extern uchar rockColor[];
extern uchar rockColor2[];
extern uchar cloud[];
extern uchar cloudColor[];
extern uchar tree[];
extern uchar treeColor[];
extern uchar tree2[];
extern uchar treeColor2[];

#pragma comment(lib, "Winmm.lib")

enum Colors {
    ColBlack = 0
    ,ColGreen = 2
    ,ColViv = FOREGROUND_INTENSITY
};

static uchar gameoverText[] = 
"   ____                         ___                  "
"  / ___| __ _ _ __ ___   ___   / _ \\__   _____ _ __  "
" | |  _ / _` | '_ ` _ \\ / _ \\ | | | \\ \\ / / _ \\ '__| "
" | |_| | (_| | | | | | |  __/ | |_| |\\ V /  __/ |    "
"  \\____|\\__,_|_| |_| |_|\\___|  \\___/  \\_/ \\___|_|    ";
static uchar gameOverColor[sizeof(gameoverText)];

struct Bullet {
    int x, y;
    uchar t;
};

struct Enemy : public Sprite {
    int hp;
    int x, y, drunk, speed;
    int id;
    static int counter;

    static Enemy Genemy(int x_, int y_) {
        static Enemy enem[3];

        for (int i = 0; i < 3; ++i) {
            enem[i].w = 3;
            enem[i].h = 3;
            enem[i].hp = rand() % 3 + 1;
            enem[i].speed = 1;
            enem[i].drunk = i;
        }

        uchar c = BKG | FOREGROUND_GREEN | FOREGROUND_RED | ColViv;
        static uchar ac[] = { 255, c, c, FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN | BKG | ColViv, c, c, 255, c, c };
        static uchar at[] = { 255, 191, 218,
                              2, 16, 93,
                              255, 217, 192
        };

        enem[0].colors = ac;
        enem[0].text   = at;
        c = BKG | FOREGROUND_RED | ColViv;
        static uchar bc[] = { 255, c, c, FOREGROUND_RED | FOREGROUND_BLUE | BKG | ColViv, c, c, 255, c, c };
        static uchar bt[] = { 255, 191, 218,
                              2, 29, 175,
                              255, 217, 192
        };

        enem[1].colors = bc;
        enem[1].text   = bt;
        enem[1].hp    += 1;
        enem[1].speed += 1;

        c = BKG | FOREGROUND_RED | FOREGROUND_GREEN | ColViv;
        static uchar cc[] = { c, c, c, FOREGROUND_RED | BKG | ColViv, c, c, c, c, c };
        static uchar ct[] = { 29, 201, 187,
                              17, 16, 219,
                              29, 200, 188
        };

        enem[2].colors = cc;
        enem[2].text   = ct;
        enem[2].hp    += 2;
        enem[2].speed += 1;

        Enemy e = enem[rand()%3];
        e.x = x_;
        e.y = y_;
        e.id = counter++;
        return e;
    }
};

int Enemy::counter = 0;

struct vec2 { 
    int x, y, speed; 
    vec2(int x_ = 0, int y_ = 0) : x(x_), y(y_), speed(rand()%3+1) {} 
    vec2(int x_, int y_, int t) : x(x_), y(y_), speed(t) {} 
    
};

struct Game : public Apep
{
    bool running;
    int frames;
    
    int x, y;

    std::list<Bullet> bullet;
    std::list<Enemy> enemy;

    int hp, score;

    static const uchar const smig[];
    static uchar smigu(int i) { return smig[(i/4) & 0x3]; }

    Sprite GmOvr;
    Sprite gory, gory2;
    Sprite chmura;
    Sprite drzewo, drzewo2;
    std::list<vec2> chmurki;
    std::list<vec2> drzewa;
    Game() : running(true), frames(0), x(5), y(30) {
        hp = 100;
        score = 0;
        how_fucked = 10;
        eticks = 15;
        mode = Mode_Game;
        for (int i = 0; i < sizeof(gameoverText); ++i) {
            gameOverColor[i] = BACKGROUND_BLUE | FOREGROUND_RED | FOREGROUND_INTENSITY;
        }

        GmOvr.colors = gameOverColor;
        GmOvr.text   = gameoverText;
        GmOvr.w      = 53;
        GmOvr.h      = 5;

        uchar c, cc;
        for (int i = 0; i < 72 * 16; ++i) {
            if (rock[i] =='8') rock[i] = 0xB2;//0xDB;
            switch (rock[i]) {
            case ' ': c = cc = BKG; break;
            case '_': c =  FOREGROUND_RED ; cc = 0; break;
            default: 
                c =  FOREGROUND_RED  | BKG; 
                cc = 0;
                break;
            }
            rockColor[i] = c;
            rockColor2[i] = cc;
        }
        gory.colors = rockColor;
        gory.text   = rock;
        gory.h = 16;
        gory.w = 72;
        gory2.colors = rockColor2;
        gory2.text   = rock;
        gory2.h = 16;
        gory2.w = 72;
        bkgOff = 0;

        chmura.w = 17;
        chmura.h = 5;
        for (int i = 0; i < chmura.w * chmura.h; ++i) {
            cloudColor[i] = BKG | FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY;
            if (cloud[i] == ' ') cloud[i] = 255;
        }
        chmura.text = cloud;
        chmura.colors = cloudColor;

        drzewo.w = 10;
        drzewo.h = 7;
        for (int i = 0; i < drzewo.h * drzewo.w; ++i) {
            if (treeColor[i] == 0 || tree[i] == ' ') tree[i] = 255;
        }
        drzewo.text = tree;
        drzewo.colors  = treeColor;

        drzewo2.w = 10;
        drzewo2.h = 7;
        for (int i = 0; i < drzewo2.h * drzewo2.w; ++i) {
            if (treeColor2[i] == 0 || tree2[i] == ' ') tree2[i] = 255;
        }
        drzewo2.text = tree;
        drzewo2.colors  = treeColor;

        for (int i = 0; i < 20; ++i)
            add_enemy();
    }

    void up_cloud() {
        if (rand() % 210 == 0 && chmurki.size() <= 3) chmurki.push_back(vec2(79, rand()%23));

        if (rand() % 280 == 0 && drzewa.size() <= 4) drzewa.push_back(vec2(79, 50 - drzewo.h + rand()%3, rand()%2));

        if (frames % 20 == 0) {
            for (auto it = chmurki.begin(); it != chmurki.end(); ) {
                it->x -= it->speed;
                if (it->x <= -chmura.w) {
                    it = chmurki.erase(it);
                } else ++it;
            }
        }

        if (frames % 8 == 0) {
             for (auto it = drzewa.begin(); it != drzewa.end(); ) {
                it->x -= 2;
                if (it->x <= -drzewo.w) {
                    it = drzewa.erase(it);
                } else ++it;
            }
        }
    }

    void draw_cloud() {
        for (auto it = chmurki.begin(); it != chmurki.end(); ++it) {
            blit(chmura, it->x, it->y);
        }
    }

    void draw_trees() {
        for (auto it = drzewa.begin(); it != drzewa.end(); ++it) {
            if (it->speed == 0) blit(drzewo, it->x, it->y);
            else                blit(drzewo2, it->x, it->y);
        }
    }

#define PBULL '='
    // player shot
    void pshot() {
        static int wait = 0;
        if (wait % 3) {
            Bullet b = { x + 6, y + 1, PBULL };
            bullet.push_back(b);
        }
        ++wait;
    }

    void up_bullet() {
        for (auto p = bullet.begin(); p != bullet.end(); ++p) {
            if (p->t == PBULL) {
                p->x += frames % 2? 1 : 0;
            }
        }

        auto end = bullet.end();
        for (auto p = bullet.begin(); p != end; ){
            if (p->x >= 80) {
                p = bullet.erase(p);
            }
            else ++p;
        }
    }

    int how_fucked;

    void add_enemy() {
        if ((frames % 18 == 0) && (rand() % 200) <= how_fucked) {
            enemy.push_back(Enemy::Genemy(79, rand() % 47));
        }
    }

    static int clamp(int v, int a, int b) {
        if (v < a) return a;
        if (v > b) return b;
        return v;
    }

    int eticks;

    bool colide(Enemy& e) {
        int a = e.x,
            b = e.x + e.w,
            c = e.y,
            d = e.y + e.h;

        for (auto it = bullet.begin(); it != bullet.end(); ++it) {
            int x = it->x;
            int y = it->y;

            if (x >= a && x <= b && y >= c && y <= d) {
                bullet.erase(it);
                return true;
            }
        }

        return false;
    }

    enum Mode {
        Mode_Intro,
        Mode_Game,
        Mode_GameOver
    };

    Mode mode;

    bool player_destroyed_by(Enemy & e) {
        int r2left = e.x,
            r1right = x + 4,
            r2right = e.x + 3,
            r2bottom = e.y + 3,
            r2top = e.y,
            r1left = x,
            r1top = y + 1,
            r1bottom = y + 3;

        return !(r2left > r1right || r2right < r1left || r2top > r1bottom || r2bottom < r1top);
    }

    void up_enemy() {
        add_enemy();

        for (auto it = enemy.begin(); it != enemy.end(); ++it) {
            if (frames % eticks == 0) {
                it->x -= it->speed;
                if (it->drunk == 2) {
                    it->y = clamp( sin((frames + it->id * 2) * 0.1) * 4 + it->y, 0, 45);
                    if (rand() % 5 == 0) it->y += 1 - rand() % 2;
                } else if (it->drunk == 1) {
                    it->y = clamp( cos((frames + it->id * 4)* 0.03) * 6 + it->y, 0, 45);
                }
                if (rand() % 6 == 0) it->y += 1 - rand() % 3;
            }
        }

        bool hit = false;
        for (auto it = enemy.begin(); it != enemy.end(); ) {
            hit = colide(*it);
            if (it->x < -2)
                it = enemy.erase(it);
            else if (hit) {
                it->hp -= 1;
                if (it->hp < 1) {
                    it = enemy.erase(it);
                    score += 10;
                    ++it;
                }
            }
            else if(player_destroyed_by(*it)) {
                mode = Mode_GameOver;
                frameOver = frames;
                return;
            }
            else
                ++it;
        }
    }

    void key(int keycode) {
        if (keycode == 'p')
            running = false;
        if (keycode == ' ')
            pshot();
        if (keycode == 'a') --x;
        if (keycode == 'd') ++x;
        if (keycode == 'w') --y;
        if (keycode == 's') ++y;

        /// zostanie na planszy
        if (x < 0) x = 0;
        else if (x > 64) x = 64;
        if (y < 0) y = 0;
        else if (y > 45) y = 45;
    }

    Sprite player_sprite()
    {
        static Sprite spr; //9 - smiglo
        static uchar t[] = { 'L', 255, 255, 255, 255,
                     219, 219, 220, 16, smigu(frames),
                     17, 245, 255, 255, 255, 255
        };

        t[9] = smigu(frames);

        uchar def = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED | BKG;
        static uchar col[] = { def, def, def, def, def,
            def, def, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED | BACKGROUND_BLUE |BACKGROUND_GREEN | BACKGROUND_RED | BACKGROUND_INTENSITY, def, def,
                        def, def, def, def, def
        };

        spr.colors = col;
        spr.text   = t;
        spr.w = 5;
        spr.h = 3;

        return spr;
    }

    int bkgOff;
    void game_draw() {
        if (mode == Mode_Game) {
            clean_screen(0,  BKG);
            
            draw_cloud();

            bkgOff += frames % 3 == 0? -1 : 0;

            blit2(gory2, (bkgOff/8 - 41) % gory.w, 30, 10000, 1);
            blit2(gory, bkgOff/2 % gory.w, 35, 10000, 1);

            for (auto p = enemy.begin(); p != enemy.end(); ++p)
                blit(*p, p->x, p->y);

            for (auto p = bullet.begin(); p != bullet.end(); ++p) {
                blit(p->t, p->x, p->y, BKG | FOREGROUND_RED | FOREGROUND_INTENSITY);
            }

            blit(player_sprite(), x, y);

            draw_trees();
        } else if (mode == Mode_GameOver) {
            clean_screen(219,  BKG| FOREGROUND_BLUE);
            blit(GmOvr, sin((frames+10) * 0.04) * 10 + 10, sin(frames * 0.04) * 10 + 20 );
            blit(player_sprite(), x, y);
            y += frames % 2 == 0? 1 : 0;
        }

        writeScore(score, 0, 0, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    }

    int frameOver;
    bool game_update() {
        ++frames;

        if (mode == Mode_Game) {
            up_bullet();
            up_enemy();
            up_cloud();

            if (frames % 100 == 0) how_fucked += 4;
            if (frames % 260 == 0) eticks = --eticks < 1? 1 : eticks;
        } else if (mode == Mode_GameOver) {
            if ((frameOver + 1600 < frames) || !running) {
                return false;
            }
            return true;
        }
        else {
            
        }

        return running;
    }
};

uchar g[] = { 219, 178, 177, 176 };
const uchar const Game::smig[] =  { '|', '/', '-', '\\' };

#include <ctime>
int main(int, char**argv) {
    Game game;
    game.initialize("Invasion++", 80, 50);

    srand(time(nullptr));
    
    // play muzik!
    PlaySound("D:\\8_Bit_Sound_-_bit_5.wav", nullptr, SND_FILENAME | SND_LOOP | SND_ASYNC);
    
    game.run();

    return 0;
}
