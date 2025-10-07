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

void add_user(){
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

void display_users(){
     FILE *fp = fopen(FILENAME, "r");
     if (fp == NULL)
     {
          printf("Error opening file.\n");
          return;
     }
     user u;
     printf("ID\tName\tAge\tExperience\n");
     while (fscanf(fp, "%d %s %d %d", &u.id, u.name, &u.age, &u.experience) == 4){
          printf("%d\t%s\t%d\t%d\n", u.id, u.name, u.age, u.experience);
     }
     fclose(fp);
}

void update_user(){
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
     while (fscanf(fp, "%d %s %d %d", &u.id, u.name, &u.age, &u.experience) == 4)
     {
          if (u.id == id)
          {
               printf("Enter the new Name: ");
               scanf("%s", u.name);
               printf("Enter the new Age: ");
               scanf("%d", &u.age);
               printf("Enter the new Experience: ");
               scanf("%d", &u.experience);
               found = 1;
          }
          fprintf(temp, "%d %s %d %d\n", u.id, u.name, u.age, u.experience);
     }
     fclose(fp);
     fclose(temp);
     remove(FILENAME);
     rename("temp.txt", FILENAME);
     if (found){
          printf("User updated successfully.\n");
     }
     else{
          printf("User not found.\n");
     }
}

void delete_user(){
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
     while (fscanf(fp, "%d %s %d %d", &u.id, u.name, &u.age, &u.experience) == 4)
     {
          if (u.id == id){
               found = 1;
          }
          else{
               fprintf(temp, "%d %s %d %d\n", u.id, u.name, u.age, u.experience);
          }
     }
     fclose(fp);
     fclose(temp);
     remove(FILENAME);
     rename("temp.txt", FILENAME);
     if (found){
          printf("User deleted successfully.\n");
     }
     else{
          printf("User not found.\n");
     }
}

int main(){
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