#include <stdio.h>
#include <string.h>

#define MAX_STUDENTS 100

typedef struct Student
{
    int roll;
    char name[50];
    int marks[3];
    int total;
    float average;
    char grade;
} Student;

void sortByRollNumber(struct Student students[], int n);
int calculateTotal(int marks[], int n);
float calculateAverage(int total);
char assignGrade(float average);
void printStars(float average);
void printRollNumbers(Student students[], int i, int n);

int main()
{
    int n;
    Student students[MAX_STUDENTS];

    printf("Enter number of students: ");
    scanf("%d", &n);
    if (n < 1 || n > MAX_STUDENTS)
    {
        printf("Please enter a valid number of students.\n");
        return 1;
    }

    for (int i = 0; i < n; i++)
    {
        printf("Enter details for student %d:\n", i + 1);
        printf("Enter Roll Number: ");
        scanf("%d", &students[i].roll);
        if (students[i].roll < 1)
        {
            printf("Please enter a valid roll number.\n\n");
            i--;
            continue;
        }
        printf("Enter Name: ");
        scanf("%s", students[i].name);
        printf("Enter Marks in 3 subjects (out of 100): ");
        for (int j = 0; j < 3; j++)
        {
            printf("Subject %d: ", j + 1);
            scanf("%d", &students[i].marks[j]);
            if (students[i].marks[j] < 0 || students[i].marks[j] > 100)
            {
                printf("Please enter valid marks between 0 and 100.\n\n");
                j--;
                continue;
            }
        }

        students[i].total = calculateTotal(students[i].marks, 3);
        students[i].average = calculateAverage(students[i].total);
        students[i].grade = assignGrade(students[i].average);
    }
    printf("\n\n");
    sortByRollNumber(students, n);
    for (int i = 0; i < n; i++)
    {
        printf("Roll Number: %d\n", students[i].roll);
        printf("Name: %s\n", students[i].name);
        printf("Total: %d\n", students[i].total);
        printf("Average: %.2f\n", students[i].average);
        printf("Grade: %c \n", students[i].grade);
        if (students[i].average < 35)
        {
            printf("\n\n");
            continue;
        }
        printf("Performance: ");
        printStars(students[i].average);
        printf("\n\n");
    }

    printf("List of Roll Numbers:\n");
    printRollNumbers(students, 0, n);
    return 0;
}

void sortByRollNumber(struct Student students[], int n)
{
    for (int i = 0; i < n - 1; i++)
    {
        for (int j = i + 1; j < n; j++)
        {
            if (students[i].roll > students[j].roll)
            {
                struct Student temp = students[i];
                students[i] = students[j];
                students[j] = temp;
            }
        }
    }
}

int calculateTotal(int marks[], int n)
{
    int totalMarks = 0;
    for (int i = 0; i < n; i++)
    {
        totalMarks += marks[i];
    }
    return totalMarks;
}

float calculateAverage(int total)
{
    return (float)total / 3;
}

char assignGrade(float average)
{
    if (average >= 85)
    {
        return 'A';
    }
    else if (average >= 70)
    {
        return 'B';
    }
    else if (average >= 50)
    {
        return 'C';
    }
    else if (average >= 35)
    { //
        return 'D';
    }
    else
    {
        return 'F';
    }
}

void printStars(float average)
{
    int stars = 0;
    switch ((int)average)
    {
    case 85 ... 100:
        stars = 5;
        break;
    case 70 ... 84:
        stars = 4;
        break;
    case 50 ... 69:
        stars = 3;
        break;
    case 35 ... 49:
        stars = 2;
        break;
    default:
        stars = 0;
        break;
    }
    for (int i = 0; i < stars; i++)
    {
        printf("*");
    }
}

void printRollNumbers(Student students[], int i, int n)
{
    if (i >= n)
    {
        return;
    }
    printf("%d ", students[i].roll);
    printRollNumbers(students, i + 1, n);
}
