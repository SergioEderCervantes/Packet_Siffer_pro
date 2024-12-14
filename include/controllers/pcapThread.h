#ifndef PCAPTHREAD_H
#define PCAPTHREAD_H

#include <QThread>
#include <string>
#ifdef _WIN32
#include <winsock2.h>
#endif

#include "views/snifferWindow.h"
class PcapThread : public QThread
{
    Q_OBJECT
public:
    explicit PcapThread(SnifferWindow* window, std::string name);
    ~PcapThread();
protected:
    void run() override;

    static void packetHandler(u_char *user, const struct pcap_pkthdr *pkthdr, const u_char *packetd_ptr);

    static void call_me(u_char *user, const struct pcap_pkthdr *pkthdr, const u_char *packetd_ptr);


private:
    SnifferWindow* window;
    static SnifferWindow* windowPtr;
    const char* choosenDevName;
};

#endif // PCAPTHREAD_H
