#include <stdio.h>
#include <stdlib.h>
#define N 5
int numOfMalloc = 0;
int tt = 0;
struct map {
        unsigned m_size;// ���з����ĳ���
        char *m_addr;	// ����������ʼ��ַ
 };
struct map coremap[N];
struct map *startPoint;

char* lmalloc(unsigned size)
{
    int total = 0;
    register char *a;
    register struct map *bp;
    int i = 0;
    int nonZero = 0;
    int cycle = 1;
    for(i=0;i<N;i++)
    {
        total += coremap[i].m_size;
        nonZero++;
    }
    //printf("total:%d\n",total);
    if(total < size||(total == size && nonZero > 1)){return 0;}//�ռ䲻��Ĵ���
    if(numOfMalloc == 0){startPoint = coremap;} //ѭ��ת����һ������ı�־��ͬʱ���ڵ�һ�ε��ú���ʱ����ʼ����ʼָ��

    for (bp = startPoint; bp->m_size; bp++)
    {

	    if(bp->m_size >= size) {
	        a = bp->m_addr;
	        bp->m_addr += size;
	        if((bp->m_size -= size) == 0)
		        {startPoint = bp;
		           do	{
		            bp++;
		            (bp-1)->m_addr = bp->m_addr;
		        } while((bp-1)->m_size = bp->m_size);
                // printf("1 StartPoint:%d",startPoint->m_size);
                //if((bp->m_size -= size) == 0&&startPoint->m_size == 0){startPoint = coremap;}
                //return (a);
		        }
		        startPoint = bp++;
		        numOfMalloc++;//ѭ����־1
		        cycle = 2;//ѭ����־2
               // printf("2 StartPoint:%d",startPoint->m_size);
	        return(a);
	    }
    }

    if(cycle == 1){numOfMalloc = 0;lmalloc(size);tt++;}//�ݹ���ã�ʵ��ѭ��


}

void lfree(unsigned size, char *aa)
{
  struct map *bp;
    char *a,*t;
    unsigned tt;

    a = aa;
    for(bp=coremap;bp->m_addr<=a&&bp->m_size!=0;bp++);
    if(bp>coremap&&(bp-1)->m_addr+(bp-1)->m_size ==a){
    (bp-1)->m_size += size; //���1
    if(a+size == bp->m_addr)
      {
        (bp-1)->m_size += bp->m_size;
        while(bp->m_size)  //���2
        {
            bp++;
            (bp-1)->m_addr = bp->m_addr;
            (bp-1)->m_size = bp->m_size;
        }
       // printf("2.1 StartPoint:%d\n",startPoint->m_size);
        //printf("2\n");
        if(startPoint->m_addr > bp->m_addr)startPoint++;
        //printf("2.2 StartPoint:%d\n",startPoint->m_size);
      }
    }
    else{
    if(a+size==bp->m_addr&&bp->m_size)
    {
        bp->m_addr -= size; //���3
        bp->m_size += size;
        //printf("3.1 StartPoint:%d\n",startPoint->m_size);
         //printf("3\n");
        if(startPoint->m_addr > bp->m_addr)startPoint++;
        else if(startPoint->m_addr-bp->m_size == bp->m_addr)startPoint = bp;
       // printf("3.2 StartPoint:%d\n",startPoint->m_size);
    }
    else if(size)
    {do{                   //���4
        t = bp->m_addr;
        bp->m_addr = a;
        a = t;
        tt = bp->m_size;
        bp->m_size = size;
        bp++;
      }while(size = tt);
      //printf("4.1 StartPoint:%d\n",startPoint->m_size);
       //printf("4\n");
    if(startPoint->m_addr > bp->m_addr)startPoint++;
     else if(startPoint->m_addr-bp->m_size == bp->m_addr)startPoint = bp;}}
     //printf("4.2 StartPoint:%d\n",startPoint->m_size);
    }

void initCoremap(char *base_addr,unsigned size)//��ʼ����������
{
    int i;
    coremap[0].m_addr = base_addr;
    coremap[0].m_size = size;
    for(i = 1;i < N;i++)
    {
        coremap[i].m_addr = 0;
        coremap[i].m_size = 0;
    }
    return;
}

void printCoremap(char *base_addr)//��ӡ�洢����
{
    FILE *p;

    p=fopen("output.txt","a");
    int i,zero = 0;
    //printf("%u\n",coremap[0].m_addr);
    fprintf(p,"             Size\t\t\tR_Addr\t\tAddr\n");
    for(i = 0;i < N;i++)
    {
        if(coremap[i].m_size != 0)
            fprintf(p,"coremap[%d]:%u\t\t%u\t\t%u\n",i,coremap[i].m_size,(coremap[i].m_addr-base_addr),coremap[i].m_addr);
        else fprintf(p,"coremap[%d]:%u\t\t%d\t\t%d\n",i,coremap[i].m_size,zero,zero);

    }

}

void showHelp()//��ʾ����
{
    printf("m size : �����ڴ�\n");
    printf("f size addr : �ͷ��ڴ�\n");
    printf("p : ��ӡcoremap��\n");
    printf("h : ��ʾ����\n");
    printf("q : �˳�ִ��\n");
}

int main()
{

    unsigned size,total_size;
    char *base_addr,*addr,*start_addr,cmdchar;
    unsigned r_addr;
    printf("Please input the malloc size:");
    scanf("%u",&total_size);
    base_addr = malloc(total_size);//��ϵͳ����һ��ռ�
    initCoremap(base_addr,total_size);
    FILE *file;
	file = fopen("output.txt","w");//���I/Oת��
    do
    {
        printf("Please input command,input h for help.");
        do cmdchar = getchar();
        while(cmdchar == ' '||cmdchar == '\t'||cmdchar == '\n');//�̿ո�
        switch(cmdchar)//����ʶ�𡢴�����
        {

            case 'm':
            scanf("%u",&size);
            if(size < 0||size >= total_size)
            {
                printf("����size����\n");
                break;
            }
            start_addr = lmalloc(size);
            if(start_addr == 0)
            {
                printf("�ռ䲻�㣡\n");
                break;
            }

            printCoremap(base_addr);
            break;

            case 'f':
            scanf("%u%u",&size,&r_addr);
            if(r_addr<0||r_addr >= total_size)
            {
                printf("������Χ���޷��ͷſռ䣡\n");
                break;
            }
            addr = base_addr + r_addr;
            lfree(size,addr);
            printCoremap(base_addr);
            break;

            case 'p':
            printCoremap(base_addr);
            break;
            case 'h':
            showHelp();
            break;
            case 'q':
            break;
            default:
            printf("�Ƿ����������h�鿴�����������룡\n");
            continue;

        }
    }while(cmdchar != 'q');

    free(base_addr);//�ͷ�����Ŀռ�

    return 0;
}

