#include "skiplist.h"
#include "random.h"
#include <cassert>
#include <climits>
#include <cstdlib>
#include <iostream>
#include <unordered_map>

using namespace std;

// checks if the value needs to be inserted at a higher level
bool SkipList::shouldInsertAtHigherLevel() const {
  return probability >= Random::random() % 100;
}

// you code goes here

// Gets the SNode pointers before the given value
vector<SNode *> SkipList::getBeforeNodes(int val) const {
  vector<SNode *> previous;

  // search for the value
  for (int i = 0; i < levels; i++) {
    SNode *curr = head;
    while (curr->next[i] != nullptr && curr->next[i]->val < val) {
      curr = curr->next[i];
    }
    previous.push_back(curr);
  }
  return previous;
}

// SNode constructor
SNode::SNode(int val) {
  this->val = val;
  next.push_back(nullptr);
}

// SNode copy constructor
SNode::SNode(const SNode &other) {
  this->val = other.val;
  for (int i = 0; i < other.next.size(); i++) {
    next[i] = other.next[i];
  }
}

// Prints out the Skip list
ostream &operator<<(ostream &out, const SkipList &skip) {

  for (int i = skip.levels; i > 0; i--) {
    SNode *curr = skip.head->next[i - 1];
    out << "[level: " << i << "] ";

    while (curr != nullptr) {
      out << curr->val << "-->";
      curr = curr->next[i - 1];
    }

    out << "nullptr" << endl;
  }
  return out;
}

// SkipList constructor
SkipList::SkipList(int levels, int probability) {
  head = new SNode();
  for (int i = 0; i < levels; i++) {
    head->next[i] = nullptr;
  }
  this->levels = levels;
  this->probability = probability;
}

// SkipList copy constructor
SkipList::SkipList(const SkipList &other) {
  head = new SNode();
  this->levels = other.levels;
  this->probability = other.probability;
  SNode *tmp = head;

  // insert nullptr to the new head
  for (int i = 0; i < levels; i++) {
    tmp->next[i] = nullptr;
  }

  // inserts the rest of pointers to seperate nodes
  for (int i = 0; i < levels; i++) {
    SNode *otherHead = other.head;
    SNode *curr = head;

    while (otherHead->next[i] != nullptr) {
      SNode *newNode = new SNode(otherHead->next[i]->val);
      while (curr->next[i] != nullptr) {
        curr = curr->next[i];
      }
      curr->next[i] = newNode;
      otherHead = otherHead->next[i];
    }
  }
}

// destructor
SkipList::~SkipList() {
  SNode *curr = head;
  SNode *next;
  while (curr != nullptr) {
    next = curr->next[0];
    delete curr;
    curr = next;
  }
  head = nullptr;
}

void SkipList::add(int val) {
  SNode *newNode = new SNode(val);
  vector<SNode *> prevNodes = getBeforeNodes(val);
  int index = 0;

  // adds everything to level 1
  SNode *curr = head;
  if (curr->next[index] == nullptr) {
    curr->next[index] = newNode;
  } else {
    curr = prevNodes[index];
    newNode->next[index] = curr->next[index];
    curr->next[index] = newNode;
  }

  // moves to the next level
  index++;

  // adds to level 2 and up if needs to be inserted
  while (index < levels && shouldInsertAtHigherLevel()) {
    SNode *curr = head;
    if (curr->next[index] == nullptr) {
      curr->next[index] = newNode;
    } else {
      curr = prevNodes[index];
      newNode->next[index] = curr->next[index];
      curr->next[index] = newNode;
    }
    index++;
  }
}

// Add to list, assume no duplicates
void SkipList::add(const vector<int> &values) {
  for (int i = 0; i < values.size(); i++) {
    add(values[i]);
  }
}

// removes a node with the determined value
bool SkipList::remove(int val) {
  vector<SNode *> prevNodes = getBeforeNodes(val);
  SNode *curr = head;

  // checks if the value is inside the skiplist
  if (!contains(val)) {
    return false;
  }

  // deletes the skiplist for each level
  for (int i = 0; i < levels; i++) {
    curr = prevNodes[i];
    if (curr->next[i]->val == val) {
      curr->next[i] = curr->next[i]->next[i];
    }
  }

  return true;
}

// return true if found in skiplist
bool SkipList::contains(int val) const {
  SNode *curr = head;
  int index = levels - 1;

  while (index >= 0) {
    // if the highest level points to null and still have lower levels
    if (curr->next[index] == nullptr) {
      index--;
    } else {
      // checks if there is the target value in the level if not it
      // it will go down by 1 level
      while (curr->next[index] != nullptr) {
        if (curr->next[index]->val > val) {
          if (index > 0) {
            index--;
          } else {
            return false;
          }
          // if the value is found
        } else if (curr->next[index]->val == val) {
          return true;
        } else {
          curr = curr->next[index];
        }
      }
    }
  }
  return false;
}