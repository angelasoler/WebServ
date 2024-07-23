#!/bin/sh

# class: Orthodox Canonical Form

if [ $# -eq 0 ]; then
	echo "usage: script [CLASS]"
	exit 1
fi

ARG1=$1

#config
CLASS=$(echo "$ARG1" | awk '{print toupper(substr($0,1,1)) substr($0,2)}')
CLASS_UPPER=$(echo "$CLASS" | tr '[:lower:]' '[:upper:]')
FILE_CPP=$CLASS.cpp
FILE_HPP=$CLASS.hpp
FILE_TEST=test.cpp

echo "
#ifndef "$CLASS_UPPER"_HPP
#define "$CLASS_UPPER"_HPP

#include <iostream>

class $CLASS
{
	private:
	public:
		$CLASS(void);
		~$CLASS(void);
};

#endif /* "$CLASS_UPPER"_HPP */" >> $FILE_HPP

echo "
#include "'"'$FILE_HPP'"'"

"$CLASS"::"$CLASS"(void) {}

"$CLASS"::~"$CLASS"(void) {}" >> $FILE_CPP

echo "
#include \"$FILE_HPP\"
#include \"doctest.h\"

TEST_SUITE(\"Class $CLASS\")
{
	TEST_CASE(\"\")
	{
	}
}" >> $FILE_TEST
