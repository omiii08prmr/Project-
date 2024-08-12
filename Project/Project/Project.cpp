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
    if (strcmp(root->country, country) == 0)
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
    toLowerCase(lowerCountry);

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

void displayParcelsByWeight(Parcel* root, const char* country, int weight, int higher, int* found)
{
    if (root == NULL) return;

    displayParcelsByWeight(root->left, country, weight, higher, found);

    if (strcmp(root->country, country) == 0 &&
        ((higher && root->weight > weight) || (!higher && root->weight < weight)))
    {
        printParcel(root);
        *found = 1;
    }

    displayParcelsByWeight(root->right, country, weight, higher, found);
}

void displayParcelsByWeightWrapper(Parcel* root, const char* country, int weight, int higher)
{
    int found = 0;
    displayParcelsByWeight(root, country, weight, higher, &found);

    if (!found)
    {
        printf("There are no parcels with a %s weight than %d grams.\n", higher ? "higher" : "lower", weight);
    }
}

void calculateLoadAndValuation(Parcel* root, const char* country, int* totalLoad, float* totalValuation)
{
    if (root == NULL) return;
    if (strcmp(root->country, country) == 0)
    {
        *totalLoad += root->weight;
        *totalValuation += root->valuation;
    }
    calculateLoadAndValuation(root->left, country, totalLoad, totalValuation);
    calculateLoadAndValuation(root->right, country, totalLoad, totalValuation);
}

void displayTotalLoadAndValuation(HashTable* table, const char* country)
{
    char lowerCountry[MAX_COUNTRY_NAME];
    strncpy(lowerCountry, country, MAX_COUNTRY_NAME - 1);
    lowerCountry[MAX_COUNTRY_NAME - 1] = '\0';
    toLowerCase(lowerCountry);  

    unsigned int index = hash(lowerCountry);
    if (table[index].root)
    {
        int totalLoad = 0;
        float totalValuation = 0;
        calculateLoadAndValuation(table[index].root, lowerCountry, &totalLoad, &totalValuation);
        printf("Total load: %d grams, Total valuation: %.2f\n", totalLoad, totalValuation);
    }
    else
    {
        printf("No parcels found for %s\n", country);
    }
}

void findCheapestAndMostExpensive(Parcel* root, const char* country, Parcel** cheapest, Parcel** mostExpensive)
{
    if (root == NULL) return;
    if (strcmp(root->country, country) == 0)
    {
        if (!(*cheapest) || root->valuation < (*cheapest)->valuation)
            *cheapest = root;
        if (!(*mostExpensive) || root->valuation > (*mostExpensive)->valuation)
            *mostExpensive = root;
    }
    findCheapestAndMostExpensive(root->left, country, cheapest, mostExpensive);
    findCheapestAndMostExpensive(root->right, country, cheapest, mostExpensive);
}

void displayCheapestAndMostExpensive(HashTable* table, const char* country)
{
    char lowerCountry[MAX_COUNTRY_NAME];
    strncpy(lowerCountry, country, MAX_COUNTRY_NAME - 1);
    lowerCountry[MAX_COUNTRY_NAME - 1] = '\0';
    toLowerCase(lowerCountry);  

    unsigned int index = hash(lowerCountry);
    if (table[index].root)
    {
        Parcel* cheapest = NULL;
        Parcel* mostExpensive = NULL;
        findCheapestAndMostExpensive(table[index].root, lowerCountry, &cheapest, &mostExpensive);
        if (cheapest)
        {
            printf("Cheapest parcel: \n");
            printParcel(cheapest);
        }
        if (mostExpensive)
        {
            printf("Most expensive parcel: \n");
            printParcel(mostExpensive);
        }
    }
    else
    {
        printf("No parcels found for %s\n", country);
    }
}

void findLightestAndHeaviest(Parcel* root, const char* country, Parcel** lightest, Parcel** heaviest)
{
    if (root == NULL) return;
    if (strcmp(root->country, country) == 0)
    {
        if (!(*lightest) || root->weight < (*lightest)->weight)
            *lightest = root;
        if (!(*heaviest) || root->weight > (*heaviest)->weight)
            *heaviest = root;
    }
    findLightestAndHeaviest(root->left, country, lightest, heaviest);
    findLightestAndHeaviest(root->right, country, lightest, heaviest);
}

void displayLightestAndHeaviest(HashTable* table, const char* country)
{
    char lowerCountry[MAX_COUNTRY_NAME];
    strncpy(lowerCountry, country, MAX_COUNTRY_NAME - 1);
    lowerCountry[MAX_COUNTRY_NAME - 1] = '\0';
    toLowerCase(lowerCountry);  

    unsigned int index = hash(lowerCountry);
    if (table[index].root)
    {
        Parcel* lightest = NULL;
        Parcel* heaviest = NULL;
        findLightestAndHeaviest(table[index].root, lowerCountry, &lightest, &heaviest);
        if (lightest)
        {
            printf("Lightest parcel: ");
            printParcel(lightest);
        }
        if (heaviest)
        {
            printf("Heaviest parcel: ");
            printParcel(heaviest);
        }
    }
    else
    {
        printf("No parcels found for %s\n", country);
    }
}

void freeBST(Parcel* root)
{
    if (!root) return;
    freeBST(root->left);
    freeBST(root->right);
    free(root);
}

void freeHashTable(HashTable* table)
{
    for (int i = 0; i < HASH_TABLE_SIZE; i++)
    {
        freeBST(table[i].root);
    }
}

void displayMenu()
{
    printf("Menu:\n");
    printf("1. Enter country name and display all the parcels details\n");
    printf("2. Enter country and weight pair\n");
    printf("3. Display the total parcel load and valuation for the country\n");
    printf("4. Enter the country name and display cheapest and most expensive parcel's details\n");
    printf("5. Enter the country name and display lightest and heaviest parcel for the country\n");
    printf("6. Exit the application\n");
}

int main()
{
    HashTable table[HASH_TABLE_SIZE] = { { NULL } };

    loadParcels(table, "couriers.txt");

    int choice;
    char country[MAX_COUNTRY_NAME];
    int weight;
    char buffer[100];
    while (1)
    {
        displayMenu();
        printf("Enter your choice: ");
        if (!fgets(buffer, sizeof(buffer), stdin)) continue;
        if (sscanf(buffer, "%d", &choice) != 1)
        {
            printf("Invalid input. Please try again.\n");
            continue;
        }

        switch (choice)
        {
        case 1:
            printf("Enter country name: ");
            if (!fgets(buffer, sizeof(buffer), stdin)) continue;

            buffer[strcspn(buffer, "\n")] = '\0';

            strncpy(country, buffer, MAX_COUNTRY_NAME - 1);
            country[MAX_COUNTRY_NAME - 1] = '\0';

            
            displayParcels(table, country);
            break;


        case 2:
            printf("Enter country name: ");
            if (!fgets(buffer, sizeof(buffer), stdin)) continue;
     
            buffer[strcspn(buffer, "\n")] = '\0';
            strncpy(country, buffer, MAX_COUNTRY_NAME - 1);
            country[MAX_COUNTRY_NAME - 1] = '\0';
            toLowerCase(country);  

            printf("Enter weight: ");
            if (!fgets(buffer, sizeof(buffer), stdin)) continue;
            
            buffer[strcspn(buffer, "\n")] = '\0';
            if (sscanf(buffer, "%d", &weight) != 1)
            {
                printf("Invalid input. Please try again.\n");
                continue;
            }

            printf("Display parcels with weight higher (1) or lower (0) than %d? ", weight);
            if (!fgets(buffer, sizeof(buffer), stdin)) continue;

            buffer[strcspn(buffer, "\n")] = '\0';

            int higher;
            if (sscanf(buffer, "%d", &higher) != 1 || (higher != 0 && higher != 1))
            {
                printf("Invalid input. Please enter 0 or 1.\n");
                continue;
            }

            displayParcelsByWeightWrapper(table[hash(country)].root, country, weight, higher);
            break;



        case 3:
            printf("Enter country name: ");
            if (!fgets(buffer, sizeof(buffer), stdin)) continue;

            buffer[strcspn(buffer, "\n")] = '\0';

            strncpy(country, buffer, MAX_COUNTRY_NAME - 1);
            country[MAX_COUNTRY_NAME - 1] = '\0';

            displayTotalLoadAndValuation(table, country);
            break;

        case 4:
            printf("Enter country name: ");
            if (!fgets(buffer, sizeof(buffer), stdin)) continue;

            buffer[strcspn(buffer, "\n")] = '\0';

            strncpy(country, buffer, MAX_COUNTRY_NAME - 1);
            country[MAX_COUNTRY_NAME - 1] = '\0';

            displayCheapestAndMostExpensive(table, country);
            break;

        case 5:
            printf("Enter country name: ");
            if (!fgets(buffer, sizeof(buffer), stdin)) continue;

            buffer[strcspn(buffer, "\n")] = '\0';

            strncpy(country, buffer, MAX_COUNTRY_NAME - 1);
            country[MAX_COUNTRY_NAME - 1] = '\0';

            displayLightestAndHeaviest(table, country);
            break;

        case 6:
            freeHashTable(table);
            printf("Exiting the application.\n");
            return 0;
        default:
            printf("Invalid choice. Please try again.\n");
        }
    }
    return 0;
}