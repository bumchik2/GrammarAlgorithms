#include "pascal_variable.h"

#include <string>
#include <iostream>
#include <memory>
#include <algorithm>
#include <exception>
#include <sstream>
#include <unordered_map>

using std::make_shared;
using std::shared_ptr;
using std::string;
using std::cin;
using std::cout;
using std::endl;
using std::swap;
using std::runtime_error;
using std::ostringstream;
using std::unordered_map;

const unordered_map<string, PascalType> string_to_type = {
	{"integer", Integer},
	{"real", Real},
	{"string", String},
	{"boolean", Boolean},
	{"char", Char},
};

const unordered_map<PascalType, string> type_to_string = {
	{Integer, "integer"},
	{Real, "real"},
	{String, "string"},
	{Boolean, "boolean"},
	{Char, "char"}
};

string getTypeName(PascalType type) {
	return type_to_string.at(type);
}

PascalType getType(const string& s) {
	return string_to_type.at(s);
}

PascalType leastCommonType(PascalType type1, PascalType type2) {
	if (type1 == type2) {
		return type1;
	} else if (((type1 == Integer) && type2 == Real) || ((type2 == Integer) && type1 == Real)) {
		return Real;
	}else if ((type1 == String && type2 == Char) || (type2 == Char && type1 == String)) {
		return String;
	} else {
		throw runtime_error("bad leastCommonType request");
	}
}

shared_ptr<PascalVariable> cast(shared_ptr<PascalVariable> from, PascalType new_type) {
	if (from->getType() == new_type) {
		return from;
	} else if (from->getType() == Integer && new_type == Real) {
		return make_shared<PascalReal>(*reinterpret_cast<int*>(from->data));
	} else if (from->getType() == Char && new_type == String) {
		char c = *reinterpret_cast<char*>(from->data);
		string s;
		s += c;
		return make_shared<PascalString>(s);
	} else if (from->getType() == String && new_type == Char) {
		string s = *reinterpret_cast<string*>(from->data);
		if (s.size() == 1) {
			return make_shared<PascalChar>(s[0]);
		}
	} else if (new_type == Boolean) {
		return toBoolean(from);
	}
	ostringstream os;
	os << "bad cast from " << getTypeName(from->getType()) << " to " << getTypeName(new_type) << endl;
	throw runtime_error(os.str());
}

PascalVariable::~PascalVariable() {
}
PascalType PascalVariable::getType() const {
	return None;
}

PascalInteger::PascalInteger(const PascalInteger& pascal_integer) {
	data = new int(*reinterpret_cast<int*>(pascal_integer.data));
}
PascalInteger::PascalInteger() {
	data = new int(0);
}
PascalInteger::PascalInteger(int n) {
	data = new int(n);
}
PascalInteger::~PascalInteger() {
	delete reinterpret_cast<int*>(data);
}
PascalType PascalInteger::getType() const {
	return Integer;
}
void PascalInteger::print() const {
	cout << *reinterpret_cast<int*>(data);
}

PascalReal::PascalReal(const PascalInteger& pascal_integer) {
	data = new double(*reinterpret_cast<int*>(pascal_integer.data));
}
PascalReal::PascalReal(const PascalReal& pascal_real) {
	data = new double(*reinterpret_cast<double*>(pascal_real.data));
}
PascalReal::PascalReal(double d) {
	data = new double(d);
}
PascalReal::PascalReal() {
	data = new double(0);
}
PascalReal::~PascalReal() {
	delete reinterpret_cast<double*>(data);
}
PascalType PascalReal::getType() const {
	return Real;
}
void PascalReal::print() const {
	cout << *reinterpret_cast<double*>(data);
}

PascalChar::PascalChar() {
	data = new char('\0');
}
PascalChar::PascalChar(char c) {
	data = new char(c);
}
PascalChar::~PascalChar() {
	delete reinterpret_cast<char*>(data);
}
PascalType PascalChar::getType() const {
	return Char;
}
void PascalChar::print() const {
	cout << *reinterpret_cast<char*>(data);
}

PascalString::PascalString() {
	data = new string;
}
PascalString::PascalString(const string& s) {
	data = new string(s);
}
PascalString::~PascalString() {
	delete reinterpret_cast<string*>(data);
}
PascalType PascalString::getType() const {
	return String;
}
void PascalString::print() const {
	cout << *reinterpret_cast<string*>(data);
}

PascalBoolean::PascalBoolean() {
	data = new bool(false);
}
PascalBoolean::PascalBoolean(bool b) {
	data = new bool(b);
}
PascalBoolean::~PascalBoolean() {
	delete reinterpret_cast<bool*>(data);
}
PascalType PascalBoolean::getType() const {
	return Boolean;
}
void PascalBoolean::print() const {
	cout << (*reinterpret_cast<bool*>(data) ? "True" : "False");
}

bool isTrue(shared_ptr<PascalVariable> pascal_variable) {
	return *reinterpret_cast<bool*>(toBoolean(pascal_variable)->data);
}

shared_ptr<PascalVariable> initializePascalVariable(PascalType type) {
	switch (type) {
	case Integer: {
		return make_shared<PascalInteger>();
		break;
	} case Real: {
		return make_shared<PascalReal>();
		break;
	} case String: {
		return make_shared<PascalString>();
		break;
	} case Char: {
		return make_shared<PascalChar>();
		break;
	} case Boolean: {
		return make_shared<PascalBoolean>();
		break;
	} default: {
		throw runtime_error("bad type in declaration");
		break;
	}
	}
}

shared_ptr<PascalBoolean> isLess(shared_ptr<PascalVariable> a, shared_ptr<PascalVariable> b) {
	PascalType common_type = leastCommonType(a->getType(), b->getType());
	a = cast(a, common_type);
	b = cast(b, common_type);
	bool comparison_result = false;
	if (common_type == Integer) {
		comparison_result = *reinterpret_cast<int*>(a->data) < *reinterpret_cast<int*>(b->data);
	} else if (common_type == Real) {
		comparison_result = *reinterpret_cast<double*>(a->data) < *reinterpret_cast<double*>(b->data);
	} else if (common_type == String) {
		comparison_result = *reinterpret_cast<string*>(a->data) < *reinterpret_cast<string*>(b->data);
	} else if (common_type == Char) {
		comparison_result = *reinterpret_cast<char*>(a->data) < *reinterpret_cast<char*>(b->data);
	} else if (common_type == Boolean) {
		comparison_result = *reinterpret_cast<bool*>(a->data) < *reinterpret_cast<bool*>(b->data);
	} else {
		throw runtime_error("unexpected error while comparing values");
	}
	return make_shared<PascalBoolean>(comparison_result);
}
shared_ptr<PascalBoolean> isGreater(shared_ptr<PascalVariable> a, shared_ptr<PascalVariable> b) {
	return isLess(b, a);
}
shared_ptr<PascalBoolean> areEqual(shared_ptr<PascalVariable> a, shared_ptr<PascalVariable> b) {
	return make_shared<PascalBoolean>(!isTrue(isLess(a, b)) && !isTrue(isLess(b, a)));
}
shared_ptr<PascalBoolean> areNotEqual(shared_ptr<PascalVariable> a, shared_ptr<PascalVariable> b) {
	return make_shared<PascalBoolean>(!*reinterpret_cast<bool*>(areEqual(a, b)->data));
}

shared_ptr<PascalVariable> sum(shared_ptr<PascalVariable> a, shared_ptr<PascalVariable> b) {
	PascalType common_type = leastCommonType(a->getType(), b->getType());
	a = cast(a, common_type);
	b = cast(b, common_type);
	if (a->getType() == Integer) {
		return make_shared<PascalInteger>(*reinterpret_cast<int*>(a->data) + *reinterpret_cast<int*>(b->data));
	} else if (a->getType() == Real) {
		return make_shared<PascalReal>(*reinterpret_cast<double*>(a->data) + *reinterpret_cast<double*>(b->data));
	} else if (a->getType() == String) {
		return make_shared<PascalString>(*reinterpret_cast<string*>(a->data) + *reinterpret_cast<string*>(b->data));
	} else {
		throw runtime_error("unexpected erorr in sum");
	}
}

shared_ptr<PascalVariable> multiply(shared_ptr<PascalVariable> a, shared_ptr<PascalVariable> b) {
	PascalType common_type = leastCommonType(a->getType(), b->getType());
	a = cast(a, common_type);
	b = cast(b, common_type);
	if (a->getType() == Integer) {
		return make_shared<PascalInteger>(*reinterpret_cast<int*>(a->data) * *reinterpret_cast<int*>(b->data));
	} else if (a->getType() == Real) {
		return make_shared<PascalReal>(*reinterpret_cast<double*>(a->data) * *reinterpret_cast<double*>(b->data));
	} else {
		throw runtime_error("unexpected erorr in sum");
	}
}

shared_ptr<PascalVariable> diff(shared_ptr<PascalVariable> a, shared_ptr<PascalVariable> b) {
	PascalType common_type = leastCommonType(a->getType(), b->getType());
	a = cast(a, common_type);
	b = cast(b, common_type);
	if (a->getType() == Integer) {
		return make_shared<PascalInteger>(*reinterpret_cast<int*>(a->data) - *reinterpret_cast<int*>(b->data));
	} else if (a->getType() == Real) {
		return make_shared<PascalReal>(*reinterpret_cast<double*>(a->data) - *reinterpret_cast<double*>(b->data));
	} else {
		throw runtime_error("unexpected erorr in sum");
	}
}

shared_ptr<PascalVariable> divide(shared_ptr<PascalVariable> a, shared_ptr<PascalVariable> b) {
	PascalType common_type = leastCommonType(a->getType(), b->getType());
	a = cast(a, common_type);
	b = cast(b, common_type);
	if (a->getType() == Integer) {
		return make_shared<PascalInteger>(*reinterpret_cast<int*>(a->data) / *reinterpret_cast<int*>(b->data));
	} else if (a->getType() == Real) {
		return make_shared<PascalReal>(*reinterpret_cast<double*>(a->data) / *reinterpret_cast<double*>(b->data));
	}else {
		throw runtime_error("unexpected erorr in sum");
	}
}

shared_ptr<PascalBoolean> toBoolean(shared_ptr<PascalVariable> pascal_variable) {
	PascalType type = pascal_variable->getType();
	if (type != Boolean && type != Integer && type != Real) {
		throw runtime_error("bad toBoolean cast");
	} else if (type == Boolean) {
		return make_shared<PascalBoolean>(*reinterpret_cast<bool*>(pascal_variable->data));
	} else if (type == Integer) {
		return make_shared<PascalBoolean>(*reinterpret_cast<int*>(pascal_variable->data) != 0);
	} else {
		return make_shared<PascalBoolean>(*reinterpret_cast<double*>(pascal_variable->data) != 0);
	}
}

shared_ptr<PascalVariable> negative(shared_ptr<PascalVariable> pascal_variable) {
	PascalType type = pascal_variable->getType();
	if (type != Integer && type != Real) {
		throw runtime_error("bad type in negative function");
	} else if (type == Integer) {
		return make_shared<PascalInteger>(-*(reinterpret_cast<int*>(pascal_variable->data)));
	} else {
		return make_shared<PascalReal>(-*(reinterpret_cast<double*>(pascal_variable->data)));
	}
}
