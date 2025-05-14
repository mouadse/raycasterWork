// -----------------------------------------------------------------------------
// DDA Raycaster – Single‑file, self‑contained example using MLX42
// -----------------------------------------------------------------------------
// Build (Linux, GLFW backend):
//   cc -Wall -Wextra -Werror raycaster_fixed.c -o raycaster \
//      -lmlx42 -lglfw -lm -ldl -pthread -lX11 -lXrandr -lXi -lXxf86vm -lXinerama \
//      -lXcursor -lXrender -lXfixes -lXext
//
// Make sure you have MLX42 properly installed. On macOS replace the linker flags
// with “-lmlx42 -framework Cocoa -framework OpenGL -framework IOKit -framework CoreVideo”.
// The four PNG textures referenced below must be placed next to the executable
// (or change TEX_* paths).
// -----------------------------------------------------------------------------

#define _POSIX_C_SOURCE 200809L
#include "MLX42/include/MLX42/MLX42.h"
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// -----------------------------------------------------------------------------
// Window & texture definitions -------------------------------------------------
// -----------------------------------------------------------------------------
#define WIDTH   1280
#define HEIGHT  720

// Texture paths (PNG, 64×64 or power‑of‑two preferably)
#define TEX_NORTH "tex_north.png"
#define TEX_SOUTH "tex_south.png"
#define TEX_WEST  "tex_west.png"
#define TEX_EAST  "tex_east.png"

// -----------------------------------------------------------------------------
// Map -------------------------------------------------------------------------
// -----------------------------------------------------------------------------
static char *g_map[] = {
    "111111111111111111111111111",
    "100000000110000000000000001",
    "101111110111110111111111101",
    "101000010100010000000000101",
    "101010110111110111111110101",
    "101010100000000100010000101",
    "101011111111110111011110101",
    "101000000100010101000010101",
    "101111110101110101111110101",
    "100000010001000100000010001",
    "111110111111111111011111101",
    "100010100000000000010000101",
    "101110101111111111010110101",
    "100000101000000000010010101",
    "111111101011111110011111101",
    "1N0000001010000010000000001",
    "101110011011101110111110111",
    "101000000000001000100010001",
    "101111110111111011101110101",
    "100000010000000010001000101",
    "111111111111111111111111111",
    NULL
};
static const int g_map_rows = 21;
static const int g_map_cols = 27;

// -----------------------------------------------------------------------------
// Minimap parameters -----------------------------------------------------------
// -----------------------------------------------------------------------------
#define TILE_SIZE                 64
#define MINIMAP_SCALE_FACTOR      0.10f
#define MINIMAP_MARGIN            10
#define MINIMAP_VIEW_RADIUS       5

// (rgba – last byte is alpha, will be overwritten dynamically) --------------
#define MINIMAP_ALPHA_BG          100
#define MINIMAP_ALPHA_WALL        200
#define MINIMAP_ALPHA_FLOOR       150
#define MINIMAP_ALPHA_RAY         170
#define MINIMAP_ALPHA_PLAYER      255
#define MINIMAP_ALPHA_BORDER      255

#define MINIMAP_BORDER_COLOR      0xFFFFFFFFU
#define MINIMAP_BACKGROUND_COLOR  0x22222200U
#define MINIMAP_WALL_COLOR        0x4A90E200U
#define MINIMAP_FLOOR_COLOR       0x33333300U
#define MINIMAP_PLAYER_COLOR      0xFF525200U
#define MINIMAP_RAY_COLOR         0xFFD70000U

// -----------------------------------------------------------------------------
// Misc helpers ----------------------------------------------------------------
// -----------------------------------------------------------------------------
static inline void fatal(const char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}

static size_t ft_strlen(const char *s)            { return s ? strlen(s) : 0; }
static char   *ft_strdup(const char *s)           { size_t l = ft_strlen(s); char *d = malloc(l + 1); if (!d) fatal("malloc"); memcpy(d, s, l + 1); return d; }
static char   *ft_strjoin(const char *a,const char *b){ size_t la=ft_strlen(a),lb=ft_strlen(b); char *s=malloc(la+lb+1); if(!s) fatal("malloc"); memcpy(s,a,la); memcpy(s+la,b,lb+1); return s; }
static char   *ft_itoa(int n)                     { char b[32]; snprintf(b,sizeof b, "%d",n); return ft_strdup(b); }

// -----------------------------------------------------------------------------
// Math types ------------------------------------------------------------------
// -----------------------------------------------------------------------------
typedef struct { float x, y; } t_vec2;
static inline t_vec2 v_new(float x,float y)       { return (t_vec2){x,y}; }
static inline t_vec2 v_add(t_vec2 a,t_vec2 b)     { return (t_vec2){a.x+b.x,a.y+b.y}; }
static inline t_vec2 v_mul(t_vec2 v,float k)      { return (t_vec2){v.x*k,v.y*k}; }
static inline t_vec2 v_rot(t_vec2 v,float deg)    { float rad=deg*(float)M_PI/180.f; float cs=cosf(rad),sn=sinf(rad); return (t_vec2){v.x*cs-v.y*sn,v.x*sn+v.y*cs}; }

typedef struct { int x,y; } t_ipoint;
static inline int signf_i(float v)                { return v < 0.f ? -1 : 1; }

// -----------------------------------------------------------------------------
// Raycasting structs -----------------------------------------------------------
// -----------------------------------------------------------------------------
typedef struct
{
    t_vec2     dir;
    t_vec2     camera_pixel;
    float      plane_multiplier;
    t_ipoint   map;
    t_ipoint   step;
    t_vec2     delta_dist;
    t_vec2     dist_side;
    float      perp_dist;
    int        hit_side;   // 0 – x, 1 – y wall
} t_dda;

typedef struct
{
    int    height;
    int    start_y;
    int    end_y;
    int    tex_x;
    float  tex_step;
    float  tex_pos;
} t_wall;

typedef struct
{
    bool w,s,a,d,left,right;
} t_keys;

typedef struct s_game
{
    mlx_t         *mlx;
    mlx_image_t   *img;
    mlx_texture_t *tex[4];
    t_vec2         pos;
    t_vec2         dir;
    t_vec2         plane;
    t_keys         keys;
    float          frame_time;
} t_game;

// -----------------------------------------------------------------------------
// Utility – MLX texture pixel fetch -------------------------------------------
// -----------------------------------------------------------------------------
static inline uint32_t get_tex_pixel(const mlx_texture_t *tex,int y,int x)
{
    if (!tex) return 0xFF00FFFFU; // magenta debug
    if (x<0 || y<0 || x>=(int)tex->width || y>=(int)tex->height) return 0xFFFFFFFFU;
    size_t idx = (y*tex->width + x) * tex->bytes_per_pixel;
    const uint8_t *p = &tex->pixels[idx];
    return (p[0]<<24)|(p[1]<<16)|(p[2]<<8)|p[3]; // RGBA → 0xRRGGBBAA
}

static inline uint32_t set_wall_colour(int side,int stepX,int stepY)
{
    if (side==0) return stepX<0 ? 0xFF4444FFU : 0xFF8888FFU; // x‑wall
    return stepY<0 ? 0xFF44FF44U : 0xFF88FF88U;              // y‑wall
}

// -----------------------------------------------------------------------------
// Input -----------------------------------------------------------------------
// -----------------------------------------------------------------------------
static void key_press(mlx_key_data_t k, void *param)
{
	t_game *g = param;
	bool v = k.action != MLX_RELEASE;

	if (k.key == MLX_KEY_W)     g->keys.w     = v;
	if (k.key == MLX_KEY_S)     g->keys.s     = v;
	if (k.key == MLX_KEY_A)     g->keys.a     = v;
	if (k.key == MLX_KEY_D)     g->keys.d     = v;
	if (k.key == MLX_KEY_LEFT)  g->keys.left  = v;
	if (k.key == MLX_KEY_RIGHT) g->keys.right = v;
	if (k.key == MLX_KEY_ESCAPE && v) mlx_close_window(g->mlx);
}

// -----------------------------------------------------------------------------
// Map helpers -----------------------------------------------------------------
// -----------------------------------------------------------------------------
static inline bool map_is_wall(int col,int row)
{
    return (col<0 || row<0 || row>=g_map_rows || col>=g_map_cols || g_map[row][col]=='1');
}

// -----------------------------------------------------------------------------
// Movement --------------------------------------------------------------------
// -----------------------------------------------------------------------------
static void handle_movement(t_game *g)
{
    const float mv = g->frame_time * 4.0f;   // move speed units/second
    const float rt = g->frame_time * 140.0f; // rotation degrees/second

    t_vec2 new_pos = g->pos;
    if (g->keys.w) { new_pos.x += g->dir.x * mv; new_pos.y += g->dir.y * mv; }
    if (g->keys.s) { new_pos.x -= g->dir.x * mv; new_pos.y -= g->dir.y * mv; }
    if (g->keys.a) { new_pos.x -= g->plane.x * mv; new_pos.y -= g->plane.y * mv; }
    if (g->keys.d) { new_pos.x += g->plane.x * mv; new_pos.y += g->plane.y * mv; }

    if (!map_is_wall((int)new_pos.x,(int)g->pos.y)) g->pos.x = new_pos.x;
    if (!map_is_wall((int)g->pos.x,(int)new_pos.y)) g->pos.y = new_pos.y;

    if (g->keys.left)  { g->dir = v_rot(g->dir, -rt); g->plane = v_rot(g->plane, -rt); }
    if (g->keys.right) { g->dir = v_rot(g->dir,  rt); g->plane = v_rot(g->plane,  rt); }
}

// -----------------------------------------------------------------------------
// Alpha blending helpers -------------------------------------------------------
// -----------------------------------------------------------------------------
static uint32_t blend_colors(uint32_t bg,uint32_t fg)
{
    uint8_t bg_r = bg>>24, bg_g = bg>>16 & 0xFF, bg_b = bg>>8 & 0xFF, bg_a = bg & 0xFF;
    uint8_t fg_r = fg>>24, fg_g = fg>>16 & 0xFF, fg_b = fg>>8 & 0xFF, fg_a = fg & 0xFF;

    float a_fg = fg_a / 255.f;
    float a_bg = bg_a / 255.f;
    float a_out = a_fg + a_bg * (1.f - a_fg);
    if (a_out==0.f) return 0;

    uint8_t r = (uint8_t)((fg_r*a_fg + bg_r*a_bg*(1.f-a_fg))/a_out);
    uint8_t g = (uint8_t)((fg_g*a_fg + bg_g*a_bg*(1.f-a_fg))/a_out);
    uint8_t b = (uint8_t)((fg_b*a_fg + bg_b*a_bg*(1.f-a_fg))/a_out);
    uint8_t a = (uint8_t)(a_out*255.f);
    return (r<<24)|(g<<16)|(b<<8)|a;
}

// -------------------------------- image pixel helpers -------------------------
static inline uint32_t get_pixel_color(const mlx_image_t *img,int x,int y)
{
    if (x>=0 && (uint32_t)x<img->width && y>=0 && (uint32_t)y<img->height)
    {
        size_t idx = (y*img->width + x) * 4U;
        const uint8_t *p = &img->pixels[idx];
        return (p[0]<<24)|(p[1]<<16)|(p[2]<<8)|p[3];
    }
    return 0;
}

static inline void safe_put_pixel_blend(mlx_image_t *img,int x,int y,uint32_t c)
{
    if (x>=0 && (uint32_t)x<img->width && y>=0 && (uint32_t)y<img->height)
    {
        uint32_t bg = get_pixel_color(img,x,y);
        mlx_put_pixel(img,x,y, blend_colors(bg,c));
    }
}

static void drawRectTransparent(t_game *g,int x,int y,int w,int h,uint32_t c)
{
    for (int j=y;j<y+h;++j) for (int i=x;i<x+w;++i) safe_put_pixel_blend(g->img,i,j,c);
}

// -----------------------------------------------------------------------------
// Bresenham line ---------------------------------------------------------------
// -----------------------------------------------------------------------------
static void bres_alg(t_ipoint p1,t_ipoint p2,t_ipoint *d,t_ipoint *s)
{
    d->x = abs(p2.x - p1.x);
    d->y = -abs(p2.y - p1.y);
    s->x = p1.x<p2.x ? 1 : -1;
    s->y = p1.y<p2.y ? 1 : -1;
}

static void draw_line(t_game *g,t_ipoint p1,t_ipoint p2,uint32_t c)
{
    t_ipoint d,s; bres_alg(p1,p2,&d,&s);
    int err=d.x + d.y;
    while (1)
    {
        safe_put_pixel_blend(g->img,p1.x,p1.y,c);
        if (p1.x==p2.x && p1.y==p2.y) break;
        int e2 = 2*err;
        if (e2>=d.y){ err+=d.y; p1.x+=s.x; }
        if (e2<=d.x){ err+=d.x; p1.y+=s.y; }
    }
}

static void drawRectBorder(t_game *g,int x,int y,int w,int h,uint32_t col,int th)
{
    col = (col & 0xFFFFFF00U)|MINIMAP_ALPHA_BORDER;
    drawRectTransparent(g,x,y,w,th,col);
    drawRectTransparent(g,x,y+h-th,w,th,col);
    drawRectTransparent(g,x,y+th,th,h-2*th,col);
    drawRectTransparent(g,x+w-th,y+th,th,h-2*th,col);
}

// -----------------------------------------------------------------------------
// Minimap ---------------------------------------------------------------------
// -----------------------------------------------------------------------------
static inline int getMapAt(int r,int c)
{
    if (r>=0 && r<g_map_rows && c>=0 && c<g_map_cols) return g_map[r][c];
    return '1';
}

static void renderMapGrid(t_game *g)
{
    int ts = (int)(TILE_SIZE * MINIMAP_SCALE_FACTOR);
    int view_d = MINIMAP_VIEW_RADIUS*2 + 1;
    int vis_w = view_d * ts;
    int vis_h = view_d * ts;
    int mx = MINIMAP_MARGIN, my = MINIMAP_MARGIN;

    int p_col = (int)g->pos.x;
    int p_row = (int)g->pos.y;
    int sc = fmax(0, p_col - MINIMAP_VIEW_RADIUS);
    int ec = fmin(g_map_cols, p_col + MINIMAP_VIEW_RADIUS + 1);
    int sr = fmax(0, p_row - MINIMAP_VIEW_RADIUS);
    int er = fmin(g_map_rows, p_row + MINIMAP_VIEW_RADIUS + 1);

    uint32_t bg = (MINIMAP_BACKGROUND_COLOR & 0xFFFFFF00U)|MINIMAP_ALPHA_BG;
    drawRectTransparent(g,mx,my,vis_w,vis_h,bg);

    for (int r=sr;r<er;++r)
        for (int c=sc;c<ec;++c)
        {
            int tx = mx + (c-sc)*ts;
            int ty = my + (r-sr)*ts;
            uint32_t base = (getMapAt(r,c)=='1') ? MINIMAP_WALL_COLOR : MINIMAP_FLOOR_COLOR;
            uint32_t col = (base & 0xFFFFFF00U) | ((getMapAt(r,c)=='1')?MINIMAP_ALPHA_WALL:MINIMAP_ALPHA_FLOOR);
            int size = ts>1?ts-1:1;
            drawRectTransparent(g,tx+1,ty+1,size,size,col);
        }

    drawRectBorder(g,mx,my,vis_w,vis_h,MINIMAP_BORDER_COLOR,1);
}

static void renderMapRays(t_game *g)
{
    int ts = (int)(TILE_SIZE * MINIMAP_SCALE_FACTOR);
    int view_d = MINIMAP_VIEW_RADIUS*2 + 1;
    int vis_w = view_d * ts;
    int vis_h = view_d * ts;
    int mx = MINIMAP_MARGIN, my = MINIMAP_MARGIN;

    int center_x = mx + vis_w/2;
    int center_y = my + vis_h/2;
    t_ipoint player_pt = {center_x,center_y};

    int p_col = (int)g->pos.x;
    int p_row = (int)g->pos.y;
    int sc = fmax(0, p_col - MINIMAP_VIEW_RADIUS);
    int sr = fmax(0, p_row - MINIMAP_VIEW_RADIUS);

    const int ray_step = 32; // screen columns per minimap ray
    for (int sx=0;sx<WIDTH;sx+=ray_step)
    {
        float plane_mul = 2.f*sx/(float)WIDTH - 1.f;
        t_vec2 cam_px = v_mul(g->plane,plane_mul);
        t_vec2 dir = v_add(g->dir,cam_px);

        int mapx = (int)g->pos.x;
        int mapy = (int)g->pos.y;
        float delta_x = dir.x==0.f ? 1e30f : fabsf(1.f/dir.x);
        float delta_y = dir.y==0.f ? 1e30f : fabsf(1.f/dir.y);
        int step_x = dir.x<0.f ? -1 : 1;
        int step_y = dir.y<0.f ? -1 : 1;
        float side_x = (dir.x<0.f) ? (g->pos.x-mapx)*delta_x : (mapx+1.f-g->pos.x)*delta_x;
        float side_y = (dir.y<0.f) ? (g->pos.y-mapy)*delta_y : (mapy+1.f-g->pos.y)*delta_y;

        int hit_side=0, steps=0, max_steps=50;
        while (steps<max_steps && !map_is_wall(mapx,mapy))
        {
            if (side_x < side_y) { side_x += delta_x; mapx += step_x; hit_side = 0; }
            else                 { side_y += delta_y; mapy += step_y; hit_side = 1; }
            ++steps;
        }
        float perp = (hit_side==0) ? (mapx-g->pos.x + (1-step_x)/2.f)/dir.x
                                   : (mapy-g->pos.y + (1-step_y)/2.f)/dir.y;
        float hit_x = g->pos.x + dir.x*perp;
        float hit_y = g->pos.y + dir.y*perp;

        int hx = center_x + (int)((hit_x - g->pos.x)*ts);
        int hy = center_y + (int)((hit_y - g->pos.y)*ts);
        hx = fmax(mx, fmin(mx+vis_w-1, hx));
        hy = fmax(my, fmin(my+vis_h-1, hy));
        t_ipoint hit_pt = {hx,hy};

        float max_d = MINIMAP_VIEW_RADIUS*1.5f;
        float clamped = fmaxf(0.1f, fminf(perp, max_d));
        float fade = 1.f - (clamped/max_d);
        uint32_t a = (uint32_t)(MINIMAP_ALPHA_RAY * fmaxf(0.1f,fade));
        if (a>255) a=255;
        uint32_t col = (MINIMAP_RAY_COLOR & 0xFFFFFF00U)|a;

        draw_line(g,player_pt,hit_pt,col);
    }
}

static void renderMapPlayer(t_game *g)
{
    int ts = (int)(TILE_SIZE * MINIMAP_SCALE_FACTOR);
    int view_d = MINIMAP_VIEW_RADIUS*2 + 1;
    int vis_w = view_d * ts;
    int vis_h = view_d * ts;
    int mx = MINIMAP_MARGIN, my = MINIMAP_MARGIN;

    int px = mx + vis_w/2;
    int py = my + vis_h/2;
    int ps = ts/3<2?2:ts/3;

    uint32_t player_col = (MINIMAP_PLAYER_COLOR & 0xFFFFFF00U)|MINIMAP_ALPHA_PLAYER;
    for (int dy=-ps;dy<=ps;++dy)
        for (int dx=-ps;dx<=ps;++dx)
            if (dx*dx+dy*dy<=ps*ps) safe_put_pixel_blend(g->img,px+dx,py+dy,player_col);

    t_ipoint p0 = {px,py};
    t_ipoint p1 = {px + (int)(g->dir.x*ps*2), py + (int)(g->dir.y*ps*2)};
    p1.x = fmax(mx, fmin(mx+vis_w-1, p1.x));
    p1.y = fmax(my, fmin(my+vis_h-1, p1.y));
    uint32_t col = (MINIMAP_BORDER_COLOR & 0xFFFFFF00U)|MINIMAP_ALPHA_BORDER;
    draw_line(g,p0,p1,col);
}

// -----------------------------------------------------------------------------
// Wall column render -----------------------------------------------------------
// -----------------------------------------------------------------------------
static void draw_column(t_game *g,int sx,const t_dda *r)
{
    t_wall w;
    w.height = (int)(HEIGHT / r->perp_dist);
    w.start_y = -w.height/2 + HEIGHT/2; if (w.start_y<0) w.start_y=0;
    w.end_y   =  w.height/2 + HEIGHT/2; if (w.end_y>=HEIGHT) w.end_y=HEIGHT-1;

    int tidx;
    if (r->hit_side==0) tidx = r->step.x<0 ? 2 : 3; // west / east
    else                tidx = r->step.y<0 ? 0 : 1; // north / south
    mlx_texture_t *tex = g->tex[tidx];

    if (tex)
    {
        float wall_x = (r->hit_side==0) ? (g->pos.y + r->perp_dist*r->dir.y)
                                        : (g->pos.x + r->perp_dist*r->dir.x);
        wall_x -= floorf(wall_x);
        w.tex_x = (int)(wall_x * tex->width);
        if ((r->hit_side==0 && r->dir.x>0) || (r->hit_side==1 && r->dir.y<0))
            w.tex_x = tex->width - w.tex_x - 1;

        w.tex_step = (float)tex->height / w.height;
        w.tex_pos  = (w.start_y - HEIGHT/2.f + w.height/2.f) * w.tex_step;

        for (int y=w.start_y; y<=w.end_y; ++y)
        {
            int tex_y = ((int)w.tex_pos) & (tex->height-1);
            w.tex_pos += w.tex_step;
            uint32_t c = get_tex_pixel(tex, tex_y, w.tex_x);
            mlx_put_pixel(g->img,sx,y,c);
        }
    }
    else
    {
        uint32_t flat = set_wall_colour(r->hit_side,r->step.x,r->step.y);
        for (int y=w.start_y;y<=w.end_y;++y) mlx_put_pixel(g->img,sx,y,flat);
    }
}

// -----------------------------------------------------------------------------
// Main render loop -------------------------------------------------------------
// -----------------------------------------------------------------------------
static void render(t_game *g)
{
    const uint32_t ceil_c  = 0xFF777777U;
    const uint32_t floor_c = 0xFF333333U;
    for (int y=0;y<HEIGHT/2;++y)  for (int x=0;x<WIDTH;++x) mlx_put_pixel(g->img,x,y,ceil_c);
    for (int y=HEIGHT/2;y<HEIGHT;++y)for (int x=0;x<WIDTH;++x) mlx_put_pixel(g->img,x,y,floor_c);

    for (int sx=0;sx<WIDTH;++sx)
    {
        t_dda r;
        r.plane_multiplier = 2.f*sx/(float)WIDTH - 1.f;
        r.camera_pixel = v_mul(g->plane, r.plane_multiplier);
        r.dir = v_add(g->dir, r.camera_pixel);

        r.map.x = (int)g->pos.x;
        r.map.y = (int)g->pos.y;
        r.delta_dist.x = r.dir.x==0.f ? 1e30f : fabsf(1.f/r.dir.x);
        r.delta_dist.y = r.dir.y==0.f ? 1e30f : fabsf(1.f/r.dir.y);
        r.step.x = signf_i(r.dir.x);
        r.step.y = signf_i(r.dir.y);
        r.dist_side.x = (r.dir.x<0.f) ? (g->pos.x-r.map.x) * r.delta_dist.x
                                      : (r.map.x+1.f-g->pos.x) * r.delta_dist.x;
        r.dist_side.y = (r.dir.y<0.f) ? (g->pos.y-r.map.y) * r.delta_dist.y
                                      : (r.map.y+1.f-g->pos.y) * r.delta_dist.y;

        while (!map_is_wall(r.map.x,r.map.y))
        {
            if (r.dist_side.x < r.dist_side.y)
            { r.dist_side.x += r.delta_dist.x; r.map.x += r.step.x; r.hit_side = 0; }
            else
            { r.dist_side.y += r.delta_dist.y; r.map.y += r.step.y; r.hit_side = 1; }
        }
        r.perp_dist = (r.hit_side==0) ? (r.dist_side.x - r.delta_dist.x)
                                      : (r.dist_side.y - r.delta_dist.y);
        if (r.perp_dist<=0.f) r.perp_dist = 0.01f;
        draw_column(g,sx,&r);
    }

    // Minim
    renderMapGrid(g);
    renderMapRays(g);
    renderMapPlayer(g);
}

static void loop_hook(void *param)
{
    t_game *g = param;
    g->frame_time = g->mlx->delta_time;
    handle_movement(g);
    render(g);
}

// -----------------------------------------------------------------------------
// Assets ----------------------------------------------------------------------
// -----------------------------------------------------------------------------
static void load_textures(t_game *g)
{
    const char *paths[4] = {TEX_NORTH,TEX_SOUTH,TEX_WEST,TEX_EAST};
    for (int i=0;i<4;++i) g->tex[i] = (access(paths[i],R_OK)==0) ? mlx_load_png(paths[i]) : NULL;
}

static void find_player_start(t_game *g)
{
    for (int r=0;g_map[r];++r)
        for (int c=0;g_map[r][c];++c)
        {
            char ch = g_map[r][c];
            if (ch=='N'||ch=='S'||ch=='E'||ch=='W')
            {
                g->pos = v_new(c+0.5f, r+0.5f);
                if (ch=='N'){ g->dir=v_new(0,-1); g->plane=v_new(0.66f,0); }
                if (ch=='S'){ g->dir=v_new(0, 1); g->plane=v_new(-0.66f,0); }
                if (ch=='W'){ g->dir=v_new(-1,0); g->plane=v_new(0,-0.66f); }
                if (ch=='E'){ g->dir=v_new(1, 0); g->plane=v_new(0,0.66f); }
                return;
            }
        }
    fatal("map: no player start (NESW) found");
}

// -----------------------------------------------------------------------------
// Entry -----------------------------------------------------------------------
// -----------------------------------------------------------------------------
int main(void)
{
    t_game g = {0};

    if (!(g.mlx = mlx_init(WIDTH,HEIGHT,"DDA Raycaster",false))) fatal("mlx_init");
    if (!(g.img = mlx_new_image(g.mlx,WIDTH,HEIGHT)))               fatal("img alloc");
    mlx_image_to_window(g.mlx,g.img,0,0);

    load_textures(&g);
    find_player_start(&g);

    mlx_key_hook(g.mlx,key_press,&g);
    mlx_loop_hook(g.mlx,loop_hook,&g);
    mlx_loop(g.mlx);

    for (int i=0;i<4;++i) if (g.tex[i]) mlx_delete_texture(g.tex[i]);
    mlx_delete_image(g.mlx,g.img);
    mlx_terminate(g.mlx);
    return 0;
}
