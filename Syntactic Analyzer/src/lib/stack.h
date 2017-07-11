#ifndef __STACK_H__
#define __STACK_H__

#include <stack>

template <class T>
class stack
{
	std::stack<T> m_stack;
public:
	void push(const T &value) { m_stack.push(value); }
	const T pop() { 
		const T val = m_stack.top();
		m_stack.pop();
		return val;
	}

	const T& top() const { return m_stack.top(); }
};

#endif