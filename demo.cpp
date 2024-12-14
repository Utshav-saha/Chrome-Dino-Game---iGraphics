#include "iGraphics.h"
#include <time.h>
#include <iostream>
#include <vector>
#define MAX_FIREBALLS 3



int bg_x = 0;
int bg_speed = 40;
int bg_w = 80;

// Dino variables
int dino_x = 10, dino_y = 50;
int dino_ground = 50;
int run_idx = 0;
int jump_idx = 0;
char dino_run_image[8][100];
char dino_jump_image[12][100];

// Jump
bool jumping = false;
float velocity = 0;
float gravity = -3.3;
float initial_vel = 25;

// Cactus
int cactus_x = 1000;

int cactus_y = 55;
int cactus_speed = 40;
char cactus_image[5][100];
int cact_idx = 0;

// Dragon
int dragon_x = 1000;
int dragon_y = 220;
int dragon_speed = 50;
int dragon_idx = 0;
char dragon_image[4][100];

// Fireball
int fireball_x = -100;
int fireball_y = 0;
int fireball_speed = 20;
bool fireball_active = false;
char fireball_image[100] = "Game Assets\\fireball.bmp";
int fireballsThrown = 0; 

clock_t cactus_off = 0;
clock_t dragon_off = 0;
clock_t last_collision = 0;
const int cooldown = 400;

int score = 0;
char life_icon[100] = "life.bmp";
char fireball_icon[100] = "fireball_icon.bmp";

int dino_lives = 5;
int fire_rem = 3;
bool cactus_active = true;
bool dragon_active = true;
bool returned = 0;

int scores[100] = {0};
int count = 0;

struct Fireball
{
    int x, y;
    bool active;
} fireballs[MAX_FIREBALLS];

//menu
bool menu = true;
bool play = false;
bool instruction_state = false;
bool g_over = false;
bool score_state = false;
bool game_start = false;
bool sound = false;

//get scores 
void scan_scores()
{
    FILE *file = fopen("leaderboard.txt", "r");

    int score;
    count = 0;
    while((fscanf(file,"%d", &scores[count]) == 1) && count < 100)
    {
        count++;
    }
    fclose(file);

}

//save scores
void save_scores()
{
    FILE *file = fopen("leaderboard.txt","a");

    for(int i=0; i<count; i++)
    {
        fprintf(file, "%d\n", scores[i]);
    }

    fclose(file);
}

void sort()
{
    for (int i = 0; i < count-1; i++)
    {
        for (int j = 0; j < count-1-i; j++)
        {
            if (scores[j] < scores[j+1])
            {
                int temp = scores[j];
                scores[j] = scores[j+1];
                scores[j+1] = temp;
            }
        }
    }
}

//update leaderboard
void update_leaderboard(int num)
{
    if(count < 5)
    {
        scores[count] = num;
        count++;
    }

    else if(num > scores[4])
    {
        scores[4] = num;
    }
    sort();
    save_scores();
}


//show menu
void show_menu()
{
    iShowBMP(0,0,"menu.bmp");
}

//show instructions
void show_instructions()
{
    iShowBMP(0,0,"instructions.bmp");
}

//show score
void show_score()
{   
    iShowBMP(0,0,"leaderboard.bmp");
    iSetColor(255, 255, 255);

    for(int i=0; i<count; i++)
    {
        char score[50];
        sprintf(score, "%d. %d", i+1, scores[i]);
        iText(100, 300 -(i*50), score, GLUT_BITMAP_HELVETICA_18);
    }
}

//Game_over
void game_over() {
    
    iShowBMP(0,0,"game_over.bmp");
    char scoreText[50];
    sprintf(scoreText, "%d", score);
    iText(140, 355, scoreText, GLUT_BITMAP_HELVETICA_18);

    PlaySound(NULL,0,0);
    sound = false;

    if(!returned)
    {
    update_leaderboard(score);
    returned = 1;
    }
}


void increaseSpeed() {
    if(play)
    {
    bg_speed += 3;        
    cactus_speed += 3;   
    dragon_speed += 10; 
    }
}

// Day Background
char *bgdfile[14] = {"bgd_04.bmp", "bgd_05.bmp", "bgd_06.bmp", "bgd_07.bmp", "bgd_08.bmp",
                     "bgd_09.bmp", "bgd_10.bmp", "bgd_04.bmp", "bgd_05.bmp", "bgd_06.bmp",
                     "bgd_07.bmp", "bgd_08.bmp", "bgd_09.bmp", "bgd_10.bmp"};

void show_bg()
{
    for (int i = 0; i < 14; i++)
    {

        iShowBMP(bg_x + i * bg_w, 0, bgdfile[i]);

        iShowBMP(bg_x + (14 + i) * bg_w, 0, bgdfile[i]);
    }
}

void jump()
{
    if (jumping)
    {
        iShowBMP2(dino_x, dino_y, dino_jump_image[jump_idx], 0xFFFFFF);
    }
    else
    {
        iShowBMP2(dino_x, dino_y, dino_run_image[run_idx], 0xFFFFFF);
    }
}
// Load Dino run images
void dino_run_images()
{
    for (int i = 0; i < 8; i++)
    {
        sprintf(dino_run_image[i], "Game Assets\\Sprites\\dino\\resize\\Run\\run00%d.bmp", i);
    }
}

// Load Dino jump images
void dino_jump_images()
{
    for (int i = 0; i < 12; i++)
    {
        sprintf(dino_jump_image[i], "Game Assets\\Sprites\\dino\\resize\\Jump\\jump00%d.bmp", i);
    }
}

// Cactus Images
void cactus_images()
{
    for (int i = 0; i < 5; i++)
    {
        sprintf(cactus_image[i], "Game Assets\\Cactus\\cac00%d.bmp", i);
    }
}

void dragon_images()
{
    for (int i = 0; i < 4; i++)
    {
        sprintf(dragon_image[i], "Game Assets\\Sprites\\dragon\\drag00%d.bmp", i);
    }
}

// Update Dino run
void update_run_dragon()
{
    if (!jumping)
    {
        run_idx = (run_idx + 1) % 8;
    }

    if(play)
    {
    if (!dragon_active) {
        if ((clock() - dragon_off) * 1000 / CLOCKS_PER_SEC >= 5000) {
            dragon_active = true;
            dragon_x = 1000 + rand() % 500; 
        }
    } else {
        dragon_x -= dragon_speed;
        if (dragon_x + 2100 < 0) {
            dragon_x = 1000 + rand() % 500;
        }
        dragon_idx = (dragon_idx + 1) % 4;
    }
    }
}

// Update Dino jump
void update_jump_cactus()
{
    if (jumping)
    {
        dino_y += velocity;
        velocity += gravity;

        if (dino_y <= dino_ground)
        {
            dino_y = dino_ground;
            jumping = false;
            jump_idx = 0;
        }
        else
        {

            jump_idx = (jump_idx + 1) % 12;
        }
    }

    if(play)
    {
    if (!cactus_active) {
        if ((clock() - cactus_off) * 1000 / CLOCKS_PER_SEC >= 1000) {
            cactus_active = true;
            cactus_x = 1000;       
            cact_idx = rand() % 5;  
        }
    } else {
        cactus_x -= cactus_speed;
        if (cactus_x + 50 < 0) {
            cactus_x = 1000;
            cact_idx = rand() % 5;
        }
    }
    }
}






//Collision Function
bool iscollision(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2)
{
    if (x1 + w1 < x2 || x2 + w2 < x1 || y1 + h1 < y2 || y2 + h2 < y1)
    {
        return false;
    }
    return true;
}

//Check Collisions
void check_collisions() {
    if(play)
    {
    clock_t current = clock();

    if ((current - last_collision) * 1000 / CLOCKS_PER_SEC < cooldown) 
    {
            return; 
    }

    if (cactus_active && 
        iscollision(dino_x + 30, dino_y + 10, 90, 110, cactus_x, cactus_y, 35, 75))
    {
        printf("Collision with cactus!\n");
        last_collision = current;
        dino_lives--;
    }

    if (dragon_active && 
        iscollision(dino_x + 30, dino_y + 10, 90, 110, dragon_x, dragon_y, 115, 72))
    {
        printf("Collision with dragon!\n");
        last_collision = current;
        dino_lives--;
    }

    if (dino_lives <= 0) {

        play = false;
        g_over = true;
        
    }
    }
}

//Collision with fireball
void update_fireballs() {

    if(play)
    {
    for (int i = 0; i < MAX_FIREBALLS; i++) {
        if (fireballs[i].active) {
            fireballs[i].x += fireball_speed;

            if (cactus_active &&
                iscollision(fireballs[i].x, fireballs[i].y, 30, 30, cactus_x, cactus_y, 35, 75)) {
                fireballs[i].active = false; 
                cactus_active = false;      
                cactus_off = clock(); 
            }

            if (dragon_active &&
                iscollision(fireballs[i].x, fireballs[i].y, 30, 30, dragon_x, dragon_y, 115, 72)) {
                fireballs[i].active = false; 
                dragon_active = false;     
                dragon_off = clock();
            }

            if (fireballs[i].x > 1000) {
                fireballs[i].active = false;
            }
        }
    }
    }
}




// void draw_bounding_boxes()
// {
//     iRectangle(dino_x + 30, dino_y + 10, 90, 110);
//     iRectangle(cactus_x, cactus_y, 35, 100);
//     // iRectangle(cactus_x2, cactus_y, 35, 75);
//     iRectangle(dragon_x, dragon_y, 115, 72);
// }

// Draw bg
void iDraw() {

    iClear();

    if (menu) {

        if (!sound) {
        PlaySound(NULL, 0, 0); 
        PlaySound(TEXT("menu.wav"), NULL, SND_ASYNC | SND_LOOP);
        sound = true; 
    }
        show_menu();  
    } else if (instruction_state) {
        show_instructions();  
    } else if (score_state) {
        show_score();  
    } else if (g_over) {
        PlaySound(NULL,0,0);
        game_over();
    }

    else if(play)
    {
    iShowBMP(0, 0, "bgd_08.bmp");
    show_bg();
    jump();

    if (cactus_active) {
        iShowBMP2(cactus_x, cactus_y, cactus_image[cact_idx], 0xFFFFFF);
    }
    if (dragon_active) {
        iShowBMP2(dragon_x, dragon_y, dragon_image[dragon_idx], 0xFFFFFF);
    }

    for (int i = 0; i < MAX_FIREBALLS; i++) {
        if (fireballs[i].active) {
            iShowBMP2(fireballs[i].x, fireballs[i].y, fireball_image, 0xFFFFFF);
        }
    }
    // life icons
    for (int i = 0; i < dino_lives; i++) {
        iShowBMP2(850 + i * 25, 340, life_icon, 0xFFFFFF); 
    }

    // fireball icons
    for (int i = 0; i < fire_rem; i++) {
        iShowBMP2(850 + i * 20, 320, fireball_icon, 0xFFFFFF); 
    }

    char scoreText[50];
    sprintf(scoreText, "Score: %d", score);
    iSetColor(255, 255, 255); 
    iText(850, 370, scoreText, GLUT_BITMAP_HELVETICA_18);

    }

}


// void debug_bounding_boxes()
// {
//     printf("Dino: x=%d, y=%d, w=%d, h=%d\n", dino_x, dino_y, 50, 105);
//     printf("Cactus1: x=%d, y=%d, w=%d, h=%d\n", cactus_x, cactus_y, 35, 75);
//     printf("Cactus2: x=%d, y=%d, w=%d, h=%d\n", cactus_x2, cactus_y, 35, 75);
//     printf("Dragon: x=%d, y=%d, w=%d, h=%d\n", dragon_x, dragon_y, 115, 72);
// }

// Move bg
void bg_move()
{   
    if(play)
    {
    bg_x -= bg_speed;

    if (bg_x <= -14 * bg_w)
    {
        bg_x = 0;
    }

     score++;
    }
}

/*
    function iMouseMove() is called when the user presses and drags the mouse.
    (mx, my) is the position where the mouse pointer is.
    */
void iMouseMove(int mx, int my)
{
    printf("x = %d, y= %d\n", mx, my);
    // 	//place your codes here
}

/*
    function iMouse() is called when the user presses/releases the mouse.
    (mx, my) is the position where the mouse pointer is.
    */
void iMouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {

        if (menu) {

            if (x <= 306 && x >= 120 && y >= 277 && y <= 346) {
                menu = false; 
                play = true;

                PlaySound(NULL, 0, 0); 
                PlaySound(TEXT("game.wav"), NULL, SND_ASYNC | SND_LOOP);

                if (!game_start) {
                    iSetTimer(20, bg_move);
                    iSetTimer(100, update_run_dragon);
                    iSetTimer(20, update_jump_cactus);
                    iSetTimer(20, update_fireballs);
                    iSetTimer(200, check_collisions);
                    iSetTimer(10000, increaseSpeed);
                    
                    game_start = true;  
                }
            }

            else if (x <= 306 && x >= 120 && y >= 210 && y <= 274) {
                menu = false;
                instruction_state = true; 
            }

            
            else if (x <= 306 && x >= 120 && y >= 142 && y <= 206) {
                menu = false;
                score_state = true;  
            }

            
            else if (x <= 306 && x >= 120 && y >= 70 && y <= 137) {
                exit(0); 
            }
        }

       
        if (instruction_state) 
        {
           
           // go back
            if (x >= 852 && x <= 945 && y >= 50 && y <= 80) {
                instruction_state = false;
                menu = true;  
            }
        }

        if (score_state) 
        {
           
           // go back
            if (x >= 852 && x <= 945 && y >= 50 && y <= 80) {
                score_state = false;
                menu = true;  
            }
        }
    }
}

/*
    function iKeyboard() is called whenever the user hits a key in keyboard.
    key- holds the ASCII value of the key pressed.
    */
void iKeyboard(unsigned char key) {
    if (key == ' ') {
        if (!jumping) {
            jumping = true;
            velocity = initial_vel;
        }
    } else if (key == 'f' && fire_rem > 0) { 
        for (int i = 0; i < MAX_FIREBALLS; i++) {
            if (!fireballs[i].active) { 
                fireballs[i].active = true;
                fireballs[i].x = dino_x + 80; 
                fireballs[i].y = dino_y + 80;
                fire_rem--; 
                break;
            }
        }
    } else if (key == 'q') {
        exit(0);
    }

    if (key == 'r' && g_over) {
    g_over = false;
    play = true;
    dino_lives = 5;
    fire_rem = 3;
    score = 0;
    cactus_active = true;
    dragon_active = true;
    cactus_x = 1000;
    dragon_x = 1000;
    bg_speed = 40;
    cactus_speed = 40;
    dragon_speed = 50;
    returned = 0;

    PlaySound(NULL, 0, 0); 
    PlaySound(TEXT("game.wav"), NULL, SND_ASYNC | SND_LOOP);
    sound = true;
    } 
    else if (key == 'm' && g_over)
    {
    g_over = false;
    menu = true;
    play = false;
    dino_lives = 5;
    fire_rem = 3;
    score = 0;
    cactus_active = true;
    dragon_active = true;
    cactus_x = 1000;
    dragon_x = 1000;
    bg_speed = 40;
    cactus_speed = 40;
    dragon_speed = 50;
    returned = 0;

    PlaySound(NULL, 0, 0); 
    PlaySound(TEXT("menu.wav"), NULL, SND_ASYNC | SND_LOOP);
    sound = true;
    }
}


/*
    function iSpecialKeyboard() is called whenver user hits special keys like-
    function keys, home, end, pg up, pg down, arraows etc. you have to use
    appropriate constants to detect them. A list is:
    GLUT_KEY_F1, GLUT_KEY_F2, GLUT_KEY_F3, GLUT_KEY_F4, GLUT_KEY_F5, GLUT_KEY_F6,
    GLUT_KEY_F7, GLUT_KEY_F8, GLUT_KEY_F9, GLUT_KEY_F10, GLUT_KEY_F11, GLUT_KEY_F12,
    GLUT_KEY_LEFT, GLUT_KEY_UP, GLUT_KEY_RIGHT, GLUT_KEY_DOWN, GLUT_KEY_PAGE UP,
    GLUT_KEY_PAGE DOWN, GLUT_KEY_HOME, GLUT_KEY_END, GLUT_KEY_INSERT
    */
void iSpecialKeyboard(unsigned char key)
{

    // if (key == GLUT_KEY_END) {
    // 	exit(0);
    // }                            
    // 	//place your codes for other keys here
}

int main()
{
    scan_scores();
    sort();
    srand(time(0));

    dino_run_images();
    dino_jump_images();
    cactus_images();
    dragon_images();

    for (int i = 0; i < MAX_FIREBALLS; i++)
    {
        fireballs[i].x = -100;
        fireballs[i].y = 0;
        fireballs[i].active = false;
    }


    iInitialize(1000, 400, "Dino Run with Scrolling Background");

    return 0;
}
