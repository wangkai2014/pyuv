
#include "pyuv.h"

#include "helper.c"

#include "loop.c"
#include "async.c"
#include "timer.c"
#include "prepare.c"
#include "idle.c"
#include "check.c"
#include "signal.c"
#include "stream.c"
#include "pipe.c"
#include "tcp.c"
#include "tty.c"
#include "udp.c"
#include "dns.c"
#include "fs.c"
#include "threadpool.c"
#include "process.c"
#include "util.c"
#include "errno.c"
#include "error.c"

#define LIBUV_VERSION UV_VERSION_MAJOR.UV_VERSION_MINOR-LIBUV_REVISION

#ifdef PYUV_PYTHON3
static PyModuleDef pyuv_module = {
    PyModuleDef_HEAD_INIT,
    "pyuv",                 /*m_name*/
    NULL,                   /*m_doc*/
    -1,                     /*m_size*/
    NULL,                   /*m_methods*/
};
#endif

/* Module */
PyObject*
init_pyuv(void)
{
    /* Modules */
    PyObject *pyuv;
    PyObject *errno_module;
    PyObject *error_module;
    PyObject *dns;
    PyObject *fs;
    PyObject *util;

    /* Initialize GIL */
    PyEval_InitThreads();

    /* Main module */
#ifdef PYUV_PYTHON3
    pyuv = PyModule_Create(&pyuv_module);
#else
    pyuv = Py_InitModule("pyuv", NULL);
#endif

    /* Errno module */
    errno_module = init_errno();
    if (errno_module == NULL) {
        goto fail;
    }
    PyUVModule_AddObject(pyuv, "errno", errno_module);

    /* Error module */
    error_module = init_error();
    if (error_module == NULL) {
        goto fail;
    }
    PyUVModule_AddObject(pyuv, "error", error_module);

    /* DNS module */
    dns = init_dns();
    if (dns == NULL) {
        goto fail;
    }
    PyUVModule_AddObject(pyuv, "dns", dns);

    /* FS module */
    fs = init_fs();
    if (fs == NULL) {
        goto fail;
    }
    PyUVModule_AddObject(pyuv, "fs", fs);

    /* Util module */
    util = init_util();
    if (util == NULL) {
        goto fail;
    }
    PyUVModule_AddObject(pyuv, "util", util);

    /* Types */
    TCPType.tp_base = &IOStreamType;
    PipeType.tp_base = &IOStreamType;
    TTYType.tp_base = &IOStreamType;

    PyUVModule_AddType(pyuv, "Loop", &LoopType);
    PyUVModule_AddType(pyuv, "Async", &AsyncType);
    PyUVModule_AddType(pyuv, "Timer", &TimerType);
    PyUVModule_AddType(pyuv, "Prepare", &PrepareType);
    PyUVModule_AddType(pyuv, "Idle", &IdleType);
    PyUVModule_AddType(pyuv, "Check", &CheckType);
    PyUVModule_AddType(pyuv, "Signal", &SignalType);
    PyUVModule_AddType(pyuv, "TCP", &TCPType);
    PyUVModule_AddType(pyuv, "Pipe", &PipeType);
    PyUVModule_AddType(pyuv, "TTY", &TTYType);
    PyUVModule_AddType(pyuv, "UDP", &UDPType);
    PyUVModule_AddType(pyuv, "ThreadPool", &ThreadPoolType);
    PyUVModule_AddType(pyuv, "Process", &ProcessType);

    /* PyStructSequence types */
    if (LoopCountersResultType.tp_name == 0)
        PyStructSequence_InitType(&LoopCountersResultType, &loop_counters_result_desc);
    if (DNSHostResultType.tp_name == 0)
        PyStructSequence_InitType(&DNSHostResultType, &dns_host_result_desc);
    if (DNSNameinfoResultType.tp_name == 0)
        PyStructSequence_InitType(&DNSNameinfoResultType, &dns_nameinfo_result_desc);
    if (DNSAddrinfoResultType.tp_name == 0)
        PyStructSequence_InitType(&DNSAddrinfoResultType, &dns_addrinfo_result_desc);
    if (DNSQueryMXResultType.tp_name == 0)
        PyStructSequence_InitType(&DNSQueryMXResultType, &dns_query_mx_result_desc);
    if (DNSQuerySRVResultType.tp_name == 0)
        PyStructSequence_InitType(&DNSQuerySRVResultType, &dns_query_srv_result_desc);
    if (DNSQueryNAPTRResultType.tp_name == 0)
        PyStructSequence_InitType(&DNSQueryNAPTRResultType, &dns_query_naptr_result_desc);

    /* Constants */
    PyModule_AddIntMacro(pyuv, UV_JOIN_GROUP);
    PyModule_AddIntMacro(pyuv, UV_LEAVE_GROUP);

    /* Handle types */
    PyModule_AddIntMacro(pyuv, UV_UNKNOWN_HANDLE);
    PyModule_AddIntMacro(pyuv, UV_FILE);
#define XX(uc, lc) PyModule_AddIntMacro(pyuv, UV_##uc);
    UV_HANDLE_TYPE_MAP(XX)
#undef XX

    /* Module version (the MODULE_VERSION macro is defined by setup.py) */
    PyModule_AddStringConstant(pyuv, "__version__", __MSTR(MODULE_VERSION));

    /* libuv version */
    PyModule_AddStringConstant(pyuv, "LIBUV_VERSION", __MSTR(LIBUV_VERSION));

    return pyuv;

fail:
#ifdef PYUV_PYTHON3
    Py_DECREF(pyuv);
#endif
    return NULL;

}

#ifdef PYUV_PYTHON3
PyMODINIT_FUNC
PyInit_pyuv(void)
{
    return init_pyuv();
}
#else
PyMODINIT_FUNC
initpyuv(void)
{
    init_pyuv();
}
#endif


