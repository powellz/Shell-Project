
#include <cstdlib>
#include <string.h>
#include <stdio.h>

#include "TreeDictionary.h"

TreeDictionary::TreeDictionary()
{
	_root = NULL;
    _numberOfEntries = 0;
}

// Add a record to the dictionary. Overwrite data if key
// already exists. It returns false if key already exists
bool 
TreeDictionary::addRecord( const char * key, int data) {
    TreeNode * n = _root;
	TreeNode * prev = NULL;
	int temp = 0;
        while (n != NULL) {
                if (strcmp(key, n->_key) < 0) {
						prev = n;
                        n = n->_left;
						temp = 1;
						//printf("left!");
                }
                else if (strcmp(key, n->_key) > 0) {
						prev = n;
                        n = n->_right;
						temp = 0;
						//printf("right!");
                }
                else {
                	n->_data = data;
					return false;
                }
        }
	// Create new node
        n = new TreeNode();
        n->_key = strdup(key);
        n->_data = data;
        n->_left = NULL;
        n->_right = NULL;

	// Attach new node
	if (prev==NULL) {
		// Tree was empty
		_root = n;
		_numberOfEntries++;
	}
	else {
		// tree not empty. Check what child 
		// to insert it to.
		if (temp == 1) {
			prev->_left = n;
						//printf("left!");
		}
		else {
			prev->_right = n;
						//printf("right!");
		}
        _numberOfEntries++;
	}
	//printIndented();
    return true;
}

int
TreeDictionary::numberOfEntries() {
  return _numberOfEntries;
}

// Find a key in the dictionary and return corresponding record.
bool 
TreeDictionary::findRecord( const char * key, int & data) {
        TreeNode * n = _root;
        while (n != NULL) {
			if (strcmp(key, n->_key) < 0) {
                    n = n->_left;
                }
            else if (strcmp(key, n->_key) > 0) {
                    n = n->_right;
                }
			else {			
				data = n->_data;
				return true;
			}
        }
	return false;
}

void 
TreeDictionary::printIndented(TreeNode * node, int level) {
	int temp = level;
	while (temp > 0)
	{
		temp--;
		printf("  ");
	}
	if (node == NULL)
	{
		printf("NULL\n");
		return;
	}	
	printf("%s:%d\n", node->_key, node->_data);
	printIndented(node->_left, level + 1);
	printIndented(node->_right, level + 1);
}

void 
TreeDictionary::printIndented(){
	printIndented(_root, 0);
}

void 
TreeDictionary::print(TreeNode * node) {
	if (node == NULL)
		return;
	printf("%s:%d\n", node->_key, node->_data);
	print(node->_left);
	print(node->_right);
}

void TreeDictionary::print()
{
	print(_root);
}

// Max depth in tree
void
TreeDictionary::computeDepthHelper(TreeNode * node, int depth, int & currentMaxDepth) {
	if (node == NULL)
		return;
	if (depth > currentMaxDepth)
		currentMaxDepth = depth;
	computeDepthHelper(node->_left, depth + 1, currentMaxDepth);
	computeDepthHelper(node->_right, depth + 1, currentMaxDepth);
	return;
}

int 
TreeDictionary::maxDepth() {
	int CMD = -1;
	computeDepthHelper(_root, 0, CMD);
  return CMD;
}


TreeDictionary::~TreeDictionary()
{
	delete _root->_left;
	delete _root->_right;
    delete _root;
}

// Minimum key. The leftmost node
const char * 
TreeDictionary::minimumKey()
{
	TreeNode * n = _root;
	TreeNode * prev = NULL;
	int temp = 0;
        while (n != NULL) {
			prev = n;
            n = n->_left;
		}
   return prev->_key;
}

// Maximum Key. The rightmost node
const char * 
TreeDictionary::maximumKey()
{
	TreeNode * n = _root;
	TreeNode * prev = NULL;
	int temp = 0;
        while (n != NULL) {
			prev = n;
            n = n->_right;
		}
   return prev->_key;
}

void
TreeDictionary::collectKeys(TreeNode * node, const char **keys, int & nkeys)
{
	if (node == NULL)
		return;
	collectKeys(node->_left, keys, nkeys);
	keys[nkeys] = node->_key;
	nkeys++;
	collectKeys(node->_right, keys, nkeys);
}

const char ** 
TreeDictionary::keys( int & nkeys)
{
	const char** a = (const char**)malloc(sizeof(char*));
	nkeys = 0;
	collectKeys(_root, a, nkeys);
	
  	return a;
}




