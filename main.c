#include <ncurses.h>
#include <dirent.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define NAME_LEGHT 255

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


int main()
{
    
    int i = 0, slash=0;
    bool prog_exit = false;
    char new_folder[NAME_LEGHT]; 
    

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
   
        mvprintw(0, 0 ,"%d  %s\n", current,directory);
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