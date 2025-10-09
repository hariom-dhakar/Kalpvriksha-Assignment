#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FILENAME "users_data.txt"

typedef struct
{
     int id;
     char name[50];
     int age;
     int experience;
} user;

void add_user()
{
     FILE *fp = fopen(FILENAME, "a");
     if (fp == NULL)
     {
          printf("Error opening file.\n");
          return;
     }
     user u;
     printf("Enter the ID: ");
     scanf("%d", &u.id);
     printf("Enter the Name: ");
     scanf("%s", u.name);
     printf("Enter the Age: ");
     scanf("%d", &u.age);
     printf("Enter the Experience(in years): ");
     scanf("%d", &u.experience);
     fprintf(fp, "%d %s %d %d\n", u.id, u.name, u.age, u.experience);
     fclose(fp);
     printf("New User added successfully\n");
}

void display_users()
{
     FILE *fp = fopen(FILENAME, "r");
     if (fp == NULL)
     {
          printf("Error opening file.\n");
          return;
     }
     user u;
     printf("ID\tName\tAge\tExperience\n");
     while (fscanf(fp, "%d %s %d %d", &u.id, u.name, &u.age, &u.experience) == 4)
     {
          printf("%d\t%s\t%d\t%d\n", u.id, u.name, u.age, u.experience);
     }
     fclose(fp);
}

void update_user()
{
     FILE *fp = fopen(FILENAME, "r");
     FILE *temp = fopen("temp.txt", "w");
     if (!fp || !temp)
     {
          printf("Error opening file.\n");
          return;
     }
     user u;
     int id, found = 0;
     printf("Enter the user ID to update: ");
     scanf("%d", &id);

     user users[100];
     int count = 0;
     int total = 0;

     while (fscanf(fp, "%d %s %d %d", &u.id, u.name, &u.age, &u.experience) == 4)
     {
          users[total++] = u;
     }
     fclose(fp);

     printf("User with User id: %d\n", id);
     for (int i = 0; i < total; i++)
     {
          if (users[i].id == id)
          {
               count++;
               printf("%d. Name: %s  Age: %d  Experience: %d\n",
                      count, users[i].name, users[i].age, users[i].experience);
          }
     }

     if (count == 0)
     {
          printf("No user found with this ID %d.\n", id);
          fclose(temp);
          remove("temp.txt");
          return;
     }

     int choice;

     if (count > 1)
     {
          printf("Multiple users found with same ID.\n");
          printf("Enter the number (1-%d) of the user you want to update: ", count);
          scanf("%d", &choice);
          if (choice < 1 || choice > count)
          {
               printf("Invalid choice.\n");
               fclose(temp);
               remove("temp.txt");
               return;
          }
     }
     else
     {
          choice = 1;
     }

     int current = 0;
     for (int i = 0; i < total; i++)
     {
          if (users[i].id == id)
          {
               current++;
               if (current == choice)
               {
                    printf("Updating user %d (Name: %s, Age: %d, Exp: %d)\n",
                           current, users[i].name, users[i].age, users[i].experience);
                    printf("Enter the new Name: ");
                    scanf("%s", users[i].name);
                    printf("Enter the new Age: ");
                    scanf("%d", &users[i].age);
                    printf("Enter the new Experience: ");
                    scanf("%d", &users[i].experience);
                    found = 1;
               }
          }
     }
     for (int i = 0; i < total; i++)
     {
          fprintf(temp, "%d %s %d %d\n", users[i].id, users[i].name, users[i].age, users[i].experience);
     }
     fclose(temp);

     remove(FILENAME);
     rename("temp.txt", FILENAME);
     if (found)
     {
          printf("User updated successfully.\n");
     }
     else
     {
          printf("User not found or update failed.\n");
     }
}

void delete_user()
{
     FILE *fp = fopen(FILENAME, "r");
     FILE *temp = fopen("temp.txt", "w");
     if (!fp || !temp)
     {
          printf("Error opening file.\n");
          return;
     }
     user u;
     int id, found = 0;
     printf("Enter the user ID to delete: ");
     scanf("%d", &id);
     user users[100];
     int total = 0;

     while (fscanf(fp, "%d %s %d %d", &u.id, u.name, &u.age, &u.experience) == 4)
     {
          users[total++] = u;
     }
     fclose(fp);

     printf("Users with user id: %d\n", id);
     int count = 0;
     for (int i = 0; i < total; i++)
     {
          if (users[i].id == id)
          {
               count++;
               printf("%d. Name: %s  Age: %d  Experience: %d\n",
                      count, users[i].name, users[i].age, users[i].experience);
          }
     }
     if (count == 0)
     {
          printf("No user found with this User ID %d.\n", id);
          return;
     }
     int choice = 1;
     if (count > 1)
     {
          printf("Multiple users found with same User ID.\n");
          printf("Enter the number (1-%d) of the user to delete: ", count);
          scanf("%d", &choice);
          if (choice < 1 || choice > count)
          {
               printf("Invalid choice.\n");
               return;
          }
     }
     int current = 0;
     for (int i = 0; i < total; i++)
     {
          if (users[i].id == id)
          {
               current++;
               if (current == choice)
               {
                    printf("Deleting user #%d (Name: %s, Age: %d, Exp: %d)\n",
                           current, users[i].name, users[i].age, users[i].experience);
                    found = 1;
                    continue;
               }
          }
          fprintf(temp, "%d %s %d %d\n",
                  users[i].id, users[i].name, users[i].age, users[i].experience);
     }
     fclose(temp);
     remove(FILENAME);
     rename("temp.txt", FILENAME);
     if (found)
     {
          printf("User deleted successfully.\n");
     }
     else
     {
          printf("User not found.\n");
     }
}

int main()
{
     int choice;
     do
     {
          printf("\n1. Add New User\n2. Display all Users\n3. Update an User\n4. Delete an User\n5. Exit\nEnter your choice: ");
          scanf("%d", &choice);
          switch (choice)
          {
          case 1:
               add_user();
               break;
          case 2:
               display_users();
               break;
          case 3:
               update_user();
               break;
          case 4:
               delete_user();
               break;
          case 5:
               printf("Exiting \n");
               break;
          default:
               printf("Invalid choice \n");
          }
     } while (choice != 5);
     return 0;
}