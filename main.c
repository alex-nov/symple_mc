#include <ncurses.h>
#include <dirent.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define NAME_LEGHT 255

typedef struct _win_border_struct 
{
	chtype 	ls, rs, ts, bs, 
	 	tl, tr, bl, br;
}WIN_BORDER;

typedef struct _WIN_struct 
{

	int startx, starty;
	int height, width;
	WIN_BORDER border;
}WIN;

char directory[NAME_LEGHT] = "/";

long int count_files=0, current = 0;
char **file_names;
unsigned char *file_types;

    DIR *dir;    

int create_filelist()
{
    int i=0, count = 0;
    
    struct dirent *ent;
    
    if(file_names != NULL)
    {
        clear_memory();        
    }
    
    while ((ent=readdir(dir)) != false) 
    {
            ++count;
    }
    rewinddir(dir);    
    
    
    file_names = (char**)malloc(sizeof(char*) * count);
    file_types = (unsigned char*)malloc(sizeof(unsigned char) * count);
    for(i=0; i< count; ++i)
    {
        file_names[i] = (char*)malloc(sizeof(char) * NAME_LEGHT);
    }
    
    i=0;
    while ((ent=readdir(dir)) != false) 
    {
            strcpy(file_names[i], ent->d_name);
            file_types[i] = ent->d_type; 

            ++i;
    }
    rewinddir(dir);
    
    return count;
}

void clear_memory()
{
    int i;
    
    for(i=0; i<count_files; ++i)
    {
        free(file_names[i]);
    }
    free(file_names);
    free(file_types);  
}


void init_win_params(WIN *p_win, int row, int col, int x, int y)
{
//     printw("")
    p_win->height = row - 3;
    p_win->width = col/2 - 5;
    p_win->starty = y;	
    p_win->startx = x;

    p_win->border.ls = '|';
    p_win->border.rs = '|';
    p_win->border.ts = '-';
    p_win->border.bs = '-';
    p_win->border.tl = '+';
    p_win->border.tr = '+';
    p_win->border.bl = '+';
    p_win->border.br = '+';
}


void create_box(WIN *p_win, bool flag)
{
    int i, j;
    int x, y, w, h;

    x = p_win->startx;
    y = p_win->starty;
    w = p_win->width;
    h = p_win->height;

    if(flag == TRUE)
    {
        attron(COLOR_PAIR(3));
        mvaddch(y, x, p_win->border.tl);
	mvaddch(y, x + w, p_win->border.tr);
	mvaddch(y + h, x, p_win->border.bl);
	mvaddch(y + h, x + w, p_win->border.br);
	mvhline(y, x + 1, p_win->border.ts, w - 1);
	mvhline(y + h, x + 1, p_win->border.bs, w - 1);
	mvvline(y + 1, x, p_win->border.ls, h - 1);
	mvvline(y + 1, x + w, p_win->border.rs, h - 1);
        attroff(COLOR_PAIR(3));
    }
    else
        for(j = y; j <= y + h; ++j)
            for(i = x; i <= x + w; ++i)
                mvaddch(j, i, ' ');

    refresh();

}


void print_win_params(WIN *p_win)
{
// #ifdef _DEBUG
	mvprintw(1, 1, "%d %d %d %d", p_win->startx, p_win->starty, 
				p_win->width, p_win->height);
	refresh();
// #endif
}

void show_right_block(char *new_f, int y)
{
    DIR *tmp_dir;
    struct dirent *ent;
    int i =0;
    
//     printw("%s\n",new_f);
//     getch();
    
    tmp_dir = opendir(new_f);          // Открываем новый каталог
    if(!tmp_dir)
    {
        return;
    }
    
    attron(COLOR_PAIR(2)); 
    
    while ((ent=readdir(tmp_dir)) != false) 
    {
        mvprintw(i+2, y  ,"%s\n", ent->d_name);
        ++i;
        
    }
    
    attroff(COLOR_PAIR(2));
    closedir(tmp_dir);
}


int main()
{
    
    int i = 0, slash=0;
    int row, col;
    bool prog_exit = false;
    char new_folder[NAME_LEGHT];
    
    WIN win1,win2;
    

    struct dirent *ent;
    
    
    initscr();                  // Переход в curses-режим
    start_color();              // Включаем цветовой движок
    cbreak();
    
    keypad(stdscr, true);
    noecho();
    
    getmaxyx(stdscr, row, col);
    
    init_win_params(&win1, row, col, 1 , 1);
    init_win_params(&win2, row, col, row /2 , col/2 );

//     printw("%d - %d\n",col, row);
//     print_win_params(&win1);
//     getch(); 
//     print_win_params(&win2);
//     getch();    
   
    init_pair (1, COLOR_WHITE, COLOR_BLUE);
    init_pair (2, COLOR_WHITE, COLOR_BLACK);
    init_pair (3, COLOR_BLUE, COLOR_BLACK);
    dir = opendir(directory);
    
    count_files = create_filelist();
    
    do
    {
        clear();
   
        mvprintw(0, 0 ,"%d  %s\n", current,directory);       
        
        for(i=0; i<count_files; ++i)
        {
            if(current == i)
                attron(COLOR_PAIR(1));
            else
                attron(COLOR_PAIR(2));
        
            if(file_types[i] == DT_DIR)
                attron(A_BOLD);
                
            mvprintw(i+2, 2  ,"%s\n", file_names[i]);
        
            attroff(COLOR_PAIR(1));
            attroff(COLOR_PAIR(2));
            attroff(A_BOLD);
        }
        
        if(file_types[current] == DT_DIR )
        {
            memset(new_folder, 0, NAME_LEGHT );
            if(strcmp(file_names[current],"..") == 0 ) // Если это .. - сокращаем строку директории
            {
                for(slash=0, i=0; i<strlen(directory); ++i)
                {
                    if(directory[i] == '/') slash = i;
                }
                if(slash == 0) ++slash;
                        
                strncpy(new_folder, directory, slash);
                }
                else // Иначе добавляем к текущему пути выбранную папку
                {
                    strcat(new_folder, directory);
                    if(strcmp(directory, "/") ) strcat(new_folder, "/");
                    strcat(new_folder, file_names[current]);
                }
                
                show_right_block(new_folder, col/2 + 2);
        }
        
        create_box(&win1, TRUE);
        create_box(&win2, TRUE);
        
        refresh();                   // Вывод списка текущего каталога на настоящий экран
                
        switch ( getch() )
        {
            case KEY_UP:
                
                if ( current ) //Если возможно, переводим указатель вверх
                    --current; 
                break;
                
            case KEY_DOWN:
                
                if ( current < count_files-1 ) //Если возможно, переводим указатель вниз
                    ++current;
                break;
                
            case KEY_RIGHT:
                
                // Если тип - директория и это не . - переходим в нее
                if(file_types[current] == DT_DIR && strcmp(file_names[current],".") != 0 )
                {
                    closedir(dir);
                    dir = opendir(new_folder);          // Открываем новый каталог
                    if(!dir) // Если не можем войти в директорию, остаемся в текущем
                    {
                        dir = opendir(directory);
                    }
                    else
                    {
                        count_files = create_filelist();    // Заново создаем список файлов
                        current = 0;
                    
                        memset(directory, 0, NAME_LEGHT );
                        strcpy(directory, new_folder);
                    }
                }
                break;
                
            case 'q':
                prog_exit = true;
                break;
                
            default:
                break;
                
        }
    } while(!prog_exit);
    
    clear_memory();
    
    closedir(dir);               // Закрываем последний используемый каталог
    endwin();                    // Выход из curses-режима. Обязательная команда.
    
    
    return 0;
}