#ifndef __TIMER_LIST_H__
#define __TIMER_LIST_H__

// ����timer list,
struct timer_list*
create_timer_list();

// ɨ��ִ�����ǵ�timer list���� ��ʱ��ĺ���
// ���ص���: timer list������һ���������timer�����ʱ��;

int
update_timer_list(struct timer_list* list);

// end 
// ���ٵ�timer list
void
destroy_timer_list(struct timer_list* list);

#endif

