// stdafx.cpp : source file that includes just the standard includes
// libsolidpaint3d.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include <PlastilinaCore/Stable.h>
#include <PlastilinaCore/Logging.h>

// TODO: reference any additional headers you need in STDAFX.H
// and not in this file
// uint32_t qHash(const Eigen::Matrix<float, 3, 1, 2, 3, 1> &key)
//{
//	qint16 x = lrintf(key[0] * 1000.0f);
//	qint16 y = lrintf(key[1] * 1000.0f);
//	qint16 z = lrintf(key[2] * 1000.0f);
//	quint64 d = (quint64)(x & 0xFFFF) << 32 | (quint64)(y & 0xFFFF) << 16
//| (quint64)(z & 0xFFFF); 	return qHash(d);
//}
//
// uint32_t qHash(const std::pair<int, int> & v)
//{
//    return qHash(v.first ^ (v.second << 8));
//}

static THREAD void* logptr = 0;

struct Log
{
    std::string indent;
    int         indentlevel;

    Log()
        : indent("")
        , indentlevel(0)
    {
    }
};

void Logging_increaseIndentation()
{
    if (logptr == NULL)
    {
        logptr = new Log;
    }
    if (logptr)
    {
        Log* l = static_cast<Log*>(logptr);
        l->indentlevel++;
        if (l->indentlevel >= 0)
        {
            l->indent.resize(l->indentlevel * 2, ' ');
        }
    }
}

void Logging_decreaseIndentation()
{
    if (logptr == NULL)
    {
        logptr = new Log;
    }
    if (logptr)
    {
        Log* l = static_cast<Log*>(logptr);
        l->indentlevel--;
        if (l->indentlevel >= 0)
        {
            l->indent.resize(l->indentlevel * 2, ' ');
        }
    }
}

// void myMessageOutput(QtMsgType type, const char *msg) {
//    const char* indent = "";
//
//    if (logptr) {
//        indent = static_cast<Log*>(logptr)->indent.c_str();
//    }
//    switch (type) {
//        case QtDebugMsg:
//            fprintf(stderr, "%s%s\n", indent, msg);
//            break;
//        case QtWarningMsg:
//            fprintf(stderr, "%s%s\n", indent, msg);
//            break;
//        case QtCriticalMsg:
//            fprintf(stderr, "%s%s\n", indent, msg);
//            break;
//        case QtFatalMsg:
//            fprintf(stderr, "%s%s\n", indent, msg);
//            abort();
//    }
//}
