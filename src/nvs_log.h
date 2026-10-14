/* The core logs a lot; none of it survives into a 64 KB part. */
#ifndef NVS_LOG_H_
#define NVS_LOG_H_
#define LOG_DBG(...) ((void)0)
#define LOG_INF(...) ((void)0)
#define LOG_WRN(...) ((void)0)
#define LOG_ERR(...) ((void)0)
#define LOG_HEXDUMP_DBG(...) ((void)0)
#define LOG_MODULE_REGISTER(...)
#endif
