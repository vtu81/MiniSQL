#include "Attribute.h"

Attribute::Attribute(string n, int t, bool i) {
	name = n;
	type = t;
	ifUnique = i;
	index = "";

}

int Attribute::getType() {
	return type;
}

int Attribute::getSize() {
	if (type == TYPE_FLOAT) {
		return sizeof(float);
	}
	else if (type == TYPE_INT) {
		return sizeof(int);
	}
	else {
		return sizeof(char)*type;
}