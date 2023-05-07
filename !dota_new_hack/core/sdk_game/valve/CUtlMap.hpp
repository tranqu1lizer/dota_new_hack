#pragma once

#include "CUtlRBTree.hpp"

//-----------------------------------------------------------------------------
// An associative container. Pretty much identical to std::map.
//-----------------------------------------------------------------------------
template <typename K, typename T, typename I = unsigned short>
class CUtlMap
{
public:
	typedef K KeyType_t;
	typedef T ElemType_t;
	typedef I IndexType_t;

	// Less func typedef
	// Returns true if the first parameter is "less" than the second
	typedef bool (*LessFunc_t)(const KeyType_t&, const KeyType_t&);

	// constructor, destructor
	// Left at growSize = 0, the memory will first allocate 1 element and double in size
	// at each increment.
	// LessFunc_t is required, but may be set after the constructor using SetLessFunc() below
	CUtlMap(int growSize = 0, int initSize = 0, LessFunc_t lessfunc = 0)
		: m_Tree(growSize, initSize, CKeyLess(lessfunc))
	{
	}

	CUtlMap(LessFunc_t lessfunc)
		: m_Tree(CKeyLess(lessfunc))
	{
	}

	// gets particular elements
	ElemType_t& Element(IndexType_t i) { return m_Tree.Element(i).elem; }
	const ElemType_t& Element(IndexType_t i) const { return m_Tree.Element(i).elem; }
	ElemType_t& operator[](IndexType_t i) { return m_Tree.Element(i).elem; }
	const ElemType_t& operator[](IndexType_t i) const { return m_Tree.Element(i).elem; }
	KeyType_t& Key(IndexType_t i) { return m_Tree.Element(i).key; }
	const KeyType_t& Key(IndexType_t i) const { return m_Tree.Element(i).key; }

	// Num elements
	unsigned int Count() const { return m_Tree.Count(); }

	// Max "size" of the vector
	IndexType_t  MaxElement() const { return m_Tree.MaxElement(); }

	// Checks if a node is valid and in the map
	bool  IsValidIndex(IndexType_t i) const { return m_Tree.IsValidIndex(i); }

	// Checks if the map as a whole is valid
	bool  IsValid() const { return m_Tree.IsValid(); }

	// Invalid index
	static IndexType_t InvalidIndex() { return CTree::InvalidIndex(); }

	// Find method
	IndexType_t  Find(const KeyType_t& key) const
	{
		Node_t dummyNode;
		dummyNode.key = key;
		return m_Tree.Find(dummyNode);
	}

	// Iteration
	IndexType_t  FirstInorder() const { return m_Tree.FirstInorder(); }
	IndexType_t  NextInorder(IndexType_t i) const { return m_Tree.NextInorder(i); }
	IndexType_t  PrevInorder(IndexType_t i) const { return m_Tree.PrevInorder(i); }
	IndexType_t  LastInorder() const { return m_Tree.LastInorder(); }

	struct Node_t
	{
		Node_t()
		{
		}

		Node_t(const Node_t& from)
			: key(from.key),
			elem(from.elem)
		{
		}

		KeyType_t	key;
		ElemType_t	elem;
	};

	class CKeyLess
	{
	public:
		CKeyLess(LessFunc_t lessFunc) : m_LessFunc(lessFunc) {}

		bool operator!() const
		{
			return !m_LessFunc;
		}

		bool operator()(const Node_t& left, const Node_t& right) const
		{
			return m_LessFunc(left.key, right.key);
		}

		LessFunc_t m_LessFunc;
	};

	typedef CUtlRBTree<Node_t, I, CKeyLess> CTree;

	CTree* AccessTree() { return &m_Tree; }

protected:
	CTree m_Tree;
};