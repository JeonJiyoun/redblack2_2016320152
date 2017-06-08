#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <tchar.h>
#include <strsafe.h>

typedef struct Node* NodePtr;
typedef struct RBtree* RBPtr;
int nb = 0;
int insert = 0;
int miss = 0;
int delete = 0;
struct Node {
	int key;
	NodePtr left;
	NodePtr right;
	NodePtr p;

	enum { RED, BLACK } color;
};
NodePtr node_alloc(int newval) {
	NodePtr self = (NodePtr)malloc(sizeof(struct Node));
	self->key = newval;
	self->left = NULL;
	self->right = NULL;
	self->p = NULL;

	return self;
}

struct RBtree {
	NodePtr root;
	NodePtr nil;
};

RBPtr create_nilnode() {
	RBPtr self = (RBPtr)malloc(sizeof(struct RBtree));
	self->nil = (NodePtr)malloc(sizeof(struct Node));
	self->nil->color = BLACK;
	self->root = self->nil;
	return self;
}

void left_rotate(RBPtr r, NodePtr x) {
	NodePtr temp;
	temp = x->right;
	x->right = temp->left;

	if (temp->left != r->nil) {
		temp->left->p = x;

	}
	temp->p = x->p;
	if (x->p == r->nil) {
		r->root = temp;
	}
	else if (x == x->p->left) {
		x->p->left = temp;
	}
	else {
		x->p->right = temp;
	}
	temp->left = x;
	x->p = temp;
}

void right_rotate(RBPtr r, NodePtr y) {
	NodePtr temp;
	temp = y->left;
	y->left = temp->right;
	if (temp->right != r->nil) {
		temp->right->p = y;
	}
	temp->p = y->p;

	if (y->p == r->nil) {
		r->root = temp;
	}
	else if (y == y->p->left) {
		y->p->left = temp;
	}
	else {
		y->p->right = temp;
	}
	temp->right = y;
	y->p = temp;
}
void insert_fixup(RBPtr r, NodePtr x) {
	NodePtr u = NULL;

	while (x->p->color == RED) {
		if (x->p == (x->p->p)->left) {
			u = x->p->p->right;

			if (u->color == RED) {
				x->p->color = BLACK;
				u->color = BLACK;

				x->p->p->color = RED;

				x = x->p->p;
			}
			else {
				if (x == x->p->right) {
					x = x->p;
					left_rotate(r, x);
					
				}
				x->p->color = BLACK;
				x->p->p->color = RED;

				right_rotate(r, x->p->p);
				
			}
		}
		else {
			u = x->p->p->left;

			if (u->color == RED) {
				x->p->color = BLACK;
				u->color = BLACK;

				x->p->p->color = RED;

				x = x->p->p;
			}
			else {
				if (x == (x->p)->left) {
					x = x->p;
					right_rotate(r, x);
				
				}
				x->p->color = BLACK;
				x->p->p->color = RED;

				left_rotate(r, x->p->p);
				
			}
		}
	}
	r->root->color = BLACK;
}
void insert_node(RBPtr r, NodePtr tree, NodePtr n) {
	NodePtr p = r->nil;
	n->left = r->nil;
	n->right = r->nil;
	n->color = RED;


	while (tree != r->nil) {
		p = tree;
		if (n->key > tree->key) {
			tree = tree->right;
		}
		else {
			tree = tree->left;
		}
	}
	n->p = p;

	if (p == r->nil) {
		r->root = n;
	}
	else if (n->key > p->key) {
		p->right = n;
	}
	else {
		p->left = n;
	}

	insert_fixup(r, n);
}

void tree_transplant(RBPtr r, NodePtr t, NodePtr c) {
	if (t->p == r->nil) {
		r->root = c;

	}
	else if (t == t->p->left) {
		t->p->left = c;
	}
	else {
		t->p->right = c;
	}
	c->p = t->p;
}

void delete_fixup(RBPtr r, NodePtr x) {
	NodePtr s = NULL;

	while ((x != r->root) && (x->color == BLACK)) {
		if (x == x->p->left) {
			s = x->p->right;

			if (s->color == RED) {
				s->color = BLACK;
				x->p->color = RED;
				left_rotate(r, x->p);
				s = x->p->right;
			}

			if (s->left->color == BLACK&&s->right->color == BLACK) {
				s->color = RED;
				x = x->p;
			}
			else if (s->left->color == RED&&s->right->color == BLACK) {
				s->color = RED;
				s->left->color = BLACK;
				right_rotate(r, s);
				s = s->p->right;
			}
			if (s->right->color == RED) {
				s->color = x->p->color;
				s->right->color = BLACK;
				x->p->color = BLACK;
				left_rotate(r, x->p);

				x = r->root;
			}
		}
		else {
			s = x->p->left;

			if (s->color == RED) {
				s->color = BLACK;
				x->p->color = RED;
				right_rotate(r, x->p);

				s = x->p->left;
			}
			if (s->left->color == BLACK&&s->right->color == BLACK) {
				s->color = RED;
				x = x->p;
			}
			else if (s->right->color == RED&&s->left->color == BLACK) {
				s->color = RED;
				s->right->color = BLACK;
				left_rotate(r, s);
				s = x->p->left;
			}
			if (s->left->color == RED) {
				s->color = x->p->color;
				s->left->color = BLACK;
				x->p->color = BLACK;
				right_rotate(r, x->p);

				x = r->root;
			}
		}
	}
	x->color = BLACK;
}
void delete_node(RBPtr r, int key) {
	NodePtr target = r->root;
	NodePtr temp = NULL;
	NodePtr x = NULL;
	int t_clr;

	while (key != target->key) {
		if (target->key > key) {
			target = target->left;

		}
		else {
			target = target->right;
		}
	}
	t_clr = target->color;

	if (target->left == r->nil) {
		x = target->right;
		tree_transplant(r, target, target->right);
	}
	else if (target->right == r->nil) {
		x = target->left;
		tree_transplant(r, target, target->left);
	}
	else {
		temp = target->right;

		while (temp->left != r->nil) {
			temp = temp->left;
		}
		t_clr = temp->color;
		x = temp->right;

		tree_transplant(r, temp, temp->right);
		temp->right = target->right;
		temp->right->p = temp;

		tree_transplant(r, target, temp);
		temp->left = target->left;
		temp->left->p = temp;
		temp->color = target->color;
	}
	if (t_clr == 1) {
		delete_fixup(r, x);
	}
	free(target);

}

NodePtr search_node(RBPtr r,NodePtr tree, int value) {
	if (tree == r->nil) {
		return tree;
	}
	while (tree->key != value) {
		if (tree->key > value) {
			if (tree->left != r->nil)
				tree = tree->left;
			else {
				return tree;
			}
		}
		else {
			if (tree->right != r->nil)
				tree = tree->right;
			else
				return tree;
		}
	}
	
	return tree;
}
void rbt_inorder(RBPtr self, NodePtr tree) {
	if (tree == self->nil)
		return;
	else {
		rbt_inorder(self, tree->left);
		printf("%d ", tree->key);
		if (tree->color == RED) {
			printf("R\n");
		}
		else
			printf("B\n");
		rbt_inorder(self, tree->right);
	}

}
void numofnb(RBPtr self, NodePtr tree) {
	int i = 1;
	if (tree == self->nil)
		return;
	else {
		numofnb(self, tree->left);
		numofnb(self, tree->right);
	}
	if (tree->color == BLACK) {
		nb++;
	}	
}


int BlackHeight(RBPtr self, NodePtr root) { 
	int i = 0;
	while (root != self->nil) {
		if (root->color == BLACK) {
			i++;
		}
		root = root->left;
	}
	return i;
}
void rbt_print(RBPtr self, NodePtr tree, int level) {
	if (tree->right != self->nil)
		rbt_print(self, tree->right, level + 1);
	for (int i = 0; i < level; i++)
		printf("    ");
	printf("%d", tree->key);
	if (tree->color == RED)
		printf("[R]\n");
	else
		printf("[B]\n");
	if (tree->left != self->nil)
		rbt_print(self, tree->left, level + 1);
}
int main() {

	RBPtr rbt = create_nilnode();
	FILE *fp;
	int data;
	int not_exist = 0;
	int total = 0;
	fp = fopen("input.txt", "r");
		while (fscanf(fp, "%d", &data) != EOF) {
		
			if (data > 0) {
				int z =data;
					insert_node(rbt, rbt->root, node_alloc(data));
					total++;
					insert++;
			}
			else if(data<0){
				int y = -data;
				NodePtr x=search_node(rbt,rbt->root, y);
				if (x->key != y) {
					not_exist++;
				}
				else {
					delete_node(rbt, x->key);
					total--;
					delete++;

				}	
			}
			else {
				break;
				
			}
	}
	
		

	fclose(fp);
	WIN32_FIND_DATA ffd;
	TCHAR szDir[MAX_PATH];
	size_t length_of_arg;
	HANDLE hFind = INVALID_HANDLE_VALUE;
	DWORD dwError = 0;

	StringCchCopy(szDir, MAX_PATH, TEXT("."));

	_tprintf(TEXT("\nTarget directory is %s\n\n"), szDir);

	// Prepare string for use with FindFile functions.  First, copy the
	// string to a buffer, then append '\*' to the directory name.


	StringCchCat(szDir, MAX_PATH, TEXT("\\*"));

	// Find the first file in the directory.

	hFind = FindFirstFile(szDir, &ffd);

	if (INVALID_HANDLE_VALUE == hFind)
	{
		printf("Error: FindFirstFile\n");
	
	}

	// List all the files in the directory with some info about them.

	do
	{
		if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			_tprintf(TEXT("  %s   <DIR>\n"), ffd.cFileName);
		}
		else
		{
			_tprintf(TEXT("  %s\n"), ffd.cFileName);
		}
	} while (FindNextFile(hFind, &ffd) != 0);

	dwError = GetLastError();
	if (dwError != ERROR_NO_MORE_FILES)
	{
		printf("Error: FindFirstFile\n");
	}

	FindClose(hFind);

	printf("total = %d\n", total);
	printf("insert = %d\n",insert);
	printf("deleted = %d\n", delete);
	printf("miss = %d\n", not_exist);
	numofnb(rbt, rbt->root);
	printf("nb = %d\n", nb);
	printf("bh = %d\n\n", BlackHeight(rbt, rbt->root));
	rbt_inorder(rbt, rbt->root);
	printf("\n");
	rbt_print(rbt,rbt->root,0);
	
	return 0;
}
