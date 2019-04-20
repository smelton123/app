#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <uv.h>
#include <unistd.h>
#include "vkVersion.h"
#include "vkSummary.h"
#include <curl/curl.h>

void Summary::printVersions(void)
{
    char buf[256] = { 0 };
    int cpu_cores = sysconf(_SC_NPROCESSORS_ONLN);

#   if defined(__clang__)
    snprintf(buf, sizeof buf, "clang:%d.%d.%d", __clang_major__, __clang_minor__, __clang_patchlevel__);
#   elif defined(__GNUC__)
    snprintf(buf, sizeof buf, "gcc:%d.%d.%d", __GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__);
#   elif defined(_MSC_VER)
    snprintf(buf, sizeof buf, "MSVC:%d", MSVC_VERSION);
#   endif

    printf(" * %-13s%s:%s %s core:%d\n", "About", APP_NAME, APP_VERSION, buf, cpu_cores);

    printf(" * %-13slibuv:%s curl:%s\n", "Libs", uv_version_string(), curl_version());    
}

