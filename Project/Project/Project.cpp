#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#pragma warning(disable:4996)

#define HASH_TABLE_SIZE 127
#define MAX_COUNTRY_NAME 21

typedef struct Parcel
{
    char country[MAX_COUNTRY_NAME];
    int weight;
    float valuation;
    struct Parcel* left, * right;
} Parcel;

typedef struct HashTable
{
    Parcel* root;
} HashTable;

unsigned int hash(const char* str)
{
    unsigned long hash = 5381;
    int c;
    while ((c = *str++))
        hash = ((hash << 5) + hash) + c; // hash * 33 + c
    return hash % HASH_TABLE_SIZE;
}

void toLowerCase(char* str)
{
    for (int i = 0; str[i]; i++)
    {
        str[i] = tolower(str[i]);
    }
}

Parcel* createParcel(const char* country, int weight, float valuation)
{
    Parcel* newParcel = (Parcel*)malloc(sizeof(Parcel));
    if (newParcel == NULL)
    {
        printf("Failed to allocate memory for new parcel");
        exit(EXIT_FAILURE);
    }
    strncpy(newParcel->country, country, MAX_COUNTRY_NAME - 1);
    newParcel->country[MAX_COUNTRY_NAME - 1] = '\0';
    toLowerCase(newParcel->country);  // Normalize to lowercase
    newParcel->weight = weight;
    newParcel->valuation = valuation;
    newParcel->left = newParcel->right = NULL;
    return newParcel;
}

Parcel* insertParcel(Parcel* root, const char* country, int weight, float valuation)
{
    if (root == NULL)
    {
        return createParcel(country, weight, valuation);
    }
    if (weight < root->weight)
    {
        root->left = insertParcel(root->left, country, weight, valuation);
    }
    else if (weight > root->weight)
    {
        root->right = insertParcel(root->right, country, weight, valuation);
    }
    return root;
}

void loadParcels(HashTable* table, const char* filename)
{
    FILE* file = fopen(filename, "r");
    if (file == NULL)
    {
        printf("Failed to open file");
        exit(EXIT_FAILURE);
    }

    char line[256];
    char country[MAX_COUNTRY_NAME];
    int weight = 0;
    float valuation = 0.0f;

    while (fgets(line, sizeof(line), file))
    {
        line[strcspn(line, "\n")] = '\0';

        char* token = strtok(line, ",");
        if (token != NULL)
        {
            strncpy(country, token, MAX_COUNTRY_NAME - 1);
            country[MAX_COUNTRY_NAME - 1] = '\0';
        }

        token = strtok(NULL, ",");
        if (token != NULL)
        {
            weight = atoi(token);
        }

        token = strtok(NULL, ",");
        if (token != NULL)
        {
            valuation = (float)atof(token);
        }

        toLowerCase(country);
        unsigned int index = hash(country);
        table[index].root = insertParcel(table[index].root, country, weight, valuation);
    }

    if (fclose(file) != 0)
    {
        printf("Failed to close the file");
        exit(EXIT_FAILURE);
    }
}

void inorderTraversal(Parcel* root, const char* country, void (*func)(Parcel*))
{
    if (root == NULL) return;
    inorderTraversal(root->left, country, func);
    if (strcmp(root->country, country) == 0)  // Check country match
        func(root);
    inorderTraversal(root->right, country, func);
}

void printParcel(Parcel* parcel)
{
    printf("Country: %s, Weight: %d, Valuation: %.2f\n", parcel->country, parcel->weight, parcel->valuation);
}

void displayParcels(HashTable* table, const char* country)
{
    char lowerCountry[MAX_COUNTRY_NAME];
    strncpy(lowerCountry, country, MAX_COUNTRY_NAME - 1);
    lowerCountry[MAX_COUNTRY_NAME - 1] = '\0';
    toLowerCase(lowerCountry);  // Normalize to lowercase

    unsigned int index = hash(lowerCountry);
    if (table[index].root)
    {
        inorderTraversal(table[index].root, lowerCountry, printParcel);
    }
    else
    {
        printf("No parcels found for %s\n", country);
    }
}