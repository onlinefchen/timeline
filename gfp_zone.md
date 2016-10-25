### GFP_ZONE的实现, ZONE的选择

gfp_zone是通过gfp_flag的最后4个bit来获取返回当前申请的highest-zone  

```
#define ___GFP_DMA		    0x01u
#define ___GFP_HIGHMEM		0x02u
#define ___GFP_DMA32		  0x04u
#define ___GFP_MOVABLE		0x08u

enum zone_type {
#ifdef CONFIG_ZONE_DMA
	ZONE_DMA,
#endif
#ifdef CONFIG_ZONE_DMA32
	ZONE_DMA32,
#endif
	ZONE_NORMAL,
#ifdef CONFIG_HIGHMEM
	ZONE_HIGHMEM,
#endif
	ZONE_MOVABLE,
#ifdef CONFIG_ZONE_DEVICE
	ZONE_DEVICE,
#endif
	__MAX_NR_ZONES

}; 
```

最开始的实现是这样的:  
下面比较容易理解，  
如果定义了ZONE_DMA，并且flag有__GFP_DMA，则只能从DMA_ZONE中申请  
如果定义了ZONE_DMA32，并且
```
static inline enum zone_type gfp_zone(gfp_t flags)
{
#ifdef CONFIG_ZONE_DMA
 if (flags & __GFP_DMA)
  return ZONE_DMA;
#endif
#ifdef CONFIG_ZONE_DMA32
 if (flags & __GFP_DMA32)
  return ZONE_DMA32;
#endif
 if ((flags & (__GFP_HIGHMEM | __GFP_MOVABLE)) ==
  (__GFP_HIGHMEM | __GFP_MOVABLE))
  return ZONE_MOVABLE;
#ifdef CONFIG_HIGHMEM
 if (flags & __GFP_HIGHMEM)
  return ZONE_HIGHMEM;
#endif
 return ZONE_NORMAL;
}
```
现在的实现是这样的：  
```
#define GFP_ZONE_TABLE ( \
	(ZONE_NORMAL << 0 * GFP_ZONES_SHIFT)				       \
	| (OPT_ZONE_DMA << ___GFP_DMA * GFP_ZONES_SHIFT)		       \
	| (OPT_ZONE_HIGHMEM << ___GFP_HIGHMEM * GFP_ZONES_SHIFT)	       \
	| (OPT_ZONE_DMA32 << ___GFP_DMA32 * GFP_ZONES_SHIFT)		       \
	| (ZONE_NORMAL << ___GFP_MOVABLE * GFP_ZONES_SHIFT)		       \
	| (OPT_ZONE_DMA << (___GFP_MOVABLE | ___GFP_DMA) * GFP_ZONES_SHIFT)    \
	| (ZONE_MOVABLE << (___GFP_MOVABLE | ___GFP_HIGHMEM) * GFP_ZONES_SHIFT)\
	| (OPT_ZONE_DMA32 << (___GFP_MOVABLE | ___GFP_DMA32) * GFP_ZONES_SHIFT)\
)

static inline enum zone_type gfp_zone(gfp_t flags)
{
	enum zone_type z;
	int bit = (__force int) (flags & GFP_ZONEMASK);

	z = (GFP_ZONE_TABLE >> (bit * GFP_ZONES_SHIFT)) &
					 ((1 << GFP_ZONES_SHIFT) - 1);
	VM_BUG_ON((GFP_ZONE_BAD >> bit) & 1);
	return z;
}
```
