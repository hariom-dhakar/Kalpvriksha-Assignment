#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#define MIN_INITIAL 1
#define MAX_INITIAL 100
#define MAX_NAME_LENGTH 51
#define MIN_PRODUCT_ID 1
#define MAX_PRODUCT_ID 10000
#define MIN_PRICE 0.0f
#define MAX_PRICE 100000.0f
#define MIN_QUANTITY 0
#define MAX_QUANTITY 1000000

typedef struct Product
{
    int productId;
    char name[MAX_NAME_LENGTH];
    float price;
    int quantity;
} Product;

void displayMenu();
void clearInputBuffer();
bool readString(char *buffer, int size);
bool strcasestr_exists(const char *haystack, const char *needle);
bool isIdUnique(int id);
void printProduct(const Product *p);
bool addProduct();
void viewAllProducts();
void updateQuantity();
void searchProductById();
void searchProductByName();
void searchProductByPriceRange();
void deleteProduct();
void cleanupMemory();

Product *gInventory = NULL;
int gProductCount = 0;
int gProductCapacity = 0;

int main()
{
    int initial;
    printf("Enter initial number of products (1-100): ");
    while (1)
    {
        if (scanf("%d", &initial) == 1 && initial >= MIN_INITIAL && initial <= MAX_INITIAL)
        {
            clearInputBuffer();
            break;
        }
        clearInputBuffer();
        printf("Invalid input, please enter a number between %d and %d: ", MIN_INITIAL, MAX_INITIAL);
    }

    gInventory = (Product *)calloc(initial, sizeof(Product));
    if (!gInventory)
    {
        fprintf(stderr, "Error: initial memory allocation failed\n");
        return 1;
    }
    gProductCapacity = initial;
    gProductCount = 0;

    printf("\nEnter details for the initial %d products:\n", initial);
    for (int i = 0; i < initial; ++i)
    {
        printf("\nEnter details for product %d:\n", i + 1);
        if (!addProduct())
        {
            fprintf(stderr, "Error adding initial product %d. Exiting\n", i + 1);
            cleanupMemory();
            return 1;
        }
    }

    int choice;
    do
    {
        displayMenu();
        printf("Enter your choice: ");
        if (scanf("%d", &choice) != 1)
        {
            clearInputBuffer();
            choice = -1;
        }
        else
        {
            clearInputBuffer();
        }

        switch (choice)
        {
        case 1:
            printf("Enter new product details:\n");
            if (addProduct())
            {
                printf("Product added successfully\n");
            }
            break;
        case 2:
            viewAllProducts();
            break;
        case 3:
            updateQuantity();
            break;
        case 4:
            searchProductById();
            break;
        case 5:
            searchProductByName();
            break;
        case 6:
            searchProductByPriceRange();
            break;
        case 7:
            deleteProduct();
            break;
        case 8:
            printf("Exiting program...\n");
            break;
        default:
            printf("Invalid choice, please try again\n");
            break;
        }
    } while (choice != 8);

    cleanupMemory();
    printf("Memory released successfully, Exiting program...\n");
    return 0;
}

void displayMenu()
{
    printf("\n========= INVENTORY MENU =========\n");
    printf("1. Add New Product\n");
    printf("2. View All Products\n");
    printf("3. Update Quantity\n");
    printf("4. Search Product by ID\n");
    printf("5. Search Product by Name\n");
    printf("6. Search Product by Price Range\n");
    printf("7. Delete Product\n");
    printf("8. Exit\n");
    printf("==================================\n");
}

void clearInputBuffer()
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
    {
    }
}

bool readString(char *buffer, int size)
{
    if (fgets(buffer, size, stdin) == NULL)
    {
        buffer[0] = '\0';
        return false;
    }
    size_t len = strlen(buffer);
    if (len > 0 && buffer[len - 1] == '\n')
    {
        buffer[len - 1] = '\0';
    }
    else
    {
        clearInputBuffer();
    }
    return true;
}

bool strcasestr_exists(const char *haystack, const char *needle)
{
    if (!*needle)
        return true;
    for (; *haystack; haystack++)
    {
        const char *h = haystack;
        const char *n = needle;
        while (*h && *n && tolower((unsigned char)*h) == tolower((unsigned char)*n))
        {
            h++;
            n++;
        }
        if (!*n)
            return true;
    }
    return false;
}

bool isIdUnique(int id)
{
    for (int i = 0; i < gProductCount; ++i)
    {
        if (gInventory[i].productId == id)
            return false;
    }
    return true;
}

void printProduct(const Product *p)
{
    printf("Product ID: %d | Name: %s | Price: %.2f | Quantity: %d\n",
           p->productId, p->name, p->price, p->quantity);
}

bool addProduct()
{
    if (gProductCount >= gProductCapacity)
    {
        int newCap = (gProductCapacity == 0) ? 1 : gProductCapacity * 2;
        Product *tmp = (Product *)realloc(gInventory, newCap * sizeof(Product));
        if (!tmp)
        {
            fprintf(stderr, "Error: memory reallocation failed while adding product\n");
            return false;
        }
        gInventory = tmp;
        gProductCapacity = newCap;
        printf("Inventory capacity increased to %d\n", gProductCapacity);
    }

    Product p;
    int id;
    char nameBuf[MAX_NAME_LENGTH];
    float price;
    int qty;

    while (1)
    {
        printf("Product ID (%d-%d): ", MIN_PRODUCT_ID, MAX_PRODUCT_ID);
        if (scanf("%d", &id) == 1)
        {
            clearInputBuffer();
            if (id >= MIN_PRODUCT_ID && id <= MAX_PRODUCT_ID && isIdUnique(id))
            {
                p.productId = id;
                break;
            }
        }
        else
        {
            clearInputBuffer();
        }
        printf(" -> Invalid or duplicate ID, please enter a unique number between %d and %d\n",
               MIN_PRODUCT_ID, MAX_PRODUCT_ID);
    }

    printf("Product Name (max 50 chars): ");
    if (!readString(nameBuf, MAX_NAME_LENGTH) || strlen(nameBuf) == 0)
    {
        printf(" -> Invalid name\n");
        return false;
    }
    strncpy(p.name, nameBuf, MAX_NAME_LENGTH);
    p.name[MAX_NAME_LENGTH - 1] = '\0';

    while (1)
    {
        printf("Product Price (%.2f - %.2f): ", MIN_PRICE, MAX_PRICE);
        if (scanf("%f", &price) == 1)
        {
            clearInputBuffer();
            if (price >= MIN_PRICE && price <= MAX_PRICE)
            {
                p.price = price;
                break;
            }
        }
        else
        {
            clearInputBuffer();
        }
        printf(" -> Invalid price, please enter a number between %.2f and %.2f\n", MIN_PRICE, MAX_PRICE);
    }

    while (1)
    {
        printf("Product Quantity (%d - %d): ", MIN_QUANTITY, MAX_QUANTITY);
        if (scanf("%d", &qty) == 1)
        {
            clearInputBuffer();
            if (qty >= MIN_QUANTITY && qty <= MAX_QUANTITY)
            {
                p.quantity = qty;
                break;
            }
        }
        else
        {
            clearInputBuffer();
        }
        printf(" -> Invalid quantity, please enter a number between %d and %d\n", MIN_QUANTITY, MAX_QUANTITY);
    }

    gInventory[gProductCount++] = p;
    return true;
}

void viewAllProducts()
{
    printf("\n========= PRODUCT LIST =========\n");
    if (gProductCount == 0)
    {
        printf("Inventory is empty\n");
    }
    else
    {
        for (int i = 0; i < gProductCount; ++i)
        {
            printProduct(&gInventory[i]);
        }
    }
    printf("================================\n");
}

void updateQuantity()
{
    if (gProductCount == 0)
    {
        printf("Inventory is empty, cannot update quantity\n");
        return;
    }
    int id;
    printf("Enter Product ID to update quantity: ");
    if (scanf("%d", &id) != 1)
    {
        clearInputBuffer();
        printf("Invalid ID format entered\n");
        return;
    }
    clearInputBuffer();

    int idx = -1;
    for (int i = 0; i < gProductCount; ++i)
    {
        if (gInventory[i].productId == id)
        {
            idx = i;
            break;
        }
    }
    if (idx == -1)
    {
        printf("Product with ID %d not found\n", id);
        return;
    }

    int newQty;
    while (1)
    {
        printf("Enter new Quantity (%d - %d): ", MIN_QUANTITY, MAX_QUANTITY);
        if (scanf("%d", &newQty) == 1)
        {
            clearInputBuffer();
            if (newQty >= MIN_QUANTITY && newQty <= MAX_QUANTITY)
            {
                gInventory[idx].quantity = newQty;
                printf("Quantity updated successfully\n");
                return;
            }
        }
        else
        {
            clearInputBuffer();
        }
        printf(" -> Invalid quantity, please enter a number between %d and %d\n", MIN_QUANTITY, MAX_QUANTITY);
    }
}

void searchProductById()
{
    if (gProductCount == 0)
    {
        printf("Inventory is empty, cannot search\n");
        return;
    }
    int id;
    printf("Enter Product ID to search: ");
    if (scanf("%d", &id) != 1)
    {
        clearInputBuffer();
        printf("Invalid ID format entered\n");
        return;
    }
    clearInputBuffer();

    for (int i = 0; i < gProductCount; ++i)
    {
        if (gInventory[i].productId == id)
        {
            printf("Product Found: ");
            printProduct(&gInventory[i]);
            return;
        }
    }
    printf("Product with ID %d not found\n", id);
}

void searchProductByName()
{
    if (gProductCount == 0)
    {
        printf("Inventory is empty, cannot search\n");
        return;
    }
    char term[MAX_NAME_LENGTH];
    printf("Enter name to search (partial allowed): ");
    if (!readString(term, MAX_NAME_LENGTH) || strlen(term) == 0)
    {
        printf(" -> Invalid or empty search term entered\n");
        return;
    }

    bool found = false;
    printf("Products Found:\n");
    for (int i = 0; i < gProductCount; ++i)
    {
        if (strcasestr_exists(gInventory[i].name, term))
        {
            printProduct(&gInventory[i]);
            found = true;
        }
    }
    if (!found)
    {
        printf("No products found matching '%s'\n", term);
    }
}

void searchProductByPriceRange()
{
    if (gProductCount == 0)
    {
        printf("Inventory is empty, cannot search\n");
        return;
    }
    float minP, maxP;
    while (1)
    {
        printf("Enter minimum price: ");
        if (scanf("%f", &minP) == 1 && minP >= MIN_PRICE)
        {
            clearInputBuffer();
            break;
        }
        clearInputBuffer();
        printf(" -> Invalid minimum price, please enter a non-negative number\n");
    }
    while (1)
    {
        printf("Enter maximum price: ");
        if (scanf("%f", &maxP) == 1 && maxP >= minP && maxP <= MAX_PRICE)
        {
            clearInputBuffer();
            break;
        }
        clearInputBuffer();
        printf(" -> Invalid maximum price, must be >= minimum price (%.2f) and <= %.2f.\n", minP, MAX_PRICE);
    }

    bool found = false;
    printf("Products in price range [%.2f - %.2f]:\n", minP, maxP);
    for (int i = 0; i < gProductCount; ++i)
    {
        if (gInventory[i].price >= minP && gInventory[i].price <= maxP)
        {
            printProduct(&gInventory[i]);
            found = true;
        }
    }
    if (!found)
    {
        printf("No products found in this price range\n");
    }
}

void deleteProduct()
{
    if (gProductCount == 0)
    {
        printf("Inventory is empty cannot delete\n");
        return;
    }
    int id;
    printf("Enter Product ID to delete: ");
    if (scanf("%d", &id) != 1)
    {
        clearInputBuffer();
        printf("Invalid ID format entered\n");
        return;
    }
    clearInputBuffer();

    int idx = -1;
    for (int i = 0; i < gProductCount; ++i)
    {
        if (gInventory[i].productId == id)
        {
            idx = i;
            break;
        }
    }
    if (idx == -1)
    {
        printf("Product with ID %d not found\n", id);
        return;
    }

    for (int i = idx; i < gProductCount - 1; ++i)
    {
        gInventory[i] = gInventory[i + 1];
    }
    gProductCount--;
    printf("Product deleted successfully\n");
}

void cleanupMemory()
{
    if (gInventory != NULL)
    {
        free(gInventory);
        gInventory = NULL;
    }
    gProductCount = 0;
    gProductCapacity = 0;
}