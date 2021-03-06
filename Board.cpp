/*******************************************************************************
 * Program name: Final Project - SaveThePie - Board class
 * Author: Felipe Groberio
 * Date: December 10th, 2019
 ******************************************************************************/
#include "Board.hpp"

Board::Board() {
    srand(time(NULL));
    head = nullptr;
    tail = nullptr;
}
/*******************************************************************************
 * Deletes all the nodes on a Board object 2D linked list
 ******************************************************************************/
Board::~Board() {
    Space *ptr = head;
    while (ptr->up) {
        ptr = ptr->up;
    }

    while (ptr) {
        Space *colPtr = ptr;
        ptr = ptr->down;
        while (colPtr) {
            Space *garbage = colPtr;
            colPtr = colPtr->right;
            delete garbage;
        }
    }
}
/*******************************************************************************
 * Creates doubly linked list to for a single row for a given number of
 * columns. The nodes are then connected with pointers to nodes on for left and
 * right positions.
 ******************************************************************************/
void Board::createRow(int col) {
    for (int i = 1; i <= col; i++) {
        if (i == 1) {
            tail = new Field;
            tail->up = head;
            if (head) {
                head->down = tail;
            }
            head = tail;
        } else if (i == col) {
            Space *newPtr = new Field;
            tail->right = newPtr;
            newPtr->left = tail;
            tail = newPtr;

        } else {
            Space *newPtr = new Field;
            tail->right = newPtr;
            newPtr->left = tail;
            tail = newPtr;
        }
    }
}
/*******************************************************************************
 * Connects two rows by linking each node with the up and down pointers
 ******************************************************************************/
void Board::linkRow() {
    /* the head position is always at the first node on the last row */
    Space *top = head->up;
    Space *bot = head;
    top->down = head;
    /* connects all nodes on that same row until hits nullspace */
    while (top->right) {
        top = top->right;
        bot = bot->right;
        top->down = bot;
        bot->up = top;
    }
}
/*******************************************************************************
 * generates a rectangular objject and attempts to place it on the board
 ******************************************************************************/
void Board::addObstacle(int row, int col) {
    bool badLocation = true;
    int attempts = 0;
    int x, y;
    while (badLocation) {
        y = (rand() % this->brow);
        x = (rand() % this->bcol);
        attempts++;
        for (int i = 0; i < row; i++) {
            for (int j = 0; j < col; j++) {
                if (dynamic_cast<Field *>(this->getSpace(y + i, x + j))) {
                    badLocation = false;
                } else {
                    badLocation = true;
                    i = 999;
                    j = 999;
                }
            }
        }
        if (attempts > 2) {
            return;
        }
    }
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < col; j++) {
            this->swapSpace(new Wall, y + i, x + j);
        }
    }
}
/*******************************************************************************
* createBoard creates a 2D linked list for a given number of rows and columns.
* It creates n rows with m columns and then connects the rows to create a board
* in which the game will be played. The third parameter is the number of pies,
* which increases with each new level played.
 ******************************************************************************/
void Board::createBoard(int row, int col, int pies) {
    this->brow = row;
    this->bcol = col;

    /*  creates and empty 2D linked list */
    this->createRow(col);
    for (int i = 1; i < row; i++) {
        this->createRow(col);
        this->linkRow();
    }

    /* replaces empty spaces around the board with Wall nodes
     * to create the boundaries of the board */
    for (int i = 1; i < col; i++) {
        this->swapSpace(new Wall, 1, i);
        this->swapSpace(new Wall, row - 1, i);
    }
    for (int i = 1; i < row; i++) {
        this->swapSpace(new Wall, i, 1);
        this->swapSpace(new Wall, i, col - 1);
    }

    /* places the player node at the middle of the board */
    this->swapSpace(new Player, row / 2, col / 2);
    playerPtr = this->getSpace(row / 2, col / 2);

    /* generates a random quantity of obstacles, creating
     * a unique board on every level played */
    int randQuant = (rand() % 3) + 1;
    for (int i = 0; i < randQuant; i++) {
        this->addObstacle(10, 20);
    }
    randQuant = (rand() % 10) + 13;
    for (int i = 0; i < randQuant; i++) {
        this->addObstacle(3, 16);
    }

    randQuant = (rand() % 8) + 12;
    for (int i = 0; i < randQuant; i++) {
        this->addObstacle(8, 6);
    }

    randQuant = (rand() % 7) + 20;
    for (int i = 0; i < randQuant; i++) {
        this->addObstacle(3, 6);
    }

    /* cleanWall() ensures that no obstacles are touching
     * the boundary walls, this is only a design feature */
    this->cleanWall();

    /* adds a quantity of Bombs to the level depending
     * on the size of the board */
    for (int i = 0; i < row * col / 300; i++) {
        this->addObject(new Bomb);
    }

    /* adds Enemies to the level depending on the size of
     * the board */
    for (int i = 0; i < row * col / 200; i++) {
        this->addEnemy();
    }

    /* ensures that the pieArray is empty */
    while (!this->pieArray.empty()) {
        this->pieArray.pop_back();
    }
    /* Adds pies */
    for (int i = 0; i < pies; i++) {
        this->addPie();
    }
}
/*******************************************************************************
 * This function returns a Space object for a given row and column on a 2D
 * linked list (board object)
 ******************************************************************************/
Space *Board::getSpace(int row, int col) {
    Space *ptr = head;
    while (ptr->up) {
        ptr = ptr->up;
    }
    for (int i = 1; i < row; i++) {
        ptr = ptr->down;
    }
    for (int i = 1; i < col; i++) {
        ptr = ptr->right;
    }
    return ptr;
}
/*******************************************************************************
 * changeSpace disconnects a node from one position and connects it a given
 * position. The function will disconnect the node from its current position
 * and add an empty space to it. Next, it will disconnect the node to its
 * future position and connect it to the four adjacent nodes.
 ******************************************************************************/
void Board::changeSpace(int fromY, int fromX, int toY, int toX) {
    Space *fromPtr = this->getSpace(fromY, fromX);
    Space *toPtr = this->getSpace(toY, toX);
    Space *tempPtr = 0;

    Space *newField = new Field;
    /* Disconnects the moving node and connects an empty space
     * to its old position */
    if (fromPtr->up) {
        tempPtr = fromPtr->up;
        tempPtr->down = newField;
        newField->up = tempPtr;
    }
    if (fromPtr->down) {
        tempPtr = fromPtr->down;
        tempPtr->up = newField;
        newField->down = tempPtr;
    }
    if (fromPtr->left) {
        tempPtr = fromPtr->left;
        tempPtr->right = newField;
        newField->left = tempPtr;
    }
    if (fromPtr->right) {
        tempPtr = fromPtr->right;
        tempPtr->left = newField;
        newField->right = tempPtr;
    }
    // Disconnects old node, and links new node to top node
    if (toPtr->up) {
        tempPtr = toPtr->up;
        tempPtr->down = fromPtr;
        fromPtr->up = tempPtr;
    }
    // Disconnects old node, and links new node to bottom node
    if (toPtr->down) {
        tempPtr = toPtr->down;
        tempPtr->up = fromPtr;
        fromPtr->down = tempPtr;
    }
    // Disconnects old node, and links new node to left node
    if (toPtr->left) {
        tempPtr = toPtr->left;
        tempPtr->right = fromPtr;
        fromPtr->left = tempPtr;
    }
    // Disconnects old node, and links new node to right node
    if (toPtr->right) {
        tempPtr = toPtr->right;
        tempPtr->left = fromPtr;
        fromPtr->right = tempPtr;
    }

    // delete old node
    delete toPtr;
}
/*******************************************************************************
 * swapSpace is different to changeSpace because only one position is involved.
 * This function will remove the current node at the given position and replace
 * it with the given Space object. The function will also take care of
 * disconnecting the old node and linking the new node to its neighbors.
 ******************************************************************************/
void Board::swapSpace(Space *ptrIn, int row, int col) {
    Space *remPtr = this->getSpace(row, col);
    Space *newPtr = ptrIn;
    Space *tempPtr = 0;

    // Disconnect old node, and link new node to top node
    if (remPtr->up) {
        tempPtr = remPtr->up;
        tempPtr->down = newPtr;
        newPtr->up = tempPtr;
    }
    // Disconnect old node, and link new node to bottom node
    if (remPtr->down) {
        tempPtr = remPtr->down;
        tempPtr->up = newPtr;
        newPtr->down = tempPtr;
    }
    // Disconnect old node, and link new node to left node
    if (remPtr->left) {
        tempPtr = remPtr->left;
        tempPtr->right = newPtr;
        newPtr->left = tempPtr;
    }
    // Disconnect old node, and link new node to right node
    if (remPtr->right) {
        tempPtr = remPtr->right;
        tempPtr->left = newPtr;
        newPtr->right = tempPtr;
    }

    // delete old node
    delete remPtr;
}
/*******************************************************************************
 * returns the Y-position (or row) of a given object stored on a Board
 ******************************************************************************/
int Board::getSpaceY(Space *ptrIn) {
    Space *ptr = ptrIn;
    int y = 0;
    while (ptr) {
        ptr = ptr->up;
        y++;
    }
    return y;
}
/*******************************************************************************
 * returns the X-position (or column) of a given object stored on a Board
 ******************************************************************************/
int Board::getSpaceX(Space *ptrIn) {
    Space *ptr = ptrIn;
    int x = 0;
    while (ptr) {
        ptr = ptr->left;
        x++;
    }
    return x;
}
/*******************************************************************************
 * replaces an empty space with the given Space object.  This is a helper
 * function to the swapSpace() funciton
 ******************************************************************************/
void Board::addObject(Space *ptrIn) {
    bool badLocation = true;
    int x, y;
    while (badLocation) {
        y = (rand() % (this->brow - 3)) + 2;
        x = (rand() % (this->bcol - 3)) + 2;
        if (dynamic_cast<Field *>(this->getSpace(y, x))) {
            this->swapSpace(ptrIn, y, x);
            badLocation = false;
        }
    }
}
/*******************************************************************************
 * ensures the boundary walls are not touched by any of the random structures
 ******************************************************************************/
void Board::cleanWall() {
    for (int i = 2; i < 4; i++) {
        for (int j = 2; j < bcol - 1; j++) {
            this->swapSpace(new Field, i, j);
        }
    }
    for (int i = brow - 3; i < brow - 1; i++) {
        for (int j = 2; j < bcol - 1; j++) {
            this->swapSpace(new Field, i, j);
        }
    }
    for (int i = 4; i < brow - 3; i++) {
        for (int j = 2; j < 5; j++) {
            this->swapSpace(new Field, i, j);
        }
    }
    for (int i = 4; i < brow - 3; i++) {
        for (int j = bcol - 4; j < bcol - 1; j++) {
            this->swapSpace(new Field, i, j);
        }
    }
}
/*******************************************************************************
 * manages Pie nodes and the array container that stores its nodes
 ******************************************************************************/
void Board::addPie() {

    bool badLocation = true;
    int x, y;
    while (badLocation) {
        y = (rand() % (this->brow - 3)) + 2;
        x = (rand() % (this->bcol - 3)) + 2;
        if (dynamic_cast<Field *>(this->getSpace(y, x))) {
            this->swapSpace(new Pie, y, x);
            this->pieArray.push_back(this->getSpace(y, x));
            badLocation = false;
        }
    }
}
/*******************************************************************************
 * manages Enemy nodes and the array container that stores its nodes
 ******************************************************************************/
void Board::addEnemy() {

    bool badLocation = true;
    int x, y;
    while (badLocation) {
        y = (rand() % (this->brow - 3)) + 2;
        x = (rand() % (this->bcol - 3)) + 2;
        if (dynamic_cast<Field *>(this->getSpace(y, x))) {
            this->swapSpace(new Enemy, y, x);
            this->enemyArray.push_back(this->getSpace(y, x));
            badLocation = false;
        }
    }
}
/*******************************************************************************
 * Removes a given Enemy object from the enemy array. This function is
 * performed when an enemy is killed by the player
 ******************************************************************************/
void Board::removeEnemy(Space *eIn) {

    for (int i = 0; i < this->enemyArray.size(); i++) {
        if (enemyArray[i] == eIn) {
            enemyArray.erase(enemyArray.begin() + i);
            this->swapSpace(new Fire, this->getSpaceY(eIn),
                            this->getSpaceX(eIn));
            return;
        }
    }
}
/*******************************************************************************
 * popBomb() detects if the player is in close proximity of a Bomb object. If
 * a bomb object is found, it detonates it. The activation of a bomb is
 * handdled by bombExplosion()
 ******************************************************************************/
void Board::popBomb() {
    int pY = this->getSpaceY(this->playerPtr);
    int pX = this->getSpaceX(this->playerPtr);

    if (dynamic_cast<Bomb *>(this->getSpace(pY, pX - 1))) {
        this->bombExplosion(this->getSpace(pY, pX - 1));

    } else if (dynamic_cast<Bomb *>(this->getSpace(pY - 1, pX - 1))) {
        this->bombExplosion(this->getSpace(pY - 1, pX - 1));

    } else if (dynamic_cast<Bomb *>(this->getSpace(pY - 1, pX))) {
        this->bombExplosion(this->getSpace(pY - 1, pX));

    } else if (dynamic_cast<Bomb *>(this->getSpace(pY - 1, pX + 1))) {
        this->bombExplosion(this->getSpace(pY - 1, pX + 1));

    } else if (dynamic_cast<Bomb *>(this->getSpace(pY, pX + 1))) {
        this->bombExplosion(this->getSpace(pY, pX + 1));

    } else if (dynamic_cast<Bomb *>(this->getSpace(pY + 1, pX + 1))) {
        this->bombExplosion(this->getSpace(pY + 1, pX + 1));

    } else if (dynamic_cast<Bomb *>(this->getSpace(pY + 1, pX))) {
        this->bombExplosion(this->getSpace(pY + 1, pX));

    } else if (dynamic_cast<Bomb *>(this->getSpace(pY + 1, pX - 1))) {
        this->bombExplosion(this->getSpace(pY + 1, pX - 1));
    }
}
/*******************************************************************************
 * replaces Field and Enemy objects with Fire objects, effectively creating an
 * exlosion animation and removing the enemies around the player
 ******************************************************************************/
void Board::bombExplosion(Space *bombIn) {
    int bY = this->getSpaceY(bombIn);
    int bX = this->getSpaceX(bombIn);
    this->swapSpace(new Field, bY, bX);
    bY = bY - 3;
    bX = bX - 6;
    for (int i = bY; i < bY + 7; i++) {
        for (int j = bX; j < bX + 13; j++) {
            if (i > 1 && i < this->brow && j > 1 && j < this->bcol) {
                if (dynamic_cast<Field *>(this->getSpace(i, j))) {
                    this->swapSpace(new Fire, i, j);
                } else if (dynamic_cast<Enemy *>(this->getSpace(i, j))) {
                    this->removeEnemy(this->getSpace(i, j));
                }
            }
        }
    }
}
/*******************************************************************************
 * this function is used to check if a player has collected a pie.  It also
 * manages the pie array
 ******************************************************************************/
void Board::collectPie(int nextY, int nextX) {

    if (dynamic_cast<Pie *>(this->getSpace(nextY, nextX))) {
        this->swapSpace(new Field, nextY, nextX);
        this->pieArray.pop_back();
    }
}
