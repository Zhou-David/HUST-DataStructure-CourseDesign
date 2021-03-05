#include<stdio.h>
#include<stdlib.h>
#include<time.h>
int boolCount;  //布尔变元数量
int clauseCount;  //子句数量
char fileName[100]; //文件名

//十字链表结构体
typedef struct SATNode{
	int data;  //数据域
	SATNode* next;
}SATNode;
typedef struct SATList {
	SATNode* head;  //表头
	SATList* next;
}SATList;

//函数声明
int ReadFile(SATList*& cnf);
void destroyClause(SATList*& cnf);
int isUnitClause(SATNode* cnf);
int evaluateClause(SATNode* cnf, int v[]);
int removeClause(SATList*& cnf, SATList*& root);
int removeNode(SATNode*& cnf, SATNode*& head);
int addClause(SATList* cnf, SATList*& root);
int emptyClause(SATList* cnf);
int DPLL(SATList*& cnf, int value[]);
void CopyClause(SATList*& a, SATList* b);
int WriteFile(int result, double time, int value[]);
void CreateBinary(int size);
int SolvePuzzle(int chess[], int size);

//函数定义
/*
 * 函数名称: main
 * 接受参数: void
 * 函数功能: 主函数
 * 返回值: int
 */
int main(void)
{
	SATList* CNFList = NULL, * lp;
	SATNode* tp;
	clock_t start, finish;  //设置时间变量
	double time;
	int op = 1, i, result;
	int* value;  //保存结果
	while (op)
	{
		system("cls");	printf("\n\n");
		printf("		功能菜单 \n");
		printf("-------------------------------------------------\n");
		printf("1.读取cnf文件	        2.遍历输出每个子句\n");
		printf("3.DPLL求解算例并保存	4.二进制数独游戏\n");
		printf("0.退出\n");
		printf("-------------------------------------------------\n");
		printf("	请选择你的操作[0~4]:");
		scanf_s("%d", &op);
		switch (op)
		{
		case 1:
			printf("输入要读取的cnf文件:");
			scanf_s("%s", fileName, 100);
			ReadFile(CNFList);
			getchar(); getchar();
			break;
		case 2:
			if (CNFList == NULL) printf("未导入文件\n");
			else
			{
				printf("cnf子句如下：\n");
				for (lp = CNFList; lp != NULL; lp = lp->next)
				{
					for (tp = lp->head; tp != NULL; tp = tp->next)
					{
						printf("%d ", tp->data);
					}
					printf("\n");
				}
			}
			getchar(); getchar();
			break;
		case 3:
			if (CNFList == NULL) printf("未导入文件\n");
			else
			{
				value = (int*)malloc(sizeof(int) * (boolCount + 1));
				for (i = 1; i <= boolCount; i++) value[i] = 1;  //初始化，均赋为1
				start = clock();  //计时开始;
				result = DPLL(CNFList, value);
				finish = clock();    //结束
				printf("求解结果：%d\n", result);
				if (result == 1)
				{
					for (i = 1; i <= boolCount; i++)
					{
						if (value[i] == 1) printf("%d ", i);
						else printf("%d ", -i);
					}
					printf("\n");
				}
				time = (double)(finish - start) / CLOCKS_PER_SEC;//计算运行时间
				printf("运行时间=%lfms\n", time * 1000);//输出运行时间
				if (WriteFile(result, time, value) == 1)
					printf("结果已保存至同名文件.res\n");
				else printf("结果保存失败\n");
			}
			getchar(); getchar();
			break;
		case 4:
			printf("生成棋盘大小(为偶数)：");
			scanf_s("%d", &i);
			CreateBinary(i);
			getchar();
			break;
		case 0:
			break;
		}
	}
	return 0;
}

/*
 * 函数名称: ReadFile
 * 接受参数: SATList*&
 * 函数功能: 用文件指针fp打开用户指定的文件，并读取文件内容保存到给定参数中，读取成功返回1，失败返回0
 * 返回值: int
 */
int ReadFile(SATList*& cnf)
{
	FILE* fp;
	char ch;
	int number, i;
	SATList* lp;
	SATNode* tp;
	if (fopen_s(&fp, fileName, "r")) {
		printf("文件打开失败!\n");
		return 0;
	}
	while ((ch = getc(fp)) == 'c') {
		while ((ch = getc(fp)) != '\n')
			continue;           //弃去一整行
	}   //运行到此，已经读取了字符p
	getc(fp); getc(fp); getc(fp); getc(fp); //弃去cnf三个字母
	fscanf_s(fp, "%d", &boolCount);    //p后的第1个数值是布尔变元数量
	fscanf_s(fp, "%d", &clauseCount);  //p后的第2个数值是子句数量
	cnf = (SATList*)malloc(sizeof(SATList));
	cnf->next = NULL;
	cnf->head = (SATNode*)malloc(sizeof(SATNode));
	cnf->head->next = NULL;
	lp = cnf;
	tp = cnf->head;

	//创建数量为clauseCount的子句, i为计数器
	for (i = 0; i < clauseCount; i++, lp = lp->next, tp = lp->head)
	{
		fscanf_s(fp, "%d", &number);
		for (; number != 0; tp = tp->next)
		{
			tp->data = number;  //数据域赋值
			tp->next = (SATNode*)malloc(sizeof(SATNode));  //开辟新结点
			fscanf_s(fp, "%d", &number);
			if (number == 0) tp->next = NULL;
		}
		lp->next = (SATList*)malloc(sizeof(SATList));  //开辟新表
		lp->next->head = (SATNode*)malloc(sizeof(SATNode));
		if (i == clauseCount - 1)
		{
			lp->next = NULL;
			break;
		}
	}
	printf("读取完毕\n");
	fclose(fp);
	return 1;
}

/*
 * 函数名称: destroyClause
 * 接受参数: SATList*&
 * 函数功能: 销毁链表
 * 返回值: int
 */
void destroyClause(SATList*& cnf)
{
	SATList* lp1, * lp2;
	SATNode* tp1, * tp2;
	for (lp1 = cnf; lp1 != NULL; lp1 = lp2)
	{
		lp2 = lp1->next;
		for (tp1 = lp1->head; tp1 != NULL; tp1 = tp2)
		{
			tp2 = tp1->next;
			free(tp1);
		}
		free(lp1);
	}
	cnf = NULL;
}

/*
 * 函数名称: isUnitClause
 * 接受参数: SATNode*
 * 函数功能: 判断是否为单子句，是返回1，不是返回0
 * 返回值: int
 */
int isUnitClause(SATNode* cnf)
{
	if (cnf != NULL && cnf->next == NULL)
		return 1;
	else
		return 0;
}

/*
 * 函数名称: evaluateClause
 * 接受参数: SATList*
 * 函数功能: 评估子句的真假状态，真返回1，假返回0
 * 返回值: int
 */
int evaluateClause(SATNode* cnf,int v[])
{
	SATNode* tp = cnf;
	while (tp != NULL)
	{
		if (tp->data > 0 && v[tp->data] == 1 ||
			tp->data < 0 && v[-tp->data] == 0)
			return 1;
	}
	return 0;
}

/*
 * 函数名称: removeClause
 * 接受参数: SATList*,SATList*
 * 函数功能: 在已有的十字链表中删除指定的子句，删除成功返回1，失败返回0
 * 返回值: int
 */
int removeClause(SATList*& cnf, SATList*& root)
{
	SATList* lp = root;
	if (lp == cnf) root = root->next;  //删除为头
	else
	{
		while (lp != NULL && lp->next != cnf) lp = lp->next;
		lp->next = lp->next->next;
	}
	free(cnf);
	cnf = NULL;
	return 1;
}

/*
 * 函数名称: removeNote
 * 接受参数: SATNode*,SATNode*
 * 函数功能: 在指定的子句中删除指定的文字，删除成功返回1，失败返回0
 * 返回值: int
 */
int removeNode(SATNode*& cnf, SATNode*& head)
{
	SATNode* lp = head;
	if (lp == cnf) head = head->next;  //删除为头
	else
	{
		while (lp != NULL && lp->next != cnf) lp = lp->next;
		lp->next = lp->next->next;
	}
	free(cnf);
	cnf = NULL;
	return 1;
}

/*
 * 函数名称: addClause
 * 接受参数: SATList*,SATList*
 * 函数功能: 在已有的十字链表中添加指定的子句，添加成功返回1，失败返回0
 * 返回值: int
 */
int addClause(SATList* cnf, SATList*& root)
{
	//直接插入在表头
	if (cnf != NULL)
	{
		cnf->next = root;
		root = cnf;
		return 1;
	}
	return 0;
}

/*
 * 函数名称: emptyClause
 * 接受参数: SATList*
 * 函数功能: 判断是否含有空子句，是返回1，不是返回0
 * 返回值: int
 */
int emptyClause(SATList* cnf)
{
	SATList* lp = cnf;
	while (lp != NULL)
	{
		if (lp->head == NULL) return 1;
		lp = lp->next;
	}
	return 0;
}

/*
 * 函数名称: CopyClause
 * 接受参数: SATList*,SATList*
 * 函数功能: 将链表b的值复制到链表a中
 * 返回值: void
 */
void CopyClause(SATList*& a, SATList* b)
{
	SATList* lpa,*lpb;
	SATNode* tpa,*tpb;
	a = (SATList*)malloc(sizeof(SATList));
	a->head = (SATNode*)malloc(sizeof(SATNode));
	a->next = NULL;
	a->head->next = NULL;
	for (lpb = b, lpa = a; lpb != NULL; lpb = lpb->next, lpa = lpa->next)
	{
		for (tpb = lpb->head, tpa = lpa->head; tpb != NULL; tpb = tpb->next, tpa = tpa->next)
		{
			tpa->data = tpb->data;
			tpa->next = (SATNode*)malloc(sizeof(SATNode));
			tpa->next->next = NULL;
			if (tpb->next == NULL)
			{
				free(tpa->next);
				tpa->next = NULL;
			}
		}
		lpa->next = (SATList*)malloc(sizeof(SATList));
		lpa->next->head = (SATNode*)malloc(sizeof(SATNode));
		lpa->next->next = NULL;
		lpa->next->head->next = NULL;
		if (lpb->next == NULL)
		{
			free(lpa->next->head);
			free(lpa->next);
			lpa->next = NULL;
		}
	}
}

/*
* 函数名称: DPLL
* 接受参数: SATList *
* 函数功能: 求解SAT问题，给出满足条件时的一个式子,若有解则返回1，无解返回0
* 返回值: int
*/
int DPLL(SATList*& cnf, int value[])
{
	SATList* tp = cnf, * lp = cnf, * sp;
	SATNode* dp;
	int* count, i, MaxWord, max, re; //count记录每个文字出现次数,MaxWord记录出现最多次数的文字
	count = (int*)malloc(sizeof(int) * (boolCount * 2 + 1));
FIND:	while (tp != NULL && isUnitClause(tp->head) == 0) tp = tp->next;  //找到表中的单子句
	if (tp != NULL)
	{
		//单子句规则简化
		if (tp->head->data > 0) value[tp->head->data] = 1;
		else value[-tp->head->data] = 0;
		re = tp->head->data;
		for (lp = cnf; lp != NULL; lp = sp)
		{
			sp = lp->next;

			//查找含有核心文字的句子
			for (dp = lp->head; dp != NULL; dp = dp->next)
			{
				if (dp->data == re)
				{
					removeClause(lp, cnf);  //删除子句，简化式子
					break;
				}
				if (dp->data == -re)
				{
					removeNode(dp, lp->head);  //删除文字，简化子句
					break;
				}
			}
		}
		//极简化规则简化后
		if (cnf == NULL)
		{
			free(count);
			return 1;
		}
		else if (emptyClause(cnf))
		{
			free(count);
			destroyClause(cnf);
			return 0;
		}
		tp = cnf;
		goto FIND;  //继续简化
	}
	for (i = 0; i <= boolCount * 2; i++) count[i] = 0;  //初始化

	//计算子句中各文字出现次数
	for (lp = cnf; lp != NULL; lp = lp->next)
	{
		for (dp = lp->head; dp != NULL; dp = dp->next)
		{
			if (dp->data > 0) count[dp->data]++;
			else count[boolCount - dp->data]++;
		}
	}
	max = 0;

	//找到出现次数最多的正文字
	for (i = 2; i <= boolCount; i++)
	{
		if (max < count[i])
		{
			max = count[i];
			MaxWord = i;
		}
	}

	if (max == 0)
	{
		//若没有出现正文字,找到出现次数最多的负文字
		for (i = boolCount + 1; i <= boolCount * 2; i++)
		{
			if (max < count[i])
			{
				max = count[i];
				MaxWord = -i;
			}
		}
	}
	free(count);
	lp = (SATList*)malloc(sizeof(SATList));
	lp->head = (SATNode*)malloc(sizeof(SATNode));
	lp->head->data = MaxWord;
	lp->head->next = NULL;
	lp->next = NULL;
	CopyClause(tp, cnf);
	addClause(lp, tp);
	if (DPLL(tp, value) == 1) return 1;  //在第一分支中搜索
	destroyClause(tp);
	lp = (SATList*)malloc(sizeof(SATList));
	lp->head = (SATNode*)malloc(sizeof(SATNode));
	lp->head->data = -MaxWord;
	lp->head->next = NULL;
	lp->next = NULL;
	addClause(lp, cnf);
	re = DPLL(cnf, value); //回溯到执行分支策略的初态进入另一分支
	destroyClause(cnf);
	return re;
}

/*
* 函数名称: WriteFile
* 接受参数: int,int,int[]
* 函数功能: 将运行结果保存至同名文件，文件拓展名为.res,保存成功返回1，失败返回0
* 返回值: int
*/
int WriteFile(int result, double time, int value[])
{
	FILE* fp;
	int i;
	for (i = 0; fileName[i] != '\0'; i++)
	{
		//修改拓展名
		if (fileName[i] == '.' && fileName[i + 4] == '\0')
		{
			fileName[i + 1] = 'r';
			fileName[i + 2] = 'e';
			fileName[i + 3] = 's';
			break;
		}
	}
	if (fopen_s(&fp, fileName, "w")) {
		printf("文件打开失败!\n");
		return 0;
	}
	fprintf(fp, "s %d\nv ", result);  //求解结果
	if (result == 1)
	{
		//保存解值
		for (i = 1; i <= boolCount; i++)
		{
			if (value[i] == 1) fprintf(fp, "%d ", i);
			else fprintf(fp, "%d ", -i);
		}
	}
	fprintf(fp, "\nt %lf", time * 1000);  //运行时间/毫秒
	fclose(fp);
	return 1;
}

/*
* 函数名称: CreateBinary
* 接受参数: int
* 函数功能: 按照指定大小生成棋盘
* 返回值: void
*/
void CreateBinary(int size)
{
	int* chess, i, j = 0;
	//开辟size*size大小的数组
	chess = (int*)malloc(sizeof(int) * (size * size + 1));
	for (i = 0; i <= size*size; i++) chess[i] = -1;  //初始化
	srand(time(0));  //时间随机
	chess[rand() % (size * size + 1)] = rand() % 2;  //随机给定棋盘一初始条件
	SolvePuzzle(chess, size);  //生成二进制数独棋盘
	for (i = 0; i <= rand() % (size / 2) + size * (size - 2); i++)
	{
		//随机选择size * (size - 2)到size * (size - 2)+(size / 2)个数字清空
		while (j == 0 || chess[j] == -1) j = rand() % (size * size + 1);  //选择合适的数字
		chess[j] = -1;
	}
	for (j = 0, i = 0; j <= size * size || i <= (size+1) * (size+1);)
	{
		if (j % size == 0)
		{
			//打印棋盘
			if (i % (size + 1) == 0 && i > 0) printf("┃\n");
			i++;
			if (i == 1) printf("┌-");
			else if (i < size + 1) printf("-┬-");
			else if (i == size + 1) printf("-┐");
			else if (i == (size + 1) * size + 1) printf("└-");
			else if (i == (size + 1) * (size + 1)) printf("-┘");
			else if (i > (size + 1)* size) printf("-┷-");
			else if (i % (size + 1) == 1) printf("├-");
			else if (i % (size + 1) == 0) printf("-┤");
			else printf("-┿-");
		}
		if (i % (size + 1) == 0)
		{
			//输出数独
			if (j % size == 0) printf("\n");
			j++;
			if (j > size* size) break;
			if (chess[j] == -1) printf("┃  ");
			else printf("┃ %d", chess[j]);
		}
	}
	printf("按Enter键输出数独答案");
	getchar(); getchar();
	SolvePuzzle(chess, size);  //求解答案
	for (i = 1; i <= size * size; i++)
	{
		printf("%d ", chess[i]);
		if (i % size == 0) printf("\n");
	}
	free(chess);
}

/*
* 函数名称: SolvePuzzle
* 接受参数: int[],int
* 函数功能: 求解输入棋盘，有解返回1，无解返回0
* 返回值: int
*/
int SolvePuzzle(int chess[], int size)
{
	SATList* cnf = NULL, * lp;
	SATNode* dp;
	int* remember, i, j, k, rol;
	boolCount = size * size;

	//添加单子句
	for (i = 1; i <= size * size; i++)
	{
		if (chess[i] == 0)
		{
			lp = (SATList*)malloc(sizeof(SATList));
			lp->head = (SATNode*)malloc(sizeof(SATNode));
			lp->head->next = NULL;
			lp->next = NULL;
			lp->head->data = -i;
			addClause(lp, cnf);
		}
		else if (chess[i] == 1)
		{
			lp = (SATList*)malloc(sizeof(SATList));
			lp->head = (SATNode*)malloc(sizeof(SATNode));
			lp->head->next = NULL;
			lp->next = NULL;
			lp->head->data = i;
			addClause(lp, cnf);
		}
	}

	//约束1：不出现连续三个重复数字
	for (i = 0; i < size; i++)
	{
		for (j = 0; j < size - 2; j++)
		{
			//横向，正文字
			lp = (SATList*)malloc(sizeof(SATList));
			lp->head = (SATNode*)malloc(sizeof(SATNode));
			lp->head->next = (SATNode*)malloc(sizeof(SATNode));
			lp->head->next->next = (SATNode*)malloc(sizeof(SATNode));
			lp->head->next->next->next = NULL;
			lp->next = NULL;
			lp->head->data = i * size + j + 1;
			lp->head->next->data = i * size + j + 2;
			lp->head->next->next->data = i * size + j + 3;
			addClause(lp, cnf);
			//横向，负文字
			lp = (SATList*)malloc(sizeof(SATList));
			lp->head = (SATNode*)malloc(sizeof(SATNode));
			lp->head->next = (SATNode*)malloc(sizeof(SATNode));
			lp->head->next->next = (SATNode*)malloc(sizeof(SATNode));
			lp->head->next->next->next = NULL;
			lp->next = NULL;
			lp->head->data = -(i * size + j + 1);
			lp->head->next->data = -(i * size + j + 2);
			lp->head->next->next->data = -(i * size + j + 3);
			addClause(lp, cnf);
			//纵向，正文字
			lp = (SATList*)malloc(sizeof(SATList));
			lp->head = (SATNode*)malloc(sizeof(SATNode));
			lp->head->next = (SATNode*)malloc(sizeof(SATNode));
			lp->head->next->next = (SATNode*)malloc(sizeof(SATNode));
			lp->head->next->next->next = NULL;
			lp->next = NULL;
			lp->head->data = i + j * size + 1;
			lp->head->next->data = i + (j + 1) * size + 1;
			lp->head->next->next->data = i + (j + 2) * size + 1;
			addClause(lp, cnf);
			//纵向，负文字
			lp = (SATList*)malloc(sizeof(SATList));
			lp->head = (SATNode*)malloc(sizeof(SATNode));
			lp->head->next = (SATNode*)malloc(sizeof(SATNode));
			lp->head->next->next = (SATNode*)malloc(sizeof(SATNode));
			lp->head->next->next->next = NULL;
			lp->next = NULL;
			lp->head->data = -(i + j * size + 1);
			lp->head->next->data = -(i + (j + 1) * size + 1);
			lp->head->next->next->data = -(i + (j + 2) * size + 1);
			addClause(lp, cnf);
		}
	}

	//约束2：在每一行、每一列中1与0的个数相同
	remember = (int*)malloc(sizeof(int) * (size / 2 + 1));
	//每一行
	for (rol = 0; rol < size; rol++)
	{
		for (i = 0; i < size / 2 + 1; i++) remember[i] = i + 1;  //初始化
	COMBINATION1:for (i = size / 2; remember[i] <= size; remember[i]++)
	{
		lp = (SATList*)malloc(sizeof(SATList));
		lp->head = (SATNode*)malloc(sizeof(SATNode));
		lp->head->next = NULL;
		lp->next = NULL;
		for (j = 0, dp = lp->head; j < size / 2 + 1; j++, dp = dp->next)
		{
			dp->data = remember[j] + rol * size;
			if (j == size / 2) break;
			dp->next = (SATNode*)malloc(sizeof(SATNode));
			dp->next->next = NULL;
		}
		addClause(lp, cnf);
		lp = (SATList*)malloc(sizeof(SATList));
		lp->head = (SATNode*)malloc(sizeof(SATNode));
		lp->head->next = NULL;
		lp->next = NULL;
		for (j = 0, dp = lp->head; j < size / 2 + 1; j++, dp = dp->next)
		{
			dp->data = -(remember[j] + rol * size);
			if (j == size / 2) break;
			dp->next = (SATNode*)malloc(sizeof(SATNode));
			dp->next->next = NULL;
		}
		addClause(lp, cnf);
	}
	for (i = size / 2; i >= 0 && remember[i] >= size / 2 + i; i--);  //找到达到饱和的最高位
	if (i <= 0) continue;  //该行组合序列全部排完,进入下一行
	remember[i]++;
	for (j = i + 1; j < size / 2 + 1; j++) remember[j] = remember[j - 1] + 1; //序列后移
	goto COMBINATION1;
	}
	//每一列
	for (rol = 1; rol <= size; rol++)
	{
		for (i = 0; i < size / 2 + 1; i++) remember[i] = i;  //初始化
	COMBINATION2:for (i = size / 2; remember[i] < size; remember[i]++)
	{
		lp = (SATList*)malloc(sizeof(SATList));
		lp->head = (SATNode*)malloc(sizeof(SATNode));
		lp->head->next = NULL;
		lp->next = NULL;
		for (j = 0, dp = lp->head; j < size / 2 + 1; j++, dp = dp->next)
		{
			dp->data = remember[j] * size + rol;
			if (j == size / 2) break;
			dp->next = (SATNode*)malloc(sizeof(SATNode));
			dp->next->next = NULL;
		}
		addClause(lp, cnf);
		lp = (SATList*)malloc(sizeof(SATList));
		lp->head = (SATNode*)malloc(sizeof(SATNode));
		lp->head->next = NULL;
		lp->next = NULL;
		for (j = 0, dp = lp->head; j < size / 2 + 1; j++, dp = dp->next)
		{
			dp->data = -(remember[j] * size + rol);
			if (j == size / 2) break;
			dp->next = (SATNode*)malloc(sizeof(SATNode));
			dp->next->next = NULL;
		}
		addClause(lp, cnf);
	}
	for (i = size / 2; i >= 0 && remember[i] > size / 2 + i; i--);  //找到达到饱和的最高位
	if (i <= 0) continue;  //该列组合序列全部排完,进入下一列
	remember[i]++;
	for (j = i + 1; j < size / 2 + 1; j++) remember[j] = remember[j - 1] + 1; //序列后移
	goto COMBINATION2;
	}

	//约束3：不存在重复的行与重复的列
	//不重复行：
	for (i = 0; i < size - 1; i++)
	{
		for (j = i + 1; j < size; j++)
		{
			rol = boolCount; //记录添加变元之前变元数量
			for (k = 1; k <= size; k++)
			{
				//引入前一个新变元：
				boolCount++;
				//语句1：
				lp = (SATList*)malloc(sizeof(SATList));
				lp->head = (SATNode*)malloc(sizeof(SATNode));
				lp->head->next = (SATNode*)malloc(sizeof(SATNode));
				lp->head->next->next = NULL;
				lp->next = NULL;
				lp->head->data = i * size + k;
				lp->head->next->data = -boolCount;
				addClause(lp, cnf);
				//语句2：
				lp = (SATList*)malloc(sizeof(SATList));
				lp->head = (SATNode*)malloc(sizeof(SATNode));
				lp->head->next = (SATNode*)malloc(sizeof(SATNode));
				lp->head->next->next = NULL;
				lp->next = NULL;
				lp->head->data = -(j * size + k);
				lp->head->next->data = -boolCount;
				addClause(lp, cnf);
				//语句3：
				lp = (SATList*)malloc(sizeof(SATList));
				lp->head = (SATNode*)malloc(sizeof(SATNode));
				lp->head->next = (SATNode*)malloc(sizeof(SATNode));
				lp->head->next->next = (SATNode*)malloc(sizeof(SATNode));
				lp->head->next->next->next = NULL;
				lp->next = NULL;
				lp->head->data = -(i * size + k);
				lp->head->next->data = j * size + k;
				lp->head->next->next->data = boolCount;
				addClause(lp, cnf);
				//引入后一个新变元：
				boolCount++;
				//语句1：
				lp = (SATList*)malloc(sizeof(SATList));
				lp->head = (SATNode*)malloc(sizeof(SATNode));
				lp->head->next = (SATNode*)malloc(sizeof(SATNode));
				lp->head->next->next = NULL;
				lp->next = NULL;
				lp->head->data = -(i * size + k);
				lp->head->next->data = -boolCount;
				addClause(lp, cnf);
				//语句2：
				lp = (SATList*)malloc(sizeof(SATList));
				lp->head = (SATNode*)malloc(sizeof(SATNode));
				lp->head->next = (SATNode*)malloc(sizeof(SATNode));
				lp->head->next->next = NULL;
				lp->next = NULL;
				lp->head->data = j * size + k;
				lp->head->next->data = -boolCount;
				addClause(lp, cnf);
				//语句3：
				lp = (SATList*)malloc(sizeof(SATList));
				lp->head = (SATNode*)malloc(sizeof(SATNode));
				lp->head->next = (SATNode*)malloc(sizeof(SATNode));
				lp->head->next->next = (SATNode*)malloc(sizeof(SATNode));
				lp->head->next->next->next = NULL;
				lp->next = NULL;
				lp->head->data = i * size + k;
				lp->head->next->data = -(j * size + k);
				lp->head->next->next->data = boolCount;
				addClause(lp, cnf);
			}
			//添加长句：不重复行满足的关系
			lp = (SATList*)malloc(sizeof(SATList));
			lp->head = (SATNode*)malloc(sizeof(SATNode));
			lp->head->next = NULL;
			lp->next = NULL;
			for (k = rol + 1, dp = lp->head; k <= boolCount; k++, dp = dp->next)
			{
				dp->data = k;
				if (k == boolCount) break;
				dp->next = (SATNode*)malloc(sizeof(SATNode));
				dp->next->next = NULL;
			}
			addClause(lp, cnf);
		}
	}
	//不重复列
	for (i = 1; i <= size - 1; i++)
	{
		for (j = i + 1; j <= size; j++)
		{
			rol = boolCount; //记录添加变元之前变元数量
			for (k = 0; k < size; k++)
			{
				//引入前一个新变元：
				boolCount++;
				//语句1：
				lp = (SATList*)malloc(sizeof(SATList));
				lp->head = (SATNode*)malloc(sizeof(SATNode));
				lp->head->next = (SATNode*)malloc(sizeof(SATNode));
				lp->head->next->next = NULL;
				lp->next = NULL;
				lp->head->data = i + k * size;
				lp->head->next->data = -boolCount;
				addClause(lp, cnf);
				//语句2：
				lp = (SATList*)malloc(sizeof(SATList));
				lp->head = (SATNode*)malloc(sizeof(SATNode));
				lp->head->next = (SATNode*)malloc(sizeof(SATNode));
				lp->head->next->next = NULL;
				lp->next = NULL;
				lp->head->data = -(j + k * size);
				lp->head->next->data = -boolCount;
				addClause(lp, cnf);
				//语句3：
				lp = (SATList*)malloc(sizeof(SATList));
				lp->head = (SATNode*)malloc(sizeof(SATNode));
				lp->head->next = (SATNode*)malloc(sizeof(SATNode));
				lp->head->next->next = (SATNode*)malloc(sizeof(SATNode));
				lp->head->next->next->next = NULL;
				lp->next = NULL;
				lp->head->data = -(i + k * size);
				lp->head->next->data = j + k * size;
				lp->head->next->next->data = boolCount;
				addClause(lp, cnf);
				//引入后一个新变元：
				boolCount++;
				//语句1：
				lp = (SATList*)malloc(sizeof(SATList));
				lp->head = (SATNode*)malloc(sizeof(SATNode));
				lp->head->next = (SATNode*)malloc(sizeof(SATNode));
				lp->head->next->next = NULL;
				lp->next = NULL;
				lp->head->data = -(i + k * size);
				lp->head->next->data = -boolCount;
				addClause(lp, cnf);
				//语句2：
				lp = (SATList*)malloc(sizeof(SATList));
				lp->head = (SATNode*)malloc(sizeof(SATNode));
				lp->head->next = (SATNode*)malloc(sizeof(SATNode));
				lp->head->next->next = NULL;
				lp->next = NULL;
				lp->head->data = j + k * size;
				lp->head->next->data = -boolCount;
				addClause(lp, cnf);
				//语句3：
				lp = (SATList*)malloc(sizeof(SATList));
				lp->head = (SATNode*)malloc(sizeof(SATNode));
				lp->head->next = (SATNode*)malloc(sizeof(SATNode));
				lp->head->next->next = (SATNode*)malloc(sizeof(SATNode));
				lp->head->next->next->next = NULL;
				lp->next = NULL;
				lp->head->data = i + k * size;
				lp->head->next->data = -(j + k * size);
				lp->head->next->next->data = boolCount;
				addClause(lp, cnf);
			}
			//添加长句：不重复行满足的关系
			lp = (SATList*)malloc(sizeof(SATList));
			lp->head = (SATNode*)malloc(sizeof(SATNode));
			lp->head->next = NULL;
			lp->next = NULL;
			for (k = rol + 1, dp = lp->head; k <= boolCount; k++, dp = dp->next)
			{
				dp->data = k;
				if (k == boolCount) break;
				dp->next = (SATNode*)malloc(sizeof(SATNode));
				dp->next->next = NULL;
			}
			addClause(lp, cnf);
		}
	}
	free(remember);
	remember = (int*)malloc(sizeof(int) * (boolCount + 1));
	for (i = 1; i <= boolCount; i++) remember[i] = 1;  //初始化
	if (DPLL(cnf, remember) == 1)
	{
		for (i = 1; i <= size * size; i++) chess[i] = remember[i];
		free(remember);
		destroyClause(cnf);
		return 1;
	}
	else
	{
		free(remember);
		destroyClause(cnf);
		return 0;
	}
}