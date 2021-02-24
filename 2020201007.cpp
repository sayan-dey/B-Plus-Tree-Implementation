#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <string>
#include <fstream>
#include <string.h>

#define ll long long

const ll null = 1000000001;
const ll ninf = -1000000001;

using namespace std;

typedef struct Node
{
	Node *parent;
	vector<ll> keys;
	vector<Node *> pointer;

	Node *buffer;

	bool isDead;
	bool isLeaf;
} Node;

Node *init_node(ll n, bool leaFlag)
{
	Node *node = new Node;
	node->parent = NULL;
	node->keys = vector<ll>(n, null);
	node->pointer = vector<Node *>(n + 1);
	node->isLeaf = leaFlag;
	node->isDead = false;
	node->buffer = NULL;
	return node;
}

void unDead(Node *parent, Node *child, ll value)
{
	if (parent != NULL)
	{
		bool flag = false;
		ll i = 1;
		while (i < parent->pointer.size())
		{
			if (parent->pointer[i] == child)
			{
				flag = true;
				parent->keys[i - 1] = value;
			}
			i++;
		}
		if (parent->isDead && flag)
			unDead(parent->parent, parent, value);
	}
}

Node *insert(Node *node, ll value)
{
	Node *root = NULL;
	ll node_size = node->keys.size();
	bool full_flag = false;
	if (node->keys[node_size - 1] != null)
		full_flag = true;

	switch (full_flag)
	{
	case true:
	{
		vector<ll> tempKeys = node->keys;
		vector<Node *> tempPointers = node->pointer;

		ll tempIndex = upper_bound(tempKeys.begin(), tempKeys.end(), value) - tempKeys.begin();
		ll ubp, newVal;
		tempKeys.insert(tempKeys.begin() + tempIndex, value);
		if (!node->isLeaf)
			tempPointers.insert(tempPointers.begin() + tempIndex + 1, node->buffer);
		Node *new_node = init_node(node_size, node->isLeaf);

		new_node->parent = node->parent;
		if (node->isLeaf)
		{
			new_node->pointer[node_size] = node->pointer[node_size];
			node->pointer[node_size] = new_node;

			double tempFloat = node_size + 1;
			ubp = (ll)ceil(tempFloat / 2);
		}
		else
		{
			double tempFloat = node_size + 2;
			ubp = (ll)ceil((tempFloat) / 2);

			ll i = 0;
			while (i < tempPointers.size())
			{
				if (i < ubp)
					node->pointer[i] = tempPointers[i];
				else
				{
					new_node->pointer[i - ubp] = tempPointers[i];
					new_node->pointer[i - ubp]->parent = new_node;
					if (i <= node_size)
						node->pointer[i] = NULL;
				}
				i++;
			}

			ubp--;
			newVal = tempKeys[ubp];
			tempKeys.erase(tempKeys.begin() + ubp);
		}

		ll i = 0;
		while (i < tempKeys.size())
		{
			if (i < ubp)
				node->keys[i] = tempKeys[i];
			else
			{
				new_node->keys[i - ubp] = tempKeys[i];
				if (i < node_size)
					node->keys[i] = null;
			}
			i++;
		}

		if (node->isDead && value != node->keys[0] && tempIndex < ubp)
		{
			node->isDead = false;
			unDead(node->parent, node, value);
		}

		tempIndex = upper_bound(new_node->keys.begin(), new_node->keys.end(), node->keys[ubp - 1]) - new_node->keys.begin();
		if (new_node->keys[tempIndex] == null)
		{
			newVal = new_node->keys[0];
			new_node->isDead = true;
		}
		else if (node->isLeaf)
			newVal = new_node->keys[tempIndex];

		if (node->parent != NULL)
		{
			node->parent->buffer = new_node;
			root = insert(node->parent, newVal);
		}
		else
		{
			root = init_node(node_size, false);
			root->keys[0] = newVal;
			root->pointer[0] = node;
			root->pointer[1] = new_node;
			node->parent = root;
			new_node->parent = root;
		}
	}
	break;
	case false:
	{
		bool insert_flag = false;
		ll tempKey = null;
		Node *tempPointer = NULL;

		ll i = 0;
		while (i < node_size)
		{
			switch (insert_flag)
			{
			case true:
			{
				swap(node->keys[i], tempKey);
				if (!node->isLeaf)
					swap(node->pointer[i + 1], tempPointer);
			}
			break;
			case false:
			{
				if (value < node->keys[i] || node->keys[i] == null)
				{
					insert_flag = true;
					tempKey = node->keys[i];
					node->keys[i] = value;
					if (!node->isLeaf)
					{
						tempPointer = node->pointer[i + 1];
						node->pointer[i + 1] = node->buffer;
					}
				}
				if (value != node->keys[0] && node->isDead)
				{
					node->isDead = false;
					unDead(node->parent, node, value);
				}
			}
			break;
			}
			i++;
		}
	}
	break;
	}
	return root;
}

Node *lookup(Node *node, ll value, bool up)
{
	while (!node->isLeaf)
	{
		ll lb = ninf, ub, node_size = node->keys.size(), index;
		ll i = 0;
		while (i < node_size)
		{
			if (node->keys[i] == null)
			{
				index = i;
				break;
			}
			ub = node->keys[i];
			if (lb <= value && value < ub)
			{
				index = i;
				break;
			}
			else if (lb <= value && value == ub && !up && node->pointer[i + 1]->isDead)
			{
				index = i;
				break;
			}
			else
				index = i + 1;
			lb = ub;
			i++;
		}
		node = node->pointer[index];
	}
	return node;
}

Node *insert_val(Node *root, ll value)
{
	Node *temp;
	temp = insert(lookup(root, value, true), value);
	if (temp != NULL)
		root = temp;
	return root;
}

bool find(Node *leaf, ll val)
{
	ll i = 0;
	while (i < leaf->keys.size())
	{
		if (leaf->keys[i] == val)
			return true;
		i++;
	}
	return false;
}

ll count(Node *leaf, ll val)
{
	ll count = 0, flag = false, last_index = leaf->pointer.size() - 1;
	while (leaf != NULL)
	{
		ll i = 0;
		while (i < leaf->keys.size())
		{
			if (leaf->keys[i] == val)
				count++;
			else if (leaf->keys[i] > val && leaf->keys[i] != null)
			{
				flag = true;
				break;
			}
			i++;
		}
		if (flag)
			break;
		leaf = leaf->pointer[last_index];
	}
	return count;
}

ll range(Node *leaf, ll lb, ll ub)
{
	ll count = 0, flag = false, last_index = leaf->pointer.size() - 1;
	while (leaf != NULL)
	{
		ll i = 0;
		while (i < leaf->keys.size())
		{
			if (leaf->keys[i] >= lb && leaf->keys[i] <= ub)
			{
				// cout << leaf->keys[i] << " ";
				count++;
			}
			else if (leaf->keys[i] > ub && leaf->keys[i] != null)
			{
				flag = true;
				break;
			}
			i++;
		}
		if (flag)
			break;
		leaf = leaf->pointer[last_index];
	}

	return count;
}

int main(int argc, char const *argv[])
{
	ll n = 3; //ORDER

	Node *root = init_node(n, true);
	ll i, j, val1, val2;

	//Eg: string st = "INSERT X";
	string st;

	// Read from the text file
	ifstream MyReadFile(argv[1]);
	// Write to output file
	ofstream MyWriteFile("2020201007_output.txt");

	// Use a while loop together with the getline() function to read the file line by line
	while (getline(MyReadFile, st))
	{

		//removing leading and trailing whitespaces
		i = 0;
		while (st[0] == ' ')
		{
			st.erase(st.begin());
		}
		while (st[st.size() - 1] == ' ')
		{
			st.erase(st.end() - 1);
		}

		//finding the command
		char c = st[0];
		c = (char)toupper(c);
		for (i = 0; i < st.size(); i++)
		{
			if (st[i] == ' ')
				break;
		}
		string temp = "";
		for (j = i + 1; st[j] != ' ' && j < st.size(); j++)
		{
			temp += st[j];
		}

		val1 = stoll(temp);

		temp = "";
		for (i = j + 1; i < st.size(); i++)
		{
			temp += st[i];
		}

		try
		{
			val2 = stoll(temp);
		}
		catch (exception e)
		{
			/*  */
		}

		if (c == 'I')
		{
			root = insert_val(root, val1);
		}
		else if (c == 'F')
		{
			if (find(lookup(root, val1, false), val1))
				MyWriteFile << "YES\n";
			else
				MyWriteFile << "NO\n";
		}
		else if (c == 'C')
		{
			MyWriteFile << count(lookup(root, val1, false), val1);
			MyWriteFile << "\n";
		}
		else if (c == 'R')
		{
			MyWriteFile << range(lookup(root, val1, false), val1, val2);
			MyWriteFile << "\n";
		}
	}

	// Close the files
	MyReadFile.close();
	MyWriteFile.close();

	return 0;
}
