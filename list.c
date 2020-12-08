/*
 * C Program to List Files in Directory
 */
#include <dirent.h>
#include <stdio.h>
#include "listFiles.h"

#define BUFFSIZE 100

struct LinkedList
{
    char buffer[BUFFSIZE];
    struct LinkedList *next;
};

char **listFiles(int *len)
{   
    int length=0;
    struct LinkedList *head = NULL;
    head = (struct LinkedList*)malloc(sizeof(struct LinkedList)); 
    
    DIR *d;
    struct dirent *dir;
    d = opendir(".");
    if (d)
    {
        while((dir = readdir(d)) != NULL)
        {
            if(dir->d_type==8)
            {
                if(strstr(dir->d_name, ".mp4") || strstr(dir->d_name, ".mp3") || strstr(dir->d_name, ".wav"))
                {
                    struct LinkedList *current_node = head;
                    struct LinkedList *new_node;
                    while ( current_node != NULL && current_node->next != NULL) {
                        current_node = current_node->next;
                    }
                    new_node = (struct LinkedList *) malloc(sizeof(struct LinkedList));
                    strcpy(new_node->buffer, dir->d_name);
                    new_node->next= NULL;
                    if (current_node != NULL)
                        current_node->next = new_node;
                    else
                        head = new_node;
                }
            }
        }
        closedir(d);
    }
    struct LinkedList *tmp = head;
    while(tmp->next != NULL)
    {
        tmp = tmp->next;
        length++;
    }
    
    char **arr;
    int i=0;

    arr = malloc(sizeof(int*)*length);
    for(int j=0; j<length; j++)
    {
        arr[j] = malloc(sizeof(char*) * BUFFSIZE);
    }

    while(head->next != NULL)
    {
        head = head->next;
        strcpy(arr[i++], head->buffer);
    }
    *len = length;
    return arr;
}