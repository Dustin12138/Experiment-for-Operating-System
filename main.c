#include <stdio.h>
#include <stdlib.h>
#define N 5
int numOfMalloc = 0;
int tt = 0;
struct map {
        unsigned m_size;// 空闲分区的长度
        char *m_addr;	// 空闲区的起始地址
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
    if(total < size||(total == size && nonZero > 1)){return 0;}//空间不足的处理
    if(numOfMalloc == 0){startPoint = coremap;} //循环转到第一个表项的标志，同时，在第一次调用函数时，初始化起始指针

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
		        numOfMalloc++;//循环标志1
		        cycle = 2;//循环标志2
               // printf("2 StartPoint:%d",startPoint->m_size);
	        return(a);
	    }
    }

    if(cycle == 1){numOfMalloc = 0;lmalloc(size);tt++;}//递归调用，实现循环


}

void lfree(unsigned size, char *aa)
{
  struct map *bp;
    char *a,*t;
    unsigned tt;

    a = aa;
    for(bp=coremap;bp->m_addr<=a&&bp->m_size!=0;bp++);
    if(bp>coremap&&(bp-1)->m_addr+(bp-1)->m_size ==a){
    (bp-1)->m_size += size; //情况1
    if(a+size == bp->m_addr)
      {
        (bp-1)->m_size += bp->m_size;
        while(bp->m_size)  //情况2
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
        bp->m_addr -= size; //情况3
        bp->m_size += size;
        //printf("3.1 StartPoint:%d\n",startPoint->m_size);
         //printf("3\n");
        if(startPoint->m_addr > bp->m_addr)startPoint++;
        else if(startPoint->m_addr-bp->m_size == bp->m_addr)startPoint = bp;
       // printf("3.2 StartPoint:%d\n",startPoint->m_size);
    }
    else if(size)
    {do{                   //情况4
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

void initCoremap(char *base_addr,unsigned size)//初始化空闲区表
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

void printCoremap(char *base_addr)//打印存储区表
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

void showHelp()//显示帮助
{
    printf("m size : 分配内存\n");
    printf("f size addr : 释放内存\n");
    printf("p : 打印coremap表\n");
    printf("h : 显示帮助\n");
    printf("q : 退出执行\n");
}

int main()
{

    unsigned size,total_size;
    char *base_addr,*addr,*start_addr,cmdchar;
    unsigned r_addr;
    printf("Please input the malloc size:");
    scanf("%u",&total_size);
    base_addr = malloc(total_size);//向系统申请一块空间
    initCoremap(base_addr,total_size);
    FILE *file;
	file = fopen("output.txt","w");//输出I/O转向
    do
    {
        printf("Please input command,input h for help.");
        do cmdchar = getchar();
        while(cmdchar == ' '||cmdchar == '\t'||cmdchar == '\n');//吞空格
        switch(cmdchar)//命令识别、错误处理
        {

            case 'm':
            scanf("%u",&size);
            if(size < 0||size >= total_size)
            {
                printf("参数size错误！\n");
                break;
            }
            start_addr = lmalloc(size);
            if(start_addr == 0)
            {
                printf("空间不足！\n");
                break;
            }

            printCoremap(base_addr);
            break;

            case 'f':
            scanf("%u%u",&size,&r_addr);
            if(r_addr<0||r_addr >= total_size)
            {
                printf("超出范围，无法释放空间！\n");
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
            printf("非法命令！请输入h查看帮助再行输入！\n");
            continue;

        }
    }while(cmdchar != 'q');

    free(base_addr);//释放申请的空间

    return 0;
}

