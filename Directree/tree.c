// #include "tree.h"
// #include "../utils/string_parser.h"
// #include "hashChild.h"
// #include "trie.h"
// #include"../utils/colors.h"
#include "../DirectoryManager.h"
#include "../include.h"
#include "NodeElement.h"
#define ll long long int


TreeNode *init_tree() {
    char name[5] = "root";

    TreeNode *first_node = (TreeNode *)malloc(sizeof(TreeNode));
    assert(first_node != NULL);
    first_node->file = (FileDetails *)malloc(sizeof(FileDetails));
    assert(first_node->file != NULL);
    first_node->file->name = (char *)malloc(5 * sizeof(char));
    assert(first_node->file->name != NULL);

    first_node->ht = initTable(TABLE_SIZE);
    first_node->childTrie = createTrie();

    first_node->file->is_file = 0;
    strcpy(first_node->file->name, name);

    first_node->first_child = NULL;
    first_node->next = NULL;
    first_node->current = first_node;
    first_node->parent = first_node;

    return first_node;
}

TreeNode *init_node(char *name, bool is_file, TreeNode *parent) {
    int name_length = strlen(name) + 1;

    // Mallocs
    TreeNode *new_node;
    new_node = (TreeNode *)malloc(sizeof(TreeNode));
    assert(new_node != NULL);
    new_node->file = (FileDetails *)malloc(sizeof(FileDetails));
    assert(new_node->file != NULL);
    new_node->file->name = (char *)malloc(name_length * sizeof(char));
    assert(new_node->file->name != NULL);

    if (!is_file) {
        new_node->ht = initTable(TABLE_SIZE);
        new_node->childTrie = createTrie();
    }

    else {
        new_node->ht = NULL;
        new_node->childTrie = NULL;
    }

    // Assignments
    new_node->file->is_file = is_file;
    strcpy(new_node->file->name, name);

    new_node->first_child = NULL;
    new_node->next = NULL;
    new_node->current = new_node;
    new_node->parent = parent;

    return new_node;
}

void add_node(TreeNode *parent, char *name, bool is_file) {
    TreeNode *new_node = init_node(name, is_file, parent);

    if (insertIntoTable(parent->ht, new_node)) {
        add_at_start(new_node, parent);
        trieInsert(parent->childTrie, name);
    }

    else {
        red();
        printf(
            "Error: File/folder with the given name already exists\nNothing was added\n");
        reset();

        delete_tree(new_node);
    }
}

void add_at_start(TreeNode *newNode, TreeNode *parent) {
    newNode->next = parent->first_child;
    parent->first_child = newNode;
}

TreeNode *traversal(char *path, TreeNode *root) {
    char **finalarray;
    finalarray = String_Parser(path);
    TreeNode *temp = NULL;
    int length = len_of_parser_func(path);
    if (is_Correct_Path(path, finalarray)) {
        for (int i = 0; i < length; i++) {
            free(finalarray[i]);
        }
        free(finalarray);
        return NULL;
    }

    if (strcmp(finalarray[0], root->file->name) == 0) {
        temp = root;
        for (int i = 1; i < length; i++) {
            temp = findInTable(temp->ht, finalarray[i]);
            if (temp == NULL) {
                for (int i = 0; i < length; i++) {
                    free(finalarray[i]);
                }
                free(finalarray);
                return NULL;
            }
        }
    } else {
        red();
        printf("Error: Incorrect root argument\n");
        reset();
    }
    for (int i = 0; i < length; i++) {
        free(finalarray[i]);
    }
    free(finalarray);
    return temp;
}

int print_contents(TreeNode *current) {
    TreeNode *temp = current->first_child;
    int count = 0;

    printf("%s\n", current->file->name);
    while (temp) {
        count++;
        // printf("%d. ", count);
        // count++;
        if (temp->file->is_file) {
            blue();
            RIGHTARROW();
            printf("(File)  \t");
            reset();
        } else {
            green();
            RIGHTARROW();
            printf("(Folder)\t");
            reset();
        }

        printf("%s\n", temp->file->name);
        temp = temp->next;
    }
    printf("%d item(s) in directory %s\n\n", count, current->file->name);
    return count;
}

char *get_path(TreeNode *current) {
    char names[100][100], c;
    int i = 0, j, k, x = 0;

    for (j = 0; j < 100; j++) {
        names[j][0] = 0;
    }

    while (current->parent != current) {
        strcpy(names[i], current->file->name);
        i++;
        current = current->parent;
    }
    strcpy(names[i], "root");
    i++;

    char *path = malloc((i) * 100 * sizeof(char) + 1);

    for (int j = i - 1; j >= 0; j--) {
        k = 0, c = '0';
        while (names[j][k]) {
            c = names[j][k];
            path[x] = c;
            k++;
            x++;
        }
        path[x] = '/';
        x++;
    }
    path[x] = 0;
    return path;
}

void print_current_path(TreeNode *current) {
    char *path = get_path(current);
    purple();
    printf("%s ", path);
    reset();
    free(path);
}

void delete_tree(TreeNode *root) {
    // End of children list
    if (root == NULL) {
        return;
    }

    TreeNode *temp = root->first_child;
    TreeNode *temp_next;

    // Iterates through each child and deletes subdirectories recursively
    while (temp) {
        temp_next = temp->next;
        delete_tree(temp);
        temp = temp_next;
    }

    // The root is either a file or an empty directory
    free(root->file->name);
    free(root->file);
    if (root->childTrie)
        deleteTrie(root->childTrie);
    if (root->ht) {
        free(root->ht->table); // No need to free each element in the table array
        // since they are just the node in the tree and it is
        // freed anyway
        free(root->ht);
    }
    free(root);
    root = NULL;
    return;
}

void dfsTree(TreeNode *curr, int numSpaces) {
    //print spaces first
    for (int i = 0; i < numSpaces; i++) printf(" ");

    //print current node
    if (curr->file->is_file) {
        blue();
        printf("|__ %s\n", curr->file->name);
        reset();
    } else if (!(curr->file->is_file)) {
        green();
        printf("|__ %s\n", curr->file->name);
        reset();
    }

    //recurse on children
    TreeNode* temp = curr->first_child;
    while (temp) {
        dfsTree(temp, numSpaces + 2);
        temp = temp->next;
    }
}
