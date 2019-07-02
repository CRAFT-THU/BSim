
#ifndef ASSERT_H
#define ASSERT_H

#include "gtest/gtest.h"

template<typename T>
::testing::AssertionResult ASSERT_EQ_ARRAY(const T &expected, const T &actual, size_t size)
{
	for (size_t i=0; i < size; ++i){
		if (expected[i] != actual[i]){
			return ::testing::AssertionFailure() << "array[" << i
				<< "] (" << actual[i] << ") != expected[" << i
				<< "] (" << expected[i] << ")";
		}
	}

	return ::testing::AssertionSuccess();
}

#endif // ASSERT_H
