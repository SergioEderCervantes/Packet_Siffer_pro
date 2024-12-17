#ifndef PCAPTHREAD_H
#define PCAPTHREAD_H

#include <QThread>
#include <QString>
#include <QStringList>

#ifdef _WIN32
#include <winsock2.h>
#include <pcap/pcap.h>
#else
#include <pcap/pcap.h>
#endif

class PcapThread : public QThread
{
    Q_OBJECT
public:
    explicit PcapThread(char* name, QString filter, int index, QObject *parent = nullptr);
    ~PcapThread();
    void run() override;

public slots:
    void handlerKiller(); // Slot para detener el hilo
    void stop();
    void resume();

signals:
    void packetCaptured(const QStringList &packedData); // Señal para enviar paquetes capturados
    void sendPacketToDB(const QStringList &packedData, const QByteArray &rawData);
    void finished(); // Señal cuando el hilo finaliza

protected:
    static void staticPacketHandler(u_char *user, const struct pcap_pkthdr *pkthdr, const u_char *packetd_ptr);
    void packetHandler(u_char *user, const struct pcap_pkthdr *pkthdr, const u_char *packetd_ptr);


private:
    char* choosenDevName; // Nombre del dispositivo elegido
    QString filterType;   // Tipo de filtro
    pcap_t *capdev;       // Descriptor de captura
    bool stopRequested;   // Bandera para detener el hilo
    bool cont;
    int indexFilter;
};

#endif // PCAPTHREAD_H
