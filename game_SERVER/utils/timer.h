#ifndef __TIMER_H__
#define __TIMER_H__

// ����: ���һ�����Ա�����һ�ε�timer,
// ����1:ÿһ���߳�/���̶�Ӧ�����Լ���timer list,�������ܱ�֤,�Լ����߳�
// ���timer��������ʱ������ͬһ���߳�����;
// ����2: ��timer�����Ժ����Ӧ����;
// ����3: ��timer�����Ժ���Ҫ������������������û������ݣ�
// ����4: ���Ƕ������Ժ󴥷����ǵ�timer��Ӧ;
// ����ֵ: ÿһ��timer�������һ�����ظ���id��,���id�ž���timer�ҵ���
// Ψһ�ı�ʶ��;
unsigned int
add_timer(struct timer_list* list, 
          void (*on_timer)(void* udata), 
		  void* udata, float after_sec);

// ����: ȡ����һ��timer�Ĵ���
// ����1: timer list;
// ����2: add_timer���ص�timeid, ���ǿ���ͨ�����timeid��timerlist�����ҵ����
// timer;
void
cancel_timer(struct timer_list* list, unsigned int timeid);

// ����ÿ��һ��ʱ�䶼�������timer��������Σ�֪����cancel�����timer
unsigned int
schedule_timer(struct timer_list* list,
               void(*on_timer)(void* udata),
			   void* udata, float after_sec);


#endif

