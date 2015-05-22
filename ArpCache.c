# Arp-Cache-Implement-Using-Hash-Table
# 哈希表实例之arp缓存池
#ifndef _READANDPRINTARPLIST_C_
#define _READANDPRINTARPLIST_C_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define debug 1
/* The Path Of Cache File */
#ifdef debug
#define ARP_ENTRY_FILE  "./arp_test"
#else
#define ARP_ENTRY_FILE  "/proc/net/arp"
#endif
#define HASH_TABLE_MAX_SIZE	1000
#define LINESIZE	100
#define IPSIZE		20
#define MACSIZE		20

typedef struct SimpleArp{
      //char flag;
        char sIp[IPSIZE];
        char sMac[MACSIZE];
        struct SimpleArp *pNext;
}HashNode;

/* The Hash Table */
HashNode* pHashTable[HASH_TABLE_MAX_SIZE];
int pI4HashTableSize;

/*
 * Return: 1-success
 * */
int HashTableInit(void)
{
	pI4HashTableSize = 0;
	bzero(pHashTable, sizeof(HashNode*)*HASH_TABLE_MAX_SIZE);
	return 1;
}

/*
 * Return: The HashValue Of The Str
 * Notes: 关于IP/Mac的哈希算法，utt更多使用按每个字节^(异或)，这里未使用
 * */
unsigned int SDBMHash(char* pStr)
{
	unsigned int i4Hash = 0;
	while(*pStr)
	{
		i4Hash = (*pStr++) + (i4Hash << 6) + (i4Hash << 16) - i4Hash;
	}
	return (i4Hash & 0x7FFFFFFF);
}

/*
 * Return: 1-success, 0-faliure
 * Notes: 哈希表的潜规则定义了作为key的字串在整个哈希表里最多只能存在一个
 * */
int HashTableInsert(char* pSKey, char* pSMac)
{
	if(pI4HashTableSize >= HASH_TABLE_MAX_SIZE)
	{
		fprintf(stdout,"[%s,%d]Eror...\n",__func__,__LINE__);
		return 0;
	}
	unsigned int uI4Pos = 0;
	HashNode* pTmpNode = NULL;
	uI4Pos = SDBMHash(pSKey) % HASH_TABLE_MAX_SIZE;
	HashNode* pHead = pHashTable[uI4Pos];

	/* Insert Into Hash Node */
	if(pHead)
	{

		if(!strcmp(pHead->sIp, pSKey))
		{
			fprintf(stdout,"[%s,%d]Already Exist...\n",__func__,__LINE__);
			return 1;
		}
		pHead = pHead->pNext;
	}
	pHead = (HashNode*)malloc(sizeof(HashNode));
	strcpy(pHead->sIp, pSKey);
	strcpy(pHead->sMac, pSMac);
	pHead->pNext = NULL;
	
	/* Increae Size Of HashTable */
	pI4HashTableSize = (pHashTable[uI4Pos] == NULL ? pI4HashTableSize+1 : pI4HashTableSize);

	pHashTable[uI4Pos] = pHead;
	return 1;
}

/*
 * Return: 1-success
 * */
int HashTablePrint(void)
{
	int i = 0;
	HashNode* pHead = NULL;

	for(i=0; i<HASH_TABLE_MAX_SIZE; i++)
	{
		if(pHashTable[i])
		{
			pHead = pHashTable[i];
			while(pHead)
			{
				printf("%s\t%s\n",pHead->sIp, pHead->sMac);
				pHead = pHead->pNext;
			}
		}
	}
	return 1;
}

/*
 * Return: 1-success, 0-failure
 * */
int ReadArpCacheAndShow(void)
{
	FILE* fp = NULL;
	char sLine[LINESIZE] = {0};
	char sIp[IPSIZE]  = {0};
	char sMac[MACSIZE] = {0};
	
	/* Open The Cache File */	
	fp = fopen(ARP_ENTRY_FILE, "r");
	if(NULL == fp)
	{
		perror("Open Error");
		return 0;
	}

	/* Read The Cache */
	do{
		bzero((void*)sLine, sizeof(sLine));
		if(fgets(sLine, LINESIZE, fp))
		{
			sscanf(sLine, "%s %*s %*s %s", sIp, sMac);
			if(strcmp(sIp, "IP"))
			{
				HashTableInsert(sIp, sMac);
			}
		}
	}while(strlen(sLine));

	/* Show The Cache */
	HashTablePrint();

	/* Close The Cache File */
	fclose(fp);
	fp = NULL;

	return 1;
}

int main(int argc, char** argv)
{
	HashTableInit();
	ReadArpCacheAndShow();
	return 1;
}
#endif
