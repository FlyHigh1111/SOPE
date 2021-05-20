#include "common.h"

void WriteLog(struct Log log)
{
    // structure: inst ; i ; t ; pid ; tid ; res ; oper
    fprintf(stdout, "%ld ; %d ; %d ; %d ; %ld ; %d ; %s\n", time(NULL), log.i, log.t, log.pid, log.tid, log.res, log.oper);
}