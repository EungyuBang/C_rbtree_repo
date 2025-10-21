#include "rbtree.h"
#include <stdlib.h>

rbtree *new_rbtree(void) {
  rbtree *p = (rbtree *)calloc(1, sizeof(rbtree));
  // TODO: initialize struct if needed
  node_t *nil_node = (node_t *)calloc(1, sizeof(node_t));
  nil_node -> color = RBTREE_BLACK;

  p -> nil = nil_node;
  p -> root = p -> nil;
  return p;
}

// 트리 노드 지워주는 헬퍼 함수
void free_nodes(rbtree *t, node_t *node) {
  if(node == t->nil) {
    return;
  }
  free_nodes(t, node -> left);
  free_nodes(t, node -> right);
  free(node);
}
// rbtree 초기화
void delete_rbtree(rbtree *t) {
  // TODO: reclaim the tree nodes's memory
  free_nodes(t, t->root);
  free(t->nil);
  free(t);
}
/*
  RB 트리에 노드 넣는 경우
  1. 그냥 빈트리에 넣을 경우 (그 노드가 루트가 되어야 함)
  2. 이후 BST 처럼 삽입하고, 새 노드는 무조건 빨간색으로 시작
  3. 해당 노드의 부모가 red인지 확인 -> 더블레드 문제 확인
  4. 문제가 있다면, 삼촌의 색깔(부모의 형제)을 보고 색깔을 바꾸거나, 회전 후 색 변경
  5. 루트 black 인지 확인하고, 아니면 현재 노드 블랙으로 바꿔주기
*/
void left_rotate(rbtree *t, node_t *x) {
    // 1. y를 x의 오른쪽 자식으로 설정
    node_t *y = x->right;
    // 2. y의 왼쪽 서브트리를 x의 오른쪽 서브트리로 옮김
    x->right = y->left;
    if (y->left != t->nil) {
        y->left->parent = x;
    }
    // 3. y의 부모를 x의 부모로 연결 (x를 건너뜀)
    y->parent = x->parent;
    // 4. x의 부모가 y를 가리키도록 연결
    if (x->parent == t->nil) {
        t->root = y; // x가 루트였다면 y가 새 루트
    } else if (x == x->parent->left) {
        x->parent->left = y;
    } else {
        x->parent->right = y;
    }
    // 5. x를 y의 왼쪽 자식으로 설정
    y->left = x;
    x->parent = y;
}
void right_rotate(rbtree *t, node_t *y) {
    // 1. x를 y의 왼쪽 자식으로 설정
    node_t *x = y->left;
    
    // 2. x의 오른쪽 서브트리를 y의 왼쪽 서브트리로 옮김
    y->left = x->right;
    if (x->right != t->nil) {
        x->right->parent = y;
    }
    
    // 3. x의 부모를 y의 부모로 연결 (y를 건너뜀)
    x->parent = y->parent;
    
    // 4. y의 부모가 x를 가리키도록 연결
    if (y->parent == t->nil) {
        t->root = x; // y가 루트였다면 x가 새 루트
    } else if (y == y->parent->left) {
        y->parent->left = x;
    } else {
        y->parent->right = x;
    }
    
    // 5. y를 x의 오른쪽 자식으로 설정
    x->right = y;
    y->parent = x;
}
void rbtree_insert_fix(rbtree *t, node_t *new_node) {
  while(new_node -> parent -> color == RBTREE_RED) { // 더블레드 문제 확인
    if(new_node -> parent == new_node -> parent -> parent -> left) { // 부모가 할아버지의 왼쪽 자식일 경우
      node_t *uncle = new_node -> parent -> parent -> right; // 삼촌 지정
      // 삼촌이 RED
      if(uncle -> color == RBTREE_RED) {
        new_node->parent->color = RBTREE_BLACK;
        uncle -> color = RBTREE_BLACK;
        new_node -> parent -> parent -> color = RBTREE_RED; // 삼촌과 부모를 다 블랙으로 바꿨는데 할아버지도 검은색이면 왼쪽 black height가 오른쪽 black height 보다 1 커짐 -> 할아버지를 빨간색으로 
        new_node = new_node -> parent -> parent; // 할아버지로 이동 -> 할아버지를 red로 바꿨는데 증조 할아버지가 red라서 더블레드 문제 생길 수 있으니까
      }// 삼촌이 Black 
      else {
        // new node가 부모의 오른쪽
        if(new_node == new_node -> parent -> right) {
          // 부모님 기준으로 왼쪽 회전
          new_node = new_node -> parent; 
          left_rotate(t, new_node);
        }
        // new node가 부모의 왼쪽 (위의 if 문 지나면 여기에 걸림, 아님 그냥 처음부터 왼쪽일 수도 있고)
        new_node -> parent -> color = RBTREE_BLACK;
        new_node -> parent -> parent -> color = RBTREE_RED;
        // 할아버지 기준 오른쪽 회전
        right_rotate(t, new_node -> parent -> parent);
      }
    }
    // 부모가 할아버지의 오른쪽 자식일 경우 (위와 대칭)
    else {
      node_t *uncle = new_node -> parent -> parent -> left;
      if(uncle -> color == RBTREE_RED) {
        new_node -> parent -> color = RBTREE_BLACK;
        uncle -> color = RBTREE_BLACK;
        new_node -> parent -> parent -> color = RBTREE_RED;
        new_node = new_node -> parent -> parent;
      } else {
        if(new_node == new_node -> parent -> left) {
          new_node = new_node -> parent;
          right_rotate(t, new_node);
        }
        new_node -> parent -> color = RBTREE_BLACK;
        new_node -> parent -> parent -> color = RBTREE_RED;
        left_rotate(t, new_node -> parent -> parent);
      }
    }
  }
  t -> root -> color = RBTREE_BLACK; // 루트의 색은 항상 검은색 고정
}
node_t *rbtree_insert(rbtree *t, const key_t key) {
  // TODO: implement insert
  // 노드 만들기
  node_t *new_node = (node_t *)calloc(1, sizeof(node_t));
  new_node -> key = key;
  new_node -> left = t -> nil;
  new_node -> right = t -> nil;
  new_node -> color = RBTREE_RED; // 새로 삽입되는 노드의 색은 빨간색

  node_t *root_node = t -> nil;
  node_t *node_next = t -> root;

  while(node_next != t -> nil) {
    root_node = node_next;
    if (new_node ->key < node_next->key) {
      node_next = root_node -> left;
    } else {
      node_next = root_node -> right;
    }
  }

  new_node -> parent = root_node;
  if(root_node == t -> nil) {
    t -> root = new_node;
  } else if (new_node -> key  < root_node-> key) {
    root_node -> left = new_node;
  } else {
    root_node -> right = new_node;
  }
  rbtree_insert_fix(t, new_node);
  return new_node;
}

node_t *rbtree_find(const rbtree *t, const key_t key) {
  // TODO: implement find
  // 입력값 들어오면 트리 순회하면서 찾기? -> 반복문으로 그때의 값 보다 작으면 왼쪽 크면 오른쪽으로 보내면 언젠간 나오겠지 뭐...
  /*- ptr = `tree_find(tree, key)`
  - RB tree내에 해당 key가 있는지 탐색하여 있으면 해당 node pointer 반환
  - 해당하는 node가 없으면 NULL 반환 */
  node_t *curNode = t -> root;
  while (curNode != t -> nil) {
    // 같으면 반환
    if(curNode->key == key) {
      return curNode;
    }
    // 입력된 키 값이 현재노드의 키보다 작으면? 왼쪽으로 이동
    else if (key < curNode->key) {
      curNode = curNode -> left;
    } 
    else {
      curNode = curNode -> right;
    }
  }
  return NULL;
}

node_t *rbtree_min(const rbtree *t) {
  // TODO: implement find
  // - ptr = `tree_min(tree)`: RB tree 중 최소 값을 가진 node pointer 반환
  // 그냥 계속 왼쪽으로 보내면 그게 최소겠지
  node_t *curNode = t -> root;
  if (curNode == t -> nil) {
    return NULL;
  }
  while(curNode->left != t -> nil) {
    curNode = curNode -> left;
  }
  return curNode;
}

node_t *rbtree_max(const rbtree *t) {
  // TODO: implement find
  // - ptr = `tree_max(tree)`: 최대값을 가진 node pointer 반환
  // 그냥 계속 오른쪽으로 보내면 그게 최대겠지
  node_t *curNode = t -> root;
  if (curNode == t -> nil) {
    return NULL;
  }
  while(curNode -> right != t -> nil) {
    curNode = curNode -> right;
  }
  return curNode;
}
/* 
  복구 시작: 균형을 맞추기 위해, 삭제된 자리로 온 노드의 '형제' 노드 상태를 살핀다.
  (복구-1): 형제가 빨간색이면, 색을 바꾸고 회전시켜 '형제가 검은색인 상황'으로 만든 뒤 다시 확인한다.
  (복구-2): 형제와 그 자식까지 모두 검은색이면, 형제만 빨간색으로 바꾸고 문제는 '부모'에게 넘겨서 위에서부터 다시 시작한다.
  (복구-3): 형제의 '안쪽' 자식만 빨간색이면, 회전시켜 '바깥쪽' 자식이 빨간색인 상황(아래 4번)으로 바꾼다.
  (복구-4): 형제의 '바깥쪽' 자식이 빨간색이면, 최종적으로 색을 조정하고 회전하여 문제를 완전히 해결하고 복구를 끝낸다.
  마무리: 모든 과정이 끝나면, 혹시 모르니 루트 노드가 검은색인지 다시 한번 확인하고 맞춘다.
*/
void rbtree_erase_fix(rbtree *t, node_t *x) {
  while(x != t->root && x->color == RBTREE_BLACK) {
    // x가 부모 왼쪽 -> 형제는 부모 오른쪽
    if(x == x -> parent -> left) {
      node_t *bro = x -> parent -> right; // bro

      // case 1 : 형제가 빨간색 -> 부모, 형제 색 교체 + 부모기준 회전
      if (bro->color == RBTREE_RED) {
        bro->parent->color = RBTREE_RED;
        bro->color = RBTREE_BLACK;
        left_rotate(t, x->parent);
        bro = x -> parent -> right;
      }
      // case 2 : 형제 블랙 + 형제 자식 왼,오 다 블랙 -> 형제 색 바꿔주기
      if(bro->left->color == RBTREE_BLACK && bro->right->color == RBTREE_BLACK) {
        bro->color = RBTREE_RED;
        x = x -> parent;
      } else {
        // case 3 : 형제 블랙, 형제의 오른쪽 블랙, 왼쪽 레드 -> 형제 기준 회전
        if(bro->right->color == RBTREE_BLACK) {
          bro->left->color = RBTREE_BLACK;
          bro->color = RBTREE_RED;
          right_rotate(t, bro);
          bro = x -> parent -> right;
        }
        // case 4 : 형제 블랙 -> 형제 오른쪽 자식 레드 -> 색 조정 후 부모기준 회전
        bro -> color = x -> parent -> color;
        x -> parent -> color = RBTREE_BLACK;
        bro -> right -> color = RBTREE_BLACK;
        left_rotate(t, x->parent);
        x = t -> root;
      }
    }
   else {
     node_t *bro = x->parent->left;  // 형제 노드

      // CASE 1 (대칭)
      if (bro->color == RBTREE_RED) {
        bro->color = RBTREE_BLACK;
        x->parent->color = RBTREE_RED;
        right_rotate(t, x->parent);
        bro = x->parent->left;
      }
      //  CASE 2 (대칭)
      if (bro->right->color == RBTREE_BLACK && bro->left->color == RBTREE_BLACK) {
        bro->color = RBTREE_RED;
        x = x->parent;
      }
      else {
        //  CASE 3 (대칭)
        if (bro->left->color == RBTREE_BLACK) {
          bro->right->color = RBTREE_BLACK;
          bro->color = RBTREE_RED;
          left_rotate(t, bro);
          bro = x->parent->left;
        }
        //  CASE 4 (대칭)
        bro->color = x->parent->color;
        x->parent->color = RBTREE_BLACK;
        bro->left->color = RBTREE_BLACK;
        right_rotate(t, x->parent);
        x = t->root;
      }
    }
  }
  x -> color = RBTREE_BLACK;
}

// 후계자는 삭제될 노드보다 큰 값중 가장 작은 값 -> 한번 오른쪽 -> 이후 제일 왼쪽
node_t *rbtree_successor(rbtree *t, node_t *p) {
  node_t *curNode = p -> right;
  while ((curNode -> left != t -> nil))
  {
    curNode = curNode -> left;
  }
  return curNode;
}

// 트리 t에서 노드 u의 자리를 노드 v로 대체하는 함수
void rbtree_transplant(rbtree *t, node_t *u, node_t *v) {
  if(u -> parent == t -> nil) {
    t -> root = v;
  } else if (u == u->parent->left) {
    u -> parent -> left = v;
  } else {
    u -> parent -> right = v;
  }
  v -> parent = u -> parent;
}

int rbtree_erase(rbtree *t, node_t *p) {
  // TODO: implement erase
  // - tree_erase(tree, ptr): RB tree 내부의 ptr로 지정된 node를 삭제하고 메모리 반환
  /*
  단순화: 삭제할 노드의 자식이 2개면, '후계자' 노드의 값을 대신 채우고 그 후계자를 삭제 대상으로 바꾼다. 
  연결 끊기: 이제 자식이 최대 1개인 삭제 대상을 트리에서 빼내고, 있던 자식으로 그 자리를 메꾼다.      
  문제 확인: 삭제된 노드의 원래 색이 검은색이었다면, Black Height 균형이 깨졌으므로 복구 작업을 시작한다. (빨간색이었다면 여기서 끝!)
  메모리 해제: 진짜 삭제 대상이었던 노드를 메모리에서 완전히 해제한다.
  */
  // 여기엔 그냥 bst 노드 삭제 로직 => 마지막에 수정 함수 호출
  node_t *eraseNode = p; // 삭제 될 노드 y
  node_t *shiftNode; // 그 자리 채울 노드 x
  color_t eraseNodeOriginColor = eraseNode -> color;
  // 왼쪽 자녀가 없을 때
  if(p -> left == t -> nil) {
    shiftNode = p -> right;
    rbtree_transplant(t, p, p -> right);
  } // 오른쪽 자녀가 없을때
  else if(p -> right == t -> nil) {
    shiftNode = p -> left;
    rbtree_transplant(t, p, p -> left);
  } // 자녀가 양쪽 다 있을 때
  else {
    eraseNode = rbtree_successor(t, p);
    eraseNodeOriginColor = eraseNode -> color;
    // shiftNode 는 후계자의 오른쪽 자식 (후계자는 왼쪽 끝까지 내려와서 후계자의 left는 항상 nil)
    shiftNode = eraseNode -> right;

    if(eraseNode -> parent == p) {
      shiftNode -> parent = eraseNode;
    } else {
      rbtree_transplant(t, eraseNode, eraseNode -> right);
      eraseNode -> right = p -> right;
      eraseNode -> right -> parent = eraseNode;
    }
    rbtree_transplant(t,p, eraseNode);

    eraseNode -> left = p -> left;
    eraseNode -> left -> parent = eraseNode;

    eraseNode -> color = p -> color;
  }
  if(eraseNodeOriginColor == RBTREE_BLACK) {
    rbtree_erase_fix(t, shiftNode);
  }
  free(p);
  return 0;
}


void recursiveInorder(const rbtree *t,node_t *node,key_t *arr, const size_t n, size_t *index) {
  if(node == t->nil || *index > n) return;

  recursiveInorder(t, node->left, arr,n, index);

  if (*index < n) {
    arr[*index] = node -> key;
    (*index)++;
  }

  recursiveInorder(t, node->right, arr, n, index);
}


int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) {
  /*- `tree_to_array(tree, array, n)`
  - RB tree의 내용을 *key 순서대로* 주어진 array로 변환
  - array의 크기는 n으로 주어지며 tree의 크기가 n 보다 큰 경우에는 순서대로 n개 까지만 변환
  - array의 메모리 공간은 이 함수를 부르는 쪽에서 준비하고 그 크기를 n으로 알려줍니다.*/
  // 왼 -> 루 -> 오  --> 중위 순회
  size_t index = 0;
  recursiveInorder(t, t->root, arr,n, &index);
  return 0;
}
