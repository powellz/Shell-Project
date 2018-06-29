
#include <stdio.h>

class TreeNode {
public:
	int _value;
	TreeNode * _left;
	TreeNode * _right;
};

class Tree {
	TreeNode * _root;
public:
	Tree();

	// Insert number in tree
	bool insert(int val);

	// Return true if val is in tree
	bool find(int val);

	// Print tree
	void print();

	// Auxilia method to print tree
	void printAuxiliar(TreeNode * node);
};

Tree::Tree()
{
	_root = NULL;
}

#if 1
// Solution using "prev"  pointer

// Insert number in tree
bool
Tree::insert(int val)
{
        TreeNode * n = _root;
	TreeNode * prev = NULL;
        while (n != NULL) {
                if (val < n->_value) {
			prev = n;
                        n = n->_left;
                }
                else if (val > n->_value) {
			prev = n;
                        n = n->_right;
                }
                else {
                  return false;
                }
        }
	// Create new node
        n = new TreeNode();
        n->_value = val;
        n->_left = NULL;
        n->_right = NULL;

	// Attach new node
	if (prev==NULL) {
		// Tree was empty
		_root = n;
	}
	else {
		// tree not empty. Check what child 
		// to insert it to.
		if (val < prev->_value) {
			prev->_left = n;
		}
		else {
			prev->_right = n;
		}
	}

        return true;
}

#else

// Alternative solution using double pointers
// Insert number in tree
bool 
Tree::insert(int val) 
{
	TreeNode ** n = &_root;
        while (*n != NULL) {
                if (val < (*n)->_value) {
                        n = &(*n)->_left;
                }
                else if (val > (*n)->_value) {
                        n = &(*n)->_right;
                }
		else {
		  return false;
		}
        }
        *n = new TreeNode();
        (*n)->_value = val;
        (*n)->_left = NULL;
        (*n)->_right = NULL;
	return true;
}
#endif

bool 
Tree::find(int val)
{
        TreeNode * n = _root;
        while (n != NULL) {
                if (val < n->_value) {
                        n = n->_left;
                }
                else if (val > n->_value) {
                        n = n->_right;
                }
		else {
		  return true;
		}
        }

	return false;
}



// Print tree
void
Tree::print()
{
	printAuxiliar(_root);
}

void
Tree::printAuxiliar(TreeNode * node)
{
	if (node==NULL) return;
	printAuxiliar(node->_left);
	printf("%d\n", node->_value);
	printAuxiliar(node->_right);
}

int
main(int argc, char **argv)
{
	Tree tree;
	tree.insert(5);
	tree.insert(9);
	tree.insert(3);
	tree.insert(10);
	tree.insert(11);

	tree.print();

	bool found = tree.find(3);
	if (found) {
		printf("found 3\n");
	}
	else {
		printf("did not find 3\n");
	}

	found = tree.find(7);
	if (found) {
		printf("found 7\n");
	}
	else {
		printf("did not find 7\n");
	}


}



