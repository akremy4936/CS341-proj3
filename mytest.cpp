#include "squeue.h"
#include <iostream>
#include <cstdlib>
using namespace std;

//Priority functions                                                                                                                                                              
int priorityFn1(const Post & post) {
    int priority = post.getNumLikes() + post.getInterestLevel();
    if (priority >= 1 && priority <= 510) {
        return priority;
    } else {
        return 0;
    }
}

int priorityFn2(const Post & post) {
    int priority = post.getPostTime() + post.getConnectLevel();
    if (priority >= 2 && priority <= 55) {
        return priority;
    } else {
        return 0;
    }
}

class Tester {
public:
    bool checkMinHeapProperty(Post* node, prifn_t priorityFn) {
        if (!node) return true;
        int currentPriority = priorityFn(*node);
        if (node->m_left && currentPriority > priorityFn(*node->m_left)) return false;
        if (node->m_right && currentPriority > priorityFn(*node->m_right)) return false;
        return checkMinHeapProperty(node->m_left, priorityFn) &&
               checkMinHeapProperty(node->m_right, priorityFn);
    }

    bool checkMaxHeapProperty(Post* node, prifn_t priorityFn) {
        if (!node) return true;
        int currentPriority = priorityFn(*node);
        if (node->m_left && currentPriority < priorityFn(*node->m_left)) return false;
        if (node->m_right && currentPriority < priorityFn(*node->m_right)) return false;
        return checkMaxHeapProperty(node->m_left, priorityFn) &&
               checkMaxHeapProperty(node->m_right, priorityFn);
    }

    bool testMinHeapPropertyIsMaintained() {
        SQueue q(priorityFn2, MINHEAP, LEFTIST);
        for (int i = 0; i < 300; i++) {
            Post p(100001 + i, rand() % 501, 1 + rand() % 5,
                   1 + rand() % 50, 1 + rand() % 10);
            q.insertPost(p);
        }
        return checkMinHeapProperty(q.m_heap, priorityFn2);
    }

    bool testMaxHeapPropertyIsMaintained() {
        SQueue q(priorityFn1, MAXHEAP, LEFTIST);
        for (int i = 0; i < 300; i++) {
            Post p(100001 + i, rand() % 501, 1 + rand() % 5,
                   1 + rand() % 50, 1 + rand() % 10);
            q.insertPost(p);
        }
        return checkMaxHeapProperty(q.m_heap, priorityFn1);
    }

    bool testMinHeapRemovalsAreInOrder() {
        SQueue q(priorityFn2, MINHEAP, LEFTIST);
        for (int i = 0; i < 300; i++) {
            Post p(100001 + i, rand() % 501, 1 + rand() % 5,
                   1 + rand() % 50, 1 + rand() % 10);
            q.insertPost(p);
        }

        int lastPriority = -1;
        while (q.numPosts() > 0) {
            Post p = q.getNextPost();
            int currentPriority = priorityFn2(p);
            if (currentPriority < lastPriority) return false;
            lastPriority = currentPriority;
        }
        return true;
    }

    bool testMaxHeapRemovalsAreInOrder() {
        SQueue q(priorityFn1, MAXHEAP, LEFTIST);
        for (int i = 0; i < 300; i++) {
            Post p(100001 + i, rand() % 501, 1 + rand() % 5,
                   1 + rand() % 50, 1 + rand() % 10);
            q.insertPost(p);
        }

        int lastPriority = 10000; // max possible priorityFn1 is 510                                                                                                              
        while (q.numPosts() > 0) {
            Post p = q.getNextPost();
            int currentPriority = priorityFn1(p);
            if (currentPriority > lastPriority) return false;
            lastPriority = currentPriority;
        }
        return true;
    }
  // NPL check: whether each node has the correct NPL value                                                                                                                       
  bool checkNPLCorrectness(Post* node) {
    if (!node) return true;

    int leftNPL;
    if (node->m_left != nullptr) {
        leftNPL = node->m_left->getNPL();
    } else {
        leftNPL = -1;
    }

    int rightNPL;
    if (node->m_right != nullptr) {
        rightNPL = node->m_right->getNPL();
    } else {
        rightNPL = -1;
    }

    int expectedNPL;
    if (leftNPL < rightNPL) {
        expectedNPL = 1 + leftNPL;
    } else {
        expectedNPL = 1 + rightNPL;
    }

    if (node->getNPL() != expectedNPL) return false;

    return checkNPLCorrectness(node->m_left) && checkNPLCorrectness(node->m_right);
}

  bool testLeftistHeapNPLValuesCorrect() {
    SQueue q(priorityFn1, MAXHEAP, LEFTIST);
    for (int i = 0; i < 300; i++) {
        Post p(100001 + i, rand() % 501, 1 + rand() % 5,
               1 + rand() % 50, 1 + rand() % 10);
        q.insertPost(p);
    }
    return checkNPLCorrectness(q.m_heap);
  }
  // NPL ordering check: left child's NPL >= right child's NPL                                                                                                                    
  bool checkNPLLeftistProperty(Post* node) {
    if (!node) return true;

    int leftNPL;
    if (node->m_left != nullptr) {
        leftNPL = node->m_left->getNPL();
    } else {
        leftNPL = -1;
    }

    int rightNPL;
    if (node->m_right != nullptr) {
        rightNPL = node->m_right->getNPL();
    } else {
        rightNPL = -1;
    }

    if (leftNPL < rightNPL) return false;

    return checkNPLLeftistProperty(node->m_left) && checkNPLLeftistProperty(node->m_right);
}

  bool testLeftistHeapNPLOrdering() {
    SQueue q(priorityFn1, MAXHEAP, LEFTIST);
    for (int i = 0; i < 300; i++) {
        Post p(100001 + i, rand() % 501, 1 + rand() % 5,
               1 + rand() % 50, 1 + rand() % 10);
        q.insertPost(p);
    }
    return checkNPLLeftistProperty(q.m_heap);
  }
  bool testSetPriorityFnRebuildsHeapCorrectly() {
    SQueue q(priorityFn2, MINHEAP, LEFTIST);

    for (int i = 0; i < 300; i++) {
        Post p(100001 + i, rand() % 501, 1 + rand() % 5,
               1 + rand() % 50, 1 + rand() % 10);
        q.insertPost(p);
    }

    q.setPriorityFn(priorityFn1, MAXHEAP);  //Change to MAXHEAP                                                                                                                   

    //Check new heap structure respects max-heap with new function                                                                                                                
    Tester check;
    return check.checkMaxHeapProperty(q.m_heap, priorityFn1);
  }

  bool testMergeEmptyWithNormalQueue() {
    SQueue emptyQ(priorityFn1, MAXHEAP, LEFTIST);
    SQueue populatedQ(priorityFn1, MAXHEAP, LEFTIST);

    for (int i = 0; i < 100; i++) {
        Post p(200001 + i, rand() % 501, 1 + rand() % 5,
               1 + rand() % 50, 1 + rand() % 10);
        populatedQ.insertPost(p);
    }

    emptyQ.mergeWithQueue(populatedQ);

    return emptyQ.numPosts() == 100 && populatedQ.numPosts() == 0;
  }
  bool testConstructorCreatesIndependentHeapWithData() {
    SQueue q1(priorityFn1, MAXHEAP, LEFTIST);

    for (int i = 0; i < 100; i++) {
        Post p(300001 + i, rand() % 501, 1 + rand() % 5,
               1 + rand() % 50, 1 + rand() % 10);
        q1.insertPost(p);
    }

    //Create a new queue using the constructor from an existing one                                                                                                               
    SQueue q2(q1);

    // Check if size is the same                                                                                                                                                  
    if (q1.numPosts() != q2.numPosts()) return false;

    //Check if structure is still valid                                                                                                                                           
    return checkMaxHeapProperty(q2.m_heap, priorityFn1);
  }

  bool testConstructorFromEmptyQueueWorks() {
    SQueue q1(priorityFn2, MINHEAP, LEFTIST);

    //Create another queue from the empty one                                                                                                                                     
    SQueue q2(q1);

    //Check if the new queue is also empty and safe                                                                                                                               
    return q2.numPosts() == 0 && q2.m_heap == nullptr;
  }
  bool testAssignmentOperatorWithData() {
    SQueue q1(priorityFn1, MAXHEAP, LEFTIST);

    for (int i = 0; i < 100; i++) {
        Post p(400001 + i, rand() % 501, 1 + rand() % 5,
               1 + rand() % 50, 1 + rand() % 10);
        q1.insertPost(p);
    }

    SQueue q2(priorityFn1, MAXHEAP, LEFTIST);
    q2 = q1;  //Assignment                                                                                                                                                        

    //Size match                                                                                                                                                                  
    if (q2.numPosts() != q1.numPosts()) return false;

    //Validate q2 heap structure                                                                                                                                                  
    return checkMaxHeapProperty(q2.m_heap, priorityFn1);
  }

   bool testAssignmentOperatorFromEmptyQueue() {
    SQueue q1(priorityFn2, MINHEAP, LEFTIST);  //empty                                                                                                                            
    SQueue q2(priorityFn2, MINHEAP, LEFTIST);

    for (int i = 0; i < 50; i++) {
        Post p(500001 + i, rand() % 501, 1 + rand() % 5,
               1 + rand() % 50, 1 + rand() % 10);
        q2.insertPost(p);
    }

    q2 = q1;  //Assign empty into non-empty                                                                                                                                       

    //q2 should now be empty                                                                                                                                                      
    return q2.numPosts() == 0 && q2.m_heap == nullptr;
  }
  bool testDequeueFromEmptyThrowsOutOfRange() {
    SQueue q(priorityFn1, MAXHEAP, LEFTIST);

    try {
        q.getNextPost();  //should throw                                                                                                                                          
    } catch (out_of_range&) {
        return true;
    } catch (...) {
        return false;  //wrong exception                                                                                                                                          
    }

    return false;  //no exception                                                                                                                                                 
  }

  bool testMergeWithDifferentPriorityFnsThrowsDomainError() {
    SQueue q1(priorityFn1, MAXHEAP, LEFTIST);
    SQueue q2(priorityFn2, MINHEAP, LEFTIST);

    Post p(100001, 250, 3, 10, 6);
    q2.insertPost(p);

    try {
        q1.mergeWithQueue(q2);  //should throw                                                                                                                                    
    } catch (domain_error&) {
        return true;
    } catch (...) {
        return false;  //wrong exception                                                                                                                                          
    }

    return false;  //no exception                                                                                                                                                 
  }

};

int main() {
    Tester t;
    cout << "testMinHeapPropertyIsMaintained: ";
    if (t.testMinHeapPropertyIsMaintained()) cout << "pass" << endl;
    else cout << "fail" << endl;

    cout << "testMaxHeapPropertyIsMaintained: ";
    if (t.testMaxHeapPropertyIsMaintained()) cout << "pass" << endl;
    else cout << "fail" << endl;

    cout << "testMinHeapRemovalsAreInOrder: ";
    if (t.testMinHeapRemovalsAreInOrder()) cout << "pass" << endl;
    else cout << "fail" << endl;

    cout << "testMaxHeapRemovalsAreInOrder: ";
    if (t.testMaxHeapRemovalsAreInOrder()) cout << "pass" << endl;
    else cout << "fail" << endl;

    cout << "testLeftistHeapNPLValuesCorrect: ";
    if (t.testLeftistHeapNPLValuesCorrect()) cout << "pass" << endl;
    else cout << "fail" << endl;

    cout << "testLeftistHeapNPLOrdering: ";
    if (t.testLeftistHeapNPLOrdering()) cout << "pass" << endl;
    else cout << "fail" << endl;

    cout << "testSetPriorityFnRebuildsHeapCorrectly: ";
    if (t.testSetPriorityFnRebuildsHeapCorrectly()) cout << "pass" << endl;
    else cout << "fail" << endl;

    cout << "testMergeEmptyWithNormalQueue: ";
    if (t.testMergeEmptyWithNormalQueue()) cout << "pass" << endl;
    else cout << "fail" << endl;

    cout << "testConstructorCreatesIndependentHeapWithData: ";
    if (t.testConstructorCreatesIndependentHeapWithData()) cout << "pass" << endl;
    else cout << "fail" << endl;

    cout << "testConstructorFromEmptyQueueWorks: ";
    if (t.testConstructorFromEmptyQueueWorks()) cout << "pass" << endl;
    else cout << "fail" << endl;

    cout << "testAssignmentOperatorWithData: ";
    if (t.testAssignmentOperatorWithData()) cout << "pass" << endl;
    else cout << "fail" << endl;

    cout << "testAssignmentOperatorFromEmptyQueue: ";
    if (t.testAssignmentOperatorFromEmptyQueue()) cout << "pass" << endl;
    else cout << "fail" << endl;

    cout << "testDequeueFromEmptyThrowsOutOfRange: ";
    if (t.testDequeueFromEmptyThrowsOutOfRange()) cout << "pass" << endl;
    else cout << "fail" << endl;

    cout << "testMergeWithDifferentPriorityFnsThrowsDomainError: ";
    if (t.testMergeWithDifferentPriorityFnsThrowsDomainError()) cout << "pass" << endl;
    else cout << "fail" << endl;

    return 0;
}






