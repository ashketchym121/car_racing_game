#include <stdio.h>
#include <conio.h>
#include <windows.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define WIDTH 30
#define HEIGHT 20

int lanesX[3] = {30, 45, 60};
int carLane = 1;
int enemyLanes[2];
int enemyYs[2];
int enemyCount = 1;
int score = 0;
int gameover = 0;
int paused = 0;
int lastAchievementShown = 0;
int highScore = 0;
int lives = 3; /* Added lives */
char highestRank[20] = "None";
char highScoreName[50] = "None";
char playerName[50];
int theme = 0; /* 0=Day,1=Night for dynamic road theme */

/* Move cursor to x,y */
void gotoxy(int x, int y) {
    COORD coord;
    coord.X = (SHORT)x;
    coord.Y = (SHORT)y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

/* Draw side borders */
void drawBorder(void) {
    int i;
    for (i = 0; i <= 24; i++) {
        gotoxy(25, i);
        printf("|");
        gotoxy(65, i);
        printf("|");
    }
}

/* Draw road lines with dynamic theme */
void drawRoadLines(void) {
    int i;
    char lineChar;
    if (theme == 0) {
        /* Day theme - use | */
        lineChar = '|';
    } else {
        /* Night theme - use '.' for lines */
        lineChar = '.';
    }
    for (i = 0; i <= 24; i += 2) {
        gotoxy(40, i);
        printf("%c", lineChar);
        gotoxy(55, i);
        printf("%c", lineChar);
    }
}

/* Draw the player's car */
void drawCar(void) {
    int x = lanesX[carLane];
    gotoxy(x, 20);
    printf("  ^  ");
    gotoxy(x, 21);
    printf(" /|\\ ");
    gotoxy(x, 22);
    printf(" / \\ ");
}

/* Erase player's car */
void eraseCar(void) {
    int x = lanesX[carLane];
    int i;
    for (i = 20; i <= 22; i++) {
        gotoxy(x, i);
        printf("     ");
    }
}

/* Draw enemy car */
void drawEnemy(int index) {
    int x = lanesX[enemyLanes[index]];
    int y = enemyYs[index];
    gotoxy(x, y);
    printf(" ### ");
    gotoxy(x, y + 1);
    printf(" ### ");
    gotoxy(x, y + 2);
    printf(" ### ");
}

/* Erase enemy car */
void eraseEnemy(int index) {
    int x = lanesX[enemyLanes[index]];
    int y = enemyYs[index];
    int i;
    for (i = y; i <= y + 2; i++) {
        gotoxy(x, i);
        printf("     ");
    }
}

/* Reset enemy position and lane */
void resetEnemy(int index) {
    enemyYs[index] = 0;
    enemyLanes[index] = rand() % 3;
}

/* Check collision between car and one enemy */
int checkCollisionSingle(int enemyIndex) {
    int carLeft = lanesX[carLane];
    int carRight = carLeft + 4;
    int enemyLeft = lanesX[enemyLanes[enemyIndex]];
    int enemyRight = enemyLeft + 4;

    int carTop = 20;
    int carBottom = 22;
    int enemyTop = enemyYs[enemyIndex];
    int enemyBottom = enemyYs[enemyIndex] + 2;

    int xOverlap = !(carRight < enemyLeft || enemyRight < carLeft);
    int yOverlap = !(carBottom < enemyTop || enemyBottom < carTop);

    return (xOverlap && yOverlap);
}

/* Check collision for all enemies */
int checkCollision(void) {
    int i;
    for (i = 0; i < enemyCount; i++) {
        if (checkCollisionSingle(i)) {
            return 1;
        }
    }
    return 0;
}

/* Display score, player name, lives, high score and controls */
void scoreBoard(void) {
    int i;
    gotoxy(0, 0);
    printf("Player: %s", playerName);
    gotoxy(0, 1);
    printf("Score: %d", score);
    gotoxy(0, 2);
    printf("High Score: %d (%s)", highScore, highScoreName);
    gotoxy(0, 3);
    printf("Rank: %s", highestRank);

    /* Display lives as hearts */
    gotoxy(0, 4);
    printf("Lives: ");
    for (i = 0; i < lives; i++) {
        printf("\3 "); // Heart symbol in console (ASCII 3)
    }
    if (lives == 0) {
        printf("No lives left!");
    }

    gotoxy(0, 5);
    printf("Controls: A-Left | D-Right | P-Pause | T-Theme | X-Exit");
}

/* Explosion animation */
void explosionEffect(void) {
    int x = lanesX[carLane];
    int y = 20;
    int i;
    char *explosionFrames[3] = {
        "  *** ",
        " *!!!*",
        "  *** "
    };

    for (i = 0; i < 6; i++) {
        gotoxy(x, y);
        printf("%s", explosionFrames[i % 3]);
        gotoxy(x, y + 1);
        printf("%s", explosionFrames[(i + 1) % 3]);
        gotoxy(x, y + 2);
        printf("%s", explosionFrames[(i + 2) % 3]);
        Sleep(200);
    }
}

/* Game over screen and high score update */
void gameOverScreen(void) {
    system("cls");
    printf("\n\n\t\tGAME OVER!\n\t\tYour Score: %d\n", score);
    if (score > highScore) {
        highScore = score;
        strncpy(highScoreName, playerName, 49);
        highScoreName[49] = '\0';
        printf("\t\tNEW HIGH SCORE!\n");
    }
    printf("\n\t\tPress any key to return to menu...");
    getch();
}

/* Show achievement with blinking text */
void showAchievement(char *text) {
    int i;
    int x = 70;
    for (i = 0; i < 4; i++) {
        gotoxy(x, 2);
        printf("<< %s >>", text);
        Sleep(300);
        gotoxy(x, 2);
        printf("                   ");
        Sleep(200);
    }
    strncpy(highestRank, text, 19);
    highestRank[19] = '\0';
}

/* Animated blinking title screen before menu */
void animatedTitleScreen(void) {
    int i;
    char *title = "RACING RUSH!";
    system("cls");
    for (i = 0; i < 6; i++) {
        gotoxy(30, 10);
        if (i % 2 == 0) {
            printf("%s", title);
        } else {
            printf("           "); /* clear */
        }
        Sleep(500);
    }
}

/* Intro screen with player name input */
void fancyIntroScreen(void) {
    animatedTitleScreen();
    printf("\n\t==============================\n");
    printf("\t   WELCOME TO RACING RUSH!   \n");
    printf("\t==============================\n");
    printf("\n\t    Developed by Bestie Dev    \n");
    printf("\n\tGet ready to test your reflexes!\n");
    printf("\n\tEnter your player name: ");
    if (fgets(playerName, sizeof(playerName), stdin) != NULL) {
        size_t len = strlen(playerName);
        if (len > 0 && playerName[len - 1] == '\n') {
            playerName[len - 1] = '\0';
        }
    }
    system("cls");
}

/* Menu screen */
void menu(void) {
    system("cls");
    printf("\n\t===== RACING CAR GAME MENU =====\n");
    printf("\t1. Start Game\n");
    printf("\t2. Continue (if paused)\n");
    printf("\t3. Show Highest Rank & Score\n");
    printf("\t4. Exit\n");
    printf("\n\tSelect an option: ");
}

/* Game loop */
void gameLoop(void) {
    int i;
    enemyCount = 1;
    resetEnemy(0);
    lives = 3;
    score = 0;
    lastAchievementShown = 0;
    theme = 0; /* Start with day theme */
    paused = 0;
    gameover = 0;

    while (!gameover) {
        if (paused) {
            gotoxy(35, 10);
            printf("Game Paused. Press 'P' to Resume, 'X' to Exit.");

            if (_kbhit()) {
                char ch = _getch();
                if (ch == 'p' || ch == 'P') {
                    paused = 0;
                    gotoxy(35, 10);
                    printf("                                   ");
                } else if (ch == 'x' || ch == 'X') {
                    gameover = 1;
                    break;
                }
            }
            Sleep(100);
            continue;
        }

        drawBorder();
        drawRoadLines();
        drawCar();

        for (i = 0; i < enemyCount; i++) {
            drawEnemy(i);
        }

        scoreBoard();

        /* Speed up with score */
        {
            int speedLevel = score / 10;
            float speedMultiplier = 1.0f + (float)speedLevel;
            int delay = (int)(100 / speedMultiplier);
            if (delay < 5) delay = 5;
            Sleep(delay);
        }

        for (i = 0; i < enemyCount; i++) {
            eraseEnemy(i);
            enemyYs[i]++;
        }

        for (i = 0; i < enemyCount; i++) {
            if (enemyYs[i] > 24) {
                score++;
                resetEnemy(i);

                if (score > 5 && enemyCount == 1) {
                    enemyCount = 2;
                    resetEnemy(1);
                }
            }
        }

        eraseCar();

        /* Input handling */
        if (_kbhit()) {
            char ch = _getch();
            if ((ch == 'a' || ch == 'A') && carLane > 0) carLane--;
            if ((ch == 'd' || ch == 'D') && carLane < 2) carLane++;
            if (ch == 'p' || ch == 'P') paused = 1;
            if (ch == 'x' || ch == 'X') {
                gameover = 1;
            }
            if (ch == 't' || ch == 'T') {
                theme = !theme; /* toggle theme */
            }
        }

        if (score >= 150 && lastAchievementShown < 150) {
            showAchievement("Legendary");
            lastAchievementShown = 150;
        } else if (score >= 100 && lastAchievementShown < 100) {
            showAchievement("Epic");
            lastAchievementShown = 100;
        } else if (score >= 75 && lastAchievementShown < 75) {
            showAchievement("Rare");
            lastAchievementShown = 75;
        } else if (score >= 50 && lastAchievementShown < 50) {
            showAchievement("Uncommon");
            lastAchievementShown = 50;
        } else if (score >= 10 && lastAchievementShown < 10) {
            showAchievement("Normal Player");
            lastAchievementShown = 10;
        }

        /* Check collision */
        if (checkCollision()) {
            lives--;
            if (lives <= 0) {
                explosionEffect();
                gameover = 1;
            } else {
                /* Reset enemies & car position on collision */
                for (i = 0; i < enemyCount; i++) {
                    eraseEnemy(i);
                    resetEnemy(i);
                }
                eraseCar();
                carLane = 1; /* reset car lane */
                paused = 0;
            }
        }
    }

    gameOverScreen();
}

int main(void) {
    int choice;
    srand((unsigned int)time(NULL));
    fancyIntroScreen();

    while (1) {
        menu();
        if (scanf("%d", &choice) != 1) {
            while (getchar() != '\n'); /* clear invalid input */
            continue;
        }
        getchar(); /* consume newline */

        if (choice == 1) {
            score = 0;
            gameover = 0;
            paused = 0;
            carLane = 1;
            lives = 3;
            system("cls");
            gameLoop();
        } else if (choice == 2) {
            if (paused) {
                paused = 0;
                system("cls");
                gameLoop();
            } else {
                printf("\nNo game paused currently.\nPress any key to continue...");
                getch();
            }
        } else if (choice == 3) {
            system("cls");
            printf("\n\tHigh Score: %d (%s)\n", highScore, highScoreName);
            printf("\tHighest Rank: %s\n", highestRank);
            printf("\n\tPress any key to return to menu...");
            getch();
        } else if (choice == 4) {
            break;
        }
    }

    return 0;
}

