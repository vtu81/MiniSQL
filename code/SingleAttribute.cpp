#include "SingleAttribute.h"

SingleAttribute::SingleAttribute(string n, int t, bool i) {
	name = n;
	type = t;
	ifUnique = i;
	index = "";

}

void SingleAttribute::SetIndex(string indexName) {
	if (ifUnique == false) {
		cout << "Not unique attribute!" << endl;
		return;
	}
	index = indexName;
}

int SingleAttribute::getType() {
	return type;
}

string SingleAttribute::indexNameGet() {
	return index;
}

int SingleAttribute::getSize() {
	if (type == TYPE_FLOAT) {
		return sizeof(float);
	}
	else if (type == TYPE_INT) {
		return sizeof(int);
	}
	else {
		return sizeof(char)*type;
}

