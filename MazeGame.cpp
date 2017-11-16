/*
 Created by Saidolda Bayan.
 Copyright (c) 2015 Bayan. All rights reserved.
 LANG: C++
 */
#include <iostream>
#include <bitset>
#include <fstream>
#include <algorithm>
#include <cmath>
#include <string>
#include <set>
#include <iterator>
#include <map>
#include <cstring>
#include <ctime>
#include <cstdlib>
#include <cstdio>
#include <ctime>
#include <vector>
#include <cassert>
#include <queue>
#include <deque>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>

using namespace std;

unsigned short level = 1, enemiesCount;
unsigned int seed;
bool end_of_level = 0;
ofstream scoreout;
//color codes
const string BOLDBLACK = "\033[1m\033[30m", BOLDGREEN = "\033[1m\033[32m", BOLDRED = "\033[1m\033[31m";
vector<pair<string, int>> highscore;
//                    kbhit() function
//  reference: http://stackoverflow.com/questions/18684332/capturing-keystrokes-in-linux
char kbhit(void)
{
    struct termios oldt, newt;
    int ch, oldf;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);
    ch = getchar();
    if(ch != EOF)
    {
        return ch;
    }
    
    return 0;
}
//-------------------------------------------------------
class Board{
public:
    int width, height;
    char area[80][210];
    int color[80][210], sz = 0;
    vector<pair<int, int>> cells;
    
    Board(){}
    //show our maze in console
    void out(){
        //"\e[x;yH" to change cursor position at coord [x;y]
        printf("\e[1;1H");
        for(int i=1; i <= height; i++){
            for(int j=1; j <= width; j++){
                cout.flush();
                if(area[i][j] == '*') cout << BOLDGREEN;
                cout << area[i][j];
                if(area[i][j] == '*')cout << BOLDBLACK;
            }
            cout << endl;
        }
    }
    //generate our maze using recursion
    void simple_generator(int x1, int y1, int x2, int y2){
        if(min(x2 - x1, y2 - y1) < 0) return;
        int x3 = 0, y3 = 0;
        if(x2 - x1 > 3){
            x3 = rand() % (x2-x1-1) + x1 + 1;
            for(int i=y1; i<=y2; i++) area[x3][i] = '#';
        }
        if(y2 - y1 > 3){
            y3 = rand() % (y2 - y1 - 1) + y1 + 1;
            for(int i=x1; i<=x2; i++) area[i][y3] = '#';
        }
        if(x3 && y3){
            simple_generator(x1, y1, x3-1, y3-1);
            simple_generator(x1, y3+1, x3-1, y2);
            simple_generator(x3+1, y1, x2, y3-1);
            simple_generator(x3+1, y3+1, x2, y2);
        }
        else if(x3){
            simple_generator(x1, y1, x3-1, y2);
            simple_generator(x3+1, y1, x2, y2);
        }
        else if(y3){
            simple_generator(x1, y1, x2, y3-1);
            simple_generator(x1, y3+1, x2, y2);
        }
    }
    //find Connected components in graph by dfs
    void dfs_color(int x, int y){
        if(area[x][y] != ' ' || color[x][y])return;
        color[x][y] = sz;
        dfs_color(x+1, y);
        dfs_color(x, y+1);
        dfs_color(x-1, y);
        dfs_color(x, y-1);
    }
    //algorithm for disjoint-set-union
    int dsu[1000];
    int find_dsu(const int x){
        if (dsu[x] == x) return x;
        return dsu[x] = find_dsu(dsu[x]);
    }
    void add_dsu(int x, int y){
        x = find_dsu(x);
        y = find_dsu(y);
        dsu[y]=x;
    }
    
    void create1(){
        sz = 0;
        memset(color, 0, sizeof color);
        width = rand()%50 + 70;
        height = rand()%13 + 36;
        width = 1. * width / 5 * level;
        height = 1. * height / 5 * level;
        for(int i=1; i<=height; i++)
            for(int j=1; j<=width; j++)
                area[i][j] = ' ';
        //create boards edges
        for(int i=1; i<=height; i++) area[i][1] = area[i][width] = '#';
        for(int i=1; i<=width; i++) area[1][i] = area[height][i] = '#';
        simple_generator(2, 2, height - 1, width - 1);
        for(int i=2; i<height; i++){
            for(int j=2; j<width; j++){
                if(area[i][j] == ' '){
                    //if this cell isn't used in connected components found before,
                    //then we add new connected component that contain this cell
                    if(!color[i][j]){
                        ++sz;
                        dsu[sz] = sz;
                        dfs_color(i, j);
                    }
                }
                else{
                    cells.push_back(make_pair(i, j));
                }
            }
        }
        random_shuffle(cells.begin(), cells.end());
        //randomly delete some walls cell to guarantee the path between empty cells
        for(auto cell : cells){
            //check if this wall is located between empty cells, then check by dsu if there are
            //from different connected components, if this is true, destroy this wall and connect connected components
            //vertically
            if(color[cell.first-1][cell.second] && color[cell.first+1][cell.second]){
                if(find_dsu(color[cell.first-1][cell.second]) !=
                   find_dsu(color[cell.first+1][cell.second])
                   ){
                    add_dsu(color[cell.first+1][cell.second], color[cell.first-1][cell.second]);
                    area[cell.first][cell.second] = ' ';
                    color[cell.first][cell.second] = find_dsu(color[cell.first+1][cell.second]);
                }
            }
            //horyzontally
            if(color[cell.first][cell.second-1] && color[cell.first][cell.second+1]){
                if(find_dsu(color[cell.first][cell.second-1]) !=
                   find_dsu(color[cell.first][cell.second+1])
                   ){
                    add_dsu(color[cell.first][cell.second+1], color[cell.first][cell.second-1]);
                    area[cell.first][cell.second] = ' ';
                    color[cell.first][cell.second] = find_dsu(color[cell.first][cell.second+1]);
                }
            }
        }
        cells.clear();
        for(int i=width-1; i ;i--)
            if(area[height-1][i] == ' '){
                area[height-1][i] = '*';
                break;
            }
    }
}maze[15];
class Player{
public:
    int x_coord, y_coord;
    bool isHero;
    string image;
    Player(int x = 1, int y = 1, string s = "\033[33m☺\033[1m\033[30m", bool b = 1): x_coord(x), y_coord(y), image(s), isHero(b){}
    void out(){
        printf("\e[%d;%dH%s",x_coord, y_coord, image.c_str());
    }
    void pressed(char key){
        int x_new = x_coord, y_new = y_coord;
        //65 is up key
        if(key == 65)
            x_new--;
        //66 is down key
        if(key == 66)
            x_new++;
        //68 is left key
        if(key == 68)
            y_new--;
        //67 is right key
        if(key == 67)
            y_new++;
        //check if we can move that way
        if(maze[level].area[x_new][y_new] == ' '){
            x_coord = x_new;
            y_coord = y_new;
        }
        //check if we have finished this level
        if(maze[level].area[x_new][y_new] == '*' && isHero){
            end_of_level = 1;
        }
    }
    void clear(){
        printf("\e[%d;%dH ",x_coord, y_coord);
    }
}hero, enemies[100];

void menu(){
    system("clear");
    while(kbhit());
    int choosed = 2;
    printf("                  Main menu\n");
    printf("-> Start game\n"); // when choosed == 2
    printf("   Highest scores\n");  //when choosed == 3
    printf("   Change maze generator key (will reset high scores)\n"); // when choosed == 4
    printf("   Exit game"); //when choosed == 5
    while(1){
        int key;
        if(key = kbhit()){
            //66 is down key
            if(key == 66){
                printf("\e[%d;1H  ", choosed);
                choosed++;
                if(choosed > 5) choosed = 2;
                printf("\e[%d;1H->", choosed);
            }
            //65 is up key
            if(key == 65){
                printf("\e[%d;1H  ", choosed);
                choosed--;
                if(choosed < 2) choosed = 5;
                printf("\e[%d;1H->", choosed);
            }
            //10 is enter key, 67 is right key
            if(key == 67 || key == 10){
                switch (choosed) {
                    case 2: //start game
                        return;
                        break;
                    case 3: //high score
                        system("clear");
                        cout << "\n TOP 10 players\n";
                        for(int i=0; i<min(10, (int)highscore.size()); i++){
                            cout <<BOLDGREEN<<highscore[i].first<<BOLDBLACK << " has finished game in "<<BOLDRED<<highscore[i].second<<BOLDBLACK<<" seconds.\n";
                        }
                        cout << "\n\n\n Press any key to leave";
                        while(kbhit());         //to clear stdin
                        while(kbhit() == 0);    //continue only when key will be pressed
                        menu();
                        return;
                        break;
                    case 4: // change generator seed
                        system("clear");
                        printf("Generator key value is %d\n", seed);
                        printf("Write new Generator key value: ");
                        int pressed, value;
                        while(kbhit());
                        value = 0;
                        while((pressed = kbhit()) != 10){
                            switch (pressed) {
                                case '0':
                                    value = value * 10;
                                    printf("0");
                                    break;
                                case '1':
                                    value = value * 10 + 1;
                                    printf("1");
                                    break;
                                case '2':
                                    value = value * 10 + 2;
                                    printf("2");
                                    break;
                                case '3':
                                    value = value * 10 + 3;
                                    printf("3");
                                    break;
                                case '4':
                                    value = value * 10 + 4;
                                    printf("4");
                                    break;
                                case '5':
                                    value = value * 10 + 5;
                                    printf("5");
                                    break;
                                case '6':
                                    value = value * 10 + 6;
                                    printf("6");
                                    break;
                                case '7':
                                    value = value * 10 + 7;
                                    printf("7");
                                    break;
                                case '8':
                                    value = value * 10 + 8;
                                    printf("8");
                                    break;
                                case '9':
                                    value = value * 10 + 9;
                                    printf("9");
                                    break;
                                default:
                                    break;
                            }
                        }
                        //check for validity
                        if(value >= 0 && value <= 1000000000){
                            seed = value;
                            scoreout.open("highscore.txt");
                            scoreout << seed;
                            highscore.clear();
                            scoreout.close();
                        }
                        else{
                            cout<<"\n  Value that you entered is not valid";
                        }
                        menu();
                        return;
                        break;
                    case 5:
                        exit(0);
                        break;
                    default:
                        break;
                }
            }
        }
    }
    system("clear");
}
int main ()
{
    system("tput civis");  //hide cursor
    cout<<BOLDBLACK;
    ifstream scorein("highscore.txt");
    string name;
    int hisscore;
    //read previously setted seed
    if(!(scorein >> seed)){
        seed = time(0);
    }
    //read details from highscore list
    while(scorein>>name && scorein >> hisscore){
        highscore.push_back({name, hisscore});
    }
    scorein.close();
    cout<<"Welcome!!!\n\nWrite your name: ";
    cout<<BOLDGREEN;
    cin >> name;
    cout<<BOLDBLACK;
    menu();
    double started_time = clock() / CLOCKS_PER_SEC;
    for(level = 1; level <= 5; level++){
        srand(seed);
        system("clear");
        hero = Player(2, 2);
        maze[level].create1();
        maze[level].out();
        enemiesCount = ceil(1. * maze[level].width / 8 * maze[level].height / 20);
        vector<pair<int, int>> empty;
        for(int i=2; i <= maze[level].height; i++){
            for(int j=2; j <= maze[level].width; j++){
                if(maze[level].area[i][j] == ' '){
                    if(i == 2 && j == 2)continue;
                    empty.push_back(make_pair(i, j));
                }
            }
        }
        random_shuffle(empty.begin(), empty.end());
        for(int i=1; i<=enemiesCount; i++){
            //create enemies in random empty cells
            enemies[i] = Player(empty[i-1].first, empty[i-1].second, "\033[1m\033[31mo\033[1m\033[30m", 0);
            enemies[i].out();
        }
        end_of_level = 0;
        hero.out();
        printf("\e[%d;1HLevel: %d", maze[level].height + 2, level);
        double time = 1. * clock() / CLOCKS_PER_SEC;
        while(!end_of_level){
            int pressedKey;
            if(pressedKey = kbhit()){
                //cheat key to skip level :)
                if(pressedKey == '='){
                    end_of_level = 1;
                }
                hero.clear();
                hero.pressed(pressedKey);
                hero.out();
                for(int i = 1; i<= enemiesCount; i++){
                    //check if we have same position with enemy
                    if(hero.x_coord == enemies[i].x_coord && hero.y_coord == enemies[i].y_coord){
                        hero.x_coord = 2;
                        hero.y_coord = 2;
                        enemies[i].out();
                        hero.out();
                    }
                }
            }
            //enemies move every 0.42 seconds
            if(1. * clock() / CLOCKS_PER_SEC > time +  0.42){
                for(int i=1; i<= enemiesCount; i++){
                    int temp[4] = {65, 67, 66, 68};
                    random_shuffle(temp, temp+4);
                    //randomly choose valid one direction from 4 directions
                    for(int j=0; j<4; j++){
                        int x = enemies[i].x_coord, y = enemies[i].y_coord;
                        cout.flush();
                        enemies[i].clear();
                        enemies[i].pressed(temp[j]);
                        enemies[i].out();
                        //if this direction is valid
                        if(x != enemies[i].x_coord || y != enemies[i].y_coord){
                            //if enemy hit the player
                            if(hero.x_coord == enemies[i].x_coord && hero.y_coord == enemies[i].y_coord){
                                hero.x_coord = 2;
                                hero.y_coord = 2;
                                enemies[i].out();
                                hero.out();
                            }
                            break;
                        }
                    }
                }
                time = 1. * clock() / CLOCKS_PER_SEC;
            }
        }
    }
    system("clear");
    //calculate score
    int score = clock() / CLOCKS_PER_SEC - started_time;
    cout<<BOLDRED<<"Congratulations!!! You have finished you game in "<< score <<" seconds\n\n";
    highscore.push_back({name, score});
    //sort highscore
    sort(highscore.begin(), highscore.end(), [](pair<string, int> a, pair<string, int> b){return a.second < b.second;});
    scoreout.open("highscore.txt");
    scoreout << seed << endl;
    for(int i=0; i<min(10, (int)highscore.size()); i++){
        scoreout << highscore[i].first <<" "<< highscore[i].second << "\n";
    }
    cout<<"\n\n\n\n"<<BOLDBLACK;
    return 0;
}
