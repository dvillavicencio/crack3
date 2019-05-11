#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "md5.h"

const int PASS_LEN=50;        // Maximum any password can be
const int HASH_LEN=33;        // Length of MD5 hash strings
const int DYNAMIC_SIZE=100;

char * contents;


// Stucture to hold both a plaintext password and a hash.
struct entry 
{
    char *password;
    char *hash;
};

int file_length(char *filename)
{
    struct stat info;
    int res = stat(filename, &info);
    if (res == -1) return -1;
    else return info.st_size;
}

int hashcompare(const void *a, const void *b)
{
    struct entry *ca = (struct entry *)a;
    struct entry *cb = (struct entry *)b;
    return strcmp(ca->hash, cb->hash);
}

int stringcompare( const void *target, const void *element)
{
    char * target_str = (char *)target;
    struct entry * celemen = (struct entry *)element;
    return strcmp(target_str, celemen->hash);
}

// TODO
// Read in the dictionary file and return an array of structs.
// Each entry should contain both the hash and the dictionary
// word.
struct entry *read_dictionary(char *filename, int *size)
{
    int filelength = file_length(filename);
    FILE * f = fopen(filename, "r");
    if(!f)
    {
        printf("Couldn't open file %s for reading\n", filename);
        exit(1);
    }
    
    contents = malloc(filelength);
    fread(contents, 1, filelength, f);
    fclose(f);
    // Loop through contents, replace '\n' with '\0'
    int lines = 0;
    for(int i = 0; i < filelength; i++)
    {
        if(contents[i] == '\n')
        {
            contents[i] = '\0';
            lines++;
        }
    }
    
    struct entry * main_entry =  malloc(lines * sizeof(struct entry));
    
    char *current_entry = &contents[0];
    int count = 0;
    for(int i = 0; i < filelength; i++)
    {
        if(contents[i] == '\0')
        {
            // char *next_entry = &contents[i] + 1;
            char *hashed_entry = md5(current_entry, strlen(current_entry));
            main_entry[count].password = current_entry;
            main_entry[count].hash = hashed_entry;
            current_entry=&contents[i] + 1;
            count++;
        }
    }
    
    *size = lines;
    return main_entry;
    
}


int main(int argc, char *argv[])
{
    if (argc < 3) 
    {
        printf("Usage: %s hash_file dict_file\n", argv[0]);
        exit(1);
    }

    // TODO: Read the dictionary file into an array of entry structures
    int dictsize;
    struct entry *dict = read_dictionary(argv[2], &dictsize);
    // TODO
    // Open the hash file for reading.
    FILE * hashes = fopen(argv[1], "r");
    if(!hashes)
    {
        printf("The file '%s' could not be opened\n", argv[1]);
        exit(1);
    }
    // char * first_element = dict; 
    qsort(dict, dictsize, sizeof(struct entry), hashcompare);
    
    char hash[HASH_LEN];
    while(fgets(hash, HASH_LEN, hashes) != NULL)
    {
        struct entry * found = bsearch(hash, dict, dictsize, sizeof(struct entry), stringcompare);
        if(found == NULL)
        {
            printf("");
        }
        else
        {
            printf("Found a match: %s %s\n", found->password, found->hash);
        }
    }
    
    for(int i = 0; i < dictsize; i++)
    {
        free(dict[i].hash);
    }
    free(dict);
    free(contents);
    
    fclose(hashes);
}
