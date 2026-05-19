#ifndef _CACHE_DRV_ESP32S3_H_
#define _CACHE_DRV_ESP32S3_H_

#define CACHE_LL_L1_ICACHE_AUTOLOAD (1 << 2)
#define CACHE_LL_L1_DCACHE_AUTOLOAD (1 << 2)
#define CACHE_LL_ID_ALL                                                        \
  2 // All of the caches in a type and level, make this value greater than any
    // id
#define CACHE_LL_LEVEL_INT_MEM 0 // Cache level for accessing internal mem
#define CACHE_LL_LEVEL_EXT_MEM 1 // Cache level for accessing external mem
#define CACHE_LL_LEVEL_ALL                                                     \
  2 // All of the cache levels, make this value greater than any level
#define CACHE_LL_LEVEL_NUMS 1 // Number of cache levels

typedef enum {
  CACHE_TYPE_DATA,
  CACHE_TYPE_INSTRUCTION,
  CACHE_TYPE_ALL // This means both ICache and DCache will be used. On some
                 // chips, I/D are controlled by a shared Cache. Also use this
                 // enum under this condition. See
                 // `SOC_SHARED_IDCACHE_SUPPORTED`.
} cache_type_t;

/**
 * Necessary hal contexts, could be maintained by upper layer in the future
 */
typedef struct {
  bool i_autoload_en;
  bool d_autoload_en;
#if CACHE_LL_ENABLE_DISABLE_STATE_SW
  // There's no register indicating if cache is enabled on these chips, use sw
  // flag to save this state.
  bool i_cache_enabled;
  bool d_cache_enabled;
#endif
} cache_hal_state_t;

typedef struct {
  cache_hal_state_t l1;
  cache_hal_state_t l2;
} cache_hal_context_t;

extern void Cache_Enable_ICache(uint32_t autoload);
extern uint32_t Cache_Disable_ICache(void);
extern void Cache_Enable_DCache(uint32_t autoload);
extern uint32_t Cache_Disable_DCache(void);

// this has to be moved for initialization of the cache context before
// cache_hal_enable is called, which is required by spi_flash_wrap_enable_77.
static cache_hal_context_t ctx;

/**
 * @brief Enable ICache
 *
 * @param inst_autoload_en ICache auto preload enabled
 */
__attribute__((always_inline)) static inline void
cache_ll_l1_enable_icache(bool inst_autoload_en) {
  Cache_Enable_ICache(inst_autoload_en ? CACHE_LL_L1_ICACHE_AUTOLOAD : 0);
}

/**
 * @brief Disable ICache
 */
__attribute__((always_inline)) static inline void
cache_ll_l1_disable_icache(void) {
  Cache_Disable_ICache();
}

/**
 * @brief Enable DCache
 *
 * @param data_autoload_en DCache auto preload enabled
 */
__attribute__((always_inline)) static inline void
cache_ll_l1_enable_dcache(bool data_autoload_en) {
  Cache_Enable_DCache(data_autoload_en ? CACHE_LL_L1_DCACHE_AUTOLOAD : 0);
}

/**
 * @brief Disable DCache
 */
__attribute__((always_inline)) static inline void
cache_ll_l1_disable_dcache(void) {
  Cache_Disable_DCache();
}

/**
 * @brief Disable Cache
 *
 * @param cache_level  level of the cache
 * @param type         see `cache_type_t`
 * @param cache_id     id of the cache in this type and level
 */
__attribute__((always_inline)) static inline void
cache_ll_disable_cache(uint32_t cache_level, cache_type_t type,
                       uint32_t cache_id) {
  switch (type) {
  case CACHE_TYPE_INSTRUCTION:
    cache_ll_l1_disable_icache();
    break;
  case CACHE_TYPE_DATA:
    cache_ll_l1_disable_dcache();
    break;
  default: // CACHE_TYPE_ALL
    cache_ll_l1_disable_icache();
    cache_ll_l1_disable_dcache();
    break;
  }
}

/**
 * @brief Enable Cache
 *
 * @param cache_level       level of the cache
 * @param type              see `cache_type_t`
 * @param cache_id          id of the cache in this type and level
 * @param data_autoload_en  data autoload enabled or not
 * @param inst_autoload_en  inst autoload enabled or not
 */
__attribute__((always_inline)) static inline void
cache_ll_enable_cache(uint32_t cache_level, cache_type_t type,
                      uint32_t cache_id, bool inst_autoload_en,
                      bool data_autoload_en) {
  switch (type) {
  case CACHE_TYPE_INSTRUCTION:
    cache_ll_l1_enable_icache(inst_autoload_en);
    break;
  case CACHE_TYPE_DATA:
    cache_ll_l1_enable_dcache(data_autoload_en);
    break;
  default: // CACHE_TYPE_ALL
    cache_ll_l1_enable_icache(inst_autoload_en);
    cache_ll_l1_enable_dcache(data_autoload_en);
    break;
  }
}

void cache_hal_disable(uint32_t cache_level, cache_type_t type) {
  cache_ll_disable_cache(cache_level, type, CACHE_LL_ID_ALL);
}

void cache_hal_enable(uint32_t cache_level, cache_type_t type) {
  if (cache_level == 1) {
    cache_ll_enable_cache(1, type, CACHE_LL_ID_ALL, ctx.l1.i_autoload_en,
                          ctx.l1.d_autoload_en);
  } else if (cache_level == 2) {
    cache_ll_enable_cache(2, type, CACHE_LL_ID_ALL, ctx.l2.i_autoload_en,
                          ctx.l2.d_autoload_en);
  }
}

#endif /* _CACHE_DRV_ESP32S3_H_ */