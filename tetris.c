#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <termios.h>
#include <unistd.h>
#include <sys/time.h>
#include <signal.h>
#include "tetris.h"

static int shapes[SHAPE_NUM][DIRECTION_NUM][BLK_ROWS][BLK_COLS] = {
    // I 型
    {
        {
            1, 0, 0, 0,
            1, 0, 0, 0,
            1, 0, 0, 0,
            1, 0, 0, 0,
        },
        {
            1, 1, 1, 1,
            0, 0, 0, 0,
            0, 0, 0, 0,
            0, 0, 0, 0,
        },
        {
            1, 0, 0, 0,
            1, 0, 0, 0,
            1, 0, 0, 0,
            1, 0, 0, 0,
        },
        {
            1, 1, 1, 1,
            0, 0, 0, 0,
            0, 0, 0, 0,
            0, 0, 0, 0,
        },
    },
    // O 型
    {
        {
            1, 1, 0, 0,
            1, 1, 0, 0,
            0, 0, 0, 0,
            0, 0, 0, 0,
        },
        {
            1, 1, 0, 0,
            1, 1, 0, 0,
            0, 0, 0, 0,
            0, 0, 0, 0,
        },
        {
            1, 1, 0, 0,
            1, 1, 0, 0,
            0, 0, 0, 0,
            0, 0, 0, 0,
        },
        {
            1, 1, 0, 0,
            1, 1, 0, 0,
            0, 0, 0, 0,
            0, 0, 0, 0,
        },
    },
    // T 型
    {
        {
            1, 1, 1, 0,
            0, 1, 0, 0,
            0, 0, 0, 0,
            0, 0, 0, 0,
        },
        {
            0, 1, 0, 0,
            1, 1, 0, 0,
            0, 1, 0, 0,
            0, 0, 0, 0,
        },
        {
            0, 1, 0, 0,
            1, 1, 1, 0,
            0, 0, 0, 0,
            0, 0, 0, 0,
        },
        {
            1, 0, 0, 0,
            1, 1, 0, 0,
            1, 0, 0, 0,
            0, 0, 0, 0,
        },
    },
    // S 型
    {
        {
            0, 1, 1, 0,
            1, 1, 0, 0,
            0, 0, 0, 0,
            0, 0, 0, 0,
        },
        {
            1, 0, 0, 0,
            1, 1, 0, 0,
            0, 1, 0, 0,
            0, 0, 0, 0,
        },
        {
            0, 1, 1, 0,
            1, 1, 0, 0,
            0, 0, 0, 0,
            0, 0, 0, 0,
        },
        {
            1, 0, 0, 0,
            1, 1, 0, 0,
            0, 1, 0, 0,
            0, 0, 0, 0,
        },
    },
    // Z 型
    {
        {
            1, 1, 0, 0,
            0, 1, 1, 0,
            0, 0, 0, 0,
            0, 0, 0, 0,
        },
        {
            0, 1, 0, 0,
            1, 1, 0, 0,
            1, 0, 0, 0,
            0, 0, 0, 0,
        },
        {
            1, 1, 0, 0,
            0, 1, 1, 0,
            0, 0, 0, 0,
            0, 0, 0, 0,
        },
        {
            0, 1, 0, 0,
            1, 1, 0, 0,
            1, 0, 0, 0,
            0, 0, 0, 0,
        },
    },
    // L 型
    {
        {
            1, 0, 0, 0,
            1, 0, 0, 0,
            1, 1, 0, 0,
            0, 0, 0, 0,
        },
        {
            1, 1, 1, 0,
            1, 0, 0, 0,
            0, 0, 0, 0,
            0, 0, 0, 0,
        },
        {
            1, 1, 0, 0,
            0, 1, 0, 0,
            0, 1, 0, 0,
            0, 0, 0, 0,
        },
        {
            0, 0, 1, 0,
            1, 1, 1, 0,
            0, 0, 0, 0,
            0, 0, 0, 0,
        },
    },
    // J 型
    {
        {
            0, 1, 0, 0,
            0, 1, 0, 0,
            1, 1, 0, 0,
            0, 0, 0, 0,
        },
        {
            1, 0, 0, 0,
            1, 1, 1, 0,
            0, 0, 0, 0,
            0, 0, 0, 0,
        },
        {
            1, 1, 0, 0,
            1, 0, 0, 0,
            1, 0, 0, 0,
            0, 0, 0, 0,
        },
        {
            1, 1, 1, 0,
            0, 0, 1, 0,
            0, 0, 0, 0,
            0, 0, 0, 0,
        },
    },
};

static int map[MAP_ROWS][MAP_COLS];

struct block cur_blk, next_blk;

static int score, level;

static void init_game(void)
{
    // 初始化终端
    init_terminal();
    // 初始化计时器
    init_timer();
    // 初始化方块
    init_blk();
    HIDE_CURSOR();
}

static void init_terminal(void)
{
    // 定义一个termios结构体变量
    struct termios tios;
    // 获取标准输入的当前属性
    tcgetattr(STDIN_FILENO, &tios);
    // 清除ICANON和ECHO标志，使输入立即生效，不回显
    tios.c_lflag &= ~(ICANON | ECHO);
    // 将修改后的属性立即生效
    tcsetattr(STDIN_FILENO, TCSANOW, &tios);
}

static void init_timer(void)
{
    // 注册SIGALRM信号处理函数
    signal(SIGALRM, move_down);

    struct itimerval timer;

    // 设置初始延迟时间为0秒，立即触发SIGALRM信号
    timer.it_value.tv_sec = 0;
    timer.it_value.tv_usec = 400000;

    // 往后每隔0.5秒重复触发一次SIGALRM信号
    timer.it_interval.tv_sec = 0; 
    timer.it_interval.tv_usec = 400000;

    // 启动定时器
    setitimer(ITIMER_REAL, &timer, NULL);
}

static void init_blk(void)
{
    srand(time(NULL));
    cur_blk.x = 0;
    cur_blk.y = MAP_COLS/2 - 1;
    cur_blk.shape = rand() % SHAPE_NUM;
    cur_blk.direction = rand() % DIRECTION_NUM;
    cur_blk.color = 1 + (rand() % (WHITE - BLACK));
    
    next_blk.x = 0;
    next_blk.y = MAP_COLS/2 - 1;
    next_blk.shape = rand() % SHAPE_NUM;
    next_blk.direction = rand() % DIRECTION_NUM;
    next_blk.color = 1 + (rand() % (WHITE - BLACK));
}

static void render(void)
{
    CLEAR();

    // 绘制上边框
    GOTOXY(BORDER_X, BORDER_Y);
    SET_COLOR(BORDER_COLOR);
    printf("--");
    for (int i = 0; i < MAP_COLS; i++)
    {
        printf("==");
    }
    printf("--");

    // 绘制两侧边框
    for (int i = 0; i < MAP_ROWS; i++)
    {
        GOTOXY(BORDER_X + 1 + i, BORDER_Y);
        printf("||");
        GOTOXY(BORDER_X + 1 + i, BORDER_Y + (MAP_COLS + 1)*2);
        printf("||");
    }

    // 绘制下边框
    GOTOXY(BORDER_X + 1 + MAP_ROWS, BORDER_Y);
    printf("--");
    for (int i = 0; i < MAP_COLS; i++)
    {
        printf("==");
    }
    printf("--");
    RESET_COLOR();

    // 绘制下一个方块
    SET_COLOR(BLACK + next_blk.color);
    for (int i = 0; i < BLK_ROWS; i++)
    {
        for (int j = 0; j < BLK_COLS; j++)
        {
            if(shapes[next_blk.shape][next_blk.direction][i][j])
            {
                GOTOXY(NEXT_X + i, NEXT_Y + j*2);
                printf("[]");
            }
        }
        
    }
    RESET_COLOR();

    // 绘制分数
    GOTOXY(SCORE_X, SCORE_Y);
    printf("SCORE");
    GOTOXY(SCORE_X + 2, SCORE_Y);
    printf("  %d", score);

    // 绘制等级
    GOTOXY(LEVEL_X, LEVEL_Y);
    printf("LEVEL");
    GOTOXY(LEVEL_X + 2, LEVEL_Y);
    printf("  %d", level);

    // 绘制当前方块
    SET_COLOR(BLACK + cur_blk.color);
    for (int i = 0; i < BLK_ROWS; i++)
    {
        for (int j = 0; j < BLK_COLS; j++)
        {
            if(shapes[cur_blk.shape][cur_blk.direction][i][j])
            {
                GOTOXY(BORDER_X + cur_blk.x + 1 + i, BORDER_Y + (cur_blk.y + 1 + j)*2);
                printf("[]");
            }
        }
        
    }
    RESET_COLOR();

    // 绘制 map
    for (int i = 0; i < MAP_ROWS; i++)
    {
        for (int j = 0; j < MAP_COLS; j++)
        {
            SET_COLOR(BLACK + map[i][j]);
            if (map[i][j])
            {
                GOTOXY(BORDER_X + i + 1, BORDER_Y + 2 + j*2);
                printf("[]");
            }
        }
    }

    RESET_COLOR();
    
    GOTOXY(BORDER_X + MAP_ROWS + 2, 1);
    printf("\nWelcome! Press W to rotate, A to move left, D to move right, S to speed up, Q to quit the game.\n");
}

static void move_left(void)
{
    cur_blk.y -= 1;
    if(!can_move())
    {
        cur_blk.y += 1;
    }
    render();
}

static void move_right(void)
{
    cur_blk.y += 1;
    if(!can_move())
    {
        cur_blk.y -= 1;
    }
    render();
}

static void move_down(int signum)
{   
    render();
    if(cur_blk.x == 0 && is_colliding())
    {
        end_game();
        printf("Game Over!\n");
        exit(0);
    }
    cur_blk.x += 1;
    if(is_colliding())
    {
        cur_blk.x -= 1;
        for (int i = 0; i < BLK_ROWS; i++)
        {
            for (int j = 0; j < BLK_COLS; j++)
            {
                if(shapes[cur_blk.shape][cur_blk.direction][i][j])
                {
                    map[cur_blk.x + i][cur_blk.y + j] = cur_blk.color;
                }
            }
        }
        cur_blk = next_blk;
        next_blk.x = 0;
        next_blk.y = MAP_COLS/2 - 1;
        next_blk.shape = rand() % SHAPE_NUM;
        next_blk.direction = rand() % DIRECTION_NUM;
        next_blk.color = 1 + (rand() % (WHITE - BLACK));

        clear_lines();
    }
}

static void rotate(void)
{
    cur_blk.direction = (cur_blk.direction + 1) % DIRECTION_NUM;
    if(!can_move())
    {
        cur_blk.direction = (cur_blk.direction - 1) % DIRECTION_NUM;
    }
    render();
}

static int can_move(void)
{
    for (int i = 0; i < BLK_ROWS; i++)
    {
        for (int j = 0; j < BLK_COLS; j++)
        {
            if(shapes[cur_blk.shape][cur_blk.direction][i][j])
            {
                if(cur_blk.x + i < 0 || cur_blk.x + i >= MAP_ROWS - 1 || cur_blk.y + j < 0 || cur_blk.y + j > MAP_COLS - 1 || map[cur_blk.x + i][cur_blk.y + j])
                {
                    return 0;
                }
            }
        }
        
    }
    return 1;
}

static int is_colliding(void)
{
    for (int i = 0; i < BLK_ROWS; i++)
    {
        for (int j = 0; j < BLK_COLS; j++)
        {
            if(shapes[cur_blk.shape][cur_blk.direction][i][j])
            {
                if(cur_blk.x + i > MAP_ROWS - 1 || map[cur_blk.x + i][cur_blk.y + j])
                {
                    return 1;
                }
            }
        }
    }
    return 0;
}

static void clear_lines(void)
{
    for (int i = MAP_ROWS - 1; i >= 0; i--)
    {
        int is_full = 1;  // 假设这一行是满的

        // 检查当前行是否满了
        for (int j = 0; j < MAP_COLS; j++)
        {
            if (!map[i][j])
            {
                is_full = 0;
                break;
            }
        }

        // 如果这一行满了
        if (is_full)
        {
            // 下移所有行，把上面的行向下压一行
            for (int k = i; k > 0; k--)
            {
                for (int j = 0; j < MAP_COLS; j++)
                {
                    map[k][j] = map[k - 1][j];
                }
            }

            // 顶部新生成一行空白
            for (int j = 0; j < MAP_COLS; j++)
            {
                map[0][j] = 0;
            }

            i++; // 检查完一行后，重新检查当前位置，防止连续消行漏掉
            score++;
        }
    }
}

static void end_game()
{
    // 定义一个termios结构体变量
    struct termios tios;
    // 获取标准输入的当前属性
    tcgetattr(STDIN_FILENO, &tios);
    // 恢复ICANON和ECHO标志，使输入按行读取，回显
    tios.c_lflag |= (ICANON | ECHO);
    // 将修改后的属性立即生效
    tcsetattr(STDIN_FILENO, TCSANOW, &tios);
    SHOW_CURSOR();
}

void start_tetris(void)
{
    init_game();
    // game loop
    int key;
    while (1)
    {
        key = getchar();
        if (key == 'w')
        {
            rotate();
        }
        else if (key == 'a')
        {
            move_left();
        }
        else if (key == 's')
        {
            move_down(1);
        }
        else if (key == 'd')
        {
            move_right();
        }
        else if (key == 'q')
        {
            break;
        }
    }
    end_game();
}