### kswpad

#### when to wake up kswapd  
![kswapd](https://github.com/puckchen/picture/blob/master/kswapd-wakeup.jpg)
#### where to over kswapd  

#### the algorithm of kswapd  
1. kswapd scan all the zones in node from highmem->normal->dma
to make all zone over high_water_mark.  And return the highest zone index which are reclaiming at.  

The first thing is to find the highest non-balanced zone.  
It skipped [non-populated-zone](#non-populated-zone) and [non-reclaimable-zone](#non-reclaimable-zone) and [balanced-zone](#balanced-zone).
```
for (i = pgdat->nr_zones - 1; i >= 0; i--) {
  struct zone *zone = pgdat->node_zones + i;

  if (!populated_zone(zone))
  				continue;
  if (sc.priority != DEF_PRIORITY &&
      !zone_reclaimable(zone))
      continue;
  if (!zone_balanced(zone, order, false, 0, 0)) {
      end_zone = i;
      break;
}
```

#### <span id="non-populated-zone">balanced-zone</span>  
If there is no pages in this zone, it's a non-populated-zone.
For example, movable zone.
```
static inline int populated_zone(struct zone *zone)
{
	return (!!zone->present_pages);
}
```
#### <span id="non-reclaimable-zone">non-reclaimable-zone</span>
If the zone scaned pages > [zone_reclaimable_pages](#zone_reclaimable_pages) *6
```
bool zone_reclaimable(struct zone *zone)
{
	return zone_page_state_snapshot(zone, NR_PAGES_SCANNED) <
		zone_reclaimable_pages(zone) * 6;
}
```
#### <span id="balanced-zone">balanced-zone</span>  
```
```

#### <span id="zone_reclaimable_pages">zone_reclaimable_pages</span>  
All the FileCache and Anons pages
