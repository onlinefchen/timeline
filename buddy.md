### buddy 算法

#### 1. 页的分割  
```
0 		1个页 		
1 		2个页
2 		4个页
3 		8个页
4 		16个页
5 		32个页
6 		64个页
7 		128个页
8 		256个页
9		 512个页
10		1024个页
```

##### 分割有3中情况：  
* 申请order = x, order = x中有1个单位，则直接返回1个单位，此时order=x还有0个单位
* 申请order = x, order = x有2个单位，则直接返回1个单位，此时order=x还有1个单位
* 申请order = x, order = x有0个单位，需要向高位借1, 然后返回1个单位,此时order=x还有1个单位
* 申请order = x, order = x有0个单位，order = x + n(0,1,2..)，也有0个单位，  
需要像order = x+ n + 1 借1个单位，这样(order = x) ~ (order = x + n(0,1,2..))，均还剩下1个单位，返回1个单位

##### 代码逻辑如下：

```
static inline
struct page *__rmqueue_smallest(struct zone *zone, unsigned int order,
						int migratetype)
{
	unsigned int current_order;
	struct free_area *area;
	struct page *page;

	/* Find a page of the appropriate size in the preferred list */
	for (current_order = order; current_order < MAX_ORDER; ++current_order) {
		area = &(zone->free_area[current_order]);
		if (list_empty(&area->free_list[migratetype])) 从当前需要的order往上遍历
			continue;

		page = list_entry(area->free_list[migratetype].next,
							struct page, lru);
		list_del(&page->lru);
		rmv_page_order(page);       从buddy去除
		area->nr_free--;
        /*页的分割，传入当前需要去申请的free_list入口，当前需要的order，和分割order*/
		expand(zone, page, order, current_order, area, migratetype); 
		set_freepage_migratetype(page, migratetype);
		return page;
	}

	return NULL;
}
```
```
static inline void expand(struct zone *zone, struct page *page,
	int low, int high, struct free_area *area,
	int migratetype)
{
	unsigned long size = 1 << high;

	/*从高到底依次加上对应的分割的页*/
	while (high > low) {
		area--;
		high--;
		size >>= 1;

		list_add(&page[size].lru, &area->free_list[migratetype]);
		area->nr_free++;
		set_page_order(&page[size], high);
	}
}
```
#### 2. 页的合并 

页面的代码看起来比较复杂，主要是这里用了一些编程技巧

1. page_idx与buddy_idx   

``` 
page_idx = pfn & ((1 << MAX_ORDER) - 1);   
pfn号与上1<< MAX_ORDER - 1，即相当于把所有的PFN与2的10次方做一个取模，即把内存按照4MB来划分  
因为最大只能合并成2^MAX_ORDER这么大。  
那么这4MB里边page_idx对应的可以合并的兄弟如何计算  
buddy_idx = page_idx ^ (1 << order)   
相当于page_idx的各个位置取反，因为一共有0 ~ 1 << order - 1 个idx  
例如:  
110的兄弟页是001  
这样两头的合并往中间靠拢
最后两个合并成一个大单位4M
可以理解成  22位
0000000000000000000000  

0000000000000000000001 	1111111111111111111110  


左边11和右边11个对齐互为buddy

则第一个
```

2. merge   





```
static inline void __free_one_page(struct page *page,
		unsigned long pfn,
		struct zone *zone, unsigned int order,
		int migratetype)
{
	unsigned long page_idx;
	unsigned long combined_idx;
	unsigned long uninitialized_var(buddy_idx);
	struct page *buddy;
	unsigned int max_order;

	max_order = min_t(unsigned int, MAX_ORDER, pageblock_order + 1);

	VM_BUG_ON(!zone_is_initialized(zone));
	VM_BUG_ON_PAGE(page->flags & PAGE_FLAGS_CHECK_AT_PREP, page);

	VM_BUG_ON(migratetype == -1);
	if (likely(!is_migrate_isolate(migratetype)))
		__mod_zone_freepage_state(zone, 1 << order, migratetype);

	page_idx = pfn & ((1 << MAX_ORDER) - 1);

	VM_BUG_ON_PAGE(page_idx & ((1 << order) - 1), page);
	VM_BUG_ON_PAGE(bad_range(zone, page), page);

continue_merging:
	while (order < max_order - 1) {
		buddy_idx = __find_buddy_index(page_idx, order);
		buddy = page + (buddy_idx - page_idx);
		if (!page_is_buddy(page, buddy, order))
			goto done_merging;
		/*
		 * Our buddy is free or it is CONFIG_DEBUG_PAGEALLOC guard page,
		 * merge with it and move up one order.
		 */
		if (page_is_guard(buddy)) {
			clear_page_guard(zone, buddy, order, migratetype);
		} else {
			list_del(&buddy->lru);
			zone->free_area[order].nr_free--;
			rmv_page_order(buddy);
		}
		combined_idx = buddy_idx & page_idx;
		page = page + (combined_idx - page_idx);
		page_idx = combined_idx;
		order++;
	}
	if (max_order < MAX_ORDER) {
		/* If we are here, it means order is >= pageblock_order.
		 * We want to prevent merge between freepages on isolate
		 * pageblock and normal pageblock. Without this, pageblock
		 * isolation could cause incorrect freepage or CMA accounting.
		 *
		 * We don't want to hit this code for the more frequent
		 * low-order merging.
		 */
		if (unlikely(has_isolate_pageblock(zone))) {
			int buddy_mt;

			buddy_idx = __find_buddy_index(page_idx, order);
			buddy = page + (buddy_idx - page_idx);
			buddy_mt = get_pageblock_migratetype(buddy);

			if (migratetype != buddy_mt
					&& (is_migrate_isolate(migratetype) ||
						is_migrate_isolate(buddy_mt)))
				goto done_merging;
		}
		max_order++;
		goto continue_merging;
	}

done_merging:
	set_page_order(page, order);

	/*
	 * If this is not the largest possible page, check if the buddy
	 * of the next-highest order is free. If it is, it's possible
	 * that pages are being freed that will coalesce soon. In case,
	 * that is happening, add the free page to the tail of the list
	 * so it's less likely to be used soon and more likely to be merged
	 * as a higher order page
	 */
	if ((order < MAX_ORDER-2) && pfn_valid_within(page_to_pfn(buddy))) {
		struct page *higher_page, *higher_buddy;
		combined_idx = buddy_idx & page_idx;
		higher_page = page + (combined_idx - page_idx);
		buddy_idx = __find_buddy_index(combined_idx, order + 1);
		higher_buddy = higher_page + (buddy_idx - combined_idx);
		if (page_is_buddy(higher_page, higher_buddy, order + 1)) {
			list_add_tail(&page->lru,
				&zone->free_area[order].free_list[migratetype]);
			goto out;
		}
	}

	list_add(&page->lru, &zone->free_area[order].free_list[migratetype]);
out:
	zone->free_area[order].nr_free++;
}
```
