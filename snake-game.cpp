#include <iostream>
#include <graphics.h>
#include <ctime>
#include <cstdlib>
#include <cstring>
#include <conio.h>

using namespace std;

// Node structure to represent a point in the game grid
struct Node
{
    int x, y;
    Node* next;
};

// Enumerations for various game features
enum Direction { UP, DOWN, LEFT, RIGHT };
enum PowerUpType { NO_POWERUP, SCORE_BOOST, MORE_FOOD, LENGTH_REDUCTION, LENGTH_INCREASE, SPECIAL_FOOD};
enum GameMode { CLASSIC, POWERUP, SURVIVAL };

// Global variables for game state
GameMode currentGameMode = CLASSIC;  // Set the initial game mode
int gridSize = 20;
int windowWidth = 840;
int windowHeight = 640;
int screenWidth = 800;
int screenHeight = 600;
int snakeLength = 5;
int score = 0;
int level = 1;
Direction currentDirection = RIGHT;
Node* head;
Node* food;
bool powerUpActive = false;
int powerUpX, powerUpY;
time_t lastPowerUpTime;
const int powerUpDuration = 10;
const int levelThreshold = 100;
int snakeColor = BLUE;
bool specialFoodPowerUpActivated = false;
Node* obstacles = nullptr;
PowerUpType currentPowerUp = NO_POWERUP;
int initialDelay = 100; // Initial delay for the game loop
int lives = 3; // Number of lives for the player

// Function declarations
void displayMenu();
void setGameMode(GameMode);
void initializeGame();
bool isPositionOccupied(Node*,int,int);
void spawnPowerUp(Node*,Node*);
void spawnObstacle(Node*);
void drawSnake();
void drawFood();
void drawPowerUp();
void drawObstacles();
void handleInput();
void displayScore();
void displayLevel();
void displayLives();
void displayGameMode();
bool checkBoundaryCollision(Node*);
bool checkSelfCollision(Node*);
void resetGameState();
void updateSnake();
void updateSnakeSur();
void updatePowerUp(Node*,Node*);
void updateLevel(Node*);
bool checkObstacleCollision(Node*);
void runGame();
void runClassic();
void runPowerUp();
void runSurvival();
void displayRules();
bool displayEndMenu(const char*);

int main()
{
    int gd = DETECT, gm;
    //initgraph(&gd, &gm, "C:\\TC\\BGI");
    initwindow(windowWidth, windowHeight, "Snake Game");
    while (true)
    {
        displayMenu();
    }
    return 0;
}

// Function to display the main menu
void displayMenu()
{
    cleardevice();
    char c1[] = "Snake Game";
    char c2[] = "1. Classic";
    char c3[] = "2. Powerup";
    char c4[] = "3. Survival";
    char c5[] = "4. Exit";

    int centerX = getmaxx() / 2;  // Calculate the center of the screen
    int centerY = getmaxy() / 2 - (textheight(c1) + textheight(c2) + textheight(c3) + textheight(c4) + textheight(c5)) / 2;

    settextstyle(8, HORIZ_DIR, 5);
    outtextxy(centerX - textwidth(c1) / 2, centerY, c1);

    settextstyle(8, HORIZ_DIR, 4);
    outtextxy(centerX - textwidth(c2) / 2, centerY + textheight(c1), c2);
    outtextxy(centerX - textwidth(c3) / 2, centerY + textheight(c1) + textheight(c2), c3);
    outtextxy(centerX - textwidth(c4) / 2, centerY + textheight(c1) + textheight(c2) + textheight(c3), c4);
    outtextxy(centerX - textwidth(c5) / 2, centerY + textheight(c1) + textheight(c2) + textheight(c3) + textheight(c4), c5);

    char choice = getch();

    switch (choice)
    {
        case '1':
            setGameMode(CLASSIC);
            break;
        case '2':
            setGameMode(POWERUP);
            break;
        case '3':
            setGameMode(SURVIVAL);
            break;
        case '4':
            closegraph();
            exit(0);
        default:
            getch();
            break;
    }
    displayRules();
    initializeGame();
    runGame();
}

// Function to set the game mode
void setGameMode(GameMode mode)
{
    currentGameMode = mode;
}

// Function to initialize the game
void initializeGame()
{
    initwindow(windowWidth, windowHeight, "Snake Game");

    head = new Node;
    head->x = 2 * gridSize;
    head->y = 2 * gridSize;
    head->next = nullptr;



    food = new Node;
    food->x = rand() % (screenWidth / gridSize) * gridSize;
    food->y = rand() % (screenHeight / gridSize) * gridSize;


}

// Function to check if a position is occupied by the snake
bool isPositionOccupied(Node* snakeHead, int x, int y)
{
    Node* temp = snakeHead;
    while (temp != nullptr)
    {
        if (temp->x == x && temp->y == y)
        {
            return true; // Position is occupied by the snake
        }
        temp = temp->next;
    }
    return false; // Position is not occupied
}

void spawnPowerUp(Node* snakeHead, Node* food)
{
    int maxX = screenWidth / gridSize;
    int maxY = screenHeight / gridSize;

    // Decide which power-up to spawn based on the game level
    if (level < 2)
    {
        // Spawn only SCORE_BOOST and SPECIAL_FOOD before reaching level 2
        int randomEffect = rand() % 2;  // 0 or 1 for SCORE_BOOST or SPECIAL_FOOD
        switch (randomEffect)
        {
            case 0:
                currentPowerUp = SCORE_BOOST;
                break;
            case 1:
                currentPowerUp = SPECIAL_FOOD;
                break;
            default:
                currentPowerUp = NO_POWERUP;
                break;
        }
    }
    else
    {
        // Spawn any power-up, including LENGTH_REDUCTION and LENGTH_INCREASE, after reaching level 2
        int randomEffect = rand() % 4;  // 0 to 3 for all four effects
        switch (randomEffect) {
            case 0:
                currentPowerUp = SCORE_BOOST;
                break;
            case 1:
                currentPowerUp = LENGTH_REDUCTION;
                break;
            case 2:
                currentPowerUp = LENGTH_INCREASE;
                break;
            case 3:
                currentPowerUp = SPECIAL_FOOD;
                break;
            default:
                currentPowerUp = NO_POWERUP;
                break;
        }
    }

    // Choose a random position for the power-up with additional conditions to avoid collisions
    powerUpX = rand() % (maxX - 1) * gridSize;
    powerUpY = rand() % (maxY - 1) * gridSize;

    lastPowerUpTime = time(nullptr);
    powerUpActive = true;
}

// Function to spawn obstacles at random positions
void spawnObstacle(Node* snakeHead)
{
    Node* newObstacle = new Node;
    // Choose a random position for the obstacle with additional conditions to avoid collisions
    do
    {
        newObstacle->x = rand() % (screenWidth / gridSize) * gridSize;
        newObstacle->y = rand() % (screenHeight / gridSize) * gridSize;
    } while (isPositionOccupied(snakeHead, newObstacle->x, newObstacle->y));

    // Add the new obstacle to the linked list
    newObstacle->next = obstacles;
    obstacles = newObstacle;
}

void drawSnake()
{
    Node* temp = head;
    // Loop through each node in the snake and draw a rectangle for each
    while (temp != nullptr)
    {
        setfillstyle(SOLID_FILL, snakeColor);
        setcolor(BLACK);
        bar(temp->x, temp->y, temp->x + gridSize, temp->y + gridSize);
        temp = temp->next;
    }
}

void drawFood()
{
    // Draw the food differently based on whether it's a special food or not
    if (currentPowerUp == SPECIAL_FOOD)
    {
        setfillstyle(SOLID_FILL, YELLOW);
        setcolor(YELLOW);
        fillellipse(food->x + gridSize / 2, food->y + gridSize / 2, gridSize / 2, gridSize / 2);
    }
    else
    {
        setfillstyle(SOLID_FILL, RED);
        setcolor(BLACK);
        fillellipse(food->x + gridSize / 2, food->y + gridSize / 2, gridSize / 2, gridSize / 2);
    }
}

void drawPowerUp()
{
    // Draw the power-up if it's active
    if (powerUpActive)
    {
        // Draw different shapes and colors based on the type of power-up
        int centerX = powerUpX + gridSize / 2;
        int centerY = powerUpY + gridSize / 2;
        if (currentPowerUp == SCORE_BOOST)
        {
            setfillstyle(HATCH_FILL, YELLOW);
            fillellipse(centerX, centerY, gridSize / 2, gridSize / 2);
        }
        else if (currentPowerUp == LENGTH_REDUCTION)
        {
            setfillstyle(HATCH_FILL, LIGHTMAGENTA);
            fillellipse(centerX, centerY, gridSize / 2, gridSize / 2);
        }
        else if (currentPowerUp == LENGTH_INCREASE)
        {
            setfillstyle(HATCH_FILL, CYAN);
            fillellipse(centerX, centerY, gridSize / 2, gridSize / 2);
        }
        else if (currentPowerUp == SPECIAL_FOOD)
        {
            setfillstyle(HATCH_FILL, LIGHTGREEN);
            fillellipse(centerX, centerY, gridSize / 2, gridSize / 2);
        }
    }
}

// Function to draw obstacles on the screen
void drawObstacles()
{
    Node* temp = obstacles;
    while (temp != nullptr)
    {
        setfillstyle(SLASH_FILL, YELLOW);
        bar(temp->x, temp->y, temp->x + gridSize, temp->y + gridSize);
        temp = temp->next;
    }
}

// Function to handle user input for controlling the snake's direction
void handleInput()
{
    if (kbhit())
    {
        char key = getch();
        switch (key)
        {
            case 'w':
                if (currentDirection != DOWN)
                    currentDirection = UP;
                break;
            case 's':
                if (currentDirection != UP)
                    currentDirection = DOWN;
                break;
            case 'a':
                if (currentDirection != RIGHT)
                    currentDirection = LEFT;
                break;
            case 'd':
                if (currentDirection != LEFT)
                    currentDirection = RIGHT;
                break;
        }
    }
}

void displayRules()
{
    cleardevice(); // Clear the existing content on the graphic window
    settextstyle(DEFAULT_FONT, HORIZ_DIR, 2);
    outtextxy(50, 100, "Welcome to the Snake Game!");
    outtextxy(50, 130, "Rules:");
    outtextxy(50, 160, "Eat the food to grow the snake.");
    setfillstyle(SOLID_FILL, RED);
    fillellipse(20, 175, 10, 10);
    outtextxy(50, 190, "Avoid collisions with the walls and the");
    outtextxy(50, 220, "snake itself.");
    switch(currentGameMode)
    {
        case CLASSIC:break;
        case POWERUP:
                    outtextxy(50, 220, "Power-ups appear randomly:");
                    outtextxy(70, 250, "Yellow Power-up: Gives Score Boost.");
                    outtextxy(70, 280, "Blue Power-up: Increase Snake Length.");
                    outtextxy(70, 310, "Green Power-up: Spawn Special Food.");
                    outtextxy(70, 340, "Light Magenta Power-up: Decrease Snake Length.");
                    setfillstyle(XHATCH_FILL, YELLOW);
                    fillellipse(40, 265, 10, 10);
                    setfillstyle(XHATCH_FILL, CYAN);
                    fillellipse(40, 295, 10, 10);
                    setfillstyle(XHATCH_FILL, LIGHTGREEN);
                    fillellipse(40, 325, 10, 10);
                    setfillstyle(XHATCH_FILL, LIGHTMAGENTA);
                    fillellipse(40, 355, 10, 10);
                     break;
        case SURVIVAL:
                    outtextxy(50, 220, "Collect food to increase time.");
                    outtextxy(50, 250, "Avoid collisions with obstacles.");
                    setfillstyle(SLASH_FILL, YELLOW);
                    bar(25, 240, 45, 260);
                    outtextxy(50, 280, "Lose a Life if collide with obstacle.");
                    outtextxy(50, 310, "Lose a Life if time Runs out.");
                    outtextxy(50, 340, "Game Over when Lives Reach 0.");
                    break;
    }
    // Add a message to prompt the user to press a key to start the game
    outtextxy(50, 400, "Press any key to start the game...");
    getch(); // Wait for a key press
    cleardevice(); // Clear the rules before starting the game
}

// Function to display the player's score on the screen
void displayScore()
{
    setcolor(WHITE);
    settextstyle(8, HORIZ_DIR, 4);
    char scoreStr[10];
    sprintf(scoreStr, "Score: %d", score);
    outtextxy(190, screenHeight+10, scoreStr);
}

// Function to display the current game level on the screen
void displayLevel()
{
    setcolor(WHITE);
    settextstyle(8, HORIZ_DIR, 4);
    char levelStr[15];
    sprintf(levelStr, "Level: %d", level);
    outtextxy(10, screenHeight+10, levelStr);
}

// Function to display the remaining lives on the screen
void displayLives()
{
    setcolor(WHITE);
    settextstyle(8, HORIZ_DIR, 4);
    char livesStr[15];
    sprintf(livesStr, "Lives: %d", lives);
    outtextxy(500, 10, livesStr);
}

// Function to display the current game mode on the screen
void displayGameMode()
{
    setcolor(WHITE);
    settextstyle(8, HORIZ_DIR, 4);
    char modeStr[20];
    switch (currentGameMode)
    {
        case CLASSIC:
            sprintf(modeStr, "Mode: Classic");
            break;
        case POWERUP:
            sprintf(modeStr, "Mode: Power-up");
            break;
        case SURVIVAL:
            sprintf(modeStr, "Mode: Survival");
            break;
    }
    outtextxy(420, screenHeight + 10, modeStr);
}

// Function to check for boundary collision and display a game over message
bool checkBoundaryCollision(Node* head)
{
    if (head->x < 0 || head->y < 0 || head->x >= screenWidth || head->y >= screenHeight)
    {
        return true; // Indicate collision
    }
    return false; // No collision
}

// Function to check if the snake collides with itself
bool checkSelfCollision(Node* head)
{
    Node* temp = head->next;
    while (temp != nullptr)
    {
        if (head->x == temp->x && head->y == temp->y)
        {

            return true; // Indicate collision
        }
        temp = temp->next;
    }
    return false; // No collision
}

// Function to reset the game state
void resetGameState()
{
    score = 0;
    snakeLength = 3;
    level = 1;
    lives=3;
    currentDirection = RIGHT;
    powerUpActive = false;
    currentPowerUp= NO_POWERUP;
    snakeColor=BLUE;
}

// Function to update the snake's position based on the current direction
void updateSnake()
{
    Node* newHead = new Node;
    newHead->next = head;
    // Move the head in the specified direction
    switch (currentDirection)
    {
        case UP:
            newHead->x = head->x;
            newHead->y = head->y - gridSize;
            break;
        case DOWN:
            newHead->x = head->x;
            newHead->y = head->y + gridSize;
            break;
        case LEFT:
            newHead->x = head->x - gridSize;
            newHead->y = head->y;
            break;
        case RIGHT:
            newHead->x = head->x + gridSize;
            newHead->y = head->y;
            break;
    }
    head = newHead;
    // Check if the head collides with the food
    if (head->x == food->x && head->y == food->y)
    {
        snakeLength++;
        if (currentPowerUp == SPECIAL_FOOD)
        {
            // Award 25 points for eating food during SPECIAL_FOOD power-up
            score += 25;
        }
        else
        {
            // Award 10 points for regular food
            score += 10;
        }

        // Generate a new position for the food
        food->x = rand() % (screenWidth / gridSize) * gridSize;
        food->y = rand() % (screenHeight / gridSize) * gridSize;

        // Activate special food power-up
        if (currentPowerUp == SPECIAL_FOOD)
        {
            specialFoodPowerUpActivated = true;
        }
    }

    // Trim the snake length to its current size
    Node* temp = head;
    for (int i = 0; i < snakeLength && temp->next != nullptr; i++)
    {
        temp = temp->next;
    }
    Node* tail = temp->next;
    temp->next = nullptr;
    delete tail;
}

void updateSnakeSur()
{
    Node* newHead = new Node;
    newHead->next = head;

    // Move the head in the specified direction
    switch (currentDirection)
    {
        case UP:
            newHead->x = head->x;
            newHead->y = head->y - gridSize;
            break;
        case DOWN:
            newHead->x = head->x;
            newHead->y = head->y + gridSize;
            break;
        case LEFT:
            newHead->x = head->x - gridSize;
            newHead->y = head->y;
            break;
        case RIGHT:
            newHead->x = head->x + gridSize;
            newHead->y = head->y;
            break;
    }

    head = newHead;

    // Trim the snake length to its current size
    Node* temp = head;
    for (int i = 0; i < snakeLength && temp->next != nullptr; i++) {
        temp = temp->next;
    }

    Node* tail = temp->next;
    temp->next = nullptr;

    delete tail;
}

// Function to update the state of the power-up
void updatePowerUp(Node* snakeHead, Node* food)
{
    Node* specialFood = nullptr;
    if (powerUpActive)
    {
        // Check if the power-up duration has expired
        time_t currentTime = time(nullptr);
        if (currentTime - lastPowerUpTime >= powerUpDuration)
        {
            powerUpActive = false;
            currentPowerUp = NO_POWERUP;
        }
        // Check if the head collides with the power-up
        if (head->x == powerUpX && head->y == powerUpY)
        {
            Node* temp = head;
            // Apply the effect of the collected power-up
            switch (currentPowerUp)
            {
                case SCORE_BOOST:
                    score += 20;
                    break;
                case LENGTH_REDUCTION:
                    // Reduce the snake's length by 3
                    for (int i = 0; i < 3 && snakeLength > 1 && temp != nullptr; i++)
                    {
                        Node* prev = nullptr;
                        while (temp->next != nullptr)
                        {
                            prev = temp;
                            temp = temp->next;
                        }
                        delete temp;
                        temp = prev;
                        if (temp != nullptr)
                        {
                            temp->next = nullptr;
                        }
                        snakeLength--;
                    }
                    break;
                case LENGTH_INCREASE:
                    // Increase snake length by 3
                    while (temp->next != nullptr)
                    {
                        temp = temp->next;
                    }
                    for (int i = 0; i < 3; i++)
                    {
                        Node* newTail = new Node;
                        newTail->x = temp->x;
                        newTail->y = temp->y;
                        newTail->next = nullptr;
                        temp->next = newTail;
                        temp = newTail;
                        snakeLength++;
                    }
                    break;
                case SPECIAL_FOOD:
                    specialFoodPowerUpActivated = true;
                    break;
                default:
                    break;
            }
            // Deactivate the power-up and spawn a new one
            powerUpActive = false;
            currentPowerUp = NO_POWERUP;
        }
    }
    else
    {
        // Check if it's time to spawn a new power-up
        time_t currentTime = time(nullptr);
        if (currentTime - lastPowerUpTime >= powerUpDuration)
        {
            spawnPowerUp(snakeHead, food);
            lastPowerUpTime=currentTime;
        }

    }

    // Handle the special food activation outside the main condition
    if (specialFoodPowerUpActivated)
    {
        // Spawn a special food worth 25 points
        specialFood = new Node;
        do
        {
            specialFood->x = rand() % (screenWidth / gridSize) * gridSize;
            specialFood->y = rand() % (screenHeight / gridSize) * gridSize;
        } while (isPositionOccupied(snakeHead, specialFood->x, specialFood->y));
        specialFood->next = food;
        food = specialFood;
        specialFoodPowerUpActivated = false;  // Reset the flag after spawning the special food
    }
}

void updateLevel(Node* snakeHead)
{
    if (score >= levelThreshold * level)
    {
        level++;
        if (level % 2 == 0)
        {
            // Increase the speed of the snake every even level
            initialDelay -= 5;
        }
        switch (level % 3)
        {
            case 0:
                snakeColor = GREEN;
                break;
            case 1:
                snakeColor = BLUE;
                break;
            case 2:
                snakeColor = WHITE;
                break;
        }
        delay(100 - level * 5);
        for (int i = 0; i < 3 && snakeLength > 1; i++)
        {
            Node* temp = snakeHead;
            while (temp->next->next != nullptr)
            {
                temp = temp->next;
            }
            delete temp->next;
            temp->next = nullptr;
            snakeLength--;
        }
        // Remove obstacles on reaching a new level
        Node* temp = obstacles;
        while (temp != nullptr)
        {
            Node* next = temp->next;
            delete temp;
            temp = next;
        }
        obstacles = nullptr;
    }
}

// Function to check collision with obstacles
bool checkObstacleCollision(Node* snakeHead) {
    Node* temp = obstacles;
    while (temp != nullptr)
    {
        if (snakeHead->x == temp->x && snakeHead->y == temp->y)
        {
            lives--;
            return true;
        }
        temp = temp->next;
    }
    return false;
}

void runGame()
{
    int lives = 3; // Reset the lives for each new game
    while (lives > 0) {
        initializeGame(); // Reset the game after each life
        switch (currentGameMode) {
            case CLASSIC:
                runClassic();  // Call the function for CLASSIC mode
                break;
            case POWERUP:
                runPowerUp();  // Call the function for POWERUP mode
                break;
            case SURVIVAL:
                runSurvival();
                break;
        }
    }
}

void runClassic()
{
    resetGameState();
    while (true)
    {
        cleardevice();
        handleInput();

        updateSnake();
        updateLevel(head);

        drawSnake();
        drawFood();
        displayScore();
        displayLevel();
        displayGameMode();

        delay(initialDelay - level * 5);

        // Check for game over conditions
        // Break out of the inner loop to reset the game after each life
        if (checkBoundaryCollision(head))
        {
            char c1[] = "You hit the screen boundary.";
            outtextxy(100,300,c1);
            delay(2000);
            break;
        }
        else if( checkSelfCollision(head))
        {
            char c1[]="You collided with yourself.";
            outtextxy(100,300,c1);
            delay(2000);
            break;
        }
    }
    // Display end menu when the player loses
    bool returnToMainMenu = displayEndMenu("Game over!You collided");
    if (returnToMainMenu)
    {
        displayMenu();
    }
    else
    {
        closegraph();
        exit(0);
    }
}

void runPowerUp()
{
    resetGameState();
    spawnPowerUp(head, food); // Ensure power-up is spawned at the beginning
    while (true)
    {
        cleardevice();
        handleInput();

        updateSnake();
        updateLevel(head);

        drawSnake();
        drawFood();
        displayScore();
        displayLevel();
        displayGameMode();
        drawPowerUp();

        delay(initialDelay - level * 5);
        updatePowerUp(head, food);
        // Check for game over conditions
        // Break out of the inner loop to reset the game after each life
        if (checkBoundaryCollision(head))
        {
            char c1[] = "You hit the screen boundary.";
            outtextxy(100,300,c1);
            delay(2000);
            break;
        }
        else if( checkSelfCollision(head))
        {
            // Display a message indicating the reason for the game over
            char c1[]="You collided with yourself.";
            outtextxy(100,300,c1);
            delay(2000);
            break;
        }
    }

    // Display end menu when the player loses
    bool returnToMainMenu = displayEndMenu("Game over!You collided");

    if (returnToMainMenu)
    {
        displayMenu();
    }
    else
    {
        closegraph();
        exit(0);
    }
}

void runSurvival()
{
    resetGameState();
    int timeLimit = 60; // Initial time limit in seconds
    int obstacleSpawnInterval = 10; // Interval for spawning obstacles in seconds
    int obstacleSpawnTimer = 0;
    time_t startTime = time(nullptr);
    while (lives>0)
    {
        cleardevice();
        handleInput();

        updateSnakeSur();
        updateLevel(head);

        drawSnake();
        drawFood();
        drawObstacles();

        displayScore();
        displayLevel();
        displayLives();
        displayGameMode();

        // Update survival mode-specific elements
        time_t currentTime = time(nullptr);
        int elapsedTime = currentTime - startTime;

        // Decrease time limit based on elapsed time
        int remainingTime = timeLimit - elapsedTime;

        // Check for time limit
        if (remainingTime <= 0)
        {
            lives--;
            if (lives > 0)
            {
                break; // Break out of the inner loop to reset the game after each life
            }
        }

        // Spawn obstacles at regular intervals
        if (elapsedTime >= obstacleSpawnTimer)
        {
            spawnObstacle(head);
            obstacleSpawnTimer += obstacleSpawnInterval;
        }

        // Check for collision with obstacles
        if (checkObstacleCollision(head))
        {
            outtextxy(100, 300, "You collided with an obstacle.");
            delay(1000);
        }

        // Check for food consumption
        if (head->x == food->x && head->y == food->y)
        {
            snakeLength++;
            score += 10;
            timeLimit += 5; // Increase time on consuming food
            food->x = rand() % (screenWidth / gridSize) * gridSize;
            food->y = rand() % (screenHeight / gridSize) * gridSize;
        }

        // Display the remaining time
        setcolor(WHITE);
        settextstyle(8, HORIZ_DIR, 4);
        char timeStr[20];
        sprintf(timeStr, "Time: %d", remainingTime);
        outtextxy(10, 10, timeStr);

        delay(initialDelay - level * 5);

        // Check for game over conditions
        // Break out of the inner loop to reset the game after each life
        if (checkBoundaryCollision(head))
        {
            char c1[] = "You hit the screen boundary.";
            delay(50);
            outtextxy(250, 200, c1);
            delay(1000);
            break;
        }
        else if( checkSelfCollision(head))
        {
            char c1[]="You collided with yourself.";
            outtextxy(100,300,c1);
            delay(1000);
            break;
        }

    }

    // Display end menu when the player runs out of lives
    bool returnToMainMenu = displayEndMenu("Game over!");

    if (returnToMainMenu)
    {
        displayMenu();
    } else
    {
        closegraph();
        exit(0);
    }
}

bool displayEndMenu(const char* reason)
{
    cleardevice();
    char c3[100];
    strcpy(c3,reason);
    // Display the reason for losing
    setcolor(WHITE);
    settextstyle(8, HORIZ_DIR, 4);
    outtextxy(250, 200, c3);

    // Display the score
    char scoreStr[20];
    sprintf(scoreStr, "Score: %d", score);
    outtextxy(300, 250, scoreStr);

    // Display the level
    char levelStr[20];
    sprintf(levelStr, "Level: %d", level);
    outtextxy(300, 300, levelStr);

    // Display the menu options
    char c1[] = "1. Return to Main Menu";
    char c2[] = "2. Exit";

    settextstyle(8, HORIZ_DIR, 4);
    outtextxy(300, 350, c1);
    outtextxy(300, 400, c2);

    char choice = getch();

    if (choice == '1')
    {
        return true; // Returning true indicates to return to the main menu
    }
    else if (choice == '2')
    {
        closegraph();
        return false; // Returning false indicates to exit the program
    }
}