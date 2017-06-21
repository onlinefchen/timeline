ARMV8页表结构



1. 页表粒度配置 Memory translation granule size

   ID_AA64MMFR0_EL1表示了当支持的粒度能力



2. 4K PageSize 粒度 页表设置

   ```
   Translation table lookup with 4KB pages:

   +--------+--------+--------+--------+--------+--------+--------+--------+
   |63    56|55    48|47    40|39    32|31    24|23    16|15     8|7      0|
   +--------+--------+--------+--------+--------+--------+--------+--------+
    |                 |         |         |         |         |
    |                 |         |         |         |         v
    |                 |         |         |         |   [11:0]  in-page offset
    |                 |         |         |         +-> [20:12] L3 index
    |                 |         |         +-----------> [29:21] L2 index
    |                 |         +---------------------> [38:30] L1 index
    |                 +-------------------------------> [47:39] L0 index
    +-------------------------------------------------> [63] TTBR0/1
   ```

   ```
   Start                   End                     Size            Use
   -----------------------------------------------------------------------
   0000000000000000        0000007fffffffff         512GB          user
   ffffff8000000000        ffffffffffffffff         512GB          kernel
   ```

   ```
   D_Table is Table descriptor                                           Level 3 table
   D_Block is Block descriptor                                           +---------+
   D_Page  is Page  descriptor                                           |         |
                                                                         |         |
                                                      Level 2 table      +---------+
                                                      +---------+        | D_Page  +--> 4K
                                                      |         |        +---------+
                                                      +---------+        |         |
                                    Level 1 table     | D_Block +--->2M  |         |
                                    +---------+       +---------+        |         |
                                    |         |       | D_Table +------->+---------+
                                    +---------+       +---------+
                     Level 0 table  | D_Block +-->1G  |         |
                     +---------+    +---------+       |         |
                     |         |    | D_Table +------>+---------+
                     |         |    +---------+
                     +---------+    |         |
                     | D_Table +--->+---------+
                     +---------+
                     |         |
                     |         |
   |------------+    |         |
   |  TTBR_ELx  +--->+---------+
   +------------+
   ```

3. 页表属性与Bit说明

   ```
   UXN or XN   bit[54]   The Excute-never bit, now it's user.
   PXN         bit[53]   The privileged excute-never bit. Determines whether the region is excutable at EL1 
   Contigous   bit[52]
   AF          bit[10]   The Access flag
   SH          bit[9:8]  Shareability field
   NS          bit[5]    Non-secure bit
   ```

