#include "rbtree.h"

#include <stdlib.h>
#include <stdio.h>

rbtree *new_rbtree(void) {
  rbtree *t = (rbtree *)calloc(1, sizeof(rbtree));
  // TODO: initialize struct if needed
  
  node_t *r = (node_t *)calloc(1, sizeof(node_t));
  t->root = r;
  // node_t *n = (node_t *)calloc(1, sizeof(node_t)); // 이거 해제가 안된채 트리가 없어짐 -> 메모리 누수
  // t->nil = n; // 그냥 r 노드 하나로 두 개 연결

  // sentinel
  t->nil = t->root;
  t->root->color = RBTREE_BLACK;
  
  if (t->root == NULL || t->nil == NULL) {
    printf("RB Tree initialization failed.\n");
    return NULL;
  }
  return t;
}

// post-order
void delete_rbtree_nodes(node_t *node, node_t *nil) {
  // free
  if (node == nil) {
    return;
  }
  
  delete_rbtree_nodes(node->left, nil);
  delete_rbtree_nodes(node->right, nil);
  free(node);
}

void delete_rbtree(rbtree *t) {
  // TODO: reclaim the tree nodes's memory
  delete_rbtree_nodes(t->root, t->nil);

  free(t->nil);
  free(t);
}

void rbtree_left_rotate(rbtree *t, node_t *x) {
  // printf("왼쪽 회전 시작\n");
  node_t *y;
  // node_t *y = (node_t *)calloc(1, sizeof(node_t)); -> 이거 메모리 누수될 수 있음
  // y는 그저 이미 생성된 노드를 가리키는 포인터로서 사용하기 때문
  y = x->right;

  // y의 왼쪽 서브트리를 x의 오른쪽으로 옮김
  x->right = y->left;
  if (y->left != NULL) {
    y->left->parent = x;
  }
  
  // x의 부모가 y의 부모가 됨
  y->parent = x->parent;
  // x가 루트노드였으면
  if (x->parent == t->nil) {
    // 새로운 루트 y
    t->root = y;
  }
  // x가 왼쪽 자식이었으면
  else if (x == x->parent->left) {
    // y가 새로운 왼쪽 자식이 됨
    x->parent->left = y;
  }
  else {
    // x가 오른쪽 자식이었으면
    // y가 새로운 오른쪽 자식이 됨
    x->parent->right = y;
  }
  // x가 y의 왼쪽 자식이 됨
  y->left = x;
  x->parent = y;

  //printf("왼쪽 회전 끝\n");
}

void rbtree_right_rotate(rbtree *t, node_t *y) {
  //printf("오른쪽 회전 시작\n");
  node_t *x;
  x = y->left;

  // x의 오른쪽 서브트리를 y의 왼쪽으로 옮김
  y->left = x->right;
  if (x->right != NULL) {
    x->right->parent = y;
  }

  // y의 부모가 x의 부모가 됨
  x->parent = y->parent;
  // y가 루트노드였으면
  if (y->parent == t->nil) {
    // 새로운 루트 x
    t->root = x;
  }
  // y가 왼쪽 자식이었으면
  else if (y == y->parent->left) {
    // x가 새로운 왼쪽 자식이 됨
    y->parent->left = x;
  }
  else {
    y->parent->right = x;
  }

  // y가 x의 오른쪽 자식이 됨
  x->right = y;
  y->parent = x;

  //printf("오른쪽 회전 끝\n");
}

void rbtree_insert_fixup(rbtree *t, node_t *z) {
  //node_t *y = (node_t *)calloc(1, sizeof(node_t)); // 여기 말고

  while (z->parent->color == RBTREE_RED) { // red의 부모가 red(4번속성 위배)일 동안 지속
    node_t *y; // 여기서 선언해야함
    
    if (z->parent == z->parent->parent->left) { // 새로 삽입되는 노드의 부모가 왼쪽 자식
      y = z->parent->parent->right; // y는 삼촌

      if (y->color == RBTREE_RED) { // z: Red, z.p: Red, y: Red, z.p.p: Black
        z->parent->color = RBTREE_BLACK;
        y->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        z = z->parent->parent;
      }
      
      else {
        if (z == z->parent->right) { // 꺾인 경우, y: BLACK
          z = z->parent;
          rbtree_left_rotate(t, z); // 펴주기
        }
        
        z->parent->color = RBTREE_BLACK; // 색 교환
        z->parent->parent->color = RBTREE_RED;
        rbtree_right_rotate(t, z->parent->parent);
      }
    }

    // 새로 삽입되는 노드의 부모가 오른쪽 자식
    else {
      y = z->parent->parent->left;

      if (y->color == RBTREE_RED) {
        z->parent->color = RBTREE_BLACK;
        y->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        z = z->parent->parent;
      }

      else {
        if (z == z->parent->left) {
          z = z->parent;
          rbtree_right_rotate(t, z);
        }
      
        z->parent->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        rbtree_left_rotate(t, z->parent->parent);
      }
    }
  }

  t->root->color = RBTREE_BLACK;
}

node_t *rbtree_insert(rbtree *t, const key_t key) {
  // TODO: implement insert

  node_t *y;
  node_t *x;
  y = t->nil;
  x = t->root;

  // 들어갈 위치 찾기
  while (x != t->nil) {
    y = x;
    if (key < x->key) { // 키값이 더 작으면 왼쪽 서브트리로
      x = x->left;
    }
    else {
      x = x->right; // 크면 오른쪽 서브트리로
    }
    // else { // 중복 왜 가능????
    //   return t->root; // 중복해서 넣을 수 없음
    // }
  }

  // 삽입되는 노드
  node_t *z = (node_t *)calloc(1, sizeof(node_t));
  z->key = key;
  z->parent = y;

  // 트리가 비어있는 경우
  // 삽입되는 노드가 루트 노드가 됨
  if (y == t->nil) {
    t->root = z;
  }
  // 삽입되는 값이 더 작으면 왼쪽에 삽입
  else if (key < y->key) {
    y->left = z;
  }
  // 삽입되는 값이 더 크면 오른쪽에 삽입
  else {
    y->right = z;
  }
  
  z->left = t->nil;
  z->right = t->nil;
  z->color = RBTREE_RED;

  // RB-INSERT_FIXUP
  rbtree_insert_fixup(t, z);

  return t->root;
}


node_t *rbtree_find(const rbtree *t, const key_t key) {
  // TODO: implement find
  //printTree(t, t->root, 0, 0);

  node_t *x = t->root;
  while (x != t->nil) {
    if (key < x->key) { // 작은 쪽 탐색
      x = x->left;
    }
    else if (key > x->key) { // 큰 쪽 탐색
      x = x->right;
    }
    else {
      break;
    }
  }

  if (x == t->nil) { // 해당하는 키를 찾을 수 없음
    return NULL;
  }

  //printf("Find %d, found %d\n", key, x->key);
  return x;
}

node_t *rbtree_min(const rbtree *t) {
  // TODO: implement find

  // 가장 왼쪽에 있는 노드가 가장 작은 노드
  node_t *x = t->root;
  while (x->left != NULL && x->left != t->nil) {
    x = x->left;
  }
  //printf("Min is %d\n", x->key);

  return x;
}

node_t *rbtree_max(const rbtree *t) {
  // TODO: implement find

  // 가장 오른쪽에 있는 노드가 가장 작은 노드
  node_t *x = t->root;
  while (x->right != NULL && x->right != t->nil) {
    x = x->right;
  }
  //printf("Max is %d\n", x->key);

  return x;
}

// 삭제할 노드 u의 후임자 노드인 v를 u 자리에 대체
void rbtree_transplant(rbtree *t, node_t *u, node_t *v) {
  // 삭제할 노드가 루트 노드
  if (u->parent == t->nil) {
    t->root = v;
  }
  // 삭제할 노드가 왼쪽 자식
  else if (u == u->parent->left) {
    u->parent->left = v;
  }
  // 삭제할 노드가 오른쪽 자식
  else {
    u->parent->right = v;
  }

  v->parent = u->parent;
}

void rbtree_delete_fixup(rbtree *t, node_t *x) {
  while (x != t->root && x->color == RBTREE_BLACK) { // x is doubly black or red-and-black
    if (x == x->parent->left) { // x가 왼쪽 자식
      node_t *w = x->parent->right; // w는 형제 노드

      // case 1: 형제가 red면 x 위로 red 올려주기 위해 회전
      if (w->color == RBTREE_RED) { 
        printf("Casecase 1\n");
        w->color = RBTREE_BLACK;
        x->parent->color = RBTREE_RED;
        rbtree_left_rotate(t, x->parent);
        w = x->parent->right;
      }

      // case 2: 형제가 black이고 그 자식이 둘다 black이면 extra black을 부모로 올려줌
      if (w->left->color == RBTREE_BLACK && w->right->color == RBTREE_BLACK) {
        printf("Casecase 2\n");
        w->color = RBTREE_RED;
        x = x->parent;
      }

      else {
        // case 3: 자식이 각각 red, black이고 red쪽 척추가 휘어진 경우 펴줌
        if (w->right->color == RBTREE_BLACK) {
          printf("Casecase 3\n");
          w->left->color = RBTREE_BLACK;
          w->color = RBTREE_RED;
          rbtree_right_rotate(t, w);
          w = x->parent->right;
        }

        // case 4: 깜깜부
        printf("Casecase 4\n");
        w->color = x->parent->color;
        printf("안뇽\n");
        x->parent->color = RBTREE_BLACK;
        printf("안뇽\n");
        if (w->right == NULL) {
          printf("NULL인디\n");
        }
        w->right->color = RBTREE_BLACK;
        printf("안뇽\n");
        rbtree_left_rotate(t, x->parent);
        x = t->root;
        printf("안뇽\n");
      }
    }

    else { // 대칭
      node_t *w = x->parent->left;

      if (w->color == RBTREE_RED) {
        w->color = RBTREE_BLACK;
        x->parent->color = RBTREE_RED;
        rbtree_right_rotate(t, x->parent);
        w = x->parent->left;
      }

      if (w->right->color == RBTREE_BLACK && w->left->color == RBTREE_BLACK) {
        w->color = RBTREE_RED;
        x = x->parent;
      }

      else {
        if (w->left->color == RBTREE_BLACK) {
          w->right->color = RBTREE_BLACK;
          w->color = RBTREE_RED;
          rbtree_left_rotate(t, w);
          w = x->parent->left;
        }

        w->color = x->parent->color;
        x->parent->color = RBTREE_BLACK;
        w->left->color = RBTREE_BLACK;
        rbtree_right_rotate(t, x->parent);
        x = t->root;
      }
    }
  }

  x->color = RBTREE_BLACK;
}

node_t *rbtree_find_successor(rbtree *t, node_t *x) {
  while (x->left != t->nil) {
    x = x->left;
  }

  return x;
}

int rbtree_erase(rbtree *t, node_t *z) {
  // TODO: implement erase
  node_t* tmp = rbtree_find(t, z->key); 
  if (tmp == NULL) // 없는 값은 지울 수 없음
    return -1; 

  node_t *y = z;
  color_t y_original_color = y->color;
  node_t *x;

  // z의 왼쪽 자식이 없으면
  if (z->left == t->nil) {
    printf("Case 1\n");
    x = z->right; // 오른쪽 자식이 z를 대체
    rbtree_transplant(t, z, z->right);
  }
  else if (z->right == t->nil) { // 오른쪽 자식이 없으면
    printf("Case 2\n");
    x = z->left;
    rbtree_transplant(t, z, z->left);
  }
  else { // 양쪽 자식이 있으면 후임자 찾기
    printf("Case 3\n");
    y = rbtree_find_successor(t, z->right);
    y_original_color = y->color;
    x = y->right;

    if (y->parent == z) { // 후임자가 z에 바로 연결되어 있으면
      x->parent = y;
    }
    else {
      //printf("척추 펴기\n");
      rbtree_transplant(t, y, y->right); // 후임자를 후임자의 자식이 대체
      y->right = z->right; // Z의 자식을 후임자에 붙여줌
      y->right->parent = y;
    }
    
    //printf("깜깜부\n");
    rbtree_transplant(t, z, y);
    y->left = z->left;
    y->left->parent = y;
    y->color = z->color;
  }

  if (y_original_color == RBTREE_BLACK) {
    rbtree_delete_fixup(t, x);
  }
  printf("%d 지웠음 \n", z->key);
  free(z);

  return 0;
}

int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) {
  // TODO: implement to_array
  return 0;
}

void rbtree_to_print(node_t *t, node_t *nil)
{
    // TODO: implement to_print
    if (t == nil)
    {
        printf("노드 바닥이에용\n");
        return;
    }
    printf("key = %d, color = %d \n", t->key, t->color);
    rbtree_to_print(t->left, nil);
    rbtree_to_print(t->right, nil);
}

void printTree(rbtree *t, node_t *cur, int level, int isLeft) {
    if (cur == t->nil) {
        return;
    }

    // 오른쪽 자식 노드 출력
    printTree(t, cur->right, level + 1, 0);

    // 현재 노드 출력
    for (int i = 0; i < level - 1; i++) {
        printf("    ");
    }
    if (level > 0) {
        printf(isLeft ? " \\_ " : " /⎺ ");  // 왼쪽 자식일 경우 "\\" 출력, 오른쪽 자식일 경우 "/" 출력
    }
    if (cur->color == RBTREE_RED)
    {
      printf("\x1b[31m%d\x1b[0m\n", cur->key);
    }
    else{
      printf("%d\n", cur->key);
    }

    // 왼쪽 자식 노드 출력
    printTree(t, cur->left, level + 1, 1);
}
