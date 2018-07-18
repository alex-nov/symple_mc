#include <ncurses.h>
#include <dirent.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define NAME_LEGHT 255

char directory[255] = "/";

long int count_files=0, current = 0;
char **file_names;
unsigned char *file_types;

    DIR *dir;    

int create_filelist()
{
    int i = 0;
    struct dirent *ent;
    
    while ((ent=readdir(dir)) != false) 
    {
            ++count_files;
    }
    rewinddir(dir);
    
    if(file_names != NULL)
    {
        clear_memory();        
    }
    
    file_names = (char**)malloc(sizeof(char*) * count_files);
    file_types = (unsigned char*)malloc(sizeof(unsigned char) * count_files);
    for(i=0; i< count_files; ++i)
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
    
    return i;
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


int main()
{
    
    int i = 0;
    bool prog_exit = false;
    

    struct dirent *ent;
    
    
    initscr();                  // Переход в curses-режим
    start_color();              // Включаем цветовой движок
    keypad(stdscr, true);
    noecho();
   
    init_pair (1, COLOR_WHITE, COLOR_BLUE);
    init_pair (2, COLOR_WHITE, COLOR_BLACK);
    
    dir = opendir(directory);
    
    count_files = create_filelist();
    
    do
    {
        clear();
   
        mvprintw(0, 0 ,"%d\n", current);
        for(i=0; i<count_files; ++i)
        {
            if(current == i)
                attron(COLOR_PAIR(1));
            else
                attron(COLOR_PAIR(2));
        
            if(file_types[i] == DT_DIR)
                attron(A_BOLD);
                
            mvprintw(i+1, 1 ,"%s\n", file_names[i]);
        
            attroff(A_BOLD);
        }
        refresh();                   // Вывод приветствия на настоящий экран
        
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
            case 'q':
                prog_exit = true;
                break;
                
            default:
                break;
                
        }
    } while(!prog_exit);
    
    clear_memory();
    
    closedir(dir);
    getch();                     // Ожидание нажатия какой-либо клавиши пользователем
    endwin();                    // Выход из curses-режима. Обязательная команда.
    
    
    return 0;
}