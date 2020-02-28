#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "timer.h"
#include "timer_list.h"

#define my_malloc malloc
#define my_free free

// ƽ̨���
#ifdef WIN32
#include <windows.h>
static unsigned int
get_cur_ms() {
	return GetTickCount();
}
#else
static unsigned int
get_cur_ms() {
	struct timeval tv;
	struct timezone tz;
	gettimeofday(&tv, &tz);

	return ((tv.tv_usec / 1000) + tv.tv_sec * 1000);
}
#endif

// end 
struct timer {
	void(*on_timer)(void* udata); // timer�ص�������ָ��;
	void* udata; // �û�Ҫ��timer�����󴫸��ص�����������ָ��;

	unsigned int duration; // ������ʱ����
	unsigned int end_time_stamp; // ������ʱ���,����;
	int repeat; // �ظ��Ĵ�����-1,��ʾһֱ����;
	unsigned int timeid; // ���timer��Ӧ��timerid;

	struct timer* hash_next; // ָ��ͬһ��hashֵ����һ���ڵ�;
};

// hash�����������ǵ�timer, hash value timerid;
#define HASH_COUNT 1024

struct timer_list {
	unsigned int global_timeid; // ��ǰ����ʹ�õ�timer��id�ţ�һֱ��������;
	// �����������Ϊ timer��ID��,��0��ʼ
	struct timer* running_timer;
	struct timer* hash_timers[HASH_COUNT]; // HASH ����,����ָ����timer��ӵ��ͬһ��hashֵ������
};

struct timer_list*
create_timer_list()  {
	struct timer_list* list = my_malloc(sizeof(struct timer_list));
	memset(list, 0, sizeof(struct timer_list));

	return list;
}

unsigned int
add_timer(struct timer_list* list,
          void(*on_timer)(void* udata),
          void* udata, float after_sec) {
	int timeid = list->global_timeid ++; // Ψһ�����ϼ�
	struct timer* t = my_malloc(sizeof(struct timer));
	memset(t, 0, sizeof(struct timer));
	t->repeat = 1;
	t->duration = (unsigned int)(after_sec * 1000);
	t->end_time_stamp = get_cur_ms() + t->duration; // timer������ʱ���,���ڵ�ʱ�� + ���ٺ����Ժ��ʱ��
	t->timeid = timeid;
	t->udata = udata;
	t->on_timer = on_timer;

	// ��hash����
	int index = timeid % HASH_COUNT;
	struct timer** walk = &list->hash_timers[index];
	while (*walk) {
		walk = &(*walk)->hash_next;
	}
	*walk = t;
	// end 

	return timeid;
}

unsigned int
schedule_timer(struct timer_list* list,
               void(*on_timer)(void* udata),
			   void* udata, float after_sec) {
	int timeid = list->global_timeid++; // Ψһ�����ϼ�
	struct timer* t = my_malloc(sizeof(struct timer));
	memset(t, 0, sizeof(struct timer));
	t->repeat = -1; // ��Զִ����ȥ;
	t->duration = (unsigned int)(after_sec * 1000);
	t->end_time_stamp = get_cur_ms() + t->duration; // timer������ʱ���,���ڵ�ʱ�� + ���ٺ����Ժ��ʱ��
	t->timeid = timeid;
	t->udata = udata;
	t->on_timer = on_timer;

	// ��hash����
	int index = timeid % HASH_COUNT;
	struct timer** walk = &list->hash_timers[index];
	while (*walk) {
		walk = &(*walk)->hash_next;
	}
	*walk = t;
	// end 

	return timeid;
}

void
cancel_timer(struct timer_list* list, unsigned int timeid) {
	// ��ס�Լ�ɾ���Լ��� timeid�ڻص���ʱ��
	if (list->running_timer != NULL && list->running_timer->timeid == timeid) {
		list->running_timer->repeat = 1;
		return;
	}
	// end

	int hash_index = timeid % HASH_COUNT;
	struct timer** walk = &list->hash_timers[hash_index];

	while (*walk) {
		if ((*walk)->timeid == timeid) { // �ҵ�timer,�����Ƴ������棻
			struct timer* t = (*walk);
			*walk = (*walk)->hash_next;

			my_free(t); // �����һ��timer;
			return;
		}
		walk = &(*walk)->hash_next;
	}
}

void
destroy_timer_list(struct timer_list* list) {
	for (int i = 0; i < HASH_COUNT; i++) {
		struct timer** walk = &list->hash_timers[i];
		while (*walk) {
			struct timer* t = (*walk);
			*walk = (*walk)->hash_next;
			my_free(t);
		}
	}

	my_free(list);
}

int
update_timer_list(struct timer_list* list) {
	unsigned int min_msec = UINT_MAX;
	struct timer* min_timer = NULL;

	unsigned int start_msec = get_cur_ms();
	list->running_timer = NULL;
	// ɨ�����е�timer,ִ�е�ʱ���timer;
	for (int i = 0; i < HASH_COUNT; i++) {
		struct timer** walk = &list->hash_timers[i];
		while (*walk) {
			unsigned int cur_time = get_cur_ms(); // ��ȡ��ǰ��ʱ��
			if ((*walk)->end_time_stamp <= cur_time) { // ��Ҫ������
				list->running_timer = (*walk);
				if ((*walk)->on_timer) {
					(*walk)->on_timer((*walk)->udata);
				}
				list->running_timer = NULL;

				if ((*walk)->repeat > 0) {
					(*walk)->repeat --;
					if ((*walk)->repeat == 0) { // ��timer�Ƴ���ȥ;
						struct timer* t = (*walk);
						(*walk) = (*walk)->hash_next;
						my_free(t);
					}
					else {
						(*walk)->end_time_stamp = get_cur_ms() + (*walk)->duration;
						if (((*walk)->end_time_stamp - start_msec) < min_msec) {
							min_timer = (*walk);
							min_msec = (*walk)->end_time_stamp - start_msec;
						}
						walk = &((*walk)->hash_next);
					}
				}
				else {
					(*walk)->end_time_stamp = get_cur_ms() + (*walk)->duration;
					if (((*walk)->end_time_stamp - start_msec) < min_msec) {
						min_timer = (*walk);
						min_msec = (*walk)->end_time_stamp - start_msec;
					}
					walk = &((*walk)->hash_next);
				}
			}
			else { // ����Ҫ����
				if (((*walk)->end_time_stamp - start_msec) < min_msec) {
					min_timer = (*walk);
					min_msec = (*walk)->end_time_stamp - start_msec;
				}
				walk = &((*walk)->hash_next);
			}
		}
	}

	if (min_timer != NULL) { // ������һ�����ȵ�timer,
		return (min_timer->end_time_stamp - get_cur_ms());
	}

	return -1; // timer list�Ѿ�û���κ�timer�ˡ�
}