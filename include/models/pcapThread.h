#ifndef PCAPTHREAD_H
#define PCAPTHREAD_H

#include <QThread>
#ifdef _WIN32
#include <winsock2.h>
#endif

class PcapThread : public QThread
{
    Q_OBJECT
public:
    explicit PcapThread(char* name, QObject *parent =nullptr);
    ~PcapThread();
    void run() override;
protected:
    static void staticPacketHandler(u_char *user, const struct pcap_pkthdr *pkthdr, const u_char *packetd_ptr);

    void packetHandler(u_char *user, const struct pcap_pkthdr *pkthdr, const u_char *packetd_ptr);


signals:
    void packetCaptured(const QStringList &packedData);
    void sendPacketToDB(const QStringList &packedData, const QByteArray &rawData);
    void finished();
private:
    char* choosenDevName;
};
#endif // PCAPTHREAD_H
