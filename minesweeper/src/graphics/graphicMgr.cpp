#include "graphicMgr.hpp"
#include <allegro5/allegro_primitives.h>
#include <algorithm>

static const struct { const char* name; const char* sub; int gw, gh, mines; }
PRESETS[3] = {
    {"FACIL",    "9x9   - 10 minas",  9,  9, 10},
    {"MEDIO",    "16x16 - 40 minas", 16, 16, 40},
    {"DIFICIL",  "30x16 - 99 minas", 30, 16, 99},
};

// Menu bottom-row button layout
static constexpr int BTN_X = 30, BTN_W = 240, BTN_H = 40, BTN_Y0 = 65, BTN_GAP = 50;
static constexpr int BOT_Y = 225, BOT_H = 35;
static constexpr int LB_X  = 30,  LB_W2  = 110;   // leaderboard button
static constexpr int PR_X  = 152, PR_W2  = 108;   // profiles button

// Profile screen layout
static constexpr int PR_ROW_Y0 = 48, PR_ROW_H = 26, PR_MAX_ROWS = 7;
static constexpr int PR_NEW_Y  = 238, PR_NEW_H = 27;

static ALLEGRO_COLOR player_color(int idx) {
    static const float c[][3] = {
        {1.0f, 0.32f, 0.32f}, {0.32f, 0.6f,  1.0f},
        {0.32f, 0.9f, 0.42f}, {1.0f,  0.9f,  0.2f},
        {1.0f, 0.5f,  0.2f},  {0.72f, 0.32f, 1.0f},
        {0.2f, 0.9f,  0.9f},  {1.0f,  0.32f, 0.72f},
    };
    int i = idx % 8;
    return al_map_rgb_f(c[i][0], c[i][1], c[i][2]);
}

// ── construction ─────────────────────────────────────────────────────────────

Graphic_Manager::Graphic_Manager(int bs) : bmp_size(bs) {
    display = al_create_display(MENU_W, MENU_H);
    font    = al_create_builtin_font();
}

Graphic_Manager::~Graphic_Manager() {
    al_destroy_font(font);
    al_destroy_display(display);
}

// ── display resize ────────────────────────────────────────────────────────────

void Graphic_Manager::start_game(int gw, int gh) {
    grid_w = gw; grid_h = gh;
    ALLEGRO_MONITOR_INFO info;
    al_get_monitor_info(0, &info);
    int sw = info.x2 - info.x1, sh = info.y2 - info.y1;
    int cs = std::min({bmp_size, sw / gw, (sh - 80 - TOOLBAR_H) / gh});
    cell_size = std::max(cs, 8);
    al_resize_display(display, gw * cell_size, gh * cell_size + TOOLBAR_H);
}

void Graphic_Manager::show_menu()        { grid_w=0;grid_h=0;cell_size=0; al_resize_display(display,MENU_W,MENU_H); }
void Graphic_Manager::show_leaderboard() { grid_w=0;grid_h=0;cell_size=0; al_resize_display(display,LB_W,LB_H); }
void Graphic_Manager::show_profiles()    { grid_w=0;grid_h=0;cell_size=0; al_resize_display(display,PROF_W,PROF_H); }

// ── hit testing ──────────────────────────────────────────────────────────────

bool Graphic_Manager::is_menu_click(int x, int y) const {
    return x>=5&&x<=35&&y>=5&&y<=35;
}
bool Graphic_Manager::is_reset_click(int x, int y) const {
    int bx=grid_w*cell_size/2-15;
    return x>=bx&&x<=bx+30&&y>=5&&y<=35;
}

int Graphic_Manager::menu_click(int x, int y) const {
    if (x >= BTN_X && x <= BTN_X+BTN_W)
        for (int i=0;i<3;i++) { int by=BTN_Y0+i*BTN_GAP; if(y>=by&&y<=by+BTN_H) return i; }
    if (y>=BOT_Y && y<=BOT_Y+BOT_H) {
        if (x>=LB_X && x<=LB_X+LB_W2) return 3;
        if (x>=PR_X && x<=PR_X+PR_W2) return 4;
    }
    return -1;
}

int  Graphic_Manager::lb_tab_click(int x, int y) const {
    if(y<5||y>40) return -1;
    for(int i=0;i<3;i++){int bx=10+i*95;if(x>=bx&&x<=bx+85)return i;}
    return -1;
}
bool Graphic_Manager::lb_back_click(int x, int y) const {
    return x>=LB_W-90&&x<=LB_W-5&&y>=5&&y<=40;
}

bool Graphic_Manager::prof_back_click(int x, int y) const {
    return x>=265&&x<=350&&y>=5&&y<=35;
}
int Graphic_Manager::prof_select_click(int x, int y, int count) const {
    if(x<10||x>285) return -1;
    for(int i=0;i<std::min(count,PR_MAX_ROWS);i++){
        int ry=PR_ROW_Y0+i*PR_ROW_H;
        if(y>=ry&&y<=ry+PR_ROW_H-2) return i;
    }
    return -1;
}
int Graphic_Manager::prof_delete_click(int x, int y, int count) const {
    if(x<295||x>345) return -1;
    for(int i=0;i<std::min(count,PR_MAX_ROWS);i++){
        int ry=PR_ROW_Y0+i*PR_ROW_H;
        if(y>=ry&&y<=ry+PR_ROW_H-2) return i;
    }
    return -1;
}
bool Graphic_Manager::prof_add_click(int x, int y) const {
    return x>=265&&x<=350&&y>=PR_NEW_Y&&y<=PR_NEW_Y+PR_NEW_H;
}

// ── drawing helpers ───────────────────────────────────────────────────────────

void Graphic_Manager::draw_scaled_text(const char* text, float sx, float sy,
                                        int align, ALLEGRO_COLOR color, float scale) {
    ALLEGRO_TRANSFORM saved, t;
    al_copy_transform(&saved, al_get_current_transform());
    al_identity_transform(&t);
    al_scale_transform(&t, scale, scale);
    al_use_transform(&t);
    al_draw_text(font, color, sx/scale, sy/scale, align, text);
    al_use_transform(&saved);
}

void Graphic_Manager::draw_lcd_number(int n, float screen_x, int align) {
    if(n<-99)n=-99; if(n>999)n=999;
    ALLEGRO_TRANSFORM saved,s;
    al_copy_transform(&saved,al_get_current_transform());
    al_identity_transform(&s); al_scale_transform(&s,2,2); al_use_transform(&s);
    const char* fmt=(n<0)?"-%02d":"%03d";
    al_draw_textf(font,al_map_rgb(220,50,50),screen_x/2.0f,6.0f,align,fmt,(n<0)?-n:n);
    al_use_transform(&saved);
}

void Graphic_Manager::draw_lcd_timer(int ms, float screen_x, int align) {
    int secs=ms/1000,millis=ms%1000; if(secs>999){secs=999;millis=999;}
    ALLEGRO_TRANSFORM saved,s;
    al_copy_transform(&saved,al_get_current_transform());
    al_identity_transform(&s); al_scale_transform(&s,2,2); al_use_transform(&s);
    al_draw_textf(font,al_map_rgb(220,50,50),screen_x/2.0f,6.0f,align,"%03d.%03d",secs,millis);
    al_use_transform(&saved);
}

// ── graph ─────────────────────────────────────────────────────────────────────

void Graphic_Manager::draw_graph(const Leaderboard& lb, int difficulty,
                                  float gx, float gy, float gw, float gh) {
    al_draw_filled_rectangle(gx,gy,gx+gw,gy+gh,al_map_rgb(15,15,25));
    auto all=lb.get_all(difficulty);
    auto players=lb.get_players();
    if(all.empty()){
        al_draw_text(font,al_map_rgb(100,100,120),gx+gw/2,gy+gh/2,
                     ALLEGRO_ALIGN_CENTRE,"Sin datos para esta dificultad");
        return;
    }
    int y_lo=all[0].elapsed_ms,y_hi=all[0].elapsed_ms,x_max=0;
    for(auto&e:all){y_lo=std::min(y_lo,e.elapsed_ms);y_hi=std::max(y_hi,e.elapsed_ms);}
    for(auto&name:players){auto h=lb.get_player_history(name,difficulty);x_max=std::max(x_max,(int)h.size());}
    float pad=(y_hi-y_lo)*0.12f+500.0f;
    float fy_lo=y_lo-pad,fy_hi=y_hi+pad;
    float mx=58,my=15,mr=15,mb=30;
    float px=gx+mx,py=gy+my,pw=gw-mx-mr,ph=gh-my-mb,px2=px+pw,py2=py+ph;
    al_draw_filled_rectangle(px,py,px2,py2,al_map_rgb(22,22,35));
    ALLEGRO_COLOR grid=al_map_rgba(55,55,70,200);
    for(int i=0;i<=5;i++){float yy=py+(float)i/5.0f*ph;al_draw_line(px,yy,px2,yy,grid,0.5f);}
    ALLEGRO_COLOR ac=al_map_rgb(140,140,160);
    al_draw_line(px,py,px,py2,ac,1.5f); al_draw_line(px,py2,px2,py2,ac,1.5f);
    for(int i=0;i<=5;i++){
        float t=(float)i/5.0f,ms2=fy_lo+(fy_hi-fy_lo)*t,yy=py2-t*ph;
        al_draw_line(px-3,yy,px,yy,ac,1); al_draw_textf(font,ac,px-5,yy-4,ALLEGRO_ALIGN_RIGHT,"%.1f",ms2/1000.0f);
    }
    int step=std::max(1,x_max/8);
    for(int i=0;i<x_max;i+=step){
        float t=(x_max>1)?(float)i/(float)(x_max-1):0.0f,xx=px+t*pw;
        al_draw_line(xx,py2,xx,py2+3,ac,1); al_draw_textf(font,ac,xx,py2+5,ALLEGRO_ALIGN_CENTRE,"%d",i+1);
    }
    int ci=0; float lx=px2-5,ly=py+5;
    for(auto&name:players){
        auto hist=lb.get_player_history(name,difficulty);
        if(hist.empty()){ci++;continue;}
        ALLEGRO_COLOR c=player_color(ci++);
        float prev_sx=-1,prev_sy=-1;
        for(int i=0;i<(int)hist.size();i++){
            float t_x=(x_max>1)?(float)i/(float)(x_max-1):0.0f;
            float t_y=(fy_hi>fy_lo)?(hist[i].elapsed_ms-fy_lo)/(fy_hi-fy_lo):0.5f;
            float sx=px+t_x*pw,sy=py2-t_y*ph;
            if(prev_sx>=0) al_draw_line(prev_sx,prev_sy,sx,sy,c,2);
            al_draw_filled_circle(sx,sy,3.5f,c);
            prev_sx=sx;prev_sy=sy;
        }
        if(ly<py2-12){
            al_draw_filled_rectangle(lx-82,ly+1,lx-67,ly+9,c);
            al_draw_text(font,c,lx-65,ly,ALLEGRO_ALIGN_LEFT,name.c_str());
            ly+=14;
        }
    }
}

// ── menu ─────────────────────────────────────────────────────────────────────

void Graphic_Manager::render_menu(const ProfileManager& pm) {
    al_set_target_backbuffer(display);
    al_clear_to_color(al_map_rgb(28,28,28));

    draw_scaled_text("BUSCAMINAS",MENU_W/2.0f,16.0f,
                     ALLEGRO_ALIGN_CENTRE,al_map_rgb(240,240,240),2.0f);

    // Active profile indicator
    if(pm.has_active()){
        std::string label="Perfil: "+pm.get_active();
        al_draw_text(font,al_map_rgb(100,210,120),MENU_W/2,40,ALLEGRO_ALIGN_CENTRE,label.c_str());
    } else {
        al_draw_text(font,al_map_rgb(110,110,120),MENU_W/2,40,ALLEGRO_ALIGN_CENTRE,"Sin perfil activo");
    }

    // Difficulty buttons
    for(int i=0;i<3;i++){
        int by=BTN_Y0+i*BTN_GAP;
        al_draw_filled_rectangle(BTN_X,by,BTN_X+BTN_W,by+BTN_H,al_map_rgb(55,55,65));
        al_draw_rectangle(BTN_X+0.5f,by+0.5f,BTN_X+BTN_W-0.5f,by+BTN_H-0.5f,al_map_rgb(130,130,140),1);
        float cx=BTN_X+BTN_W/2.0f;
        draw_scaled_text(PRESETS[i].name,cx,by+6.0f,ALLEGRO_ALIGN_CENTRE,al_map_rgb(230,230,230),2.0f);
        al_draw_text(font,al_map_rgb(150,150,160),cx,by+28.0f,ALLEGRO_ALIGN_CENTRE,PRESETS[i].sub);
    }

    // Bottom row: leaderboard + profiles
    al_draw_filled_rectangle(LB_X,BOT_Y,LB_X+LB_W2,BOT_Y+BOT_H,al_map_rgb(40,55,80));
    al_draw_rectangle(LB_X+0.5f,BOT_Y+0.5f,LB_X+LB_W2-0.5f,BOT_Y+BOT_H-0.5f,al_map_rgb(80,120,180),1);
    al_draw_text(font,al_map_rgb(130,190,255),LB_X+LB_W2/2,BOT_Y+13,ALLEGRO_ALIGN_CENTRE,"RECORDS");

    al_draw_filled_rectangle(PR_X,BOT_Y,PR_X+PR_W2,BOT_Y+BOT_H,al_map_rgb(50,70,50));
    al_draw_rectangle(PR_X+0.5f,BOT_Y+0.5f,PR_X+PR_W2-0.5f,BOT_Y+BOT_H-0.5f,al_map_rgb(80,160,80),1);
    al_draw_text(font,al_map_rgb(140,220,140),PR_X+PR_W2/2,BOT_Y+13,ALLEGRO_ALIGN_CENTRE,"PERFIL");

    al_flip_display();
}

// ── profiles screen ───────────────────────────────────────────────────────────

void Graphic_Manager::render_profiles(const ProfileManager& pm, const std::string& new_input) {
    al_set_target_backbuffer(display);
    al_clear_to_color(al_map_rgb(22,22,32));

    // Header
    al_draw_filled_rectangle(0,0,PROF_W,42,al_map_rgb(30,30,45));
    draw_scaled_text("PERFILES",PROF_W/2.0f,10.0f,ALLEGRO_ALIGN_CENTRE,al_map_rgb(220,220,255),2.0f);
    al_draw_filled_rectangle(265,5,350,37,al_map_rgb(90,40,40));
    al_draw_rectangle(265.5f,5.5f,349.5f,36.5f,al_map_rgb(180,70,70),1);
    al_draw_text(font,al_map_rgb(255,170,170),307,15,ALLEGRO_ALIGN_CENTRE,"VOLVER");

    al_draw_line(0,42,PROF_W,42,al_map_rgb(50,50,70),1);

    // Profile list
    auto& profiles=pm.get_all();
    int count=std::min((int)profiles.size(),PR_MAX_ROWS);
    for(int i=0;i<count;i++){
        int ry=PR_ROW_Y0+i*PR_ROW_H;
        bool is_active=(profiles[i]==pm.get_active());

        if(is_active)
            al_draw_filled_rectangle(10,ry,284,ry+PR_ROW_H-2,al_map_rgba(80,160,80,60));
        else if(i%2==0)
            al_draw_filled_rectangle(10,ry,284,ry+PR_ROW_H-2,al_map_rgba(255,255,255,8));

        ALLEGRO_COLOR nc=is_active?al_map_rgb(100,230,100):al_map_rgb(200,200,200);
        std::string prefix=is_active?" > ":"   ";
        al_draw_text(font,nc,15,ry+8,ALLEGRO_ALIGN_LEFT,(prefix+profiles[i]).c_str());

        // Delete [X]
        al_draw_filled_rectangle(295,ry+3,345,ry+PR_ROW_H-5,al_map_rgb(80,30,30));
        al_draw_rectangle(295.5f,ry+3.5f,344.5f,ry+PR_ROW_H-5.5f,al_map_rgb(160,60,60),1);
        al_draw_text(font,al_map_rgb(255,120,120),320,ry+8,ALLEGRO_ALIGN_CENTRE,"X");
    }

    if(profiles.empty())
        al_draw_text(font,al_map_rgb(100,100,120),PROF_W/2,130,
                     ALLEGRO_ALIGN_CENTRE,"Sin perfiles. Crea uno abajo.");

    // New profile row
    al_draw_line(0,PR_NEW_Y-8,PROF_W,PR_NEW_Y-8,al_map_rgb(45,45,65),1);
    al_draw_text(font,al_map_rgb(160,160,200),10,PR_NEW_Y+9,ALLEGRO_ALIGN_LEFT,"Nuevo:");
    al_draw_filled_rectangle(60,PR_NEW_Y,260,PR_NEW_Y+PR_NEW_H,al_map_rgb(12,12,28));
    al_draw_rectangle(60.5f,PR_NEW_Y+0.5f,259.5f,PR_NEW_Y+PR_NEW_H-0.5f,al_map_rgb(80,90,180),1);
    al_draw_text(font,al_map_rgb(210,210,255),64,PR_NEW_Y+9,ALLEGRO_ALIGN_LEFT,(new_input+"_").c_str());

    al_draw_filled_rectangle(265,PR_NEW_Y,350,PR_NEW_Y+PR_NEW_H,al_map_rgb(40,80,40));
    al_draw_rectangle(265.5f,PR_NEW_Y+0.5f,349.5f,PR_NEW_Y+PR_NEW_H-0.5f,al_map_rgb(70,160,70),1);
    al_draw_text(font,al_map_rgb(140,230,140),307,PR_NEW_Y+9,ALLEGRO_ALIGN_CENTRE,"ANADIR");

    al_flip_display();
}

// ── toolbar ───────────────────────────────────────────────────────────────────

void Graphic_Manager::render_toolbar(bool game_over, bool won,
                                      int mines_remaining, int elapsed_ms) {
    int sw=grid_w*cell_size;
    al_draw_filled_rectangle(0,0,sw,TOOLBAR_H,al_map_rgb(192,192,192));
    al_draw_line(0,TOOLBAR_H-1,sw,TOOLBAR_H-1,al_map_rgb(128,128,128),1);
    al_draw_filled_rectangle(5,5,35,35,al_map_rgb(220,220,220));
    al_draw_rectangle(5.5f,5.5f,34.5f,34.5f,al_map_rgb(80,80,80),1);
    al_draw_text(font,al_map_rgb(0,0,0),20,17,ALLEGRO_ALIGN_CENTRE,"M");
    draw_lcd_number(mines_remaining,45.0f,ALLEGRO_ALIGN_LEFT);
    draw_lcd_timer(elapsed_ms,sw-8.0f,ALLEGRO_ALIGN_RIGHT);
    int bx=sw/2-15;
    al_draw_filled_rectangle(bx,5,bx+30,35,al_map_rgb(220,220,220));
    al_draw_rectangle(bx+0.5f,5.5f,bx+29.5f,34.5f,al_map_rgb(80,80,80),1);
    const char* face=won?":D":(game_over?":(":":)");
    al_draw_text(font,al_map_rgb(0,0,0),bx+15,17,ALLEGRO_ALIGN_CENTRE,face);
}

// ── game render ───────────────────────────────────────────────────────────────

void Graphic_Manager::render(const Map& map, const Bitmaps& bitmaps,
                              bool game_over, bool won,
                              int mines_remaining, int elapsed_ms, bool flip) {
    al_set_target_backbuffer(display);
    al_clear_to_color(al_map_rgb(0,0,0));
    render_toolbar(game_over,won,mines_remaining,elapsed_ms);
    for(int y=0;y<map.get_height();y++){
        for(int x=0;x<map.get_width();x++){
            ALLEGRO_BITMAP* bmp;
            if(map.is_flagged(x,y)) bmp=bitmaps.get_flag();
            else if(!map.is_revealed(x,y)) bmp=bitmaps.get_covered();
            else{
                int val=map.get_value(x,y);
                if(val==-1) bmp=bitmaps.get_mine();
                else if(val==0) bmp=bitmaps.get_uncovered();
                else bmp=bitmaps.get_number(val-1);
            }
            al_draw_scaled_bitmap(bmp,0,0,bmp_size,bmp_size,
                                  x*cell_size,y*cell_size+TOOLBAR_H,cell_size,cell_size,0);
        }
    }
    if(game_over||won){
        int sw=grid_w*cell_size,sh=grid_h*cell_size;
        al_set_blender(ALLEGRO_ADD,ALLEGRO_ALPHA,ALLEGRO_INVERSE_ALPHA);
        al_draw_filled_rectangle(0,TOOLBAR_H,sw,TOOLBAR_H+sh,al_map_rgba(0,0,0,160));
        al_set_blender(ALLEGRO_ADD,ALLEGRO_ONE,ALLEGRO_INVERSE_ALPHA);
        ALLEGRO_COLOR color=won?al_map_rgb(80,255,80):al_map_rgb(255,60,60);
        draw_scaled_text(won?"YOU WIN!":"GAME OVER",sw/2.0f,TOOLBAR_H+sh/2.0f-8.0f,
                         ALLEGRO_ALIGN_CENTRE,color,2.0f);
    }
    if(flip) al_flip_display();
}

// ── name entry overlay ────────────────────────────────────────────────────────

void Graphic_Manager::render_name_entry(const Map& map, const Bitmaps& bitmaps,
                                         int mines_remaining, int elapsed_ms,
                                         const std::string& input) {
    render(map,bitmaps,false,true,mines_remaining,elapsed_ms,false);
    int sw=grid_w*cell_size,sh=grid_h*cell_size+TOOLBAR_H;
    int dw=360,dh=105,dx=(sw-dw)/2,dy=(sh-dh)/2;
    al_set_blender(ALLEGRO_ADD,ALLEGRO_ALPHA,ALLEGRO_INVERSE_ALPHA);
    al_draw_filled_rectangle(0,0,sw,sh,al_map_rgba(0,0,0,120));
    al_set_blender(ALLEGRO_ADD,ALLEGRO_ONE,ALLEGRO_INVERSE_ALPHA);
    al_draw_filled_rectangle(dx,dy,dx+dw,dy+dh,al_map_rgb(25,28,50));
    al_draw_rectangle(dx+0.5f,dy+0.5f,dx+dw-0.5f,dy+dh-0.5f,al_map_rgb(100,110,200),2);
    int secs=elapsed_ms/1000,ms=elapsed_ms%1000;
    draw_scaled_text("YOU WIN!",dx+dw/2,dy+7,ALLEGRO_ALIGN_CENTRE,al_map_rgb(80,255,80),1.5f);
    al_draw_textf(font,al_map_rgb(180,180,200),dx+dw/2,dy+32,
                  ALLEGRO_ALIGN_CENTRE,"Tiempo: %03d.%03d",secs,ms);
    al_draw_text(font,al_map_rgb(180,180,220),dx+15,dy+52,ALLEGRO_ALIGN_LEFT,"Nombre:");
    al_draw_filled_rectangle(dx+72,dy+48,dx+dw-15,dy+68,al_map_rgb(12,12,30));
    al_draw_rectangle(dx+72.5f,dy+48.5f,dx+dw-15.5f,dy+67.5f,al_map_rgb(90,100,200),1);
    al_draw_text(font,al_map_rgb(220,220,255),dx+76,dy+53,ALLEGRO_ALIGN_LEFT,(input+"_").c_str());
    al_draw_text(font,al_map_rgb(110,110,150),dx+dw/2,dy+80,
                 ALLEGRO_ALIGN_CENTRE,"ENTER para confirmar");
    al_flip_display();
}

// ── leaderboard screen ────────────────────────────────────────────────────────

void Graphic_Manager::render_leaderboard(const Leaderboard& lb, int difficulty) {
    al_set_target_backbuffer(display);
    al_clear_to_color(al_map_rgb(20,20,30));
    al_draw_filled_rectangle(0,0,LB_W,LB_TAB_H,al_map_rgb(30,30,45));
    static const char* tab_names[]={"FACIL","MEDIO","DIFICIL"};
    for(int i=0;i<3;i++){
        int bx=10+i*95; bool active=(i==difficulty);
        ALLEGRO_COLOR bg=active?al_map_rgb(70,80,130):al_map_rgb(45,45,65);
        al_draw_filled_rectangle(bx,5,bx+85,40,bg);
        al_draw_rectangle(bx+0.5f,5.5f,bx+84.5f,39.5f,al_map_rgb(90,100,170),1);
        al_draw_text(font,al_map_rgb(210,210,255),bx+42,17,ALLEGRO_ALIGN_CENTRE,tab_names[i]);
    }
    al_draw_filled_rectangle(LB_W-90,5,LB_W-5,40,al_map_rgb(90,40,40));
    al_draw_rectangle(LB_W-89.5f,5.5f,LB_W-5.5f,39.5f,al_map_rgb(180,70,70),1);
    al_draw_text(font,al_map_rgb(255,170,170),LB_W-47,17,ALLEGRO_ALIGN_CENTRE,"VOLVER");
    al_draw_line(LB_LIST_W+4,LB_TAB_H,LB_LIST_W+4,LB_H,al_map_rgb(55,55,75),1);
    auto sorted=lb.get_sorted(difficulty);
    ALLEGRO_COLOR hdr=al_map_rgb(160,160,210),gold=al_map_rgb(255,215,0),
                  silver=al_map_rgb(200,200,200),bronze=al_map_rgb(200,120,50),
                  normal=al_map_rgb(190,190,190);
    float hy=LB_TAB_H+8;
    al_draw_text(font,hdr,15,hy,ALLEGRO_ALIGN_LEFT,"#");
    al_draw_text(font,hdr,40,hy,ALLEGRO_ALIGN_LEFT,"Nombre");
    al_draw_text(font,hdr,205,hy,ALLEGRO_ALIGN_LEFT,"Tiempo");
    al_draw_line(10,hy+14,LB_LIST_W-5,hy+14,al_map_rgb(55,55,75),1);
    int max_rows=(int)(LB_H-LB_TAB_H-30)/22;
    for(int i=0;i<(int)sorted.size()&&i<max_rows;i++){
        float ry=LB_TAB_H+28+i*22.0f;
        ALLEGRO_COLOR rc=(i==0)?gold:(i==1)?silver:(i==2)?bronze:normal;
        if(i%2==0) al_draw_filled_rectangle(10,ry-1,LB_LIST_W-5,ry+15,al_map_rgba(255,255,255,8));
        al_draw_textf(font,rc,15,ry,ALLEGRO_ALIGN_LEFT,"%d",i+1);
        std::string name=sorted[i].name; if((int)name.size()>19) name=name.substr(0,19);
        al_draw_text(font,rc,40,ry,ALLEGRO_ALIGN_LEFT,name.c_str());
        al_draw_textf(font,rc,205,ry,ALLEGRO_ALIGN_LEFT,"%03d.%03d",
                      sorted[i].elapsed_ms/1000,sorted[i].elapsed_ms%1000);
    }
    if(sorted.empty())
        al_draw_text(font,al_map_rgb(100,100,120),LB_LIST_W/2,LB_H/2,
                     ALLEGRO_ALIGN_CENTRE,"Sin entradas todavia");
    float gx=LB_LIST_W+12.0f;
    draw_graph(lb,difficulty,gx,LB_TAB_H+5.0f,LB_W-gx-8.0f,LB_H-LB_TAB_H-10.0f);
    al_flip_display();
}
