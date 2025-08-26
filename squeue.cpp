#include "squeue.h"
#include <stdexcept>

SQueue::SQueue(prifn_t priFn, HEAPTYPE heapType, STRUCTURE structure) {
  m_priorFunc = priFn;
  m_heapType = heapType;
  m_structure = structure;
  m_heap = nullptr;
  m_size = 0;
}
SQueue::~SQueue() {
  clear();
}
void SQueue::clear() {
  clearTree(m_heap);
  m_heap = nullptr;
  m_size = 0;
}
SQueue::SQueue(const SQueue& rhs) {
  m_heap = copyTree(rhs.m_heap);
  m_size = rhs.m_size;
  m_priorFunc = rhs.m_priorFunc;
  m_heapType = rhs.m_heapType;
  m_structure = rhs.m_structure;
}

SQueue& SQueue::operator=(const SQueue& rhs) {//deep copies from other Squeue                                                                                                     
  if (this != &rhs) {
    clear(); //Deallocate current heap                                                                                                                                            
    m_heap = copyTree(rhs.m_heap);
    m_size = rhs.m_size;
    m_priorFunc = rhs.m_priorFunc;
    m_heapType = rhs.m_heapType;
    m_structure = rhs.m_structure;
  }
  return *this;
}

void SQueue::mergeWithQueue(SQueue& rhs) {
    if (this == &rhs) return;

    if (m_priorFunc != rhs.m_priorFunc || m_structure != rhs.m_structure) {
      throw domain_error("mergeWithQueue(): incompatible queues.");// throws error if priority functions/structures arent matching                                                
    }

    m_heap = merge(m_heap, rhs.m_heap);
    m_size += rhs.m_size;

    rhs.m_heap = nullptr;
    rhs.m_size = 0;
}

bool SQueue::insertPost(const Post& post) {
  Post* newNode = new Post(post);
  m_heap = merge(m_heap, newNode); //merge based on current structure                                                                                                             
  m_size++;
  return true;
}

int SQueue::numPosts() const {
  return m_size;
}
prifn_t SQueue::getPriorityFn() const {
  return m_priorFunc;
}
Post SQueue::getNextPost() {
  if (m_heap == nullptr) {
        throw out_of_range("getNextPost(): Attempted to get post from an empty queue.");
    }

    Post highestPriorityPost = *m_heap; //Copy the root post to return                                                                                                            

    Post* leftChild = m_heap->m_left;
    Post* rightChild = m_heap->m_right;

    delete m_heap;

    m_heap = merge(leftChild, rightChild); //Merge left and right subtrees                                                                                                        
    m_size--;

    return highestPriorityPost;
}

void SQueue::setPriorityFn(prifn_t priFn, HEAPTYPE heapType) {
    Post* nodeArray[MAXTIME];
    int count = 0;

    //Collect the original nodes                                                                                                                                                  
    collectPosts(m_heap, nodeArray, count, MAXTIME);

    //Create a deep copy of each node BEFORE clear()                                                                                                                              
    Post* copyArray[MAXTIME];
    for (int i = 0; i < count; ++i) {
        copyArray[i] = new Post(*nodeArray[i]);  // copy constructor                                                                                                              
    }

    //Clear the original heap                                                                                                                                                     
    clear();

    //Update priority and heap type                                                                                                                                               
    m_priorFunc = priFn;
    m_heapType = heapType;

    //Rebuild the heap from new copies                                                                                                                                            
    for (int i = 0; i < count; ++i) {
        copyArray[i]->m_left = nullptr;
        copyArray[i]->m_right = nullptr;
        copyArray[i]->m_npl = 0;
        m_heap = merge(m_heap, copyArray[i]);
        m_size++;
    }
}


void SQueue::setStructure(STRUCTURE structure) {
  Post* nodeArray[MAXTIME];
  int count = 0;

  //collect all nodes                                                                                                                                                             
  collectPosts(m_heap, nodeArray, count, MAXTIME);

  //reset and update structure                                                                                                                                                    
  m_heap = nullptr;
  m_size = 0;
  m_structure = structure;

  //rebuild heap using the new structure                                                                                                                                          
  for (int i = 0; i < count; ++i) {
    nodeArray[i]->m_left = nullptr;
    nodeArray[i]->m_right = nullptr;
    nodeArray[i]->m_npl = 0;
    m_heap = merge(m_heap, nodeArray[i]);
    m_size++;
  }
}



STRUCTURE SQueue::getStructure() const {
  return m_structure;
}
HEAPTYPE SQueue::getHeapType() const {
  return m_heapType;
}
void SQueue::printPostsQueue() const {
  cout << "Contents of the queue: " << endl;
    printPreOrder(m_heap);
}


void SQueue::dump() const {
  if (m_size == 0) {
    cout << "Empty heap." << endl;
  } else {
      dump(m_heap);
  }
  cout << endl;
}
void SQueue::dump(Post *pos) const {
  if ( pos != nullptr ) {
    cout << "(";
    dump(pos->m_left);
    if (m_structure == SKEW)
        cout << m_priorFunc(*pos) << ":" << pos->m_postID;
    else
        cout << m_priorFunc(*pos) << ":" << pos->m_postID << ":" << pos->m_npl;
    dump(pos->m_right);
    cout << ")";
  }
}

ostream& operator<<(ostream& sout, const Post& post) {
  sout  << "Post#: " << post.getPostID()
        << ", likes#: " << post.getNumLikes()
        << ", connect level: " << post.getConnectLevel();
  return sout;
}

Post* SQueue::merge(Post* h1, Post* h2) {
    if (h1 == nullptr) return h2;
    if (h2 == nullptr) return h1;

    bool condition = false;
    if (m_heapType == MINHEAP) {
        if (m_priorFunc(*h2) < m_priorFunc(*h1)) {
            condition = true;
        }
    } else { // MAXHEAP                                                                                                                                                           
        if (m_priorFunc(*h2) > m_priorFunc(*h1)) {
            condition = true;
        }
    }

    if (condition) swap(h1, h2);

    if (m_structure == SKEW) {
        h1->m_right = merge(h1->m_right, h2);
        swap(h1->m_left, h1->m_right);
    } else { // LEFTIST                                                                                                                                                           
        h1->m_right = merge(h1->m_right, h2);

        int leftNPL;
        if (h1->m_left != nullptr) {
            leftNPL = h1->m_left->getNPL();
        } else {
            leftNPL = -1;
        }

        int rightNPL;
        if (h1->m_right != nullptr) {
            rightNPL = h1->m_right->getNPL();
        } else {
            rightNPL = -1;
        }

        if (leftNPL < rightNPL) {
            swap(h1->m_left, h1->m_right);
        }

        if (leftNPL < rightNPL) {
            h1->m_npl = 1 + leftNPL;
        } else {
            h1->m_npl = 1 + rightNPL;
        }
    }

    return h1;
}


void SQueue::clearTree(Post* root) {
    if (root != nullptr) {
        clearTree(root->m_left);
        clearTree(root->m_right);
        delete root;
    }
}

Post* SQueue::copyTree(const Post* root) {
    if (root == nullptr) return nullptr;

    Post* newNode = new Post(*root); //Copy node values                                                                                                                           

    newNode->m_left = copyTree(root->m_left);
    newNode->m_right = copyTree(root->m_right);

    return newNode;
}

void SQueue::printPreOrder(Post* node) const {
    if (node != nullptr) {
        int pri = m_priorFunc(*node);
        cout << "[" << pri << "] " << *node << endl;
        printPreOrder(node->m_left);
        printPreOrder(node->m_right);
    }
}

void SQueue::collectPosts(Post* root, Post* arr[], int& index, int maxSize) {
    if (root == nullptr || index >= maxSize) return;

    arr[index++] = root;
    collectPosts(root->m_left, arr, index, maxSize);//collects nodes from the left subtree to put into array                                                                      
    collectPosts(root->m_right, arr, index, maxSize);//collects nodes from the right subtree to put into array                                                                    
}



