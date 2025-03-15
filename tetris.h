#ifndef TETRIS_H_
#define TETRIS_H_

#define BLACK 40
#define RED 41
#define GREEN 42
#define YELLOW 43
#define BLUE 44
#define PURPLE 45
#define CYAN 46
#define WHITE 47

#define SHAPE_NUM 7 // 方块种类数
#define DIRECTION_NUM 4 // 方块方向数

#define BORDER_X 5		// 边框起始X位置
#define BORDER_Y 10		// 边框起始Y位置
#define BORDER_COLOR PURPLE   // 边框颜色

#define MAP_ROWS 20     // 游戏区域行数
#define MAP_COLS 10      // 游戏区域列数
#define BLK_ROWS 4   // 方块行数
#define BLK_COLS 4   // 方块列数

#define NEXT_X (BORDER_X + 1)	// 下一个方块区域起始X位置
#define NEXT_Y (BORDER_Y + (MAP_COLS + 2 + 3)*2)	// 下一个方块区域起始Y位置，距离右边框3个方块大小单位

#define SCORE_X (BORDER_X + MAP_ROWS/2 - 2)	// SCORE区域起始X位置
#define SCORE_Y (BORDER_Y + (MAP_COLS + 2 + 3)*2)	// SCORE区域起始Y位置，距离右边框3个方块大小单位

#define LEVEL_X (BORDER_X + MAP_ROWS*3/4)	// LEVEL区域起始X位置
#define LEVEL_Y (BORDER_Y + (MAP_COLS + 2 + 3)*2)	// LEVEL区域起始Y位置，距离右边框3个方块大小单位

#define GOTOXY(x, y) printf("\033[%d;%dH", (x), (y))    // 光标移动到(x, y)位置
#define CLEAR() printf("\033[2J")   // 清屏
#define SET_COLOR(color) printf("\033[%dm", (color))    // 设置颜色
#define RESET_COLOR() printf("\033[0m") // 重置颜色
#define HIDE_CURSOR() printf("\033[?25l")   // 隐藏光标
#define SHOW_CURSOR() printf("\033[?25h")   // 显示光标

struct block
{
    int x, y;
    int shape;
    int direction;
    int color;
};

// 初始化游戏
static void init_game(void);
// 结束游戏
static void end_game(void);

// 初始化终端
static void init_terminal(void);
// 初始化定时器
static void init_timer(void);
// 初始化方块
static void init_blk(void);

// 判断是否碰撞
static int is_colliding(void);
// 判断是否可以移动
static int can_move(void);

// 渲染
static void render(void);

// 清除行
static void clear_lines(void);

// 向左移动
static void move_left(void);
// 向右移动
static void move_right(void);
// 向下移动
static void move_down(int signum);
// 旋转
static void rotate(void);

// 开始俄罗斯方块游戏
void start_tetris(void);

#endif