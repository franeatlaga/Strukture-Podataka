#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define NO_MEMORY_ERROR -1
#define MIN_RND 10
#define MAX_RND 90

typedef struct Node
{
    int key;
    struct Node *left;
    struct Node *right;
} Node;

Node *node_create(int key);
Node *tree_insert(Node *root, int key);
void tree_inorder(Node *root, FILE *out);
int tree_rewrite_desc_sum(Node *root);
int *make_random_array(int n);
void tree_free(Node *root);

int main(void)
{
    FILE *f_fixed = fopen("dat1.txt", "w");
    FILE *f_rand = fopen("dat2.txt", "w");
    if (!f_fixed || !f_rand)
    {
        printf("Greska: ne mogu otvoriti datoteku.\n");
        if (f_fixed)
            fclose(f_fixed);
        if (f_rand)
            fclose(f_rand);
        return 1;
    }

    srand((unsigned)time(NULL));

    for (;;)
    {
        int choice = -1;
        printf("Odaberi:\n");
        printf("0 - izlaz\n");
        printf("1 - zadani niz\n");
        printf("2 - niz random brojeva\n");
        if (scanf("%d", &choice) != 1)
        {
            printf("Neispravan unos.\n");
            int c;
            while ((c = getchar()) != '\n' && c != EOF)
            {
            }
            continue;
        }

        if (choice == 0)
            break;

        if (choice == 1)
        {
            Node *root = NULL;

            int arr[] = {21, 14, 28, 7, 18, 23, 35, 3, 9, 16};
            int n = (int)(sizeof(arr) / sizeof(arr[0]));

            for (int i = 0; i < n; i++)
                root = tree_insert(root, arr[i]);

            printf("Inorder prije replace: ");
            fprintf(f_fixed, "Prije funkcije\n");
            tree_inorder(root, f_fixed);
            printf("\n");
            fprintf(f_fixed, "\n");

            tree_rewrite_desc_sum(root);

            printf("Inorder nakon replace: ");
            fprintf(f_fixed, "Nakon funkcije\n");
            tree_inorder(root, f_fixed);
            printf("\n\n");
            fprintf(f_fixed, "\n\n");

            tree_free(root);
        }
        else if (choice == 2)
        {
            int m = 0;
            printf("Unesite koliko random brojeva zelite?\n> ");
            if (scanf("%d", &m) != 1 || m <= 0)
            {
                printf("Neispravan broj.\n");
                continue;
            }

            int *a = make_random_array(m);
            if (!a)
            {
                printf("Nema memorije.\n");
                continue;
            }

            Node *root = NULL;
            for (int i = 0; i < m; i++)
                root = tree_insert(root, a[i]);

            printf("Generirani brojevi:\n");
            for (int i = 0; i < m; i++)
                printf("%d\n", a[i]);

            printf("Inorder prije replace: ");
            fprintf(f_rand, "Prije funkcije\n");
            tree_inorder(root, f_rand);
            printf("\n");
            fprintf(f_rand, "\n");

            tree_rewrite_desc_sum(root);

            printf("Inorder nakon replace: ");
            fprintf(f_rand, "Nakon funkcije\n");
            tree_inorder(root, f_rand);
            printf("\n\n");
            fprintf(f_rand, "\n\n");

            free(a);
            tree_free(root);
        }
        else
        {
            printf("Nepostojeca opcija.\n\n");
        }
    }

    fclose(f_fixed);
    fclose(f_rand);
    return 0;
}

Node *node_create(int key)
{
    Node *p = (Node *)malloc(sizeof(Node));
    if (!p)
        return NULL;
    p->key = key;
    p->left = NULL;
    p->right = NULL;
    return p;
}

/*
  Insert koji daje stablo kao na slici:
  - ako je root->key > key, ide desno
  - inace ide lijevo
  (to je "obrnuti" BST u odnosu na standard)
*/
Node *tree_insert(Node *root, int key)
{
    if (!root)
        return node_create(key);

    if (root->key > key)
        root->right = tree_insert(root->right, key);
    else
        root->left = tree_insert(root->left, key);

    return root;
}

void tree_inorder(Node *root, FILE *out)
{
    if (!root)
        return;
    tree_inorder(root->left, out);
    printf("%d ", root->key);
    if (out)
        fprintf(out, "%d ", root->key);
    tree_inorder(root->right, out);
}

/*
  b) Svaki cvor postaje suma originalnih vrijednosti u lijevom+desnom podstablu.
  Funkcija vraca sumu originalnih vrijednosti cijelog podstabla (da ne radimo O(n^2)).
*/
int tree_rewrite_desc_sum(Node *root)
{
    if (!root)
        return 0;

    int leftSum = tree_rewrite_desc_sum(root->left);
    int rightSum = tree_rewrite_desc_sum(root->right);

    int original = root->key;
    root->key = leftSum + rightSum;

    return original + leftSum + rightSum;
}

int *make_random_array(int n)
{
    int *a = (int *)malloc((size_t)n * sizeof(int));
    if (!a)
        return NULL;

    for (int i = 0; i < n; i++)
        a[i] = MIN_RND + (rand() % (MAX_RND - MIN_RND + 1));

    return a;
}

void tree_free(Node *root)
{
    if (!root)
        return;
    tree_free(root->left);
    tree_free(root->right);
    free(root);
}