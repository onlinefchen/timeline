## Linux-mm-2017  

### 内核版本V4.9

#### 1. mm, compaction: add vmstats for kcompactd work 
#### 作者： (rientjes@google.com)
```
https://lkml.org/lkml/2016/12/8/45

主要添加了kcompactd的这个内核thread的相关overhead  
内核中已经了"compact_migrate_scanned" 和 "compact_free_scanned"来记录总的  
compact的操作页数，但是没有记录kcompactd线程的直接操作。 此patch添加了  
"compact_daemon_migrate_scanned" 和 "compact_daemon_free_scanned"记录相关	
已被接受 Acked-by: Vlastimil Babka <vbabka@suse.cz>
```

#### 2. CONFIG_DEBUG_VIRTUAL for arm64 第5版
#### 作者： (labbott@redhat.com)   ION Maintainer
```
https://lkml.org/lkml/2016/12/6/700

这个主要是一个维测功能，在我们的系统中（产品版）有很多不同的地址映射，除了线程地址映射以外  
还有fixed map映射。此维测功能打开以后会监测到不能用virt_to_phy类似的函数转换，
如果转换了一个不在线性区的地址，就会报错。
此patch还带来了一些编程上的变化，比如在smp启动的时候，一些函数的物理地址，以前
还是使用__pa，现在修改为使用__pa_symbol
```

#### 3. GFP_NOFAIL cleanups
#### 作者 Andrew Morton akpm@linux-foundation.org  社区2号首长
```
此patch主要是对oom reaper，内核OOM收割机的一些bugfix，并且添加了一些维测手段
1、在alloc stall的时候知道我们stall了多久
+	/* Make sure we know about allocations which stall for too long */
+	if (time_after(jiffies, alloc_start + stall_timeout)) {
+		warn_alloc(gfp_mask,
+			"page alloction stalls for %ums, order:%u",
+			jiffies_to_msecs(jiffies-alloc_start), order);
+		stall_timeout += 10 * HZ;


2、对于OOM添加了一些fix
在__alloc_pages_may_oom中添加了一些退出
+	/* Coredumps can quickly deplete all memory reserves */
+	if (current->flags & PF_DUMPCORE)
+		goto out;
+	/* The OOM killer will not help higher order allocs */
+	if (order > PAGE_ALLOC_COSTLY_ORDER)
+		goto out;
+	/* The OOM killer does not needlessly kill tasks for lowmem */
+	if (ac->high_zoneidx < ZONE_NORMAL)
+		goto out;
+	if (pm_suspended_storage())
+		goto out;

内核社区一直不太喜欢google的LMK，社区的进程kill，主要靠OOM来保证，因为LMK中
把进程杀了，会对当前的lru链表一些破坏。
此外我发现像这样资深的maintainer 像Andrew，Linus 发patch有一个特点，就是comments非常长
```
#### 4.    vm, vmscan: enahance vmscan tracepoints
#### 作者 (akpm@linux-foundation.org)
```
http://lkml.iu.edu/hypermail/linux/kernel/1701.0/03024.html


```





