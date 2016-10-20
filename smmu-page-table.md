##### How to create the pagetable of smmu

The page table is 2 9 9 12, there level page-table of hi3660 smmu.

There are 4 pgd entry.
		 512 pmd entry
         512 pte entry

eg: va 0x12345678

create 4 pgd entry firstly.
va 0x12345678 >> 30, then we know which pgd-entry the iova is belong. eg 2  
then read the value of *pgd-entry[2].  
if the value of *pgd-entry[2] is null, it means the pmd is none. So alloc  
512 * 8 byte(int) = 4K pmd-entry, and set the address to *pgd-entry[2].  
then 0x12345678 >> 21, get which pmd-entry, eg 12  
if the value of *pmd-entry[12] is null, it means the pte is none. So alloc  
512 * 8 byte(int) = 4k pte-entry. and set the address to *pmd-entry[12]  
then 0x12345678 >> 12, get which pte-entry, eg 19  
and set pte-entry[19] = 0x12345678 align 4K  
