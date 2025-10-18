#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_RANGE 255
#define MIN_RANGE 0
#define MAX_SIZE 10
#define MIN_SIZE 2

void generateImage(int**, int);
void printImage(int**, int);
void rotateImage(int**, int);
void invertRotatedImage(int**, int);
void smoothingFilter(int**, int);

int main()
{
    srand(time(NULL));
    int n;
    printf("Enter the size of the image (2-10): ");
     while (1)
    {
        if (scanf("%d", &n) == 1 && n >= MIN_SIZE && n <= MAX_SIZE)
        {
            int c;
            while ((c = getchar()) != '\n' && c != EOF){}
            break;
        }
        printf("Invalid input please enter a number between %d and %d: ", MIN_SIZE, MAX_SIZE);
        int c;
        while ((c = getchar()) != '\n' && c != EOF){}
    }
    
    int **image = (int **)malloc(n * sizeof(int *));
    generateImage(image, n);
    
    printf("Original Randomly Generated Matrix:\n");
    printImage(image, n);
    printf("\n");
    
    rotateImage(image, n);
    printf("Matrix after 90 Degrees Clockwise Rotation:\n");
    printImage(image, n);
    printf("\n");

    smoothingFilter(image, n);
    printf("Matrix after Applying 3x3 Smoothing Filter:\n");
    printImage(image, n);

    for (int i = 0; i < n; i++)
    {
        free(*(image + i));
    }

    free(image);
    return 0;
}

void generateImage(int** image, int n)
{
    for (int i = 0; i < n; i++)
    {
        *(image + i) = (int *)malloc(n * sizeof(int));
        for (int j = 0; j < n; j++)
        {
            *(*(image + i) + j) = (rand() % MAX_RANGE);
        }
    }
}

void printImage(int** image, int n)
{
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            printf("%3d ", *(*(image + i) + j));
        }
        printf("\n");
    }
}

void rotateImage(int** image, int n)
{
    int temp;
    for (int layer = 0; layer < n / 2; layer++) // layer by layer
    {
        int first = layer, last = n - 1 - layer; 
        for (int i = 0; i < last - first; i++)
        {
            int *top_ptr = *(image + first) + (first + i); 
            int *right_ptr = *(image + first + i) + last;
            int *bottom_ptr = *(image + last) + (last - i);
            int *left_ptr = *(image + last - i) + first;

            temp = *top_ptr;
            *top_ptr = *left_ptr;
            *left_ptr = *bottom_ptr;
            *bottom_ptr = *right_ptr;
            *right_ptr = temp;
        }
    }
}


void smoothingFilter(int** image, int n)
{
    int *temp_prev = (int *)malloc(n * sizeof(int)); 
    int *temp_next = (int *)malloc(n * sizeof(int)); 

    for (int i = 0; i < n; i++)
    {
        *(temp_prev + i) = *(*(image + 0) + i);
    }

    for (int i = 0; i < n; i++)
    {

        for (int j = 0; j < n; j++)
        {
            int sum = 0;
            int count = 0;
            for (int row_i = -1; row_i <= 1; row_i++) // row_i = neighboring rows
            {
                for (int col_j = -1; col_j <= 1; col_j++) // col_j = neighboring columns
                {
                    int row_e = i + row_i; // effective row
                    int col_e = j + col_j; // effective column
                    if (row_e >= 0 && row_e < n && col_e >= 0 && col_e < n)
                    {
                        int neighbor_val;
                        if (row_e < i)
                        {
                            neighbor_val = *(temp_prev + col_e);
                        }
                        else if (row_e == i)
                        {
                            neighbor_val = *(*(image + i) + col_e);
                        }
                        else
                        {
                            neighbor_val = *(*(image + row_e) + col_e);
                        }

                        sum += neighbor_val;
                        count++;
                    }
                }
            }
            *(temp_next + j) = sum / count;
        }
        for (int j = 0; j < n; j++) // Copy current row to temp_prev
        {
            *(temp_prev + j) = *(*(image + i) + j);
        }
        for (int j = 0; j < n; j++) // Update image row with smoothed values
        {
            *(*(image + i) + j) = *(temp_next + j);
        }
    }
    free(temp_prev);
    free(temp_next);
}
