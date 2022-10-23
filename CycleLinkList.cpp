//#include "CycleLinkList.h"

//CycleLinkList::CycleLinkList()
//{
//}
//
//CycleLinkList::~CycleLinkList()
//{
//}
//
//inline void CycleLinkList::Linklist_init(linknode * head)
//{
//	head->prev = head->next;
//}
//
//inline void CycleLinkList::Linklist_add_front(linknode * head, linknode * node)
//{
//	node->prev = head;
//	node->next = head->next;
//	head->next->prev = node;
//	head->next = node;
//
//}
//
//inline void CycleLinkList::Linklist_add_back(linknode * head, linknode * node)
//{
//	node->next = head;
//	node->prev = head->prev;
//	head->prev->next = node;
//	head->prev = node;
//	
//}
//
//inline int CycleLinkList::Linklist_is_empty(linknode * head)
//{
//	return head == head->next;
//}
//
//inline void CycleLinkList::Linklist_remote(linknode * node)
//{
//	node->next->prev = node->prev;
//	node->prev->next = node->next;
//	Linklist_init(node);
//}
//
//inline void CycleLinkList::Linklist_splice(linknode * head1, linknode * head2)
//{
//	if (!Linklist_is_empty(head1))
//	{
//		linknode * first = head1->next;	
//		linknode *last = head1->prev;
//		linknode *at = head2->next;
//		first->prev = head2;
//		head2->next = first;
//		last->next = at;
//		at->prev = last;
//		Linklist_init(head1);
//	}
//}
