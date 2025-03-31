#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "utils.h"
#include "resource.h"

Node* scheduleFCFS(Node* head);  // 返回一个新的链表
Node* schedulePriority(Node *head); // 优先级调度器
Node* scheduleOptimized(Node* head);  // 返回一个新的链表
void free_node_list(Node *list); // 释放链表内存
bool is_time_conflict(const Booking *a, const Booking *b); // 检查时间槽冲突
#endif // SCHEDULER_H