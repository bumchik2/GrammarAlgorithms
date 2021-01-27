#pragma once

#include <string>
#include <iostream>
#include <memory>
#include <algorithm>
#include <exception>

using std::make_shared;
using std::shared_ptr;
using std::string;
using std::cin;
using std::cout;
using std::endl;
using std::swap;
using std::runtime_error;

enum PascalType {
	None,
	Integer,
	Real,
	String,
	Boolean,
	Char
};

class PascalVariable {
public:
	PascalVariable() = default;
	virtual ~PascalVariable();
	virtual PascalType getType() const;
	virtual void print() const {};
	void* data = nullptr;
};

class PascalInteger : public PascalVariable {
public:
	using PascalVariable::data;
	PascalInteger(const PascalInteger& pascal_integer);
	PascalInteger();
	PascalInteger(int n);
	~PascalInteger();
	PascalType getType() const override;
	void print() const override;
};

class PascalReal : public PascalVariable {
public:
	PascalReal(const PascalInteger& pascal_integer);
	PascalReal(const PascalReal& pascal_real);
	PascalReal(double d);
	PascalReal();
	~PascalReal();
	PascalType getType() const override;
	void print() const override;
};

class PascalChar : public PascalVariable {
public:
	PascalChar();
	PascalChar(char c);
	~PascalChar();
	PascalType getType() const override;
	void print() const override;
};

class PascalString : public PascalVariable {
public:
	PascalString();
	PascalString(const string& s);
	~PascalString();
	PascalType getType() const override;
	void print() const override;
};

class PascalBoolean : public PascalVariable {
public:
	PascalBoolean();
	PascalBoolean(bool b);
	~PascalBoolean();
	PascalType getType() const override;
	void print() const override;
};

PascalType getType(const string& s);
string getTypeName(PascalType type);

shared_ptr<PascalVariable> sum(shared_ptr<PascalVariable> a, shared_ptr<PascalVariable> b);
shared_ptr<PascalVariable> multiply(shared_ptr<PascalVariable> a, shared_ptr<PascalVariable> b);
shared_ptr<PascalVariable> divide(shared_ptr<PascalVariable> a, shared_ptr<PascalVariable> b);
shared_ptr<PascalVariable> diff(shared_ptr<PascalVariable> a, shared_ptr<PascalVariable> b);
shared_ptr<PascalVariable> negative(shared_ptr<PascalVariable> a);

shared_ptr<PascalBoolean> toBoolean(shared_ptr<PascalVariable> a);

shared_ptr<PascalBoolean> isLess(shared_ptr<PascalVariable> a, shared_ptr<PascalVariable> b);
shared_ptr<PascalBoolean> isGreater(shared_ptr<PascalVariable> a, shared_ptr<PascalVariable> b);
shared_ptr<PascalBoolean> areEqual(shared_ptr<PascalVariable> a, shared_ptr<PascalVariable> b);
shared_ptr<PascalBoolean> areNotEqual(shared_ptr<PascalVariable> a, shared_ptr<PascalVariable> b);

PascalType leastCommonType(PascalType type1, PascalType type2);
shared_ptr<PascalVariable> cast(shared_ptr<PascalVariable> from, PascalType new_type);
shared_ptr<PascalVariable> initializePascalVariable(PascalType type);

bool isTrue(shared_ptr<PascalVariable> a);
