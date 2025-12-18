#include <stdio.h>
#include <stdlib.h>

typedef struct Node
{
    int key;
    struct Node *left;
    struct Node *right;
} Node;

typedef struct QNode
{
    Node *data;
    struct QNode *next;
} QNode;

typedef struct Queue
{
    QNode *front;
    QNode *rear;
} Queue;

static void clear_stdin_line(void);

static Node *create_node(int key);
static Node *insert_node(Node *root, int key, int *inserted); /* inserted: 1 inserted, 0 exists, -1 alloc fail */
static Node *search_node(Node *root, int key);
static Node *find_min(Node *root);
static Node *delete_node(Node *root, int key, int *deleted); /* deleted: 1 deleted, 0 not found */

static void inorder(Node *root);
static void preorder(Node *root);
static void postorder(Node *root);

static void queue_init(Queue *q);
static int queue_is_empty(Queue *q);
static int enqueue(Queue *q, Node *n);
static Node *dequeue(Queue *q);

static void level_order(Node *root);
static void free_tree(Node *root);

static void print_menu(void);

int main(void)
{
    Node *root = NULL;
    int choice;

    for (;;)
    {
        print_menu();

        if (scanf("%d", &choice) != 1)
        {
            clear_stdin_line();
            printf("Invalid input.\n");
            continue;
        }

        if (choice == 8)
            break;

        if (choice >= 1 && choice <= 3)
        {
            int value;
            printf("Enter value: ");
            if (scanf("%d", &value) != 1)
            {
                clear_stdin_line();
                printf("Invalid input.\n");
                continue;
            }

            if (choice == 1)
            {
                int inserted = 0;
                root = insert_node(root, value, &inserted);
                if (inserted == 1)
                    printf("Inserted.\n");
                else if (inserted == 0)
                    printf("Value already exists.\n");
                else
                    printf("Memory allocation failed.\n");
            }
            else if (choice == 2)
            {
                Node *found = search_node(root, value);
                if (found)
                    printf("Found %d.\n", found->key);
                else
                    printf("Not found.\n");
            }
            else if (choice == 3)
            {
                int deleted = 0;
                root = delete_node(root, value, &deleted);
                if (deleted)
                    printf("Deleted.\n");
                else
                    printf("Value not found.\n");
            }
        }
        else if (choice >= 4 && choice <= 7)
        {
            if (!root)
            {
                printf("Tree is empty.\n");
                continue;
            }

            if (choice == 4)
            {
                inorder(root);
                printf("\n");
            }
            else if (choice == 5)
            {
                preorder(root);
                printf("\n");
            }
            else if (choice == 6)
            {
                postorder(root);
                printf("\n");
            }
            else if (choice == 7)
            {
                level_order(root);
                printf("\n");
            }
        }
        else
        {
            printf("Unknown option.\n");
        }
    }

    free_tree(root);
    return 0;
}

static void clear_stdin_line(void)
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
    {
    }
}

static Node *create_node(int key)
{
    Node *n = (Node *)malloc(sizeof(Node));
    if (!n)
        return NULL;
    n->key = key;
    n->left = NULL;
    n->right = NULL;
    return n;
}

static Node *insert_node(Node *root, int key, int *inserted)
{
    if (!root)
    {
        Node *n = create_node(key);
        if (!n)
        {
            if (inserted)
                *inserted = -1;
            return NULL;
        }
        if (inserted)
            *inserted = 1;
        return n;
    }

    if (key < root->key)
    {
        root->left = insert_node(root->left, key, inserted);
    }
    else if (key > root->key)
    {
        root->right = insert_node(root->right, key, inserted);
    }
    else
    {
        if (inserted)
            *inserted = 0;
    }
    return root;
}

static Node *search_node(Node *root, int key)
{
    while (root)
    {
        if (key < root->key)
            root = root->left;
        else if (key > root->key)
            root = root->right;
        else
            return root;
    }
    return NULL;
}

static Node *find_min(Node *root)
{
    if (!root)
        return NULL;
    while (root->left)
        root = root->left;
    return root;
}

static Node *delete_node(Node *root, int key, int *deleted)
{
    if (!root)
        return NULL;

    if (key < root->key)
    {
        root->left = delete_node(root->left, key, deleted);
    }
    else if (key > root->key)
    {
        root->right = delete_node(root->right, key, deleted);
    }
    else
    {
        if (deleted)
            *deleted = 1;

        if (!root->left && !root->right)
        {
            free(root);
            return NULL;
        }
        else if (!root->left)
        {
            Node *tmp = root->right;
            free(root);
            return tmp;
        }
        else if (!root->right)
        {
            Node *tmp = root->left;
            free(root);
            return tmp;
        }
        else
        {
            Node *succ = find_min(root->right);
            root->key = succ->key;
            root->right = delete_node(root->right, succ->key, NULL);
        }
    }
    return root;
}

static void inorder(Node *root)
{
    if (!root)
        return;
    inorder(root->left);
    printf("%d ", root->key);
    inorder(root->right);
}

static void preorder(Node *root)
{
    if (!root)
        return;
    printf("%d ", root->key);
    preorder(root->left);
    preorder(root->right);
}

static void postorder(Node *root)
{
    if (!root)
        return;
    postorder(root->left);
    postorder(root->right);
    printf("%d ", root->key);
}

static void queue_init(Queue *q)
{
    q->front = NULL;
    q->rear = NULL;
}

static int queue_is_empty(Queue *q)
{
    return q->front == NULL;
}

static int enqueue(Queue *q, Node *n)
{
    QNode *nn = (QNode *)malloc(sizeof(QNode));
    if (!nn)
        return 0;
    nn->data = n;
    nn->next = NULL;

    if (!q->rear)
    {
        q->front = nn;
        q->rear = nn;
    }
    else
    {
        q->rear->next = nn;
        q->rear = nn;
    }
    return 1;
}

static Node *dequeue(Queue *q)
{
    if (!q->front)
        return NULL;

    QNode *old = q->front;
    Node *data = old->data;

    q->front = old->next;
    if (!q->front)
        q->rear = NULL;

    free(old);
    return data;
}

static void level_order(Node *root)
{
    if (!root)
        return;

    Queue q;
    queue_init(&q);

    if (!enqueue(&q, root))
    {
        printf("Queue allocation failed.\n");
        return;
    }

    while (!queue_is_empty(&q))
    {
        Node *cur = dequeue(&q);
        printf("%d ", cur->key);

        if (cur->left)
            enqueue(&q, cur->left);
        if (cur->right)
            enqueue(&q, cur->right);
    }
}

static void free_tree(Node *root)
{
    if (!root)
        return;
    free_tree(root->left);
    free_tree(root->right);
    free(root);
}

static void print_menu(void)
{
    printf("\n--- BST MENU ---\n");
    printf("1) Insert\n");
    printf("2) Search\n");
    printf("3) Delete\n");
    printf("4) Print Inorder\n");
    printf("5) Print Preorder\n");
    printf("6) Print Postorder\n");
    printf("7) Print Level-order\n");
    printf("8) Exit\n");
    printf("Choice: ");
}